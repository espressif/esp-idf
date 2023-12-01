/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/assist_debug_reg.h"
#include "esp_cpu.h"
#include "soc/dport_reg.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/spi_periph.h"
#include "soc/extmem_reg.h"

#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "esp32s3/rom/rtc.h"

#include "bootloader_common.h"
#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "bootloader_soc.h"
#include "esp_private/bootloader_flash_internal.h"
#include "esp_efuse.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "hal/rwdt_ll.h"
#include "xtensa/config/core.h"
#include "xt_instr_macros.h"


static const char *TAG = "boot.esp32s3";

static void wdt_reset_cpu0_info_enable(void)
{
    REG_SET_BIT(SYSTEM_CPU_PERI_CLK_EN_REG, SYSTEM_CLK_EN_ASSIST_DEBUG);
    REG_CLR_BIT(SYSTEM_CPU_PERI_RST_EN_REG, SYSTEM_RST_EN_ASSIST_DEBUG);
    REG_WRITE(ASSIST_DEBUG_CORE_0_RCD_PDEBUGENABLE_REG, 1);
    REG_WRITE(ASSIST_DEBUG_CORE_0_RCD_RECORDING_REG, 1);
}

static void wdt_reset_info_dump(int cpu)
{
    uint32_t inst = 0, pid = 0, stat = 0, data = 0, pc = 0,
             lsstat = 0, lsaddr = 0, lsdata = 0, dstat = 0;
    const char *cpu_name = cpu ? "APP" : "PRO";

    stat = 0xdeadbeef;
    pid = 0;
    if (cpu == 0) {
        inst    = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGINST_REG);
        dstat   = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGSTATUS_REG);
        data    = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGDATA_REG);
        pc      = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGPC_REG);
        lsstat  = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGLS0STAT_REG);
        lsaddr  = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGLS0ADDR_REG);
        lsdata  = REG_READ(ASSIST_DEBUG_CORE_0_RCD_PDEBUGLS0DATA_REG);
    } else {
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        inst    = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGINST_REG);
        dstat   = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGSTATUS_REG);
        data    = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGDATA_REG);
        pc      = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGPC_REG);
        lsstat  = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGLS0STAT_REG);
        lsaddr  = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGLS0ADDR_REG);
        lsdata  = REG_READ(ASSIST_DEBUG_CORE_1_RCD_PDEBUGLS0DATA_REG);
#else
        ESP_LOGE(TAG, "WDT reset info: %s CPU not support!", cpu_name);
        return;
#endif
    }

    ESP_LOGD(TAG, "WDT reset info: %s CPU STATUS        0x%08"PRIx32, cpu_name, stat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PID           0x%08"PRIx32, cpu_name, pid);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGINST    0x%08"PRIx32, cpu_name, inst);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGSTATUS  0x%08"PRIx32, cpu_name, dstat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGDATA    0x%08"PRIx32, cpu_name, data);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGPC      0x%08"PRIx32, cpu_name, pc);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0STAT 0x%08"PRIx32, cpu_name, lsstat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0ADDR 0x%08"PRIx32, cpu_name, lsaddr);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0DATA 0x%08"PRIx32, cpu_name, lsdata);
}

static void bootloader_check_wdt_reset(void)
{
    int wdt_rst = 0;
    soc_reset_reason_t rst_reas[2];

    rst_reas[0] = esp_rom_get_reset_reason(0);
    rst_reas[1] = esp_rom_get_reset_reason(1);
    if (rst_reas[0] == RESET_REASON_CORE_RTC_WDT || rst_reas[0] == RESET_REASON_CORE_MWDT0 || rst_reas[0] == RESET_REASON_CORE_MWDT1 ||
        rst_reas[0] == RESET_REASON_CPU0_MWDT0 || rst_reas[0] == RESET_REASON_CPU0_RTC_WDT) {
        ESP_LOGW(TAG, "PRO CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (rst_reas[1] == RESET_REASON_CORE_RTC_WDT || rst_reas[1] == RESET_REASON_CORE_MWDT0 || rst_reas[1] == RESET_REASON_CORE_MWDT1 ||
        rst_reas[1] == RESET_REASON_CPU1_MWDT1 || rst_reas[1] == RESET_REASON_CPU1_RTC_WDT) {
        ESP_LOGW(TAG, "APP CPU has been reset by WDT.");
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
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, RTC_CNTL_SWD_WKEY_VALUE);
    REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_AUTO_FEED_EN);
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, 0);
}

static inline void bootloader_ana_reset_config(void)
{
    //Enable WDT, BOD, and GLITCH reset
    bootloader_ana_super_wdt_reset_config(true);
    bootloader_ana_bod_reset_config(true);
    bootloader_ana_clock_glitch_reset_config(true);
}

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

#if XCHAL_ERRATUM_572
    uint32_t memctl = XCHAL_CACHE_MEMCTL_DEFAULT;
    WSR(MEMCTL, memctl);
#endif // XCHAL_ERRATUM_572

    bootloader_ana_reset_config();
    bootloader_super_wdt_auto_feed();

// In RAM_APP, memory will be initialized in `call_start_cpu0`
#if !CONFIG_APP_BUILD_TYPE_RAM
    // protect memory region
    bootloader_init_mem();
    /* check that static RAM is after the stack */
#ifndef NDEBUG
    {
        assert(&_bss_start <= &_bss_end);
        assert(&_data_start <= &_data_end);
    }
#endif
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
    //init mmu
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
