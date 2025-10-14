/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_macros.h"
#include "esp_rom_sys.h"
#include "riscv/rv_utils.h"
#include "esp_rom_serial_output.h"
#include "soc/soc_caps.h"
#include "soc/gpio_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "soc/rtc_periph.h"
#include "soc/uart_reg.h"
#include "hal/wdt_hal.h"
#include "hal/uart_ll.h"

#include "esp32h4/rom/cache.h"
// TODO: IDF-11911 need refactor

void esp_system_reset_modules_on_exit(void)
{
    // Flush any data left in UART FIFOs before reset the UART peripheral
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (uart_ll_is_enabled(i)) {
            esp_rom_output_tx_wait_idle(i);
        }
    }

    // Set Peripheral clk rst
    SET_PERI_REG_MASK(PCR_MSPI_CONF_REG, PCR_MSPI_RST_EN);
    SET_PERI_REG_MASK(PCR_UART0_CONF_REG, PCR_UART0_RST_EN);
    SET_PERI_REG_MASK(PCR_UART1_CONF_REG, PCR_UART1_RST_EN);
    SET_PERI_REG_MASK(PCR_SYSTIMER_CONF_REG, PCR_SYSTIMER_RST_EN);
    SET_PERI_REG_MASK(PCR_GDMA_CONF_REG, PCR_GDMA_RST_EN);
    SET_PERI_REG_MASK(PCR_MODEM_CONF_REG, PCR_MODEM_RST_EN);
    SET_PERI_REG_MASK(PCR_PWM0_CONF_REG, PCR_PWM0_RST_EN);
    SET_PERI_REG_MASK(PCR_PWM1_CONF_REG, PCR_PWM1_RST_EN);

    // Clear Peripheral clk rst
    CLEAR_PERI_REG_MASK(PCR_MSPI_CONF_REG, PCR_MSPI_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_UART0_CONF_REG, PCR_UART0_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_UART1_CONF_REG, PCR_UART1_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_SYSTIMER_CONF_REG, PCR_SYSTIMER_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_GDMA_CONF_REG, PCR_GDMA_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_MODEM_CONF_REG, PCR_MODEM_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_PWM0_CONF_REG, PCR_PWM0_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_PWM1_CONF_REG, PCR_PWM1_RST_EN);

    // Reset crypto peripherals. This ensures a clean state for the crypto peripherals after a CPU restart
    // and hence avoiding any possibility with crypto failure in ROM security workflows.
    // We also avoid resetting all the crypto peripherals at once because it would create a period when
    // all the peripherals are reset at the same time, which triggers a hardware SEC reset. The SEC reset
    // causes the crypto -> APB path to be reset, but the APB -> crypto path is not reset. This asymmetry
    // results in the crypto module hanging and refusing all access.
    SET_PERI_REG_MASK(PCR_AES_CONF_REG, PCR_AES_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_AES_CONF_REG, PCR_AES_RST_EN);
    SET_PERI_REG_MASK(PCR_ECC_CONF_REG, PCR_ECC_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_ECC_CONF_REG, PCR_ECC_RST_EN);
    SET_PERI_REG_MASK(PCR_ECDSA_CONF_REG, PCR_ECDSA_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_ECDSA_CONF_REG, PCR_ECDSA_RST_EN);
    SET_PERI_REG_MASK(PCR_HMAC_CONF_REG, PCR_HMAC_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_HMAC_CONF_REG, PCR_HMAC_RST_EN);
    SET_PERI_REG_MASK(PCR_SHA_CONF_REG, PCR_SHA_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_SHA_CONF_REG, PCR_SHA_RST_EN);

    // UART's sclk is controlled in the PCR register and does not reset with the UART module. The ROM missed enabling
    // it when initializing the ROM UART. If it is not turned on, it will trigger LP_WDT in the ROM.
    uart_ll_sclk_enable(&UART0);
}

/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void esp_restart_noos(void)
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
    Cache_Disable_Cache(CACHE_MAP_ALL);

    esp_system_reset_modules_on_exit();

    // Set CPU back to XTAL source, same as hard reset, but keep BBPLL on so that USB Serial JTAG can log at 1st stage bootloader.
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_set_to_default_config();
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
    ESP_INFINITE_LOOP();
}
