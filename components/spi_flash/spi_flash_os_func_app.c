/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <sys/lock.h>
#include <sys/param.h>  //For max/min
#include "esp_attr.h"
#include "esp_private/system_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/spi_types.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_compiler.h"
#include "esp_rom_sys.h"

#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"
#include "esp_flash_partitions.h"

#include "esp_private/spi_flash_os.h"
#include "esp_private/cache_utils.h"
#include "esp_private/spi_share_hw_ctrl.h"

// For C5 encrypted write workaround
// Functions are only available when CONFIG_SPI_FLASH_FREQ_LIMIT_C5_240MHZ is true
#include "esp_private/spi_flash_freq_limit_cbs.h"

#define SPI_FLASH_CACHE_NO_DISABLE  (CONFIG_SPI_FLASH_AUTO_SUSPEND || (CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA) || CONFIG_APP_BUILD_TYPE_RAM)
static const char TAG[] = "spi_flash";

#if SPI_FLASH_CACHE_NO_DISABLE
static _lock_t s_spi1_flash_mutex;
#endif  //  #if SPI_FLASH_CACHE_NO_DISABLE

/*
 * OS functions providing delay service and arbitration among chips, and with the cache.
 *
 * The cache needs to be disabled when chips on the SPI1 bus is under operation, hence these functions need to be put
 * into the IRAM,and their data should be put into the DRAM.
 */

/*
 * Time yield algorithm:
 * Every time spi_flash_os_check_yield() is called:
 *
 * 1. If the time since last end() function is longer than CONFIG_SPI_FLASH_ERASE_YIELD_TICKS (time
 *     to yield), all counters will be reset, as if the yield has just ends;
 * 2. If the time since last yield() is longer than CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS, will
 *    return a yield request. When the yield() is called, all counters will be reset.
 * Note: Short intervals between start() and end() after the last yield() will not reset the
 *       counter mentioned in #2, but still be counted into the time mentioned in #2.
 */
typedef struct {
    spi_bus_lock_dev_handle_t dev_lock;
    bool no_protect;    //to decide whether to check protected region (for the main chip) or not.
    uint32_t acquired_since_us;    // Time since last explicit yield()
    uint32_t released_since_us;    // Time since last end() (implicit yield)
    uint32_t start_flags;          // Flags passed to start() function, used to determine if freq_limit was called
} app_func_arg_t;

static inline void on_spi_released(app_func_arg_t* ctx);
static inline void on_spi_acquired(app_func_arg_t* ctx);
static inline void on_spi_yielded(app_func_arg_t* ctx);
static inline bool on_spi_check_yield(app_func_arg_t* ctx);

#if !SPI_FLASH_CACHE_NO_DISABLE
IRAM_ATTR static void cache_enable(void* arg)
{
    spi_flash_enable_interrupts_caches_and_other_cpu();
}

IRAM_ATTR static void cache_disable(void* arg)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
}
#endif  //#if !SPI_FLASH_CACHE_NO_DISABLE

static IRAM_ATTR esp_err_t acquire_spi_bus_lock(void *arg)
{
    spi_bus_lock_dev_handle_t dev_lock = ((app_func_arg_t *)arg)->dev_lock;

    // wait for other devices (or cache) to finish their operation
    esp_err_t ret = spi_bus_lock_acquire_start(dev_lock, portMAX_DELAY);
    if (ret != ESP_OK) {
        return ret;
    }
    spi_bus_lock_touch(dev_lock);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t release_spi_bus_lock(void *arg)
{
    return spi_bus_lock_acquire_end(((app_func_arg_t *)arg)->dev_lock);
}

static esp_err_t spi23_start(void *arg, uint32_t flags)
{
    (void)flags;
    esp_err_t ret = acquire_spi_bus_lock(arg);
    on_spi_acquired((app_func_arg_t*)arg);
    return ret;
}

static esp_err_t spi23_end(void *arg)
{
    esp_err_t ret = release_spi_bus_lock(arg);
    on_spi_released((app_func_arg_t*)arg);
    return ret;
}

static IRAM_ATTR esp_err_t spi1_start(void *arg, uint32_t flags)
{
    esp_err_t ret = ESP_OK;
    app_func_arg_t* ctx = (app_func_arg_t*)arg;
    ctx->start_flags = flags;

    /**
     * There are three ways for ESP Flash API lock:
     * 1. spi bus lock, this is used when SPI1 is shared with GPSPI Master Driver
     * 2. mutex, this is used when the Cache isn't need to be disabled.
     * 3. cache lock (from cache_utils.h), this is used when we need to disable Cache to avoid access from SPI0
     *
     * From 1 to 3, the lock efficiency decreases.
     */
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    //use the lock to disable the cache and interrupts before using the SPI bus
    ret = acquire_spi_bus_lock(arg);
#elif SPI_FLASH_CACHE_NO_DISABLE
    _lock_acquire(&s_spi1_flash_mutex);
#else
    //directly disable the cache and interrupts when lock is not used
    cache_disable(NULL);
#endif

#if CONFIG_SPI_FLASH_DISABLE_SCHEDULER_IN_SUSPEND
    // Disable scheduler
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
#ifdef CONFIG_FREERTOS_SMP
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionDisable(NULL);
#else
        // Disable scheduler on the current CPU
        vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP
    }
#endif // CONFIG_SPI_FLASH_DISABLE_SCHEDULER_IN_SUSPEND

#if CONFIG_SPI_FLASH_FREQ_LIMIT_C5_240MHZ
    if (flags & ESP_FLASH_START_FLAG_LIMIT_CPU_FREQ) {
        esp_flash_freq_limit_cb();
    }
#endif

    on_spi_acquired(ctx);
    return ret;
}

static IRAM_ATTR esp_err_t spi1_end(void *arg)
{
    esp_err_t ret = ESP_OK;
    app_func_arg_t* ctx = (app_func_arg_t*)arg;

    // Call freq_limit_unlock if needed, before releasing the lock
#if CONFIG_SPI_FLASH_FREQ_LIMIT_C5_240MHZ
    uint32_t flags = ctx->start_flags;
    if (flags & ESP_FLASH_START_FLAG_LIMIT_CPU_FREQ) {
        esp_flash_freq_unlimit_cb();
    }
#endif

    /**
     * There are three ways for ESP Flash API lock, see `spi1_start`
     */
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    ret = release_spi_bus_lock(arg);
#elif SPI_FLASH_CACHE_NO_DISABLE
    _lock_release(&s_spi1_flash_mutex);
#else
    cache_enable(NULL);
#endif

#if CONFIG_SPI_FLASH_DISABLE_SCHEDULER_IN_SUSPEND
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
#ifdef CONFIG_FREERTOS_SMP
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionEnable(NULL);
#else
        xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP
    }
#endif // CONFIG_SPI_FLASH_DISABLE_SCHEDULER_IN_SUSPEND

    on_spi_released(ctx);
    return ret;
}

static esp_err_t spi_flash_os_check_yield(void *arg, uint32_t chip_status, uint32_t* out_request)
{
    assert (chip_status == 0);  //TODO: support suspend
    esp_err_t ret = ESP_ERR_TIMEOUT;    //Nothing happened
    uint32_t request = 0;

    if (on_spi_check_yield((app_func_arg_t *)arg)) {
        request = SPI_FLASH_YIELD_REQ_YIELD;
        ret = ESP_OK;
    }
    if (out_request) {
        *out_request = request;
    }
    return ret;
}

static esp_err_t spi_flash_os_yield(void *arg, uint32_t* out_status)
{
    if (likely(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)) {
#ifdef CONFIG_SPI_FLASH_ERASE_YIELD_TICKS
        vTaskDelay(CONFIG_SPI_FLASH_ERASE_YIELD_TICKS);
#else
        vTaskDelay(1);
#endif
    }
    on_spi_yielded((app_func_arg_t*)arg);
    return ESP_OK;
}

static esp_err_t delay_us(void *arg, uint32_t us)
{
    esp_rom_delay_us(us);
    return ESP_OK;
}

static void* get_buffer_malloc(void* arg, size_t reqest_size, size_t* out_size)
{
    /* Allocate temporary internal buffer to use for the actual read. If the preferred size
        doesn't fit in free internal memory, allocate the largest available free block.

        (May need to shrink read_chunk_size and retry due to race conditions with other tasks
        also allocating from the heap.)
    */
    void* ret = NULL;
    unsigned retries = 5;
    size_t read_chunk_size = reqest_size;
    while(ret == NULL && retries--) {
        size_t largest_free = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        read_chunk_size = MIN(read_chunk_size, largest_free);
        read_chunk_size = (read_chunk_size + 3) & ~3;
        ret = heap_caps_malloc(read_chunk_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }
    ESP_LOGV(TAG, "allocate temp buffer: %p (%d)", ret, read_chunk_size);
    *out_size = (ret != NULL? read_chunk_size: 0);
    return ret;
}

static void release_buffer_malloc(void* arg, void *temp_buf)
{
    free(temp_buf);
}

static esp_err_t main_flash_region_protected(void* arg, size_t start_addr, size_t size)
{
    if (!esp_partition_is_flash_region_writable(start_addr, size)) {
        return ESP_ERR_NOT_ALLOWED;
    }
#if !CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
    if (((app_func_arg_t*)arg)->no_protect || esp_partition_main_flash_region_safe(start_addr, size)) {
        //ESP_OK = 0, also means protected==0
        return ESP_OK;
    } else {
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif // !CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
    return ESP_OK;
}

static void main_flash_op_status(uint32_t op_status)
{
    bool is_erasing = op_status & SPI_FLASH_OS_IS_ERASING_STATUS_FLAG;
    spi_flash_set_erasing_flag(is_erasing);
}

static DRAM_ATTR app_func_arg_t main_flash_arg = {};

//for SPI1, we have to disable the cache and interrupts before using the SPI bus
static const DRAM_ATTR esp_flash_os_functions_t esp_flash_spi1_default_os_functions = {
    .start = spi1_start,
    .end = spi1_end,
    .region_protected = main_flash_region_protected,
    .delay_us = delay_us,
    .get_temp_buffer = get_buffer_malloc,
    .release_temp_buffer = release_buffer_malloc,
    .check_yield = spi_flash_os_check_yield,
    .yield = spi_flash_os_yield,
#if CONFIG_SPI_FLASH_BROWNOUT_RESET
    .set_flash_op_status = main_flash_op_status,
#else
    .set_flash_op_status = NULL,
#endif
};

static const esp_flash_os_functions_t esp_flash_spi23_default_os_functions = {
    .start = spi23_start,
    .end = spi23_end,
    .delay_us = delay_us,
    .get_temp_buffer = get_buffer_malloc,
    .release_temp_buffer = release_buffer_malloc,
    .region_protected = NULL,
    .check_yield = spi_flash_os_check_yield,
    .yield = spi_flash_os_yield,
    .set_flash_op_status = NULL,
};

static bool use_bus_lock(int host_id)
{
    if (host_id != SPI1_HOST) {
        return true;
    }
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    return true;
#else
    return false;
#endif
}

// This function is only called by users usually via `spi_bus_add_flash_device` to initialise os functions.
// System will initialise them via `esp_flash_app_enable_os_functions`
esp_err_t esp_flash_init_os_functions(esp_flash_t *chip, int host_id, spi_bus_lock_dev_handle_t dev_handle)
{
    if (use_bus_lock(host_id) && !dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    chip->os_func_data = heap_caps_malloc(sizeof(app_func_arg_t),
                                     MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (chip->os_func_data == NULL) {
        return ESP_ERR_NO_MEM;
    }

    switch (host_id) {
        case SPI1_HOST:
            //SPI1
            chip->os_func = &esp_flash_spi1_default_os_functions;
            break;
        case SPI2_HOST:
#if SOC_SPI_PERIPH_NUM > 2
        case SPI3_HOST:
#endif
            //SPI2, SPI3
            chip->os_func = &esp_flash_spi23_default_os_functions;
            break;
        default:
            return ESP_ERR_INVALID_ARG;
            break;
    }

    *(app_func_arg_t*) chip->os_func_data = (app_func_arg_t) {
        .dev_lock = dev_handle,
        .no_protect = true, // This is OK because this code path isn't used for the main flash chip which requires `no_protect = false`
    };

    return ESP_OK;
}

esp_err_t esp_flash_deinit_os_functions(esp_flash_t* chip, spi_bus_lock_dev_handle_t* out_dev_handle)
{
    if (chip->os_func_data) {
        // SPI bus lock is possibly not used on SPI1 bus
        *out_dev_handle = ((app_func_arg_t*)chip->os_func_data)->dev_lock;
        free(chip->os_func_data);
    }
    chip->os_func = NULL;
    chip->os_func_data = NULL;
    return ESP_OK;
}

esp_err_t esp_flash_init_main_bus_lock(void)
{
    /* The following called functions are only defined if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
     * is set. Thus, we must not call them if the macro is not defined, else the linker
     * would trigger errors. */
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    /* bus_lock is registered by `spi_bus_lock_init_main_bus` constructor in spi_common.c  */
    spi_bus_lock_set_bg_control(g_main_spi_bus_lock, cache_enable, cache_disable, NULL);

    esp_err_t err = spi_bus_lock_init_main_dev();
    if (err != ESP_OK) {
        return err;
    }
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_flash_app_enable_os_functions(esp_flash_t* chip)
{
    main_flash_arg = (app_func_arg_t) {
        .dev_lock = g_spi_lock_main_flash_dev,
        .no_protect = false, // Required for the main flash chip
    };
    chip->os_func = &esp_flash_spi1_default_os_functions;
    chip->os_func_data = &main_flash_arg;
    return ESP_OK;
}

esp_err_t esp_flash_set_dangerous_write_protection(esp_flash_t *chip, const bool protect)
{
#if !CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
    if (chip == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (chip->os_func_data != NULL) {
        ((app_func_arg_t*)chip->os_func_data)->no_protect = !protect;
    }
#else
    (void)chip;
    (void)protect;
#endif
    return ESP_OK;
}

// The goal of this part is to manually insert one valid task execution interval, if the time since
// last valid interval exceed the limitation (CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS).
//
// Valid task execution interval: continuous time with the cache enabled, which is longer than
// CONFIG_SPI_FLASH_ERASE_YIELD_TICKS. Yield time shorter than CONFIG_SPI_FLASH_ERASE_YIELD_TICKS is
// not treated as valid interval.
static inline IRAM_ATTR bool on_spi_check_yield(app_func_arg_t* ctx)
{
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
    uint32_t time = esp_system_get_time();
    // We handle the reset here instead of in `on_spi_acquired()`, when acquire() and release() is
    // larger than CONFIG_SPI_FLASH_ERASE_YIELD_TICKS, to save one `esp_system_get_time()` call
    if ((time - ctx->released_since_us) >= CONFIG_SPI_FLASH_ERASE_YIELD_TICKS * portTICK_PERIOD_MS * 1000) {
        // Reset the acquired time as if the yield has just happened.
        ctx->acquired_since_us = time;
    } else if ((time - ctx->acquired_since_us) >= CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS * 1000) {
        return true;
    }
#endif
    return false;
}
static inline IRAM_ATTR void on_spi_released(app_func_arg_t* ctx)
{
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
    ctx->released_since_us = esp_system_get_time();
#endif
}

static inline IRAM_ATTR void on_spi_acquired(app_func_arg_t* ctx)
{
    // Ideally, when the time after `on_spi_released()` before this function is called is larger
    // than CONFIG_SPI_FLASH_ERASE_YIELD_TICKS, the acquired time should be reset. We assume the
    // time after `on_spi_check_yield()` before this function is so short that we can do the reset
    // in that function instead.
}

static inline IRAM_ATTR void on_spi_yielded(app_func_arg_t* ctx)
{
    uint32_t time = esp_system_get_time();
    ctx->acquired_since_us = time;
}
