/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/param.h>  // For MIN/MAX(a, b)
#include <soc/soc.h>
#include <soc/soc_memory_layout.h>
#include "soc/io_mux_reg.h"
#include "soc/spi_pins.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_private/system_internal.h"
#include "esp_private/startup_internal.h"
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
#include "esp_private/opi_flash_private.h"
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
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/cache_utils.h"
#include "esp_attr.h"
#include "bootloader_flash.h"
#include "bootloader_flash_config.h"
#include "esp_compiler.h"
#include "esp_rom_efuse.h"
#include "esp_rom_caps.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"
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

#if CONFIG_SPI_FLASH_ROM_IMPL
#include "esp_heap_caps.h"
#if ESP_ROM_HAS_SPI_FLASH_MMAP
#if ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE
#include "soc/mmu.h"
#define ROM_MMAP_PAGE_NUM   (CACHE_DROM_MMU_MAX_END / sizeof(uint32_t))
#else
#define ROM_MMAP_PAGE_NUM   128
#endif
#endif

void IRAM_ATTR *spi_flash_malloc_internal(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
}

void IRAM_ATTR spi_flash_rom_impl_init(void)
{
    spi_flash_guard_set(&g_flash_guard_default_ops);

#if ESP_ROM_HAS_SPI_FLASH_MMAP
    /* These two functions are in ROM only */
    extern void spi_flash_mmap_os_func_set(void *(*func1)(size_t size), void (*func2)(void *p));
    spi_flash_mmap_os_func_set(spi_flash_malloc_internal, heap_caps_free);

    extern esp_err_t spi_flash_mmap_page_num_init(uint32_t page_num);
    spi_flash_mmap_page_num_init(ROM_MMAP_PAGE_NUM);
#endif // ESP_ROM_HAS_SPI_FLASH_MMAP
}
#endif

bool IRAM_ATTR spi_flash_is_octal_mode_enabled(void)
{
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    return efuse_ll_get_flash_type();
#else
    return false;
#endif
}

void IRAM_ATTR esp_mspi_pin_init(void)
{
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    bool octal_mspi_required = spi_flash_is_octal_mode_enabled();
#if CONFIG_SPIRAM_MODE_OCT
    octal_mspi_required |= true;
#endif

    if (octal_mspi_required) {
        esp_rom_opiflash_pin_config();
        mspi_timing_set_pin_drive_strength();
    }
    //Set F4R4 board pin drive strength. TODO: IDF-3663
#endif
}

void esp_mspi_pin_reserve(void)
{
    uint64_t reserve_pin_mask = 0;
    uint8_t mspi_io;
    for (esp_mspi_io_t i = 0; i < ESP_MSPI_IO_MAX; i++) {
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
        if (!spi_flash_is_octal_mode_enabled()
                && i >=  ESP_MSPI_IO_DQS && i <= ESP_MSPI_IO_D7) {
            continue;
        }
#endif
        mspi_io = esp_mspi_get_io(i);
        if (mspi_io < 64) {     // 'reserve_pin_mask' have 64 bits length
            reserve_pin_mask |= BIT64(mspi_io);
        }
    }
    esp_gpio_reserve(reserve_pin_mask);
}

esp_err_t IRAM_ATTR spi_flash_init_chip_state(void)
{
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    if (spi_flash_is_octal_mode_enabled()) {
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
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    if (spi_flash_is_octal_mode_enabled()) {
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

static const uint8_t s_mspi_io_num_default[] = {
    MSPI_IOMUX_PIN_NUM_CLK,
    MSPI_IOMUX_PIN_NUM_MISO,
    MSPI_IOMUX_PIN_NUM_MOSI,
    MSPI_IOMUX_PIN_NUM_CS0,
    MSPI_IOMUX_PIN_NUM_HD,
    MSPI_IOMUX_PIN_NUM_WP,
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    MSPI_IOMUX_PIN_NUM_DQS,
    MSPI_IOMUX_PIN_NUM_D4,
    MSPI_IOMUX_PIN_NUM_D5,
    MSPI_IOMUX_PIN_NUM_D6,
    MSPI_IOMUX_PIN_NUM_D7
#endif // SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
};

uint8_t esp_mspi_get_io(esp_mspi_io_t io)
{
#if CONFIG_SPIRAM
    if (io == ESP_MSPI_IO_CS1) {
        return esp_psram_io_get_cs_io();
    }
#endif

    assert(io >= ESP_MSPI_IO_CLK);
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
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

#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    spiconfig = (io < ESP_MSPI_IO_WP) ? esp_rom_efuse_get_flash_gpio_info() : esp_rom_efuse_get_opiconfig();
#else
    spiconfig = esp_rom_efuse_get_flash_gpio_info();
#endif // SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE

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
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
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
#endif // SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    return mspi_io;
#else  // SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
    return s_mspi_io_num_default[io];
#endif // SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
}

#if !CONFIG_IDF_TARGET_ESP32P4 || !CONFIG_APP_BUILD_TYPE_RAM  // IDF-10019
esp_err_t IRAM_ATTR esp_mspi_32bit_address_flash_feature_check(void)
{
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
    ESP_EARLY_LOGE(TAG, "32bit address (flash over 16MB) has high risk on this chip");
    return ESP_ERR_NOT_SUPPORTED;
#elif CONFIG_IDF_TARGET_ESP32P4
    // IDF-10019
    unsigned chip_version = efuse_hal_chip_revision();
    if (unlikely(!ESP_CHIP_REV_ABOVE(chip_version, 1))) {
        ESP_EARLY_LOGE(TAG, "32bit address (flash over 16MB) has high risk on ESP32P4 v0.0");
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    return ESP_OK;
}
#endif // !CONFIG_IDF_TARGET_ESP32P4 || !CONFIG_APP_BUILD_TYPE_RAM

#if CONFIG_ESP_SLEEP_SET_FLASH_DPD
static esp_err_t dpd_enter_func(void)
{
    spi_flash_ll_enter_dpd(&SPIMEM_LL_APB);
    while (!spi_flash_ll_cmd_is_done(&SPIMEM_LL_APB)) {
        //nop
    }
    return ESP_OK;
}

static esp_err_t dpd_exit_func(void)
{
    spi_flash_ll_exit_dpd(&SPIMEM_LL_APB);
    while (!spi_flash_ll_cmd_is_done(&SPIMEM_LL_APB)) {
        //nop
    }
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(init_flash_dpd, CORE, BIT(0), 125)
{
    // Register DPD function pointers
    static spi_flash_dpd_funcs_t dpd_funcs = {
        .enter_dpd = dpd_enter_func,
        .exit_dpd = dpd_exit_func,
    };
    spi_flash_dpd_register_funcs(&dpd_funcs);

    return ESP_OK;
}
#endif // CONFIG_ESP_SLEEP_SET_FLASH_DPD

void esp_mspi_flash_ops_include_func(void)
{
    // Linker hook function, exists to make the linker examine this file so that
    // the init_flash_dpd ESP_SYSTEM_INIT_FN above is not discarded.
}
