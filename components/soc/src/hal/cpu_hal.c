// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>

#include "esp_err.h"

#include "hal/cpu_hal.h"
#include "hal/cpu_types.h"

#include "soc/cpu_caps.h"

#if SOC_CPU_BREAKPOINTS_NUM > 0
esp_err_t cpu_hal_set_breakpoint(int id, const void* addr)
{
    assert(id < SOC_CPU_BREAKPOINTS_NUM && id >= 0);
    cpu_ll_set_breakpoint(id, cpu_ll_ptr_to_pc(addr));
    return ESP_OK;
}

esp_err_t cpu_hal_clear_breakpoint(int id)
{
    assert(id < SOC_CPU_BREAKPOINTS_NUM && id >= 0);
    cpu_ll_clear_breakpoint(id);
    return ESP_OK;
}
#endif // SOC_CPU_BREAKPOINTS_NUM > 0

#if SOC_CPU_WATCHPOINTS_NUM > 0
esp_err_t cpu_hal_set_watchpoint(int id, const void* addr, size_t size, watchpoint_trigger_t trigger)
{
    assert(id < SOC_CPU_WATCHPOINTS_NUM && id >= 0);
    assert(size <= SOC_CPU_WATCHPOINT_SIZE);
    assert(trigger == WATCHPOINT_TRIGGER_ON_RO || 
           trigger == WATCHPOINT_TRIGGER_ON_WO ||
           trigger == WATCHPOINT_TRIGGER_ON_RW);

    bool on_read = false, on_write = false;

    if (trigger == WATCHPOINT_TRIGGER_ON_RO) {
        on_read = true;
    } else if (trigger == WATCHPOINT_TRIGGER_ON_WO) {
        on_write = true;
    } else {
        on_read = on_write = true;
    }

    cpu_ll_set_watchpoint(id, addr, size, on_read, on_write);

    return ESP_OK;
}

esp_err_t cpu_hal_clear_watchpoint(int id)
{
    assert(id < SOC_CPU_WATCHPOINTS_NUM && id >= 0);
    cpu_ll_clear_watchpoint(id);
    return ESP_OK;
}
#endif // SOC_CPU_WATCHPOINTS_NUM > 0