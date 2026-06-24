/*
 * SPDX-FileCopyrightText: 2016-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_private/pm_trace.h"
#include "driver/gpio.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include "freertos/FreeRTOS.h"

/* GPIOs to use for tracing of esp_pm events.
 * Array structure: s_trace_io[core_id][event_type]
 *   - First dimension: core_id, indexed from 0 to (portNUM_PROCESSORS - 1)
 *   - Second dimension: event_type, indexed by esp_pm_trace_event_t enum values
 *
 * Event type mapping (second dimension index):
 *   0: ESP_PM_TRACE_IDLE
 *   1: ESP_PM_TRACE_TICK
 *   2: ESP_PM_TRACE_FREQ_SWITCH
 *   3: ESP_PM_TRACE_CCOMPARE_UPDATE
 *   4: ESP_PM_TRACE_ISR_HOOK
 *   5: ESP_PM_TRACE_SLEEP
 *
 * NOTE: These GPIOs can be modified for debugging purposes.
 * WARNING: Do not use GPIOs that are utilized as MSPI IOs, otherwise the chip may crash.
 */
static const int DRAM_ATTR s_trace_io[portNUM_PROCESSORS][ESP_PM_TRACE_TYPE_MAX] = {
#if CONFIG_IDF_TARGET_ESP32
    {BIT(4),  BIT(16), BIT(18), BIT(19), BIT(25), BIT(27)}, // Core 0
#if portNUM_PROCESSORS > 1
    {BIT(5),  BIT(17), BIT(18), BIT(19), BIT(26), BIT(27)}, // Core 1
#endif
#elif CONFIG_IDF_TARGET_ESP32S2
    {BIT(4),  BIT(16), BIT(18), BIT(19), BIT(20), BIT(21)}, // Core 0
#elif CONFIG_IDF_TARGET_ESP32S3
    {BIT(4),  BIT(6),  BIT(14), BIT(15), BIT(16), BIT(18)}, // Core 0
#if portNUM_PROCESSORS > 1
    {BIT(5),  BIT(7),  BIT(14), BIT(15), BIT(17), BIT(18)}, // Core 1
#endif
#elif CONFIG_IDF_TARGET_ESP32H2
    {BIT(2),  BIT(4),  BIT(6),  BIT(7),  BIT(8),  BIT(10)}, // Core 0
#elif CONFIG_IDF_TARGET_ESP32C5
    {BIT(2),  BIT(4),  BIT(6),  BIT(7),  BIT(8),  BIT(23)}, // Core 0
#else
    {BIT(2),  BIT(4),  BIT(6),  BIT(7),  BIT(8),  BIT(18)}, // Core 0
#if portNUM_PROCESSORS > 1
    {BIT(3),  BIT(5),  BIT(6),  BIT(7),  BIT(9),  BIT(18)}, // Core 1
#endif
#endif
};

void esp_pm_trace_init(void)
{
    for (int core = 0; core < portNUM_PROCESSORS; core++) {
        for (int event = 0; event < ESP_PM_TRACE_TYPE_MAX; event++) {
            int io = __builtin_ffs(s_trace_io[core][event]);
            if (io == 0) {
                continue;
            }
            gpio_set_direction(io - 1, GPIO_MODE_OUTPUT);
        }
    }
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
    /* to force tracing GPIOs to keep state */
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif
}

void IRAM_ATTR esp_pm_trace_enter(esp_pm_trace_event_t event, int core_id)
{
    REG_WRITE(GPIO_OUT_W1TS_REG, s_trace_io[core_id][event]);
}

void IRAM_ATTR esp_pm_trace_exit(esp_pm_trace_event_t event, int core_id)
{
    REG_WRITE(GPIO_OUT_W1TC_REG, s_trace_io[core_id][event]);
}
