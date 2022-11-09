/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_private/pm_trace.h"
#include "driver/gpio.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"

/* GPIOs to use for tracing of esp_pm events.
 * Two entries in the array for each type, one for each CPU.
 * Feel free to change when debugging.
 */
static const int DRAM_ATTR s_trace_io[] = {
#if !defined(CONFIG_IDF_TARGET_ESP32C3) && !defined(CONFIG_IDF_TARGET_ESP32H4) && !defined(CONFIG_IDF_TARGET_ESP32C2)
    BIT(4),  BIT(5),  // ESP_PM_TRACE_IDLE
    BIT(16), BIT(17), // ESP_PM_TRACE_TICK
    BIT(18), BIT(18), // ESP_PM_TRACE_FREQ_SWITCH
    BIT(19), BIT(19), // ESP_PM_TRACE_CCOMPARE_UPDATE
    BIT(25), BIT(26), // ESP_PM_TRACE_ISR_HOOK
    BIT(27), BIT(27), // ESP_PM_TRACE_SLEEP
#else
    BIT(2),  BIT(3),  // ESP_PM_TRACE_IDLE
    BIT(4),  BIT(5),  // ESP_PM_TRACE_TICK
    BIT(6),  BIT(6),  // ESP_PM_TRACE_FREQ_SWITCH
    BIT(7),  BIT(7),  // ESP_PM_TRACE_CCOMPARE_UPDATE
    BIT(8),  BIT(9),  // ESP_PM_TRACE_ISR_HOOK
    BIT(18), BIT(18), // ESP_PM_TRACE_SLEEP
#endif
};

void esp_pm_trace_init(void)
{
    for (size_t i = 0; i < sizeof(s_trace_io) / sizeof(s_trace_io[0]); ++i) {
        int io = __builtin_ffs(s_trace_io[i]);
        if (io == 0) {
            continue;
        }
        gpio_set_direction(io - 1, GPIO_MODE_OUTPUT);
    }
}

void IRAM_ATTR esp_pm_trace_enter(esp_pm_trace_event_t event, int core_id)
{
    REG_WRITE(GPIO_OUT_W1TS_REG, s_trace_io[2 * event + core_id]);
}

void IRAM_ATTR esp_pm_trace_exit(esp_pm_trace_event_t event, int core_id)
{
    REG_WRITE(GPIO_OUT_W1TC_REG, s_trace_io[2 * event + core_id]);
}
