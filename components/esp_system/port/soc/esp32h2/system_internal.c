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
#include "riscv/interrupt.h"
#include "esp_rom_uart.h"
#include "soc/gpio_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "soc/rtc_periph.h"
#include "soc/uart_reg.h"
#include "hal/wdt_hal.h"
#include "hal/spimem_flash_ll.h"
#include "esp_private/cache_err_int.h"
#include "esp_private/mspi_timing_tuning.h"

#include "esp32h2/rom/cache.h"
#include "esp32h2/rom/rtc.h"
#include "soc/pcr_reg.h"

void IRAM_ATTR esp_system_reset_modules_on_exit(void)
{
    // Flush any data left in UART FIFOs before reset the UART peripheral
    esp_rom_uart_tx_wait_idle(0);
    esp_rom_uart_tx_wait_idle(1);

    // Set Peripheral clk rst
    SET_PERI_REG_MASK(PCR_MSPI_CONF_REG, PCR_MSPI_RST_EN);
    SET_PERI_REG_MASK(PCR_UART0_CONF_REG, PCR_UART0_RST_EN);
    SET_PERI_REG_MASK(PCR_UART1_CONF_REG, PCR_UART1_RST_EN);
    SET_PERI_REG_MASK(PCR_SYSTIMER_CONF_REG, PCR_SYSTIMER_RST_EN);
    SET_PERI_REG_MASK(PCR_GDMA_CONF_REG, PCR_GDMA_RST_EN);
    SET_PERI_REG_MASK(PCR_MODEM_CONF_REG, PCR_MODEM_RST_EN);
    SET_PERI_REG_MASK(PCR_PWM_CONF_REG, PCR_PWM_RST_EN);

    // Clear Peripheral clk rst
    CLEAR_PERI_REG_MASK(PCR_MSPI_CONF_REG, PCR_MSPI_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_UART0_CONF_REG, PCR_UART0_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_UART1_CONF_REG, PCR_UART1_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_SYSTIMER_CONF_REG, PCR_SYSTIMER_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_GDMA_CONF_REG, PCR_GDMA_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_MODEM_CONF_REG, PCR_MODEM_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_PWM_CONF_REG, PCR_PWM_RST_EN);
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
    Cache_Disable_ICache();

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);

    esp_system_reset_modules_on_exit();

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    /**
     * Turn down MSPI speed
     *
     * We set MSPI clock to a high speed one before, ROM doesn't have such high speed clock source option.
     * This function will change clock source to a ROM supported one when system restarts.
     */
    mspi_timing_change_speed_mode_cache_safe(true);
#endif  //#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

    // Set CPU back to XTAL source, same as hard reset, but keep BBPLL on so that USB Serial JTAG can log at 1st stage bootloader.
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_set_to_default_config();
#endif

    // Reset CPU
    esp_rom_software_reset_cpu(0);

    while (true) {
        ;
    }
}
