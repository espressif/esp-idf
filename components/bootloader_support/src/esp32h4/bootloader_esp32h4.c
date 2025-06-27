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
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "soc/gpio_sig_map.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include "soc/cache_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/pcr_reg.h"
#include "soc/bus_monitor_reg.h"
#include "bootloader_common.h"
#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "esp_private/regi2c_ctrl.h"
// #include "soc/regi2c_lp_bias.h"
// #include "soc/regi2c_bias.h"
#include "soc/hp_system_reg.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "bootloader_soc.h"
#include "esp_private/bootloader_flash_internal.h"
#include "esp_efuse.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "hal/clk_tree_ll.h"
#include "soc/lp_wdt_reg.h"
#include "hal/efuse_hal.h"
#include "hal/lpwdt_ll.h"

static const char *TAG = "boot.esp32h4";

// TODO: [ESP32H4] support core1 bus monitor IDF-12592
static void wdt_reset_cpu0_info_enable(void)
{
    REG_SET_BIT(PCR_ASSIST_CONF_REG, PCR_ASSIST_CLK_EN);
    REG_CLR_BIT(PCR_ASSIST_CONF_REG, PCR_ASSIST_RST_EN);
    REG_WRITE(BUS_MONITOR_CORE_0_RCD_EN_REG, BUS_MONITOR_CORE_0_RCD_PDEBUGEN | BUS_MONITOR_CORE_0_RCD_RECORDEN);
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
    if (rst_reason == RESET_REASON_CORE_RTC_WDT || rst_reason == RESET_REASON_CORE_MWDT0 || rst_reason == RESET_REASON_CORE_MWDT1 ||
        rst_reason == RESET_REASON_CPU0_MWDT0 || rst_reason == RESET_REASON_CPU0_MWDT1 || rst_reason == RESET_REASON_CPU0_RTC_WDT) {
        ESP_LOGW(TAG, "PRO CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (wdt_rst) {
        // if reset by WDT dump info from trace port
        wdt_reset_info_dump(0);
    }
    wdt_reset_cpu0_info_enable();
}

static void bootloader_super_wdt_auto_feed(void)
{
    REG_WRITE(LP_WDT_SWD_WPROTECT_REG, LP_WDT_SWD_WKEY_VALUE);
    REG_SET_BIT(LP_WDT_SWD_CONFIG_REG, LP_WDT_SWD_AUTO_FEED_EN);
    REG_WRITE(LP_WDT_SWD_WPROTECT_REG, 0);
}

void spi_flash_din_num_set(uint8_t spi_num, uint8_t din_num)
{
    uint32_t reg_val = (REG_READ(SPI_MEM_DIN_NUM_REG(spi_num)) & (~(SPI_MEM_DIN0_NUM_M | SPI_MEM_DIN1_NUM_M | SPI_MEM_DIN2_NUM_M | SPI_MEM_DIN3_NUM_M | SPI_MEM_DIN4_NUM_M | SPI_MEM_DIN5_NUM_M | SPI_MEM_DIN6_NUM_M | SPI_MEM_DIN7_NUM_M | SPI_MEM_DINS_NUM_M)))
        | (din_num << SPI_MEM_DIN0_NUM_S) | (din_num << SPI_MEM_DIN1_NUM_S) | (din_num << SPI_MEM_DIN2_NUM_S) | (din_num << SPI_MEM_DIN3_NUM_S)
        | (din_num << SPI_MEM_DIN4_NUM_S) | (din_num << SPI_MEM_DIN5_NUM_S) | (din_num << SPI_MEM_DIN6_NUM_S) | (din_num << SPI_MEM_DIN7_NUM_S) | (din_num << SPI_MEM_DINS_NUM_S);
    REG_WRITE(SPI_MEM_DIN_NUM_REG(spi_num), reg_val);
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_UPDATE);
}

void spi_flash_extra_dummy_set(uint8_t spi_num, uint8_t extra_dummy)
{
    rom_spiflash_legacy_data->dummy_len_plus[spi_num] = extra_dummy;
}

/*
 * din mode     din_num      dummy
    1           0            1
    0           0            0
    1           0            2
    0           0            1
    1           0            3
    0           0            2
    1           0            4
    0           0            3
 */
static inline void bootloader_hardware_init(void)
{
    // TODO: IDF-12285 RF disable?

    /* Enable analog i2c master clock */
    _regi2c_ctrl_ll_master_enable_clock(true); // keep ana i2c mst clock always enabled in bootloader
    regi2c_ctrl_ll_master_force_enable_clock(true); // TODO: IDF-12285 Remove this?
    regi2c_ctrl_ll_master_configure_clock();
}

static inline void bootloader_ana_reset_config(void)
{
    //Enable super WDT reset.
    bootloader_ana_super_wdt_reset_config(true);
    //Enable BOD reset
    //TODO: [ESP32H4] IDF-12300 need check
}

static inline void bootloader_config_dcache(void)
{
    REG_SET_BIT(LP_AON_SRAM_USAGE_CONF_REG, LP_AON_DCACHE_USAGE);
}

static inline void bootloader_config_icache1(void)
{
    // TODO: [ESP32H4] IDF-12289
#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    REG_CLR_BIT(LP_AON_SRAM_USAGE_CONF_REG, LP_AON_ICACHE1_USAGE);
#else
    REG_SET_BIT(LP_AON_SRAM_USAGE_CONF_REG, LP_AON_ICACHE1_USAGE);
#endif
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
    bootloader_config_dcache();
    bootloader_config_icache1();
    // check whether a WDT reset happened
    bootloader_check_wdt_reset();
    // config WDT
    bootloader_config_wdt();
    // enable RNG early entropy source
    bootloader_enable_random();

    return ret;
}
