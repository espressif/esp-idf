/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_macros.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/brownout_hal.h"
#include "hal/brownout_ll.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "sdkconfig.h"

#include "esp_tee_intr.h"

/*
 * NOTE: Brownout threshold levels
 *
 * +-----------------+-------------------+
 * | Threshold Level | Voltage (Approx.) |
 * +-----------------+-------------------+
 * | 7               | 2.51V             |
 * | 6               | 2.64V             |
 * | 5               | 2.76V             |
 * | 4               | 2.92V             |
 * | 3               | 3.10V             |
 * | 2               | 3.27V             |
 * +-----------------+-------------------+
 */
#if defined(CONFIG_ESP_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif

static __attribute__((unused)) DRAM_ATTR const char *TAG = "BOD";

#if CONFIG_ESP_SYSTEM_BROWNOUT_INTR
IRAM_ATTR static void rtc_brownout_isr_handler(void *arg)
{
    /* Normally RTC ISR clears the interrupt flag after the application-supplied
     * handler returns. Since restart is called here, the flag needs to be
     * cleared manually.
     */
    brownout_ll_intr_clear();

    // Stop the other core.
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    const uint32_t core_id = esp_cpu_get_core_id();
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_stall(other_core_id);
#endif

    // TODO: Support for resetting the flash during brownout for stability
    ESP_DRAM_LOGI(TAG, "Brownout detector was triggered\r\n\r\n");

    // Flush any data left in UART FIFOs
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (uart_ll_is_enabled(i)) {
            esp_rom_output_tx_wait_idle(i);
        }
    }

    // generate core reset
    esp_rom_software_reset_system();

    ESP_INFINITE_LOOP();
}
#endif // CONFIG_ESP_SYSTEM_BROWNOUT_INTR

void esp_tee_brownout_init(void)
{
#if CONFIG_ESP_SYSTEM_BROWNOUT_INTR
    brownout_hal_config_t cfg = {
        .threshold = BROWNOUT_DET_LVL,
        .enabled = true,
        .reset_enabled = false,
        .flash_power_down = true,
        .rf_power_down = true,
    };

    brownout_hal_config(&cfg);
    brownout_ll_intr_clear();
    // TODO IDF-6606: LP_RTC_TIMER interrupt source is shared by lp_timer and brownout detector, but lp_timer interrupt
    // is not used now. An interrupt allocator is needed when lp_timer intr gets supported.
    struct vector_desc_t bod_vd = { 0, NULL, NULL, NULL };
    bod_vd.source = ETS_LP_RTC_TIMER_INTR_SOURCE;
    bod_vd.isr = rtc_brownout_isr_handler;
    esp_tee_intr_register((void *)&bod_vd);
    brownout_ll_intr_enable(true);

#else // brownout without interrupt

    brownout_hal_config_t cfg = {
        .threshold = BROWNOUT_DET_LVL,
        .enabled = true,
        .reset_enabled = true,
        .flash_power_down = true,
        .rf_power_down = true,
    };

    brownout_hal_config(&cfg);
#endif
}

void esp_tee_brownout_disable(void)
{
    brownout_hal_config_t cfg = {
        .enabled = false,
    };

    brownout_hal_config(&cfg);
#if CONFIG_ESP_SYSTEM_BROWNOUT_INTR
    brownout_ll_intr_enable(false);
#endif // CONFIG_ESP_SYSTEM_BROWNOUT_INTR
}
