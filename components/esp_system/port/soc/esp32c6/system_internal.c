/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
#include "soc/rtc_periph.h"
#include "soc/uart_reg.h"
#include "hal/wdt_hal.h"
#include "esp_private/cache_err_int.h"

#include "esp32c6/rom/cache.h"
#include "esp32c6/rom/rtc.h"
#include "soc/pcr_reg.h"

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

    // C6 is a single core SoC, no need to reset and stall the other CPU

    // Disable TG0/TG1 watchdogs
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    // Flush any data left in UART FIFOs
    esp_rom_uart_tx_wait_idle(0);
    esp_rom_uart_tx_wait_idle(1);
    // Disable cache
    Cache_Disable_ICache();

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    // Moved to module internal
    // SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG,
    //                   SYSTEM_WIFIBB_RST | SYSTEM_FE_RST |            // TODO: IDF-5680 (esp_phy)
    //                   SYSTEM_WIFIMAC_RST |                           // TODO: IDF-5679 (esp_wifi)
    //                   SYSTEM_SDIO_RST |                              // SDIO_HINF_HINF_SDIO_RST?
    //                   SYSTEM_EMAC_RST | SYSTEM_MACPWR_RST |          // TODO: IDF-5325 (ethernet)
    //                   SYSTEM_BTBB_RST | SYSTEM_BTBB_REG_RST |        // TODO: IDF-5727 (bt)
    //                   SYSTEM_RW_BTMAC_RST | SYSTEM_RW_BTLP_RST | SYSTEM_RW_BTMAC_REG_RST | SYSTEM_RW_BTLP_REG_RST);
    // REG_WRITE(SYSTEM_CORE_RST_EN_REG, 0);

    // Set Peripheral clk rst
    SET_PERI_REG_MASK(PCR_TIMERGROUP0_CONF_REG, PCR_TG0_RST_EN);
    SET_PERI_REG_MASK(PCR_TIMERGROUP1_CONF_REG, PCR_TG1_RST_EN);
    SET_PERI_REG_MASK(PCR_MSPI_CONF_REG, PCR_MSPI_RST_EN);
    SET_PERI_REG_MASK(PCR_UART0_CONF_REG, PCR_UART0_RST_EN);
    SET_PERI_REG_MASK(PCR_UART1_CONF_REG, PCR_UART1_RST_EN);
    SET_PERI_REG_MASK(PCR_SYSTIMER_CONF_REG, PCR_SYSTIMER_RST_EN);
    SET_PERI_REG_MASK(PCR_GDMA_CONF_REG, PCR_GDMA_RST_EN);
    SET_PERI_REG_MASK(PCR_SDIO_SLAVE_CONF_REG, PCR_SDIO_SLAVE_RST_EN);
    SET_PERI_REG_MASK(PCR_MODEM_APB_CONF_REG, PCR_MODEM_RST_EN);

    // Clear Peripheral clk rst
    CLEAR_PERI_REG_MASK(PCR_TIMERGROUP0_CONF_REG, PCR_TG0_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_TIMERGROUP1_CONF_REG, PCR_TG1_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_MSPI_CONF_REG, PCR_MSPI_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_UART0_CONF_REG, PCR_UART0_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_UART1_CONF_REG, PCR_UART1_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_SYSTIMER_CONF_REG, PCR_SYSTIMER_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_GDMA_CONF_REG, PCR_GDMA_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_SDIO_SLAVE_CONF_REG, PCR_SDIO_SLAVE_RST_EN);
    CLEAR_PERI_REG_MASK(PCR_MODEM_APB_CONF_REG, PCR_MODEM_RST_EN);

    // Set CPU back to XTAL source, no PLL, same as hard reset
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_freq_set_xtal();
#endif

    // Reset PRO CPU
    esp_rom_software_reset_cpu(0);
    while (true) {
        ;
    }
}
