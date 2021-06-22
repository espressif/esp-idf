// Copyright 2020-2021 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdbool.h>
#include "soc/systimer_struct.h"
#include "hal/assert.h"

#define SYSTIMER_LL_COUNTER_CLOCK (0) // Counter used for "wallclock" time
#define SYSTIMER_LL_ALARM_CLOCK   (2) // Alarm used for "wallclock" time

#define SYSTIMER_LL_TICKS_PER_US  (80) // 80 systimer ticks == 1us

#ifdef __cplusplus
extern "C" {
#endif

// All these functions get invoked either from ISR or HAL that linked to IRAM.
// Always inline these functions even no gcc optimization is applied.

/******************* Clock *************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_clock(systimer_dev_t *dev, bool en)
{
    dev->conf.clk_en = en;
}

/******************* Counter *************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_counter(systimer_dev_t *dev, uint32_t counter_id, bool en)
{
    // ESP32-S2 only has one counter in systimer group
    (void)dev;
    (void)counter_id;
}

__attribute__((always_inline)) static inline void systimer_ll_counter_can_stall_by_cpu(systimer_dev_t *dev, uint32_t counter_id, uint32_t cpu_id, bool can)
{
    (void)dev;
    (void)counter_id;
    (void)cpu_id;
    (void)can;
}

__attribute__((always_inline)) static inline void systimer_ll_counter_snapshot(systimer_dev_t *dev, uint32_t counter_id)
{
    (void)counter_id;
    dev->update.timer_update = 1;
}

__attribute__((always_inline)) static inline bool systimer_ll_is_counter_value_valid(systimer_dev_t *dev, uint32_t counter_id)
{
    (void)counter_id;
    return dev->update.timer_value_valid;
}

__attribute__((always_inline)) static inline void systimer_ll_set_counter_value(systimer_dev_t *dev, uint32_t counter_id, uint64_t value)
{
    (void)counter_id;
    dev->load_hi.timer_load_hi = value >> 32;
    dev->load_lo.timer_load_lo = value;
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_low(systimer_dev_t *dev, uint32_t counter_id)
{
    return dev->value_lo.timer_value_lo;
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_high(systimer_dev_t *dev, uint32_t counter_id)
{
    return dev->value_hi.timer_value_hi;
}

__attribute__((always_inline)) static inline void systimer_ll_apply_counter_value(systimer_dev_t *dev, uint32_t counter_id)
{
    dev->load.timer_load = 1;
}

__attribute__((always_inline)) static inline void systimer_ll_set_step_for_pll(systimer_dev_t *dev, uint32_t step)
{
    dev->step.timer_pll_step = step;
}

__attribute__((always_inline)) static inline void systimer_ll_set_step_for_xtal(systimer_dev_t *dev, uint32_t step)
{
    dev->step.timer_xtal_step = step;
}

/******************* Alarm *************************/

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_target(systimer_dev_t *dev, uint32_t alarm_id, uint64_t value)
{
    dev->target_val[alarm_id].hi.timer_target_hi = value >> 32;
    dev->target_val[alarm_id].lo.timer_target_lo = value;
}

__attribute__((always_inline)) static inline uint64_t systimer_ll_get_alarm_target(systimer_dev_t *dev, uint32_t alarm_id)
{
    return ((uint64_t)(dev->target_val[alarm_id].hi.timer_target_hi) << 32) | dev->target_val[alarm_id].lo.timer_target_lo;
}

__attribute__((always_inline)) static inline void systimer_ll_connect_alarm_counter(systimer_dev_t *dev, uint32_t alarm_id, uint32_t counter_id)
{
    // On esp32-s2, counter int the systimer is fixed connectred to other three alarm comparators
    (void)dev;
    (void)alarm_id;
    (void)counter_id;
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_oneshot(systimer_dev_t *dev, uint32_t alarm_id)
{
    dev->target_conf[alarm_id].target_period_mode = 0;
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_period(systimer_dev_t *dev, uint32_t alarm_id)
{
    dev->target_conf[alarm_id].target_period_mode = 1;
}

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_period(systimer_dev_t *dev, uint32_t alarm_id, uint32_t period)
{
    HAL_ASSERT(period < (1 << 30));
    dev->target_conf[alarm_id].target_period = period;
}

__attribute__((always_inline)) static inline void systimer_ll_apply_alarm_value(systimer_dev_t *dev, uint32_t alarm_id)
{
    (void)dev;
    (void)alarm_id;
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm(systimer_dev_t *dev, uint32_t alarm_id, bool en)
{
    dev->target_conf[alarm_id].target_work_en = en;
}

/******************* Interrupt *************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_int(systimer_dev_t *dev, uint32_t alarm_id, bool en)
{
    if (en) {
        dev->int_ena.val |= 1 << alarm_id;
    } else {
        dev->int_ena.val &= ~(1 << alarm_id);
    }
}

__attribute__((always_inline)) static inline bool systimer_ll_is_alarm_int_fired(systimer_dev_t *dev, uint32_t alarm_id)
{
    return dev->int_raw.val & (1 << alarm_id);
}

__attribute__((always_inline)) static inline void systimer_ll_clear_alarm_int(systimer_dev_t *dev, uint32_t alarm_id)
{
    dev->int_clr.val |= 1 << alarm_id;
}

#ifdef __cplusplus
}
#endif
