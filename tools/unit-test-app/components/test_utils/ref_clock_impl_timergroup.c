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

#include "test_utils.h"
#include "driver/periph_ctrl.h"
#include "soc/periph_defs.h"
#include "hal/timer_hal.h"

#define TIMER_GROUP_ID (1)
#define TIMER_ID (0)
static timer_hal_context_t timer_hal;

void ref_clock_init(void)
{
    periph_module_enable(PERIPH_TIMG1_MODULE);
    timer_hal_init(&timer_hal, TIMER_GROUP_ID, TIMER_ID);
    timer_hal_set_use_xtal(&timer_hal, true); // Select XTAL, so ref_clock is independent of the APL clock
    timer_hal_set_divider(&timer_hal, 40); // Resolution is configured to 1MHz
    timer_hal_set_counter_increase(&timer_hal, true); // increase mode
    timer_hal_set_counter_enable(&timer_hal, false); // stop timer from running
    timer_hal_set_counter_value(&timer_hal, 0); // initial count value to zero
    timer_hal_intr_disable(&timer_hal); // disable interrupt
    timer_hal_set_alarm_enable(&timer_hal, false); // we don't need to generate any interrupt
    timer_hal_set_auto_reload(&timer_hal, false);
    timer_hal_set_counter_enable(&timer_hal, true); // start counter
}

void ref_clock_deinit(void)
{
    timer_hal_set_counter_enable(&timer_hal, false); // stop timer from running
    periph_module_disable(PERIPH_TIMG1_MODULE);
}

uint64_t ref_clock_get(void)
{
    uint64_t count_value = 0;
    timer_hal_get_counter_value(&timer_hal, &count_value);
    return count_value;
}
