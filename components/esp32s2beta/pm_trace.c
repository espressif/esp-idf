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


#include "esp_private/pm_trace.h"
#include "driver/gpio.h"
#include "soc/gpio_reg.h"

/* GPIOs to use for tracing of esp_pm events.
 * Two entries in the array for each type, one for each CPU.
 * Feel free to change when debugging.
 */
static const int DRAM_ATTR s_trace_io[] = {
        BIT(4),  BIT(5),  // ESP_PM_TRACE_IDLE
        BIT(16), BIT(17), // ESP_PM_TRACE_TICK
        BIT(18), BIT(18), // ESP_PM_TRACE_FREQ_SWITCH
        BIT(19), BIT(19), // ESP_PM_TRACE_CCOMPARE_UPDATE
        BIT(25), BIT(26), // ESP_PM_TRACE_ISR_HOOK
        BIT(27), BIT(27), // ESP_PM_TRACE_SLEEP
};

void esp_pm_trace_init(void)
{
    for (size_t i = 0; i < sizeof(s_trace_io)/sizeof(s_trace_io[0]); ++i) {
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
