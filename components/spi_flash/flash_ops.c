/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "esp_private/system_internal.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/esp_clk.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#include "esp32/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "soc/spi_mem_reg.h"
#include "esp32s3/rom/opi_flash.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/opi_flash_private.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#endif
#include "esp_rom_spiflash.h"
#include "esp_flash_partitions.h"
#include "esp_private/cache_utils.h"
#include "esp_flash.h"
#include "esp_attr.h"
#include "bootloader_flash.h"
#include "esp_compiler.h"

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

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_default_ops = {
    .start                  = spi_flash_disable_interrupts_caches_and_other_cpu,
    .end                    = spi_flash_enable_interrupts_caches_and_other_cpu,
};

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_no_os_ops = {
    .start                  = spi_flash_disable_interrupts_caches_and_other_cpu_no_os,
    .end                    = spi_flash_enable_interrupts_caches_no_os,
};

static const spi_flash_guard_funcs_t *s_flash_guard_ops;

void IRAM_ATTR spi_flash_guard_set(const spi_flash_guard_funcs_t *funcs)
{
    s_flash_guard_ops = funcs;
}

const spi_flash_guard_funcs_t *IRAM_ATTR spi_flash_guard_get(void)
{
    return s_flash_guard_ops;
}


#ifdef CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS
#define UNSAFE_WRITE_ADDRESS abort()
#else
#define UNSAFE_WRITE_ADDRESS return false
#endif


static __attribute__((unused)) bool is_safe_write_address(size_t addr, size_t size)
{
    if (!esp_partition_main_flash_region_safe(addr, size)) {
        UNSAFE_WRITE_ADDRESS;
    }
    return true;
}

#if CONFIG_SPI_FLASH_ROM_IMPL
#include "esp_heap_caps.h"

void IRAM_ATTR *spi_flash_malloc_internal(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
}

void IRAM_ATTR spi_flash_rom_impl_init(void)
{
    spi_flash_guard_set(&g_flash_guard_default_ops);

    /* These two functions are in ROM only */
    extern void spi_flash_mmap_os_func_set(void *(*func1)(size_t size), void (*func2)(void *p));
    spi_flash_mmap_os_func_set(spi_flash_malloc_internal, heap_caps_free);

    extern esp_err_t spi_flash_mmap_page_num_init(uint32_t page_num);
    spi_flash_mmap_page_num_init(128);
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
#if CONFIG_IDF_TARGET_ESP32S3
    // Currently, only esp32s3 allows high performance mode.
    return spi_flash_enable_high_performance_mode();
#else
    return ESP_OK;
#endif // CONFIG_IDF_TARGET_ESP32S3
#endif // CONFIG_ESPTOOLPY_OCT_FLASH
}

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

#if CONFIG_SPIRAM_MODE_OCT
// This function will only be called when Octal PSRAM enabled.
void IRAM_ATTR spi_flash_set_vendor_required_regs(void)
{
#if CONFIG_ESPTOOLPY_OCT_FLASH
    //Flash chip requires MSPI specifically, call this function to set them
    esp_opiflash_set_required_regs();
    SET_PERI_REG_BITS(SPI_MEM_CACHE_FCTRL_REG(1), SPI_MEM_CACHE_USR_CMD_4BYTE_V, 1, SPI_MEM_CACHE_USR_CMD_4BYTE_S);
#else
    // Set back MSPI registers after Octal PSRAM initialization.
    SET_PERI_REG_BITS(SPI_MEM_CACHE_FCTRL_REG(1), SPI_MEM_CACHE_USR_CMD_4BYTE_V, 0, SPI_MEM_CACHE_USR_CMD_4BYTE_S);
#endif // CONFIG_ESPTOOLPY_OCT_FLASH
}
#endif
