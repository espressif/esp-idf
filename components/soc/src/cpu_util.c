// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_attr.h"
#include "soc/cpu.h"
#include "soc/soc.h"
#include "soc/rtc_periph.h"
#include "sdkconfig.h"

#include "hal/cpu_hal.h"
#include "esp_debug_helpers.h"
#include "hal/cpu_types.h"

#include "hal/soc_hal.h"
#include "soc/soc_caps.h"

#include "sdkconfig.h"

void IRAM_ATTR esp_cpu_stall(int cpu_id)
{
#if SOC_CPU_CORES_NUM > 1
    soc_hal_stall_core(cpu_id);
#endif
}

void IRAM_ATTR esp_cpu_unstall(int cpu_id)
{
#if SOC_CPU_CORES_NUM > 1
    soc_hal_unstall_core(cpu_id);
#endif
}

void IRAM_ATTR esp_cpu_reset(int cpu_id)
{
    soc_hal_reset_core(cpu_id);
}

esp_err_t IRAM_ATTR esp_set_watchpoint(int no, void *adr, int size, int flags)
{
    watchpoint_trigger_t trigger;

    switch (flags)
    {
    case ESP_WATCHPOINT_LOAD:
        trigger = WATCHPOINT_TRIGGER_ON_RO;
        break;
    case ESP_WATCHPOINT_STORE:
        trigger = WATCHPOINT_TRIGGER_ON_WO;
        break;
    case ESP_WATCHPOINT_ACCESS:
        trigger = WATCHPOINT_TRIGGER_ON_RW;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    cpu_hal_set_watchpoint(no, adr, size, trigger);
    return ESP_OK;
}

void IRAM_ATTR esp_clear_watchpoint(int no)
{
    cpu_hal_clear_watchpoint(no);
}

bool IRAM_ATTR esp_cpu_in_ocd_debug_mode(void)
{
#if (CONFIG_ESP32_DEBUG_OCDAWARE == 1) || \
    (CONFIG_ESP32S2_DEBUG_OCDAWARE == 1)
    return cpu_ll_is_debugger_attached();
#else
    return false; // Always return false if "OCD aware" is disabled
#endif
}

void IRAM_ATTR esp_set_breakpoint_if_jtag(void *fn)
{
    if (esp_cpu_in_ocd_debug_mode()) {
        cpu_hal_set_breakpoint(0, fn);
    }
}