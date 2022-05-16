/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/param.h>  // For MIN/MAX(a, b)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <soc/soc.h>
#include <soc/soc_memory_layout.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_private/system_internal.h"
#include "esp_private/spi_flash_os.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#include "esp32/rom/spi_flash.h"
#include "esp32/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/spi_flash.h"
#include "esp32s2/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "soc/spi_mem_reg.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/clk.h"
#include "esp32s3/clk.h"
#include "esp32s3/opi_flash_private.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#include "esp32c3/rom/spi_flash.h"
#include "esp32c3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/cache.h"
#include "esp32h2/rom/spi_flash.h"
#include "esp32h2/clk.h"
#endif
#include "esp_flash_partitions.h"
#include "cache_utils.h"
#include "esp_flash.h"
#include "esp_attr.h"
#include "bootloader_flash.h"
#include "esp_compiler.h"

esp_rom_spiflash_result_t IRAM_ATTR spi_flash_write_encrypted_chip(size_t dest_addr, const void *src, size_t size);

/* bytes erased by SPIEraseBlock() ROM function */
#define BLOCK_ERASE_SIZE 65536

/* Limit number of bytes written/read in a single SPI operation,
   as these operations disable all higher priority tasks from running.
*/
#ifdef CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE
#define MAX_WRITE_CHUNK CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE
#else
#define MAX_WRITE_CHUNK 8192
#endif // CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE

#define MAX_READ_CHUNK 16384

static const char *TAG __attribute__((unused)) = "spi_flash";

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
static spi_flash_counters_t s_flash_stats;

#define COUNTER_START()     uint32_t ts_begin = cpu_hal_get_cycle_count()
#define COUNTER_STOP(counter)  \
    do{ \
        s_flash_stats.counter.count++; \
        s_flash_stats.counter.time += (cpu_hal_get_cycle_count() - ts_begin) / (esp_clk_cpu_freq() / 1000000); \
    } while(0)

#define COUNTER_ADD_BYTES(counter, size) \
    do { \
        s_flash_stats.counter.bytes += size; \
    } while (0)

#else
#define COUNTER_START()
#define COUNTER_STOP(counter)
#define COUNTER_ADD_BYTES(counter, size)

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

#if CONFIG_SPI_FLASH_USE_LEGACY_IMPL
static esp_err_t spi_flash_translate_rc(esp_rom_spiflash_result_t rc);
#endif //CONFIG_SPI_FLASH_USE_LEGACY_IMPL
static bool is_safe_write_address(size_t addr, size_t size);
static void spi_flash_os_yield(void);

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_default_ops = {
    .start                  = spi_flash_disable_interrupts_caches_and_other_cpu,
    .end                    = spi_flash_enable_interrupts_caches_and_other_cpu,
    .op_lock                = spi_flash_op_lock,
    .op_unlock              = spi_flash_op_unlock,
#if !CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
    .is_safe_write_address  = is_safe_write_address,
#endif
    .yield                  = spi_flash_os_yield,
};

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_no_os_ops = {
    .start                  = spi_flash_disable_interrupts_caches_and_other_cpu_no_os,
    .end                    = spi_flash_enable_interrupts_caches_no_os,
    .op_lock                = NULL,
    .op_unlock              = NULL,
#if !CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
    .is_safe_write_address  = NULL,
#endif
    .yield                  = NULL,
};

#ifdef CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS
#define UNSAFE_WRITE_ADDRESS abort()
#else
#define UNSAFE_WRITE_ADDRESS return false
#endif


/* CHECK_WRITE_ADDRESS macro to fail writes which land in the
   bootloader, partition table, or running application region.
*/
#if CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
#define CHECK_WRITE_ADDRESS(ADDR, SIZE)
#else /* FAILS or ABORTS */
#define CHECK_WRITE_ADDRESS(ADDR, SIZE) do {                            \
        if (guard && guard->is_safe_write_address && !guard->is_safe_write_address(ADDR, SIZE)) {                       \
            return ESP_ERR_INVALID_ARG;                                 \
        }                                                               \
    } while(0)
#endif // CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED

static __attribute__((unused)) bool is_safe_write_address(size_t addr, size_t size)
{
    if (!esp_partition_main_flash_region_safe(addr, size)) {
        UNSAFE_WRITE_ADDRESS;
    }
    return true;
}

#if CONFIG_SPI_FLASH_ROM_IMPL
#include "esp_heap_caps.h"
typedef void *(*malloc_internal_cb_t)(size_t size);

void IRAM_ATTR *spi_flash_malloc_internal(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
}
#endif

void IRAM_ATTR esp_mspi_pin_init(void)
{
#if CONFIG_ESPTOOLPY_OCT_FLASH || CONFIG_SPIRAM_MODE_OCT
    esp_rom_opiflash_pin_config();
    extern void spi_timing_set_pin_drive_strength(void);
    spi_timing_set_pin_drive_strength();
#else
    //Set F4R4 board pin drive strength. TODO: IDF-3663
#endif
}

esp_err_t IRAM_ATTR spi_flash_init_chip_state(void)
{
#if CONFIG_ESPTOOLPY_OCT_FLASH
    return esp_opiflash_init(rom_spiflash_legacy_data->chip.device_id);
#else
    //currently we don't need other setup for initialising Quad Flash
    return ESP_OK;
#endif
}

void spi_flash_init(void)
{
    spi_flash_init_lock();
#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    spi_flash_reset_counters();
#endif

#if CONFIG_SPI_FLASH_ROM_IMPL
    spi_flash_guard_set(&g_flash_guard_default_ops);

    /* These two functions are in ROM only */
    extern void spi_flash_mmap_os_func_set(void *(*func1)(size_t size), void (*func2)(void *p));
    spi_flash_mmap_os_func_set(spi_flash_malloc_internal, heap_caps_free);

    extern esp_err_t spi_flash_mmap_page_num_init(uint32_t page_num);
    spi_flash_mmap_page_num_init(128);
#endif
}

#if !CONFIG_SPI_FLASH_ROM_IMPL

static const spi_flash_guard_funcs_t *s_flash_guard_ops;

void IRAM_ATTR spi_flash_guard_set(const spi_flash_guard_funcs_t *funcs)
{
    s_flash_guard_ops = funcs;
}

const spi_flash_guard_funcs_t *IRAM_ATTR spi_flash_guard_get(void)
{
    return s_flash_guard_ops;
}

#endif

size_t IRAM_ATTR spi_flash_get_chip_size(void)
{
    return g_rom_flashchip.chip_size;
}

static inline void IRAM_ATTR spi_flash_guard_start(void)
{
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    if (guard && guard->start) {
        guard->start();
    }
}

static inline void IRAM_ATTR spi_flash_guard_end(void)
{
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    if (guard && guard->end) {
        guard->end();
    }
}

static inline void IRAM_ATTR spi_flash_guard_op_lock(void)
{
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    if (guard && guard->op_lock) {
        guard->op_lock();
    }
}

static inline void IRAM_ATTR spi_flash_guard_op_unlock(void)
{
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    if (guard && guard->op_unlock) {
        guard->op_unlock();
    }
}

static void IRAM_ATTR spi_flash_os_yield(void)
{
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
    if (likely(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)) {
        vTaskDelay(CONFIG_SPI_FLASH_ERASE_YIELD_TICKS);
    }
#endif
}

#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
static esp_rom_spiflash_result_t IRAM_ATTR spi_flash_unlock(void)
{
    static bool unlocked = false;
    if (!unlocked) {
        spi_flash_guard_start();
        bootloader_flash_unlock();
        spi_flash_guard_end();
        unlocked = true;
    }
    return ESP_ROM_SPIFLASH_RESULT_OK;
}
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL

esp_err_t IRAM_ATTR spi_flash_erase_sector(size_t sec)
{
    const spi_flash_guard_funcs_t *guard = spi_flash_guard_get();
    CHECK_WRITE_ADDRESS(sec * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
    return spi_flash_erase_range(sec * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
}

#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
//deprecated, only used in compatible mode
esp_err_t IRAM_ATTR spi_flash_erase_range(size_t start_addr, size_t size)
{
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    CHECK_WRITE_ADDRESS(start_addr, size);
    if (start_addr % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size + start_addr > spi_flash_get_chip_size()) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t start = start_addr / SPI_FLASH_SEC_SIZE;
    size_t end = start + size / SPI_FLASH_SEC_SIZE;
    const size_t sectors_per_block = BLOCK_ERASE_SIZE / SPI_FLASH_SEC_SIZE;
    COUNTER_START();
    esp_rom_spiflash_result_t rc;
    rc = spi_flash_unlock();
    if (rc == ESP_ROM_SPIFLASH_RESULT_OK) {
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
        int64_t no_yield_time_us = 0;
#endif
        for (size_t sector = start; sector != end && rc == ESP_ROM_SPIFLASH_RESULT_OK; ) {
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
            int64_t start_time_us = esp_system_get_time();
#endif
            spi_flash_guard_start();
#ifndef CONFIG_SPI_FLASH_BYPASS_BLOCK_ERASE
            if (sector % sectors_per_block == 0 && end - sector >= sectors_per_block) {
                rc = esp_rom_spiflash_erase_block(sector / sectors_per_block);
                sector += sectors_per_block;
                COUNTER_ADD_BYTES(erase, sectors_per_block * SPI_FLASH_SEC_SIZE);
            } else
#endif
            {
                rc = esp_rom_spiflash_erase_sector(sector);
                ++sector;
                COUNTER_ADD_BYTES(erase, SPI_FLASH_SEC_SIZE);
            }
            spi_flash_guard_end();
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
            no_yield_time_us += (esp_system_get_time() - start_time_us);
            if (no_yield_time_us / 1000 >= CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS) {
                no_yield_time_us = 0;
                if (s_flash_guard_ops && s_flash_guard_ops->yield) {
                    s_flash_guard_ops->yield();
                }
            }
#endif
        }
    }
    COUNTER_STOP(erase);

    spi_flash_guard_start();
    // Ensure WEL is 0 after the operation, even if the erase failed.
    esp_rom_spiflash_write_disable();
    spi_flash_check_and_flush_cache(start_addr, size);
    spi_flash_guard_end();

    return spi_flash_translate_rc(rc);
}

/* Wrapper around esp_rom_spiflash_write() that verifies data as written if CONFIG_SPI_FLASH_VERIFY_WRITE is set.

   If CONFIG_SPI_FLASH_VERIFY_WRITE is not set, this is esp_rom_spiflash_write().
*/
static IRAM_ATTR esp_rom_spiflash_result_t spi_flash_write_inner(uint32_t target, const uint32_t *src_addr, int32_t len)
{
#ifndef CONFIG_SPI_FLASH_VERIFY_WRITE
    return esp_rom_spiflash_write(target, src_addr, len);
#else // CONFIG_SPI_FLASH_VERIFY_WRITE
    esp_rom_spiflash_result_t res = ESP_ROM_SPIFLASH_RESULT_OK;
    assert(len % sizeof(uint32_t) == 0);

    uint32_t before_buf[ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM / sizeof(uint32_t)];
    uint32_t after_buf[ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM / sizeof(uint32_t)];
    uint32_t *expected_buf = before_buf;
    int32_t remaining = len;
    for(int i = 0; i < len; i += sizeof(before_buf)) {
        int i_w = i / sizeof(uint32_t); // index in words (i is an index in bytes)
        int32_t read_len = MIN(sizeof(before_buf), remaining);

        // Read "before" contents from flash
        res = esp_rom_spiflash_read(target + i, before_buf, read_len);
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

        for (int r = 0; r < read_len; r += sizeof(uint32_t)) {
            int r_w = r / sizeof(uint32_t); // index in words (r is index in bytes)

            uint32_t write = src_addr[i_w + r_w];
            uint32_t before = before_buf[r_w];
            uint32_t expected = write & before;
#ifdef CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE
            if ((before & write) != write) {
                spi_flash_guard_end();
                ESP_LOGW(TAG, "Write at offset 0x%x requests 0x%08x but will write 0x%08x -> 0x%08x",
                         target + i + r, write, before, before & write);
                spi_flash_guard_start();
            }
#endif
            expected_buf[r_w] = expected;
        }

        res = esp_rom_spiflash_write(target + i, &src_addr[i_w], read_len);
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

        res = esp_rom_spiflash_read(target + i, after_buf, read_len);
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

        for (int r = 0; r < read_len; r += sizeof(uint32_t)) {
            int r_w = r / sizeof(uint32_t); // index in words (r is index in bytes)

            uint32_t expected = expected_buf[r_w];
            uint32_t actual = after_buf[r_w];
            if (expected != actual) {
#ifdef CONFIG_SPI_FLASH_LOG_FAILED_WRITE
                spi_flash_guard_end();
                ESP_LOGE(TAG, "Bad write at offset 0x%x expected 0x%08x readback 0x%08x", target + i + r, expected, actual);
                spi_flash_guard_start();
#endif
                res = ESP_ROM_SPIFLASH_RESULT_ERR;
            }
        }
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }
        remaining -= read_len;
    }
    return res;
#endif // CONFIG_SPI_FLASH_VERIFY_WRITE
}


esp_err_t IRAM_ATTR spi_flash_write(size_t dst, const void *srcv, size_t size)
{
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    CHECK_WRITE_ADDRESS(dst, size);
    // Out of bound writes are checked in ROM code, but we can give better
    // error code here
    if (dst + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;
    COUNTER_START();
    const uint8_t *srcc = (const uint8_t *) srcv;
    /*
     * Large operations are split into (up to) 3 parts:
     * - Left padding: 4 bytes up to the first 4-byte aligned destination offset.
     * - Middle part
     * - Right padding: 4 bytes from the last 4-byte aligned offset covered.
     */
    size_t left_off = dst & ~3U;
    size_t left_size = MIN(((dst + 3) & ~3U) - dst, size);
    size_t mid_off = left_size;
    size_t mid_size = (size - left_size) & ~3U;
    size_t right_off = left_size + mid_size;
    size_t right_size = size - mid_size - left_size;

    rc = spi_flash_unlock();
    if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
        goto out;
    }
    if (left_size > 0) {
        uint32_t t = 0xffffffff;
        memcpy(((uint8_t *) &t) + (dst - left_off), srcc, left_size);
        spi_flash_guard_start();
        rc = spi_flash_write_inner(left_off, &t, 4);
        spi_flash_guard_end();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(write, 4);
    }
    if (mid_size > 0) {
        /* If src buffer is 4-byte aligned as well and is not in a region that requires cache access to be enabled, we
         * can write directly without buffering in RAM. */
#ifdef ESP_PLATFORM
        bool direct_write = esp_ptr_internal(srcc)
                && esp_ptr_byte_accessible(srcc)
                && ((uintptr_t) srcc + mid_off) % 4 == 0;
#else
        bool direct_write = true;
#endif
        while(mid_size > 0 && rc == ESP_ROM_SPIFLASH_RESULT_OK) {
            uint32_t write_buf[8];
            uint32_t write_size = MIN(mid_size, MAX_WRITE_CHUNK);
            const uint8_t *write_src = srcc + mid_off;
            if (!direct_write) {
                write_size = MIN(write_size, sizeof(write_buf));
                memcpy(write_buf, write_src, write_size);
                write_src = (const uint8_t *)write_buf;
            }
            spi_flash_guard_start();
            rc = spi_flash_write_inner(dst + mid_off, (const uint32_t *) write_src, write_size);
            spi_flash_guard_end();
            COUNTER_ADD_BYTES(write, write_size);
            mid_size -= write_size;
            mid_off += write_size;
        }
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
    }

    if (right_size > 0) {
        uint32_t t = 0xffffffff;
        memcpy(&t, srcc + right_off, right_size);
        spi_flash_guard_start();
        rc = spi_flash_write_inner(dst + right_off, &t, 4);
        spi_flash_guard_end();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(write, 4);
    }
out:
    COUNTER_STOP(write);

    spi_flash_guard_start();
    // Ensure WEL is 0 after the operation, even if the write failed.
    esp_rom_spiflash_write_disable();
    spi_flash_check_and_flush_cache(dst, size);
    spi_flash_guard_end();

    return spi_flash_translate_rc(rc);
}
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL

#if !CONFIG_SPI_FLASH_USE_LEGACY_IMPL
#if !CONFIG_ESPTOOLPY_OCT_FLASH // Test for encryption on opi flash, IDF-3852.
extern void spi_common_set_dummy_output(esp_rom_spiflash_read_mode_t mode);
extern void spi_dummy_len_fix(uint8_t spi, uint8_t freqdiv);
void IRAM_ATTR flash_rom_init(void)
{
    uint32_t freqdiv = 0;

#if CONFIG_IDF_TARGET_ESP32
    uint32_t dummy_bit = 0;
#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_80M;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_40M;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_26M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_26M;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_20M;
#endif
#endif//CONFIG_IDF_TARGET_ESP32

#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
    freqdiv = 1;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    freqdiv = 2;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_26M
    freqdiv = 3;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
    freqdiv = 4;
#endif

#if !CONFIG_IDF_TARGET_ESP32S2 && !CONFIG_IDF_TARGET_ESP32
    esp_rom_spiflash_read_mode_t read_mode;
#if CONFIG_ESPTOOLPY_FLASHMODE_QIO
    read_mode = ESP_ROM_SPIFLASH_QIO_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    read_mode = ESP_ROM_SPIFLASH_QOUT_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_DIO
    read_mode = ESP_ROM_SPIFLASH_DIO_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_DOUT
    read_mode = ESP_ROM_SPIFLASH_DOUT_MODE;
#endif
#endif //!CONFIG_IDF_TARGET_ESP32S2 && !CONFIG_IDF_TARGET_ESP32

#if CONFIG_IDF_TARGET_ESP32
    g_rom_spiflash_dummy_len_plus[1] = dummy_bit;
#else
    spi_dummy_len_fix(1, freqdiv);
#endif //CONFIG_IDF_TARGET_ESP32

#if !CONFIG_IDF_TARGET_ESP32S2 && !CONFIG_IDF_TARGET_ESP32
    spi_common_set_dummy_output(read_mode);
#endif //!CONFIG_IDF_TARGET_ESP32S2
    esp_rom_spiflash_config_clk(freqdiv, 1);
}
#endif //CONFIG_ESPTOOLPY_OCT_FLASH
#else
void IRAM_ATTR flash_rom_init(void)
{
    return;
}

esp_err_t IRAM_ATTR spi_flash_write_encrypted(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t err = ESP_OK;
    const spi_flash_guard_funcs_t *guard =  spi_flash_guard_get();
    CHECK_WRITE_ADDRESS(dest_addr, size);
    if ((dest_addr % 16) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((size % 16) != 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    COUNTER_START();
    esp_rom_spiflash_result_t rc = spi_flash_unlock();
    err = spi_flash_translate_rc(rc);
    if (err != ESP_OK) {
        goto fail;
    }

#ifndef CONFIG_SPI_FLASH_VERIFY_WRITE
    err = spi_flash_write_encrypted_chip(dest_addr, src, size);
    COUNTER_ADD_BYTES(write, size);
    spi_flash_guard_start();
    esp_rom_spiflash_write_disable();
    spi_flash_check_and_flush_cache(dest_addr, size);
    spi_flash_guard_end();
#else
    const uint32_t* src_w = (const uint32_t*)src;
    uint32_t read_buf[ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM / sizeof(uint32_t)];
    int32_t remaining = size;
    for(int i = 0; i < size; i += sizeof(read_buf)) {
        int i_w = i / sizeof(uint32_t); // index in words (i is an index in bytes)
        int32_t read_len = MIN(sizeof(read_buf), remaining);

        // Read "before" contents from flash
        esp_err_t err = spi_flash_read(dest_addr + i, read_buf, read_len);
        if (err != ESP_OK) {
            break;
        }

#ifdef CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE
        //The written data cannot be predicted, so warning is shown if any of the bits is not 1.
        for (int r = 0; r < read_len; r += sizeof(uint32_t)) {
            uint32_t before = read_buf[r / sizeof(uint32_t)];
            if (before != 0xFFFFFFFF) {
                ESP_LOGW(TAG, "Encrypted write at offset 0x%x but not erased (0x%08x)",
                         dest_addr + i + r, before);
            }
        }
#endif

        err = spi_flash_write_encrypted_chip(dest_addr + i, src + i, read_len);
        if (err != ESP_OK) {
            break;
        }
        COUNTER_ADD_BYTES(write, size);

        spi_flash_guard_start();
        esp_rom_spiflash_write_disable();
        spi_flash_check_and_flush_cache(dest_addr, size);
        spi_flash_guard_end();

        err = spi_flash_read_encrypted(dest_addr + i, read_buf, read_len);
        if (err != ESP_OK) {
            break;
        }

        for (int r = 0; r < read_len; r += sizeof(uint32_t)) {
            int r_w = r / sizeof(uint32_t); // index in words (r is index in bytes)

            uint32_t expected = src_w[i_w + r_w];
            uint32_t actual = read_buf[r_w];
            if (expected != actual) {
#ifdef CONFIG_SPI_FLASH_LOG_FAILED_WRITE
                ESP_LOGE(TAG, "Bad write at offset 0x%x expected 0x%08x readback 0x%08x", dest_addr + i + r, expected, actual);
#endif
                err = ESP_FAIL;
            }
        }
        if (err != ESP_OK) {
            break;
        }
        remaining -= read_len;
    }
#endif // CONFIG_SPI_FLASH_VERIFY_WRITE

fail:

    COUNTER_STOP(write);
    return err;
}

esp_err_t IRAM_ATTR spi_flash_read(size_t src, void *dstv, size_t size)
{
    // Out of bound reads are checked in ROM code, but we can give better
    // error code here
    if (src + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;
    COUNTER_START();
    spi_flash_guard_start();
    /* To simplify boundary checks below, we handle small reads separately. */
    if (size < 16) {
        uint32_t t[6]; /* Enough for 16 bytes + 4 on either side for padding. */
        uint32_t read_src = src & ~3U;
        uint32_t left_off = src & 3U;
        uint32_t read_size = (left_off + size + 3) & ~3U;
        rc = esp_rom_spiflash_read(read_src, t, read_size);
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, read_size);
#ifdef ESP_PLATFORM
        if (esp_ptr_external_ram(dstv)) {
            spi_flash_guard_end();
            memcpy(dstv, ((uint8_t *) t) + left_off, size);
            spi_flash_guard_start();
        } else {
            memcpy(dstv, ((uint8_t *) t) + left_off, size);
        }
#else
        memcpy(dstv, ((uint8_t *) t) + left_off, size);
#endif
        goto out;
    }
    uint8_t *dstc = (uint8_t *) dstv;
    intptr_t dsti = (intptr_t) dstc;
    /*
     * Large operations are split into (up to) 3 parts:
     * - The middle part: from the first 4-aligned position in src to the first
     *   4-aligned position in dst.
     */
    size_t src_mid_off = (src % 4 == 0 ? 0 : 4 - (src % 4));
    size_t dst_mid_off = (dsti % 4 == 0 ? 0 : 4 - (dsti % 4));
    size_t mid_size = (size - MAX(src_mid_off, dst_mid_off)) & ~3U;
    /*
     * - Once the middle part is in place, src_mid_off bytes from the preceding
     *   4-aligned source location are added on the left.
     */
    size_t pad_left_src = src & ~3U;
    size_t pad_left_size = src_mid_off;
    /*
     * - Finally, the right part is added: from the end of the middle part to
     *   the end. Depending on the alignment of source and destination, this may
     *   be a 4 or 8 byte read from pad_right_src.
     */
    size_t pad_right_src = (src + pad_left_size + mid_size) & ~3U;
    size_t pad_right_off = (pad_right_src - src);
    size_t pad_right_size = (size - pad_right_off);

#ifdef ESP_PLATFORM
    bool direct_read = esp_ptr_internal(dstc)
            && esp_ptr_byte_accessible(dstc)
            && ((uintptr_t) dstc + dst_mid_off) % 4 == 0;
#else
    bool direct_read = true;
#endif
    if (mid_size > 0) {
        uint32_t mid_remaining = mid_size;
        uint32_t mid_read = 0;
        while (mid_remaining > 0) {
            uint32_t read_size = MIN(mid_remaining, MAX_READ_CHUNK);
            uint32_t read_buf[8];
            uint8_t *read_dst_final = dstc + dst_mid_off + mid_read;
            uint8_t *read_dst = read_dst_final;
            if (!direct_read) {
                read_size = MIN(read_size, sizeof(read_buf));
                read_dst = (uint8_t *) read_buf;
            }
            rc = esp_rom_spiflash_read(src + src_mid_off + mid_read,
                    (uint32_t *) read_dst, read_size);
            if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
                goto out;
            }
            mid_remaining -= read_size;
            mid_read += read_size;
            if (!direct_read) {
                spi_flash_guard_end();
                memcpy(read_dst_final, read_buf, read_size);
                spi_flash_guard_start();
            } else if (mid_remaining > 0) {
                /* Drop guard momentarily, allows other tasks to preempt */
                spi_flash_guard_end();
                spi_flash_guard_start();
            }
        }
        COUNTER_ADD_BYTES(read, mid_size);
        /*
         * If offsets in src and dst are different, perform an in-place shift
         * to put destination data into its final position.
         * Note that the shift can be left (src_mid_off < dst_mid_off) or right.
         */
        if (src_mid_off != dst_mid_off) {
            if (!direct_read) {
                spi_flash_guard_end();
            }
            memmove(dstc + src_mid_off, dstc + dst_mid_off, mid_size);
            if (!direct_read) {
                spi_flash_guard_start();
            }
        }
    }
    if (pad_left_size > 0) {
        uint32_t t;
        rc = esp_rom_spiflash_read(pad_left_src, &t, 4);
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, 4);
        if (!direct_read) {
            spi_flash_guard_end();
        }
        memcpy(dstc, ((uint8_t *) &t) + (4 - pad_left_size), pad_left_size);
        if (!direct_read) {
            spi_flash_guard_start();
        }
    }
    if (pad_right_size > 0) {
        uint32_t t[2];
        int32_t read_size = (pad_right_size <= 4 ? 4 : 8);
        rc = esp_rom_spiflash_read(pad_right_src, t, read_size);
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, read_size);
        if (!direct_read) {
            spi_flash_guard_end();
        }
        memcpy(dstc + pad_right_off, t, pad_right_size);
        if (!direct_read) {
            spi_flash_guard_start();
        }
    }
out:
    spi_flash_guard_end();
    COUNTER_STOP(read);
    return spi_flash_translate_rc(rc);
}
#endif // !CONFIG_SPI_FLASH_USE_LEGACY_IMPL

esp_err_t IRAM_ATTR spi_flash_read_encrypted(size_t src, void *dstv, size_t size)
{
    if (src + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    esp_err_t err;
    const uint8_t *map;
    spi_flash_mmap_handle_t map_handle;
    size_t map_src = src & ~(SPI_FLASH_MMU_PAGE_SIZE - 1);
    size_t map_size = size + (src - map_src);

    err = spi_flash_mmap(map_src, map_size, SPI_FLASH_MMAP_DATA, (const void **)&map, &map_handle);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(dstv, map + (src - map_src), size);
    spi_flash_munmap(map_handle);
    return err;
}

#if CONFIG_SPI_FLASH_USE_LEGACY_IMPL
static esp_err_t IRAM_ATTR spi_flash_translate_rc(esp_rom_spiflash_result_t rc)
{
    switch (rc) {
    case ESP_ROM_SPIFLASH_RESULT_OK:
        return ESP_OK;
    case ESP_ROM_SPIFLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    case ESP_ROM_SPIFLASH_RESULT_ERR:
    default:
        return ESP_ERR_FLASH_OP_FAIL;
    }
}
#endif //CONFIG_SPI_FLASH_USE_LEGACY_IMPL

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS

static inline void dump_counter(spi_flash_counter_t *counter, const char *name)
{
    ESP_LOGI(TAG, "%s  count=%8d  time=%8dus  bytes=%8d\n", name,
             counter->count, counter->time, counter->bytes);
}

const spi_flash_counters_t *spi_flash_get_counters(void)
{
    return &s_flash_stats;
}

void spi_flash_reset_counters(void)
{
    memset(&s_flash_stats, 0, sizeof(s_flash_stats));
}

void spi_flash_dump_counters(void)
{
    dump_counter(&s_flash_stats.read,  "read ");
    dump_counter(&s_flash_stats.write, "write");
    dump_counter(&s_flash_stats.erase, "erase");
}

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

#if CONFIG_SPI_FLASH_USE_LEGACY_IMPL && !CONFIG_IDF_TARGET_ESP32
// TODO esp32s2: Remove once ESP32-S2 & later chips has new SPI Flash API support
esp_flash_t *esp_flash_default_chip = NULL;
#endif

void IRAM_ATTR spi_flash_set_rom_required_regs(void)
{
#if CONFIG_ESPTOOLPY_OCT_FLASH
    //Disable the variable dummy mode when doing timing tuning
    CLEAR_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);
    /**
     * STR /DTR mode setting is done every time when `esp_rom_opiflash_exec_cmd` is called
     *
     * Add any registers that are not set in ROM SPI flash functions here in the future
     */
#endif
}

void IRAM_ATTR spi_flash_set_vendor_required_regs(void)
{
#if CONFIG_ESPTOOLPY_OCT_FLASH
    //Flash chip requires MSPI specifically, call this function to set them
    esp_opiflash_set_required_regs();
#else
    //currently we don't need to set other MSPI registers for Quad Flash
#endif
}
