/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"

#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_common.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "esp_private/bootloader_flash_internal.h"
#include "esp_cpu.h"
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include "hal/gpio_hal.h"
#include "xtensa/config/core.h"
#include "xt_instr_macros.h"

#include "esp32/rom/cache.h"
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "esp_efuse.h"

static const char *TAG = "boot.esp32";

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
static void bootloader_reset_mmu(void)
{
    /* completely reset MMU in case serial bootloader was running */
    Cache_Read_Disable(0);
#if !CONFIG_FREERTOS_UNICORE
    Cache_Read_Disable(1);
#endif
    Cache_Flush(0);
#if !CONFIG_FREERTOS_UNICORE
    Cache_Flush(1);
#endif
    mmu_init(0);
#if !CONFIG_FREERTOS_UNICORE
    /* The lines which manipulate DPORT_APP_CACHE_MMU_IA_CLR bit are
        necessary to work around a hardware bug. */
    DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
    mmu_init(1);
    DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
#endif

    /* normal ROM boot exits with DROM0 cache unmasked,
        but serial bootloader exits with it masked. */
    DPORT_REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MASK_DROM0);
#if !CONFIG_FREERTOS_UNICORE
    DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DROM0);
#endif
}
#endif //#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

static esp_err_t bootloader_check_rated_cpu_clock(void)
{
    int rated_freq = bootloader_clock_get_rated_freq_mhz();
    if (rated_freq < CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ) {
        ESP_LOGE(TAG, "Chip CPU frequency rated for %dMHz, configured for %dMHz. Modify CPU frequency in menuconfig",
                 rated_freq, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void wdt_reset_cpu0_info_enable(void)
{
    //We do not reset core1 info here because it didn't work before cpu1 was up. So we put it into call_start_cpu1.
    DPORT_REG_SET_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_PDEBUG_ENABLE | DPORT_PRO_CPU_RECORD_ENABLE);
    DPORT_REG_CLR_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_RECORD_ENABLE);
}

static void wdt_reset_info_dump(int cpu)
{
    uint32_t inst = 0, pid = 0, stat = 0, data = 0, pc = 0,
             lsstat = 0, lsaddr = 0, lsdata = 0, dstat = 0;
    const char *cpu_name = cpu ? "APP" : "PRO";

    if (cpu == 0) {
        stat = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_STATUS_REG);
        pid = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PID_REG);
        inst = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGINST_REG);
        dstat = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGSTATUS_REG);
        data = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGDATA_REG);
        pc = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGPC_REG);
        lsstat = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGLS0STAT_REG);
        lsaddr = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGLS0ADDR_REG);
        lsdata = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGLS0DATA_REG);
    } else {
#if !CONFIG_FREERTOS_UNICORE
        stat = DPORT_REG_READ(DPORT_APP_CPU_RECORD_STATUS_REG);
        pid = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PID_REG);
        inst = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGINST_REG);
        dstat = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGSTATUS_REG);
        data = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGDATA_REG);
        pc = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGPC_REG);
        lsstat = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGLS0STAT_REG);
        lsaddr = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGLS0ADDR_REG);
        lsdata = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGLS0DATA_REG);
#endif
    }

    if (DPORT_RECORD_PDEBUGINST_SZ(inst) == 0 &&
            DPORT_RECORD_PDEBUGSTATUS_BBCAUSE(dstat) == DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_WAITI) {
        ESP_LOGW(TAG, "WDT reset info: %s CPU PC=0x%"PRIx32" (waiti mode)", cpu_name, pc);
    } else {
        ESP_LOGW(TAG, "WDT reset info: %s CPU PC=0x%"PRIx32, cpu_name, pc);
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
#if !CONFIG_FREERTOS_UNICORE
        wdt_reset_info_dump(1);
#endif
    }
    wdt_reset_cpu0_info_enable();
}

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

#if XCHAL_ERRATUM_572
    uint32_t memctl = XCHAL_CACHE_MEMCTL_DEFAULT;
    WSR(MEMCTL, memctl);
#endif // XCHAL_ERRATUM_572

// In RAM_APP, memory will be initialized in `call_start_cpu0`
#if !CONFIG_APP_BUILD_TYPE_RAM
    bootloader_init_mem();

    // check that static RAM is after the stack
#ifndef NDEBUG
    {
        assert(&_bss_start <= &_bss_end);
        assert(&_data_start <= &_data_end);
        int *sp = esp_cpu_get_sp();
        assert(sp < &_bss_start);
        assert(sp < &_data_start);
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
    // bootst up vddsdio
    bootloader_common_vddsdio_configure();
    // check rated CPU clock
    if ((ret = bootloader_check_rated_cpu_clock()) != ESP_OK) {
        return ret;
    }
    // config clock
    bootloader_clock_configure();
    // initialize uart console, from now on, we can use esp_log
    bootloader_console_init();
    /* print 2nd bootloader banner */
    bootloader_print_banner();

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    // reset MMU
    bootloader_reset_mmu();
    // update flash ID
    bootloader_flash_update_id();
    // Check and run XMC startup flow
    if ((ret = bootloader_flash_xmc_startup()) != ESP_OK) {
        ESP_LOGE(TAG, "failed when running XMC startup flow, reboot!");
        return ret;
    }
#if !CONFIG_APP_BUILD_TYPE_RAM
    // read bootloader header
    if ((ret = bootloader_read_bootloader_header()) != ESP_OK) {
        return ret;
    }
    // read chip revision and check if it's compatible to bootloader
    if ((ret = bootloader_check_bootloader_validity()) != ESP_OK) {
        return ret;
    }
#endif // #if !CONFIG_APP_BUILD_TYPE_RAM
    // initialize spi flash
    if ((ret = bootloader_init_spi_flash()) != ESP_OK) {
        return ret;
    }
#endif  //#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

    // check whether a WDT reset happend
    bootloader_check_wdt_reset();
    // config WDT
    bootloader_config_wdt();
    // enable RNG early entropy source
    bootloader_enable_random();
    return ret;
}
