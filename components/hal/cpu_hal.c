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
#include <stdlib.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include "hal/cpu_hal.h"
#include "hal/cpu_types.h"

#include "soc/soc_caps.h"


#if SOC_CPU_BREAKPOINTS_NUM > 0
void cpu_hal_set_breakpoint(int id, const void* addr)
{
    cpu_ll_set_breakpoint(id, cpu_ll_ptr_to_pc(addr));
}

void cpu_hal_clear_breakpoint(int id)
{
    cpu_ll_clear_breakpoint(id);
}
#endif // SOC_CPU_BREAKPOINTS_NUM > 0

#if SOC_CPU_WATCHPOINTS_NUM > 0
void cpu_hal_set_watchpoint(int id, const void* addr, size_t size, watchpoint_trigger_t trigger)
{
    bool on_read = false, on_write = false;

    if (trigger == WATCHPOINT_TRIGGER_ON_RO) {
        on_read = true;
    } else if (trigger == WATCHPOINT_TRIGGER_ON_WO) {
        on_write = true;
    } else {
        on_read = on_write = true;
    }

    cpu_ll_set_watchpoint(id, addr, size, on_read, on_write);
}

void cpu_hal_clear_watchpoint(int id)
{
    cpu_ll_clear_watchpoint(id);
}
#endif // SOC_CPU_WATCHPOINTS_NUM > 0

void cpu_hal_set_vecbase(const void* base)
{
    cpu_ll_set_vecbase(base);
}
