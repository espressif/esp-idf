/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "esp_private/system_internal.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_gpio_reserve.h"
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
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C61
#include "esp32c61/rom/cache.h"
#endif
#include "esp_rom_spiflash.h"
#include "esp_flash_partitions.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/cache_utils.h"
#include "esp_flash.h"
#include "esp_attr.h"
#include "bootloader_flash.h"
#include "bootloader_flash_config.h"
#include "esp_compiler.h"
#include "esp_rom_efuse.h"
#if CONFIG_SPIRAM
#include "esp_private/esp_psram_io.h"
#endif
#if SOC_MEMSPI_CLOCK_IS_INDEPENDENT
#include "hal/cache_hal.h"
#endif

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
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    bool octal_mspi_required = bootloader_flash_is_octal_mode_enabled();
#if CONFIG_SPIRAM_MODE_OCT
    octal_mspi_required |= true;
#endif

    if (octal_mspi_required) {
        esp_rom_opiflash_pin_config();
        mspi_timing_set_pin_drive_strength();
    }
    //Set F4R4 board pin drive strength. TODO: IDF-3663
#endif
    /* Reserve the GPIO pins */
    uint64_t reserve_pin_mask = 0;
    for (esp_mspi_io_t i = 0; i < ESP_MSPI_IO_MAX; i++) {
        reserve_pin_mask |= BIT64(esp_mspi_get_io(i));
    }
    esp_gpio_reserve(reserve_pin_mask);
}

esp_err_t IRAM_ATTR spi_flash_init_chip_state(void)
{
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    if (bootloader_flash_is_octal_mode_enabled()) {
        return esp_opiflash_init(rom_spiflash_legacy_data->chip.device_id);
    }
#endif
#if CONFIG_SPI_FLASH_HPM_ON
        return spi_flash_enable_high_performance_mode();
#endif // CONFIG_SPI_FLASH_HPM_ON
    return ESP_OK;
}

void IRAM_ATTR spi_flash_set_rom_required_regs(void)
{
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    if (bootloader_flash_is_octal_mode_enabled()) {
        //Disable the variable dummy mode when doing timing tuning
        CLEAR_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);
        /**
         * STR /DTR mode setting is done every time when `esp_rom_opiflash_exec_cmd` is called
         *
         * Add any registers that are not set in ROM SPI flash functions here in the future
         */
    }
#endif
}

#if CONFIG_SPIRAM_MODE_OCT
// This function will only be called when Octal PSRAM enabled.
void IRAM_ATTR spi_flash_set_vendor_required_regs(void)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        esp_opiflash_set_required_regs();
        SET_PERI_REG_BITS(SPI_MEM_CACHE_FCTRL_REG(1), SPI_MEM_CACHE_USR_CMD_4BYTE_V, 1, SPI_MEM_CACHE_USR_CMD_4BYTE_S);
    } else {
        //Flash chip requires MSPI specifically, call this function to set them
        // Set back MSPI registers after Octal PSRAM initialization.
        SET_PERI_REG_BITS(SPI_MEM_CACHE_FCTRL_REG(1), SPI_MEM_CACHE_USR_CMD_4BYTE_V, 0, SPI_MEM_CACHE_USR_CMD_4BYTE_S);
    }
}
#endif

static const uint8_t s_mspi_io_num_default[] = {
    SPI_CLK_GPIO_NUM,
    SPI_Q_GPIO_NUM,
    SPI_D_GPIO_NUM,
    SPI_CS0_GPIO_NUM,
    SPI_HD_GPIO_NUM,
    SPI_WP_GPIO_NUM,
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    SPI_DQS_GPIO_NUM,
    SPI_D4_GPIO_NUM,
    SPI_D5_GPIO_NUM,
    SPI_D6_GPIO_NUM,
    SPI_D7_GPIO_NUM
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE
};

uint8_t esp_mspi_get_io(esp_mspi_io_t io)
{
#if CONFIG_SPIRAM
    if (io == ESP_MSPI_IO_CS1) {
        return esp_psram_io_get_cs_io();
    }
#endif

    assert(io >= ESP_MSPI_IO_CLK);
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    assert(io <= ESP_MSPI_IO_D7);
#else
    assert(io <= ESP_MSPI_IO_WP);
#endif

#if SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
    uint8_t mspi_io = 0;
    uint32_t spiconfig = 0;

    if (io == ESP_MSPI_IO_WP) {
        /**
         * wp pad is a bit special:
         * 1. since 32's efuse does not have enough bits for wp pad, so wp pad config put in flash bin header
         * 2. rom code take 0x3f as invalid wp pad num, but take 0 as other invalid mspi pads num
         */
#if CONFIG_IDF_TARGET_ESP32
        return bootloader_flash_get_wp_pin();
#else
        spiconfig = esp_rom_efuse_get_flash_wp_gpio();
        return (spiconfig == 0x3f) ? s_mspi_io_num_default[io] : spiconfig & 0x3f;
#endif
    }

#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    spiconfig = (io < ESP_MSPI_IO_WP) ? esp_rom_efuse_get_flash_gpio_info() : esp_rom_efuse_get_opiconfig();
#else
    spiconfig = esp_rom_efuse_get_flash_gpio_info();
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE

    if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
        mspi_io = s_mspi_io_num_default[io];
    } else if (io < ESP_MSPI_IO_WP) {
        /**
         * [0 : 5] -- CLK
         * [6 :11] -- Q(D1)
         * [12:17] -- D(D0)
         * [18:23] -- CS
         * [24:29] -- HD(D3)
         */
        mspi_io = (spiconfig >> io * 6) & 0x3f;
    }
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    else {
        /**
         * [0 : 5] -- DQS
         * [6 :11] -- D4
         * [12:17] -- D5
         * [18:23] -- D6
         * [24:29] -- D7
         */
        mspi_io = (spiconfig >> (io - ESP_MSPI_IO_DQS) * 6) & 0x3f;
    }
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE
    return mspi_io;
#else  // SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
    return s_mspi_io_num_default[io];
#endif // SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
}
