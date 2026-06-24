/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc_caps.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "bootloader_soc.h"
#include "esp_efuse.h"
#include "esp_private/bootloader_flash_internal.h"
#if SOC_RTC_WDT_SUPPORTED
#include "soc/rtc_wdt_reg.h"
#include "hal/rwdt_ll.h"
#endif
#include "hal/gpio_ll.h"
#include "hal/brownout_ll.h"
#include "soc/pmu_reg.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/modem_lpcon_ll.h"
#include "soc/reset_reasons.h"
#include "hal/assist_debug_ll.h"
#include "esp_rom_sys.h"
#include "soc/regi2c_bias.h"
#include "hal/regi2c_ctrl.h"

ESP_LOG_ATTR_TAG(TAG, "boot.esp32s31");

static inline void bootloader_hardware_init(void)
{
    /* GPIO 41 is not bonded out to the package, Isolate it to suppress
     * floating leakage.*/
    gpio_ll_input_disable(&GPIO, 41);
    gpio_ll_output_disable(&GPIO, 41);
    gpio_ll_pullup_dis(&GPIO, 41);
    gpio_ll_pulldown_dis(&GPIO, 41);
    gpio_ll_func_sel(&GPIO, 41, PIN_FUNC_GPIO);

    modem_lpcon_ll_enable_bus_clock(true);

#if !CONFIG_IDF_ENV_FPGA
    /* Enable analog i2c master clock */
    _regi2c_ctrl_ll_master_enable_clock(true); // keep ana i2c mst clock always enabled in bootloader
    regi2c_ctrl_ll_master_force_enable_clock(true); // TODO: IDF-14678 Remove this?
    regi2c_ctrl_ll_master_configure_clock();
#endif

    REGI2C_WRITE_MASK(I2C_BIAS, I2C_BIAS_DREG_1P1, 10);
    REGI2C_WRITE_MASK(I2C_BIAS, I2C_BIAS_DREG_1P1_PVT, 10);
}

void bootloader_enable_cpu_reset_info(void)
{
    assist_debug_ll_enable_bus_clock(0, true);
    assist_debug_ll_enable_pc_recording(0, true);
    assist_debug_ll_lockup_monitor_enable(0, true);
    assist_debug_ll_lockup_reset_enable(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    assist_debug_ll_enable_bus_clock(1, true);
    assist_debug_ll_enable_pc_recording(1, true);
    assist_debug_ll_lockup_monitor_enable(1, true);
    assist_debug_ll_lockup_reset_enable(1);
#endif
}

void bootloader_dump_wdt_reset_info(int cpu)
{
    (void) cpu;
    // saved PC was already printed by the ROM bootloader.
    // nothing to do here.
}

bool bootloader_check_if_wdt_reset(int cpu, soc_reset_reason_t reset_reason)
{
    if (cpu == 0 && (reset_reason == RESET_REASON_CORE_MWDT0 || reset_reason == RESET_REASON_CORE_MWDT1 ||
                     reset_reason == RESET_REASON_CORE_RWDT || reset_reason == RESET_REASON_CPU_MWDT ||
                     reset_reason == RESET_REASON_CPU_RWDT || reset_reason == RESET_REASON_SYS_RWDT)) {
        ESP_LOGW(TAG, "PRO CPU has been reset by WDT.");
        return true;
    }
    if (cpu == 1 && (reset_reason == RESET_REASON_CORE_MWDT0 || reset_reason == RESET_REASON_CORE_MWDT1 ||
                     reset_reason == RESET_REASON_CORE_RWDT || reset_reason == RESET_REASON_CPU_MWDT ||
                     reset_reason == RESET_REASON_CPU_RWDT || reset_reason == RESET_REASON_SYS_RWDT)) {
        ESP_LOGW(TAG, "APP CPU has been reset by WDT.");
        return true;
    }
    return false;
}

static inline void bootloader_ana_reset_config(void)
{
    //Enable BOD reset
    brownout_ll_ana_reset_enable(true);
    bootloader_power_glitch_reset_config(true);
}

#if SOC_RTC_WDT_SUPPORTED
static void bootloader_super_wdt_auto_feed(void)
{
    REG_WRITE(RTC_WDT_SWD_WPROTECT_REG, RTC_WDT_SWD_WKEY_VALUE);
    REG_SET_BIT(RTC_WDT_SWD_CONFIG_REG, RTC_WDT_SWD_AUTO_FEED_EN);
    REG_WRITE(RTC_WDT_SWD_WPROTECT_REG, 0);
}
#endif

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

    bootloader_hardware_init();
    bootloader_ana_reset_config();
#if SOC_RTC_WDT_SUPPORTED
    bootloader_super_wdt_auto_feed();
#endif

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
    // init cache and mmu
    bootloader_init_ext_mem();
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

    // check reset reason and dump diagnostic info
    bootloader_check_reset();
#if SOC_RTC_WDT_SUPPORTED || SOC_WDT_SUPPORTED
    // config WDT
    bootloader_config_wdt();
#endif
    // enable RNG early entropy source
    bootloader_enable_random();

    return ret;
}
