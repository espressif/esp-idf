/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#include <esp32/rom/cache.h>
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/cache.h"
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/cache.h"
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#endif
#include "esp_rom_spiflash.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include <soc/soc.h>
#include "sdkconfig.h"
#ifndef CONFIG_FREERTOS_UNICORE
#include "esp_private/esp_ipc.h"
#endif
#include "esp_attr.h"
#include "esp_memory_utils.h"
#include "esp_intr_alloc.h"
#include "spi_flash_mmap.h"
#include "spi_flash_override.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/freertos_idf_additions_priv.h"
#include "esp_log.h"
#include "esp_cpu.h"

static __attribute__((unused)) const char *TAG = "cache";


/**
 * These two shouldn't be declared as static otherwise if `CONFIG_SPI_FLASH_ROM_IMPL` is enabled,
 * they won't get replaced by the rom version
 */
void spi_flash_disable_cache(uint32_t cpuid, uint32_t *saved_state);
void spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state);

// Used only on ROM impl. in idf, this param unused, cache status hold by hal
static uint32_t s_flash_op_cache_state[2];


#ifndef CONFIG_FREERTOS_UNICORE
static SemaphoreHandle_t s_flash_op_mutex;
static volatile bool s_flash_op_can_start = false;
static volatile bool s_flash_op_complete = false;
#ifndef NDEBUG
static volatile int s_flash_op_cpu = -1;
#endif

static inline bool esp_task_stack_is_sane_cache_disabled(void)
{
    const void *sp = (const void *)esp_cpu_get_sp();

    return esp_ptr_in_dram(sp)
#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
        || esp_ptr_in_rtc_dram_fast(sp)
#endif
    ;
}

void spi_flash_init_lock(void)
{
    s_flash_op_mutex = xSemaphoreCreateRecursiveMutex();
    assert(s_flash_op_mutex != NULL);
}

void spi_flash_op_lock(void)
{
    xSemaphoreTakeRecursive(s_flash_op_mutex, portMAX_DELAY);
}

void spi_flash_op_unlock(void)
{
    xSemaphoreGiveRecursive(s_flash_op_mutex);
}
/*
 If you're going to modify this, keep in mind that while the flash caches of the pro and app
 cpu are separate, the psram cache is *not*. If one of the CPUs returns from a flash routine
 with its cache enabled but the other CPUs cache is not enabled yet, you will have problems
 when accessing psram from the former CPU.
*/

void IRAM_ATTR spi_flash_op_block_func(void *arg)
{
    // Disable scheduler on this CPU
#ifdef CONFIG_FREERTOS_SMP
    /*
    Note: FreeRTOS SMP has changed the behavior of scheduler suspension. But the vTaskPreemptionDisable() function should
    achieve the same affect as before (i.e., prevent the current task from being preempted).
    */
    vTaskPreemptionDisable(NULL);
#else
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP
    // Restore interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    uint32_t cpuid = (uint32_t) arg;
    // s_flash_op_complete flag is cleared on *this* CPU, otherwise the other
    // CPU may reset the flag back to false before IPC task has a chance to check it
    // (if it is preempted by an ISR taking non-trivial amount of time)
    s_flash_op_complete = false;
    s_flash_op_can_start = true;
    while (!s_flash_op_complete) {
        // busy loop here and wait for the other CPU to finish flash operation
    }
    // Flash operation is complete, re-enable cache
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    // Restore interrupts that aren't located in IRAM
    esp_intr_noniram_enable();
#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionEnable(NULL);
#else
    // Re-enable scheduler
    xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu(void)
{
    assert(esp_task_stack_is_sane_cache_disabled());

    spi_flash_op_lock();

    int cpuid = xPortGetCoreID();
    uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;
#ifndef NDEBUG
    // For sanity check later: record the CPU which has started doing flash operation
    assert(s_flash_op_cpu == -1);
    s_flash_op_cpu = cpuid;
#endif

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        // Scheduler hasn't been started yet, it means that spi_flash API is being
        // called from the 2nd stage bootloader or from user_start_cpu0, i.e. from
        // PRO CPU. APP CPU is either in reset or spinning inside user_start_cpu1,
        // which is in IRAM. So it is safe to disable cache for the other_cpuid after
        // esp_intr_noniram_disable.
        assert(other_cpuid == 1);
    } else {
        bool ipc_call_was_send_to_other_cpu;
        do {
            #ifdef CONFIG_FREERTOS_SMP
                //Note: Scheduler suspension behavior changed in FreeRTOS SMP
                vTaskPreemptionDisable(NULL);
            #else
                // Disable scheduler on the current CPU
                vTaskSuspendAll();
            #endif // CONFIG_FREERTOS_SMP

            cpuid = xPortGetCoreID();
            other_cpuid = (cpuid == 0) ? 1 : 0;
            #ifndef NDEBUG
                s_flash_op_cpu = cpuid;
            #endif

            s_flash_op_can_start = false;

            ipc_call_was_send_to_other_cpu = esp_ipc_call_nonblocking(other_cpuid, &spi_flash_op_block_func, (void *) other_cpuid) == ESP_OK;

            if (!ipc_call_was_send_to_other_cpu) {
                // IPC call was not send to other cpu because another nonblocking API is running now.
                // Enable the Scheduler again will not help the IPC to speed it up
                // but there is a benefit to schedule to a higher priority task before the nonblocking running IPC call is done.
                #ifdef CONFIG_FREERTOS_SMP
                    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
                    vTaskPreemptionEnable(NULL);
                #else
                    xTaskResumeAll();
                #endif // CONFIG_FREERTOS_SMP
            }
        } while (!ipc_call_was_send_to_other_cpu);

        while (!s_flash_op_can_start) {
            // Busy loop and wait for spi_flash_op_block_func to disable cache
            // on the other CPU
        }
    }

    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // This CPU executes this routine, with non-IRAM interrupts and the scheduler
    // disabled. The other CPU is spinning in the spi_flash_op_block_func task, also
    // with non-iram interrupts and the scheduler disabled. None of these CPUs will
    // touch external RAM or flash this way, so we can safely disable caches.
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
#if SOC_IDCACHE_PER_CORE
    //only needed if cache(s) is per core
    spi_flash_disable_cache(other_cpuid, &s_flash_op_cache_state[other_cpuid]);
#endif
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu(void)
{
    const int cpuid = xPortGetCoreID();

#ifndef NDEBUG
    // Sanity check: flash operation ends on the same CPU as it has started
    assert(cpuid == s_flash_op_cpu);
    // More sanity check: if scheduler isn't started, only CPU0 can call this.
    assert(!(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED && cpuid != 0));
    s_flash_op_cpu = -1;
#endif

    // Re-enable cache. After this, cache (flash and external RAM) should work again.
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
#if SOC_IDCACHE_PER_CORE
    //only needed if cache(s) is per core
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;
    spi_flash_restore_cache(other_cpuid, s_flash_op_cache_state[other_cpuid]);
#endif

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        // Signal to spi_flash_op_block_task that flash operation is complete
        s_flash_op_complete = true;
    }

    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();

    // Resume tasks on the current CPU, if the scheduler has started.
    // NOTE: enabling non-IRAM interrupts has to happen before this,
    // because once the scheduler has started, due to preemption the
    // current task can end up being moved to the other CPU.
    // But esp_intr_noniram_enable has to be called on the same CPU which
    // called esp_intr_noniram_disable
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
#ifdef CONFIG_FREERTOS_SMP
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionEnable(NULL);
#else
        xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP
    }
    // Release API lock
    spi_flash_op_unlock();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void)
{
    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;

    // do not care about other CPU, it was halted upon entering panic handler
    spi_flash_disable_cache(other_cpuid, &s_flash_op_cache_state[other_cpuid]);
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // Disable cache on this CPU as well
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os(void)
{
    const uint32_t cpuid = xPortGetCoreID();

    // Re-enable cache on this CPU
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();
}

#else // CONFIG_FREERTOS_UNICORE

void spi_flash_init_lock(void)
{
}

void spi_flash_op_lock(void)
{
#ifdef CONFIG_FREERTOS_SMP
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionDisable(NULL);
    }
#else
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP
}

void spi_flash_op_unlock(void)
{
#ifdef CONFIG_FREERTOS_SMP
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionEnable(NULL);
    }
#else
    xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP
}


void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu(void)
{
    spi_flash_op_lock();
    esp_intr_noniram_disable();
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu(void)
{
    spi_flash_restore_cache(0, s_flash_op_cache_state[0]);
    esp_intr_noniram_enable();
    spi_flash_op_unlock();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void)
{
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // Disable cache on this CPU as well
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os(void)
{
    // Re-enable cache on this CPU
    spi_flash_restore_cache(0, s_flash_op_cache_state[0]);
    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();
}

#endif // CONFIG_FREERTOS_UNICORE


void IRAM_ATTR spi_flash_enable_cache(uint32_t cpuid)
{
#if CONFIG_IDF_TARGET_ESP32
    uint32_t cache_value = cache_ll_l1_get_enabled_bus(cpuid);

    // Re-enable cache on this CPU
    spi_flash_restore_cache(cpuid, cache_value);
#else
    spi_flash_restore_cache(0, 0); // TODO cache_value should be non-zero
#endif
}

/**
 * The following two functions are replacements for Cache_Read_Disable and Cache_Read_Enable
 * function in ROM. They are used to work around a bug where Cache_Read_Disable requires a call to
 * Cache_Flush before Cache_Read_Enable, even if cached data was not modified.
 */
void IRAM_ATTR spi_flash_disable_cache(uint32_t cpuid, uint32_t *saved_state)
{
    cache_hal_suspend(CACHE_TYPE_ALL);
}

void IRAM_ATTR spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state)
{
    cache_hal_resume(CACHE_TYPE_ALL);
}

bool IRAM_ATTR spi_flash_cache_enabled(void)
{
    return cache_hal_is_cache_enabled(CACHE_TYPE_ALL);
}

#if CONFIG_IDF_TARGET_ESP32S2
IRAM_ATTR void esp_config_instruction_cache_mode(void)
{
    cache_size_t cache_size;
    cache_ways_t cache_ways;
    cache_line_size_t cache_line_size;

#if CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_8KB;
#else
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_16KB;
#endif
    cache_ways = CACHE_4WAYS_ASSOC;
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_16B
    cache_line_size = CACHE_LINE_SIZE_16B;
#else
    cache_line_size = CACHE_LINE_SIZE_32B;
#endif
    ESP_EARLY_LOGI(TAG, "Instruction cache \t: size %dKB, %dWays, cache line size %dByte", cache_size == CACHE_SIZE_8KB ? 8 : 16, 4, cache_line_size == CACHE_LINE_SIZE_16B ? 16 : 32);
    Cache_Suspend_ICache();
    Cache_Set_ICache_Mode(cache_size, cache_ways, cache_line_size);
    Cache_Invalidate_ICache_All();
    Cache_Resume_ICache(0);
}

IRAM_ATTR void esp_config_data_cache_mode(void)
{
#define CACHE_SIZE_0KB  99  //If Cache set to 0 KB, cache is bypassed, the cache size doesn't take into effect. Set this macro to a unique value for log

    cache_size_t cache_size;
    cache_ways_t cache_ways;
    cache_line_size_t cache_line_size;

#if CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
#if CONFIG_ESP32S2_DATA_CACHE_0KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_0KB;
#elif CONFIG_ESP32S2_DATA_CACHE_8KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_8KB;
#else
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_DCACHE_HIGH, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_16KB;
#endif
#else
#if CONFIG_ESP32S2_DATA_CACHE_0KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_0KB;
#elif CONFIG_ESP32S2_DATA_CACHE_8KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_8KB;
#else
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_DCACHE_HIGH);
    cache_size = CACHE_SIZE_16KB;
#endif
#endif

    cache_ways = CACHE_4WAYS_ASSOC;
#if CONFIG_ESP32S2_DATA_CACHE_LINE_16B
    cache_line_size = CACHE_LINE_SIZE_16B;
#else
    cache_line_size = CACHE_LINE_SIZE_32B;
#endif
    ESP_EARLY_LOGI(TAG, "Data cache \t\t: size %dKB, %dWays, cache line size %dByte", (cache_size == CACHE_SIZE_0KB) ? 0 : ((cache_size == CACHE_SIZE_8KB) ? 8 : 16), 4, cache_line_size == CACHE_LINE_SIZE_16B ? 16 : 32);
    Cache_Set_DCache_Mode(cache_size, cache_ways, cache_line_size);
    Cache_Invalidate_DCache_All();
}

static IRAM_ATTR void esp_enable_cache_flash_wrap(bool icache, bool dcache)
{
    uint32_t i_autoload, d_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    if (dcache) {
        d_autoload = Cache_Suspend_DCache();
    }
    REG_SET_BIT(EXTMEM_PRO_CACHE_WRAP_AROUND_CTRL_REG, EXTMEM_PRO_CACHE_FLASH_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
    if (dcache) {
        Cache_Resume_DCache(d_autoload);
    }
}

#if (CONFIG_IDF_TARGET_ESP32S2 && CONFIG_SPIRAM)
static IRAM_ATTR void esp_enable_cache_spiram_wrap(bool icache, bool dcache)
{
    uint32_t i_autoload, d_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    if (dcache) {
        d_autoload = Cache_Suspend_DCache();
    }
    REG_SET_BIT(EXTMEM_PRO_CACHE_WRAP_AROUND_CTRL_REG, EXTMEM_PRO_CACHE_SRAM_RD_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
    if (dcache) {
        Cache_Resume_DCache(d_autoload);
    }
}
#endif

esp_err_t esp_enable_cache_wrap(bool icache_wrap_enable, bool dcache_wrap_enable)
{
    int icache_wrap_size = 0, dcache_wrap_size = 0;
    int flash_wrap_sizes[2] = {-1, -1}, spiram_wrap_sizes[2] = {-1, -1};
    int flash_wrap_size = 0, spiram_wrap_size = 0;
    int flash_count = 0, spiram_count = 0;
    int i;
    bool flash_spiram_wrap_together, flash_support_wrap = true, spiram_support_wrap = true;
    uint32_t drom0_in_icache = 1;//always 1 in esp32s2
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6
    drom0_in_icache = 0;
#endif

    if (icache_wrap_enable) {
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_16B || CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_16B
        icache_wrap_size = FLASH_WRAP_SIZE_16B;
#else
        icache_wrap_size = FLASH_WRAP_SIZE_32B;
#endif
    }
    if (dcache_wrap_enable) {
#if CONFIG_ESP32S2_DATA_CACHE_LINE_16B || CONFIG_ESP32S3_DATA_CACHE_LINE_16B
        dcache_wrap_size = FLASH_WRAP_SIZE_16B;
#else
        dcache_wrap_size = FLASH_WRAP_SIZE_32B;
#endif
    }

    uint32_t instruction_use_spiram = 0;
    uint32_t rodata_use_spiram = 0;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    extern uint32_t esp_spiram_instruction_access_enabled(void);
    instruction_use_spiram = esp_spiram_instruction_access_enabled();
#endif
#if CONFIG_SPIRAM_RODATA
    extern uint32_t esp_spiram_rodata_access_enabled(void);
    rodata_use_spiram = esp_spiram_rodata_access_enabled();
#endif

    if (instruction_use_spiram) {
        spiram_wrap_sizes[0] = icache_wrap_size;
    } else {
        flash_wrap_sizes[0] = icache_wrap_size;
    }
    if (rodata_use_spiram) {
        if (drom0_in_icache) {
            spiram_wrap_sizes[0] = icache_wrap_size;
        } else {
            spiram_wrap_sizes[1] = dcache_wrap_size;
            flash_wrap_sizes[1] = dcache_wrap_size;
        }
    } else {
        if (drom0_in_icache) {
            flash_wrap_sizes[0] = icache_wrap_size;
        } else {
            flash_wrap_sizes[1] = dcache_wrap_size;
        }
    }
#if (CONFIG_IDF_TARGET_ESP32S2 && CONFIG_SPIRAM)
    spiram_wrap_sizes[1] = dcache_wrap_size;
#endif
    for (i = 0; i < 2; i++) {
        if (flash_wrap_sizes[i] != -1) {
            flash_count++;
            flash_wrap_size = flash_wrap_sizes[i];
        }
    }
    for (i = 0; i < 2; i++) {
        if (spiram_wrap_sizes[i] != -1) {
            spiram_count++;
            spiram_wrap_size = spiram_wrap_sizes[i];
        }
    }
    if (flash_count + spiram_count <= 2) {
        flash_spiram_wrap_together = false;
    } else {
        flash_spiram_wrap_together = true;
    }
    ESP_EARLY_LOGI(TAG, "flash_count=%d, size=%d, spiram_count=%d, size=%d,together=%d", flash_count, flash_wrap_size, spiram_count, spiram_wrap_size, flash_spiram_wrap_together);
    if (flash_count > 1 && flash_wrap_sizes[0] != flash_wrap_sizes[1]) {
        ESP_EARLY_LOGW(TAG, "Flash wrap with different length %d and %d, abort wrap.", flash_wrap_sizes[0], flash_wrap_sizes[1]);
        if (spiram_wrap_size == 0) {
            return ESP_FAIL;
        }
        if (flash_spiram_wrap_together) {
            ESP_EARLY_LOGE(TAG, "Abort spiram wrap because flash wrap length not fixed.");
            return ESP_FAIL;
        }
    }
    if (spiram_count > 1 && spiram_wrap_sizes[0] != spiram_wrap_sizes[1]) {
        ESP_EARLY_LOGW(TAG, "SPIRAM wrap with different length %d and %d, abort wrap.", spiram_wrap_sizes[0], spiram_wrap_sizes[1]);
        if (flash_wrap_size == 0) {
            return ESP_FAIL;
        }
        if (flash_spiram_wrap_together) {
            ESP_EARLY_LOGW(TAG, "Abort flash wrap because spiram wrap length not fixed.");
            return ESP_FAIL;
        }
    }

    if (flash_spiram_wrap_together && flash_wrap_size != spiram_wrap_size) {
        ESP_EARLY_LOGW(TAG, "SPIRAM has different wrap length with flash, %d and %d, abort wrap.", spiram_wrap_size, flash_wrap_size);
        return ESP_FAIL;
    }

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_QIO
    flash_support_wrap = true;
    spi_flash_wrap_probe();
    if (!spi_flash_support_wrap_size(flash_wrap_size)) {
        flash_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "Flash do not support wrap size %d.", flash_wrap_size);
    }
#else
    ESP_EARLY_LOGW(TAG, "Flash is not in QIO mode, do not support wrap.");
#endif

#if (CONFIG_IDF_TARGET_ESP32S2 && CONFIG_SPIRAM)
    extern bool psram_support_wrap_size(uint32_t wrap_size);
    if (!psram_support_wrap_size(spiram_wrap_size)) {
        spiram_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "SPIRAM do not support wrap size %d.", spiram_wrap_size);
    }
#endif

    if (flash_spiram_wrap_together && !(flash_support_wrap && spiram_support_wrap)) {
        ESP_EARLY_LOGW(TAG, "Flash and SPIRAM should support wrap together.");
        return ESP_FAIL;
    }

    if (flash_support_wrap && flash_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "Flash wrap enabled, size = %d.", flash_wrap_size);
        spi_flash_wrap_enable(flash_wrap_size);
        esp_enable_cache_flash_wrap((flash_wrap_sizes[0] > 0), (flash_wrap_sizes[1] > 0));
    }
#if (CONFIG_IDF_TARGET_ESP32S2 && CONFIG_SPIRAM)
    extern esp_err_t psram_enable_wrap(uint32_t wrap_size);
    if (spiram_support_wrap && spiram_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "SPIRAM wrap enabled, size = %d.", spiram_wrap_size);
        psram_enable_wrap(spiram_wrap_size);
        esp_enable_cache_spiram_wrap((spiram_wrap_sizes[0] > 0), (spiram_wrap_sizes[1] > 0));
    }
#endif

    return ESP_OK;

}
#endif
#if CONFIG_IDF_TARGET_ESP32S3
IRAM_ATTR void esp_config_instruction_cache_mode(void)
{
    cache_size_t cache_size;
    cache_ways_t cache_ways;
    cache_line_size_t cache_line_size;

#if CONFIG_ESP32S3_INSTRUCTION_CACHE_16KB
    Cache_Occupy_ICache_MEMORY(CACHE_MEMORY_IBANK0, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_HALF;
#else
    Cache_Occupy_ICache_MEMORY(CACHE_MEMORY_IBANK0, CACHE_MEMORY_IBANK1);
    cache_size = CACHE_SIZE_FULL;
#endif
#if CONFIG_ESP32S3_INSTRUCTION_CACHE_4WAYS
    cache_ways = CACHE_4WAYS_ASSOC;
#else
    cache_ways = CACHE_8WAYS_ASSOC;
#endif
#if CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_16B
    cache_line_size = CACHE_LINE_SIZE_16B;
#elif CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_32B
    cache_line_size = CACHE_LINE_SIZE_32B;
#else
    cache_line_size = CACHE_LINE_SIZE_64B;
#endif
    ESP_EARLY_LOGI(TAG, "Instruction cache: size %dKB, %dWays, cache line size %dByte", cache_size == CACHE_SIZE_HALF ? 16 : 32, cache_ways == CACHE_4WAYS_ASSOC ? 4 : 8, cache_line_size == CACHE_LINE_SIZE_16B ? 16 : (cache_line_size == CACHE_LINE_SIZE_32B ? 32 : 64));
    Cache_Set_ICache_Mode(cache_size, cache_ways, cache_line_size);
    Cache_Invalidate_ICache_All();
    extern void Cache_Enable_ICache(uint32_t autoload);
    Cache_Enable_ICache(0);
}

IRAM_ATTR void esp_config_data_cache_mode(void)
{
    cache_size_t cache_size;
    cache_ways_t cache_ways;
    cache_line_size_t cache_line_size;

#if CONFIG_ESP32S3_DATA_CACHE_32KB
    Cache_Occupy_DCache_MEMORY(CACHE_MEMORY_DBANK1, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_HALF;
#else
    Cache_Occupy_DCache_MEMORY(CACHE_MEMORY_DBANK0, CACHE_MEMORY_DBANK1);
    cache_size = CACHE_SIZE_FULL;
#endif
#if CONFIG_ESP32S3_DATA_CACHE_4WAYS
    cache_ways = CACHE_4WAYS_ASSOC;
#else
    cache_ways = CACHE_8WAYS_ASSOC;
#endif
#if CONFIG_ESP32S3_DATA_CACHE_LINE_16B
    cache_line_size = CACHE_LINE_SIZE_16B;
#elif CONFIG_ESP32S3_DATA_CACHE_LINE_32B
    cache_line_size = CACHE_LINE_SIZE_32B;
#else
    cache_line_size = CACHE_LINE_SIZE_64B;
#endif
    // ESP_EARLY_LOGI(TAG, "Data cache: size %dKB, %dWays, cache line size %dByte", cache_size == CACHE_SIZE_HALF ? 32 : 64, cache_ways == CACHE_4WAYS_ASSOC ? 4 : 8, cache_line_size == CACHE_LINE_SIZE_16B ? 16 : (cache_line_size == CACHE_LINE_SIZE_32B ? 32 : 64));
    Cache_Set_DCache_Mode(cache_size, cache_ways, cache_line_size);
    Cache_Invalidate_DCache_All();
}

static IRAM_ATTR void esp_enable_cache_flash_wrap(bool icache, bool dcache)
{
    uint32_t i_autoload, d_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    if (dcache) {
        d_autoload = Cache_Suspend_DCache();
    }
    REG_SET_BIT(EXTMEM_CACHE_WRAP_AROUND_CTRL_REG, EXTMEM_CACHE_FLASH_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
    if (dcache) {
        Cache_Resume_DCache(d_autoload);
    }
}

#if (CONFIG_IDF_TARGET_ESP32S3 && CONFIG_SPIRAM)
static IRAM_ATTR void esp_enable_cache_spiram_wrap(bool icache, bool dcache)
{
    uint32_t i_autoload, d_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    if (dcache) {
        d_autoload = Cache_Suspend_DCache();
    }
    REG_SET_BIT(EXTMEM_CACHE_WRAP_AROUND_CTRL_REG, EXTMEM_CACHE_SRAM_RD_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
    if (dcache) {
        Cache_Resume_DCache(d_autoload);
    }
}
#endif

esp_err_t esp_enable_cache_wrap(bool icache_wrap_enable, bool dcache_wrap_enable)
{
    int icache_wrap_size = 0, dcache_wrap_size = 0;
    int flash_wrap_sizes[2] = {-1, -1}, spiram_wrap_sizes[2] = {-1, -1};
    int flash_wrap_size = 0, spiram_wrap_size = 0;
    int flash_count = 0, spiram_count = 0;
    int i;
    bool flash_spiram_wrap_together, flash_support_wrap = false, spiram_support_wrap = true;
    uint32_t drom0_in_icache = 0;//always 0 in chip7.2.4

    if (icache_wrap_enable) {
#if CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_16B
        icache_wrap_size = FLASH_WRAP_SIZE_16B;
#elif CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_32B
        icache_wrap_size = FLASH_WRAP_SIZE_32B;
#else
        icache_wrap_size = FLASH_WRAP_SIZE_64B;
#endif
    }
    if (dcache_wrap_enable) {
#if CONFIG_ESP32S3_DATA_CACHE_LINE_16B
        dcache_wrap_size = FLASH_WRAP_SIZE_16B;
#elif CONFIG_ESP32S3_DATA_CACHE_LINE_32B
        dcache_wrap_size = FLASH_WRAP_SIZE_32B;
#else
        dcache_wrap_size = FLASH_WRAP_SIZE_64B;
#endif
    }

    uint32_t instruction_use_spiram = 0;
    uint32_t rodata_use_spiram = 0;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    extern uint32_t esp_spiram_instruction_access_enabled(void);
    instruction_use_spiram = esp_spiram_instruction_access_enabled();
#endif
#if CONFIG_SPIRAM_RODATA
    extern uint32_t esp_spiram_rodata_access_enabled(void);
    rodata_use_spiram = esp_spiram_rodata_access_enabled();
#endif

    if (instruction_use_spiram) {
        spiram_wrap_sizes[0] = icache_wrap_size;
    } else {
        flash_wrap_sizes[0] = icache_wrap_size;
    }
    if (rodata_use_spiram) {
        if (drom0_in_icache) {
            spiram_wrap_sizes[0] = icache_wrap_size;
        } else {
            spiram_wrap_sizes[1] = dcache_wrap_size;
        }
    } else {
        if (drom0_in_icache) {
            flash_wrap_sizes[0] = icache_wrap_size;
        } else {
            flash_wrap_sizes[1] = dcache_wrap_size;
        }
    }
#if (CONFIG_IDF_TARGET_ESP32S3 && CONFIG_SPIRAM)
    spiram_wrap_sizes[1] = dcache_wrap_size;
#endif
    for (i = 0; i < 2; i++) {
        if (flash_wrap_sizes[i] != -1) {
            flash_count++;
            flash_wrap_size = flash_wrap_sizes[i];
        }
    }
    for (i = 0; i < 2; i++) {
        if (spiram_wrap_sizes[i] != -1) {
            spiram_count++;
            spiram_wrap_size = spiram_wrap_sizes[i];
        }
    }
    if (flash_count + spiram_count <= 2) {
        flash_spiram_wrap_together = false;
    } else {
        flash_spiram_wrap_together = true;
    }
    if (flash_count > 1 && flash_wrap_sizes[0] != flash_wrap_sizes[1]) {
        ESP_EARLY_LOGW(TAG, "Flash wrap with different length %d and %d, abort wrap.", flash_wrap_sizes[0], flash_wrap_sizes[1]);
        if (spiram_wrap_size == 0) {
            return ESP_FAIL;
        }
        if (flash_spiram_wrap_together) {
            ESP_EARLY_LOGE(TAG, "Abort spiram wrap because flash wrap length not fixed.");
            return ESP_FAIL;
        }
    }
    if (spiram_count > 1 && spiram_wrap_sizes[0] != spiram_wrap_sizes[1]) {
        ESP_EARLY_LOGW(TAG, "SPIRAM wrap with different length %d and %d, abort wrap.", spiram_wrap_sizes[0], spiram_wrap_sizes[1]);
        if (flash_wrap_size == 0) {
            return ESP_FAIL;
        }
        if (flash_spiram_wrap_together) {
            ESP_EARLY_LOGW(TAG, "Abort flash wrap because spiram wrap length not fixed.");
            return ESP_FAIL;
        }
    }

    if (flash_spiram_wrap_together && flash_wrap_size != spiram_wrap_size) {
        ESP_EARLY_LOGW(TAG, "SPIRAM has different wrap length with flash, %d and %d, abort wrap.", spiram_wrap_size, flash_wrap_size);
        return ESP_FAIL;
    }

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_QIO
    flash_support_wrap = true;
    spi_flash_wrap_probe();
    if (!spi_flash_support_wrap_size(flash_wrap_size)) {
        flash_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "Flash do not support wrap size %d.", flash_wrap_size);
    }
#else
    ESP_EARLY_LOGW(TAG, "Flash is not in QIO mode, do not support wrap.");
#endif


#if (CONFIG_IDF_TARGET_ESP32S3 && CONFIG_SPIRAM)
    extern bool psram_support_wrap_size(uint32_t wrap_size);
    if (!psram_support_wrap_size(spiram_wrap_size)) {
        spiram_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "SPIRAM do not support wrap size %d.", spiram_wrap_size);
    }
#endif

    if (flash_spiram_wrap_together && !(flash_support_wrap && spiram_support_wrap)) {
        ESP_EARLY_LOGW(TAG, "Flash and SPIRAM should support wrap together.");
        return ESP_FAIL;
    }

    if (flash_support_wrap && flash_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "Flash wrap enabled, size = %d.", flash_wrap_size);
        spi_flash_wrap_enable(flash_wrap_size);
        esp_enable_cache_flash_wrap((flash_wrap_sizes[0] > 0), (flash_wrap_sizes[1] > 0));
    }
#if (CONFIG_IDF_TARGET_ESP32S3 && CONFIG_SPIRAM)
    extern esp_err_t psram_enable_wrap(uint32_t wrap_size);
    if (spiram_support_wrap && spiram_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "SPIRAM wrap enabled, size = %d.", spiram_wrap_size);
        psram_enable_wrap(spiram_wrap_size);
        esp_enable_cache_spiram_wrap((spiram_wrap_sizes[0] > 0), (spiram_wrap_sizes[1] > 0));
    }
#endif

    return ESP_OK;

}
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2

static IRAM_ATTR void esp_enable_cache_flash_wrap(bool icache)
{
    uint32_t i_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    REG_SET_BIT(EXTMEM_CACHE_WRAP_AROUND_CTRL_REG, EXTMEM_CACHE_FLASH_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
}

esp_err_t esp_enable_cache_wrap(bool icache_wrap_enable)
{
    int flash_wrap_size = 0;
    bool flash_support_wrap = false;

    if (icache_wrap_enable) {
        flash_wrap_size = 32;
    }

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_QIO
    flash_support_wrap = true;
    spi_flash_wrap_probe();
    if (!spi_flash_support_wrap_size(flash_wrap_size)) {
        flash_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "Flash do not support wrap size %d.", flash_wrap_size);
    }
#else
    ESP_EARLY_LOGW(TAG, "Flash is not in QIO mode, do not support wrap.");
#endif // CONFIG_ESPTOOLPY_FLASHMODE_QIO

    if (flash_support_wrap && flash_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "Flash wrap enabled, size = %d.", flash_wrap_size);
        spi_flash_wrap_enable(flash_wrap_size);
        esp_enable_cache_flash_wrap((flash_wrap_size > 0));
    }
    return ESP_OK;
}
#endif // CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
