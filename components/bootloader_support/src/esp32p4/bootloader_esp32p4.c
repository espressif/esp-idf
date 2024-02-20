/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"
#include "esp_rom_gpio.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/assist_debug_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include "soc/cache_reg.h"
#include "soc/io_mux_reg.h"
#include "esp32p4/rom/ets_sys.h"
#include "esp32p4/rom/spi_flash.h"
#include "bootloader_common.h"
#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "esp_private/regi2c_ctrl.h"
#include "soc/regi2c_lp_bias.h"
#include "soc/regi2c_bias.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "bootloader_soc.h"
#include "esp_private/bootloader_flash_internal.h"
#include "esp_efuse.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/lpwdt_ll.h"
#include "soc/lp_wdt_reg.h"
#include "hal/efuse_hal.h"
#include "soc/regi2c_syspll.h"
#include "soc/regi2c_cpll.h"

static const char *TAG = "boot.esp32p4";

static void wdt_reset_cpu0_info_enable(void)
{
    //TODO: IDF-7688
    REG_WRITE(ASSIST_DEBUG_CORE_0_RCD_EN_REG, ASSIST_DEBUG_CORE_0_RCD_PDEBUGEN | ASSIST_DEBUG_CORE_0_RCD_RECORDEN);
}

static void wdt_reset_info_dump(int cpu)
{
    (void) cpu;
    // saved PC was already printed by the ROM bootloader.
    // nothing to do here.
}

static void bootloader_check_wdt_reset(void)
{
    int wdt_rst = 0;
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if (rst_reason == RESET_REASON_SYS_HP_WDT || rst_reason == RESET_REASON_SYS_LP_WDT || rst_reason == RESET_REASON_CORE_HP_WDT ||
        rst_reason == RESET_REASON_CORE_LP_WDT || rst_reason == RESET_REASON_CHIP_LP_WDT) {
        ESP_LOGW(TAG, "CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (wdt_rst) {
        // if reset by WDT dump info from trace port
        wdt_reset_info_dump(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        wdt_reset_info_dump(1);
#endif
    }
    wdt_reset_cpu0_info_enable();
}

static void bootloader_super_wdt_auto_feed(void)
{
    REG_WRITE(LP_WDT_SWD_WPROTECT_REG, LP_WDT_SWD_WKEY_VALUE);
    REG_SET_BIT(LP_WDT_SWD_CONFIG_REG, LP_WDT_SWD_AUTO_FEED_EN);
    REG_WRITE(LP_WDT_SWD_WPROTECT_REG, 0);
}

static inline void bootloader_hardware_init(void)
{
    // regi2c is enabled by default on ESP32P4, do nothing

    // On ESP32P4 ECO0, the default (power on reset) CPLL and SPLL frequencies are very high, lower them to avoid bias may not be enough in bootloader
    // And we are fixing SPLL to be 480MHz at all runtime
    // Suppose to fix the issue on ECO1, will check when chip comes back
    // TODO: IDF-8939
    REGI2C_WRITE_MASK(I2C_CPLL, I2C_CPLL_OC_DIV_7_0, 6); // lower default cpu_pll freq to 400M
    REGI2C_WRITE_MASK(I2C_SYSPLL, I2C_SYSPLL_OC_DIV_7_0, 8); // lower default sys_pll freq to 480M
    esp_rom_delay_us(100);
}

static inline void bootloader_ana_reset_config(void)
{
    //Enable super WDT reset.
    bootloader_ana_super_wdt_reset_config(true);
    //Enable BOD reset
    bootloader_ana_bod_reset_config(true);
}

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

    bootloader_hardware_init();
    bootloader_ana_reset_config();
    bootloader_super_wdt_auto_feed();

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
    ESP_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
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
    cache_hal_init();
    //reset mmu
    mmu_hal_init();
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

    // check whether a WDT reset happend
    bootloader_check_wdt_reset();
    // config WDT
    bootloader_config_wdt();
    // enable RNG early entropy source
    bootloader_enable_random();

    return ret;
}
