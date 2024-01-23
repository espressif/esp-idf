/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_PM_TRACE_IDLE,
    ESP_PM_TRACE_TICK,
    ESP_PM_TRACE_FREQ_SWITCH,
    ESP_PM_TRACE_CCOMPARE_UPDATE,
    ESP_PM_TRACE_ISR_HOOK,
    ESP_PM_TRACE_SLEEP,
    ESP_PM_TRACE_TYPE_MAX
} esp_pm_trace_event_t;

void esp_pm_trace_init(void);
void esp_pm_trace_enter(esp_pm_trace_event_t event, int core_id);
void esp_pm_trace_exit(esp_pm_trace_event_t event, int core_id);

#ifdef CONFIG_PM_TRACE

#define ESP_PM_TRACE_ENTER(event, core_id)   \
            esp_pm_trace_enter(ESP_PM_TRACE_ ## event, core_id)
#define ESP_PM_TRACE_EXIT(event, core_id)    \
            esp_pm_trace_exit(ESP_PM_TRACE_ ## event, core_id)

#else // CONFIG_PM_TRACE

#define ESP_PM_TRACE_ENTER(type, core_id) do { (void) core_id; } while(0)
#define ESP_PM_TRACE_EXIT(type, core_id)  do { (void) core_id; } while(0)

#endif // CONFIG_PM_TRACE

#ifdef __cplusplus
}
#endif
