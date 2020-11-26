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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/systimer_reg.h"

// All these functions get invoked either from ISR or HAL that linked to IRAM.
// Always inline these functions even no gcc optimization is applied.

/*******************counter*************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_clock(void)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_CONF_REG, SYS_TIMER_CLK_EN);
}

__attribute__((always_inline)) static inline void systimer_ll_enable_counter(uint32_t counter_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_CONF_REG, 1 << (30 - counter_id));
}

__attribute__((always_inline)) static inline void systimer_ll_counter_can_stall_by_cpu(uint32_t counter_id, uint32_t cpu_id, bool can)
{
    if (can) {
        REG_SET_BIT(SYS_TIMER_SYSTIMER_CONF_REG, 1 << ((28 - counter_id * 2) - cpu_id));
    } else {
        REG_CLR_BIT(SYS_TIMER_SYSTIMER_CONF_REG, 1 << ((28 - counter_id * 2) - cpu_id));
    }
}

__attribute__((always_inline)) static inline void systimer_ll_counter_snapshot(uint32_t counter_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_UNIT0_OP_REG + 4 * counter_id, 1 << 30);
}

__attribute__((always_inline)) static inline bool systimer_ll_is_counter_value_valid(uint32_t counter_id)
{
    return REG_GET_BIT(SYS_TIMER_SYSTIMER_UNIT0_OP_REG + 4 * counter_id, 1 << 29);
}

__attribute__((always_inline)) static inline void systimer_ll_set_counter_value(uint32_t counter_id, uint64_t value)
{
    REG_WRITE(SYS_TIMER_SYSTIMER_UNIT0_LOAD_LO_REG + 8 * counter_id, value & 0xFFFFFFFF);
    REG_WRITE(SYS_TIMER_SYSTIMER_UNIT0_LOAD_HI_REG, (value >> 32) & 0xFFFFF);
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_low(uint32_t counter_id)
{
    return REG_READ(SYS_TIMER_SYSTIMER_UNIT0_VALUE_LO_REG + 8 * counter_id);
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_high(uint32_t counter_id)
{
    return REG_READ(SYS_TIMER_SYSTIMER_UNIT0_VALUE_HI_REG + 8 * counter_id);
}

__attribute__((always_inline)) static inline void systimer_ll_apply_counter_value(uint32_t counter_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_UNIT0_LOAD_REG + 4 * counter_id, SYS_TIMER_TIMER_UNIT0_LOAD);
}

/*******************alarm*************************/

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_target(uint32_t alarm_id, uint64_t value)
{
    REG_WRITE(SYS_TIMER_SYSTIMER_TARGET0_LO_REG + alarm_id * 8, value & 0xFFFFFFFF);
    REG_WRITE(SYS_TIMER_SYSTIMER_TARGET0_HI_REG + alarm_id * 8,  (value >> 32) & 0xFFFFF);
}

__attribute__((always_inline)) static inline uint64_t systimer_ll_get_alarm_target(uint32_t alarm_id)
{
    return ((uint64_t) REG_READ(SYS_TIMER_SYSTIMER_TARGET0_HI_REG + alarm_id * 8) << 32) \
           | REG_READ(SYS_TIMER_SYSTIMER_TARGET0_LO_REG + alarm_id * 8);
}

__attribute__((always_inline)) static inline void systimer_ll_connect_alarm_counter(uint32_t alarm_id, uint32_t counter_id)
{
    REG_SET_FIELD(SYS_TIMER_SYSTIMER_TARGET0_CONF_REG + 4 * alarm_id, SYS_TIMER_TARGET0_TIMER_UNIT_SEL, counter_id);
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_oneshot(uint32_t alarm_id)
{
    REG_CLR_BIT(SYS_TIMER_SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, SYS_TIMER_TARGET0_PERIOD_MODE);
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_period(uint32_t alarm_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, SYS_TIMER_TARGET0_PERIOD_MODE);
}

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_period(uint32_t alarm_id, uint32_t period)
{
    REG_SET_FIELD(SYS_TIMER_SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, SYS_TIMER_TARGET0_PERIOD, period);
}

__attribute__((always_inline)) static inline void systimer_ll_apply_alarm_value(uint32_t alarm_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_COMP0_LOAD_REG + alarm_id * 4, SYS_TIMER_TIMER_COMP0_LOAD);
}

__attribute__((always_inline)) static inline void systimer_ll_disable_alarm(uint32_t alarm_id)
{
    REG_CLR_BIT(SYS_TIMER_SYSTIMER_CONF_REG, 1 << (24 - alarm_id));
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm(uint32_t alarm_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_CONF_REG, 1 << (24 - alarm_id));
}

/*******************interrupt*************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_int(uint32_t alarm_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_INT_ENA_REG, 1 << alarm_id);
}

__attribute__((always_inline)) static inline void systimer_ll_disable_alarm_int(uint32_t alarm_id)
{
    REG_CLR_BIT(SYS_TIMER_SYSTIMER_INT_ENA_REG, 1 << alarm_id);
}

__attribute__((always_inline)) static inline bool systimer_ll_is_alarm_int_fired(uint32_t alarm_id)
{
    return REG_GET_BIT(SYS_TIMER_SYSTIMER_INT_RAW_REG, 1 << alarm_id);
}

__attribute__((always_inline)) static inline void systimer_ll_clear_alarm_int(uint32_t alarm_id)
{
    REG_SET_BIT(SYS_TIMER_SYSTIMER_INT_CLR_REG, 1 << alarm_id);
}

#ifdef __cplusplus
}
#endif
