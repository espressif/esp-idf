/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "riscv/rv_utils.h"
#include "esp_rom_uart.h"
#include "soc/gpio_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "soc/rtc_periph.h"
#include "soc/uart_reg.h"
#include "hal/wdt_hal.h"
#include "esp_private/cache_err_int.h"

#include "esp32p4/rom/cache.h"
#include "esp32p4/rom/rtc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/hp_system_reg.h"

void IRAM_ATTR esp_system_reset_modules_on_exit(void)
{
    // Flush any data left in UART FIFOs
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (uart_ll_is_enabled(i)) {
            esp_rom_uart_tx_wait_idle(i);
        }
    }

    // Set Peripheral clk rst
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_TIMERGRP0);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_TIMERGRP1);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_STIMER);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_DUAL_MSPI_AXI);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART0_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART1_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART2_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART3_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART4_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_GDMA);

    // Clear Peripheral clk rst
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_TIMERGRP0);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_TIMERGRP1);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_STIMER);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_DUAL_MSPI_AXI);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART0_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART1_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART2_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART3_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART4_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_GDMA);
}

/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void IRAM_ATTR esp_restart_noos(void)
{
    // Disable interrupts
    rv_utils_intr_global_disable();
    // Enable RTC watchdog for 1 second
    wdt_hal_context_t rtc_wdt_ctx;
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = (uint32_t)(1000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE1, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    //Enable flash boot mode so that flash booting after restart is protected by the RTC WDT.
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, true);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

    const uint32_t core_id = esp_cpu_get_core_id();
#if !CONFIG_FREERTOS_UNICORE
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);
#endif

    // Disable TG0/TG1 watchdogs
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    // Disable cache
    Cache_Disable_L2_Cache();

    esp_system_reset_modules_on_exit();

    // Set CPU back to XTAL source, no PLL, same as hard reset
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_freq_set_xtal();
#endif

#if !CONFIG_FREERTOS_UNICORE
    // clear entry point for APP CPU
    ets_set_appcpu_boot_addr(0);
#endif

#if CONFIG_SPIRAM_INSTRUCTIONS_RODATA
    //TODO: IDF-7556
    // disable remap if enabled in menuconfig
    REG_CLR_BIT(HP_SYS_HP_PSRAM_FLASH_ADDR_INTERCHANGE_REG, HP_SYS_HP_PSRAM_FLASH_ADDR_INTERCHANGE_DMA | HP_SYS_HP_PSRAM_FLASH_ADDR_INTERCHANGE_CPU);
#endif

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
#if !CONFIG_FREERTOS_UNICORE
        esp_cpu_reset(1);
#endif
        esp_cpu_reset(0);
    }
#if !CONFIG_FREERTOS_UNICORE
    else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
#endif

    while (true) {
        ;
    }
}
