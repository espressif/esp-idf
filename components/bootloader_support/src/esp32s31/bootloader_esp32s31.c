/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"
#include "bootloader_common.h"
#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "bootloader_soc.h"
#include "esp_private/bootloader_flash_internal.h"

static const char *TAG = "boot.esp32s31";

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

    // bootloader_hardware_init();       // TODO: IDF-14696
    // bootloader_super_wdt_auto_feed();   // TODO: IDF-14656

// In RAM_APP, memory will be initialized in `call_start_cpu0`
#if !CONFIG_APP_BUILD_TYPE_RAM
    // protect memory region
    bootloader_init_mem();
    /* check that static RAM is after the stack */
    assert(&_bss_start <= &_bss_end);
    assert(&_data_start <= &_data_end);
    // clear bss section
    bootloader_clear_bss_section();
#endif // !CONFIG_APP_BUILD_TYPE_RAM

    // init eFuse virtual mode (read eFuses to RAM)
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_EARLY_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
#ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    esp_efuse_init_virtual_mode_in_ram();
#endif
#endif

    // config clock
    bootloader_clock_configure();
    // initialize console, from now on, we can use esp_log
    bootloader_console_init();
    /* print 2nd bootloader banner */
    bootloader_print_banner();

#if !CONFIG_APP_BUILD_TYPE_RAM
    //init cache hal
    // cache_hal_init();  IDF-14651
    //reset mmu
    // mmu_hal_init();   IDF-14669
    // update flash ID
    bootloader_flash_update_id();
    // Check and run XMC startup flow
    if ((ret = bootloader_flash_xmc_startup()) != ESP_OK) {
        ESP_LOGE(TAG, "failed when running XMC startup flow, reboot!");
        return ret;
    }
    // read bootloader header
    if ((ret = bootloader_read_bootloader_header()) != ESP_OK) {
        return ret;
    }
    // read chip revision and check if it's compatible to bootloader
    if ((ret = bootloader_check_bootloader_validity()) != ESP_OK) {
        return ret;
    }
    // initialize spi flash
    if ((ret = bootloader_init_spi_flash()) != ESP_OK) {
        return ret;
    }
#endif // !CONFIG_APP_BUILD_TYPE_RAM

    // check whether a WDT reset happened
    // bootloader_check_wdt_reset();     // TODO: IDF-14656
    // config WDT
    bootloader_config_wdt();
    // enable RNG early entropy source
    bootloader_enable_random();

    return ret;
}
