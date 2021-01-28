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

#include <sys/param.h>
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#include "hal/systimer_types.h"
#include "soc/systimer_caps.h"
#include "hal/clk_gate_ll.h"

#define SYSTIMER_TICKS_PER_US  (16) // Systimer clock source is fixed to 16MHz

uint64_t systimer_hal_get_counter_value(systimer_counter_id_t counter_id)
{
    uint32_t lo, lo_start, hi;
    /* Set the "update" bit and wait for acknowledgment */
    systimer_ll_counter_snapshot(counter_id);
    while (!systimer_ll_is_counter_value_valid(counter_id));
    /* Read LO, HI, then LO again, check that LO returns the same value.
     * This accounts for the case when an interrupt may happen between reading
     * HI and LO values, and this function may get called from the ISR.
     * In this case, the repeated read will return consistent values.
     */
    lo_start = systimer_ll_get_counter_value_low(counter_id);
    do {
        lo = lo_start;
        hi = systimer_ll_get_counter_value_high(counter_id);
        lo_start = systimer_ll_get_counter_value_low(counter_id);
    } while (lo_start != lo);

    systimer_counter_value_t result = {
        .lo = lo,
        .hi = hi
    };

    return result.val;
}

uint64_t systimer_hal_get_time(systimer_counter_id_t counter_id)
{
    return systimer_hal_get_counter_value(counter_id) / SYSTIMER_TICKS_PER_US;
}

void systimer_hal_set_alarm_target(systimer_alarm_id_t alarm_id, uint64_t target)
{
    systimer_counter_value_t alarm = { .val = target * SYSTIMER_TICKS_PER_US};
    systimer_ll_disable_alarm(alarm_id);
    systimer_ll_set_alarm_target(alarm_id, alarm.val);
    systimer_ll_apply_alarm_value(alarm_id);
    systimer_ll_enable_alarm(alarm_id);
}

void systimer_hal_set_alarm_period(systimer_alarm_id_t alarm_id, uint32_t period)
{
    systimer_ll_disable_alarm(alarm_id);
    systimer_ll_set_alarm_period(alarm_id, period * SYSTIMER_TICKS_PER_US);
    systimer_ll_apply_alarm_value(alarm_id);
    systimer_ll_enable_alarm(alarm_id);
}

uint64_t systimer_hal_get_alarm_value(systimer_alarm_id_t alarm_id)
{
    return systimer_ll_get_alarm_target(alarm_id);
}

void systimer_hal_enable_alarm_int(systimer_alarm_id_t alarm_id)
{
    systimer_ll_enable_alarm_int(alarm_id);
}

void systimer_hal_on_apb_freq_update(uint32_t apb_ticks_per_us)
{
    /* Nothing to do here, SYSTIMER clock is independent of APB clock */
    (void)apb_ticks_per_us;
}

void systimer_hal_counter_value_advance(systimer_counter_id_t counter_id, int64_t time_us)
{
    systimer_counter_value_t new_count = { .val = systimer_hal_get_counter_value(counter_id) + time_us * SYSTIMER_TICKS_PER_US };
    systimer_ll_set_counter_value(counter_id, new_count.val);
    systimer_ll_apply_counter_value(counter_id);
}

void systimer_hal_enable_counter(systimer_counter_id_t counter_id)
{
    systimer_ll_enable_counter(counter_id);
}

void systimer_hal_init(void)
{
    periph_ll_enable_clk_clear_rst(PERIPH_SYSTIMER_MODULE);
    systimer_ll_enable_clock();
}

void systimer_hal_select_alarm_mode(systimer_alarm_id_t alarm_id, systimer_alarm_mode_t mode)
{
    switch (mode) {
    case SYSTIMER_ALARM_MODE_ONESHOT:
        systimer_ll_enable_alarm_oneshot(alarm_id);
        break;
    case SYSTIMER_ALARM_MODE_PERIOD:
        systimer_ll_enable_alarm_period(alarm_id);
        break;
    default:
        break;
    }
}

void systimer_hal_connect_alarm_counter(systimer_alarm_id_t alarm_id, systimer_counter_id_t counter_id)
{
    systimer_ll_connect_alarm_counter(alarm_id, counter_id);
}

void systimer_hal_counter_can_stall_by_cpu(uint32_t counter_id, uint32_t cpu_id, bool can)
{
    systimer_ll_counter_can_stall_by_cpu(counter_id, cpu_id, can);
}
