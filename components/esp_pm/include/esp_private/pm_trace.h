// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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