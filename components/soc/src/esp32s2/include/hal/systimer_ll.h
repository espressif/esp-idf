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
#include "hal/hal_defs.h"

// All these functions get invoked either from ISR or HAL that linked to IRAM.
// Always inline these functions even no gcc optimization is applied.

/*******************counter*************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_clock(void)
{
    REG_SET_BIT(SYSTIMER_CONF_REG, SYSTIMER_CLK_EN);
}

__attribute__((always_inline)) static inline void systimer_ll_apply_counter_value(void)
{
    REG_SET_BIT(SYSTIMER_LOAD_REG, SYSTIMER_TIMER_LOAD);
}

__attribute__((always_inline)) static inline void systimer_ll_load_counter_value(uint64_t value)
{
    REG_WRITE(SYSTIMER_LOAD_LO_REG, value & 0xFFFFFFFF);
    REG_WRITE(SYSTIMER_LOAD_HI_REG, (value & 0xFFFFFFFF00000000) >> 32);
}

__attribute__((always_inline)) static inline void systimer_ll_set_step_for_pll(uint32_t step)
{
    REG_SET_FIELD(SYSTIMER_STEP_REG, SYSTIMER_TIMER_PLL_STEP, step);
}

__attribute__((always_inline)) static inline void systimer_ll_set_step_for_xtal(uint32_t step)
{
    REG_SET_FIELD(SYSTIMER_STEP_REG, SYSTIMER_TIMER_XTAL_STEP, step);
}

__attribute__((always_inline)) static inline void systimer_ll_counter_snapshot(void)
{
    REG_WRITE(SYSTIMER_UPDATE_REG, SYSTIMER_TIMER_UPDATE);
}

__attribute__((always_inline)) static inline bool systimer_ll_is_counter_value_valid(void)
{
    return REG_GET_BIT(SYSTIMER_UPDATE_REG, SYSTIMER_TIMER_VALUE_VALID);
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_low(void)
{
    return REG_READ(SYSTIMER_VALUE_LO_REG);
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_high(void)
{
    return REG_READ(SYSTIMER_VALUE_HI_REG);
}

/*******************alarm*************************/

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_value(uint32_t alarm_id, uint64_t value)
{
    REG_WRITE(SYSTIMER_TARGET0_LO_REG + alarm_id * 8, value & 0xFFFFFFFF);
    REG_WRITE(SYSTIMER_TARGET0_HI_REG + alarm_id * 8, (value & 0xFFFFFFFF00000000) >> 32);
}

__attribute__((always_inline)) static inline uint64_t systimer_ll_get_alarm_value(uint32_t alarm_id)
{
    return (uint64_t)REG_READ(SYSTIMER_TARGET0_HI_REG + alarm_id * 8) << 32 | REG_READ(SYSTIMER_TARGET0_LO_REG + alarm_id * 8);
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm(uint32_t alarm_id)
{
    REG_SET_BIT(SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, BIT(31));
}

__attribute__((always_inline)) static inline void systimer_ll_disable_alarm(uint32_t alarm_id)
{
    REG_CLR_BIT(SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, BIT(31));
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_oneshot(uint32_t alarm_id)
{
    REG_CLR_BIT(SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, BIT(30));
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_period(uint32_t alarm_id)
{
    REG_SET_BIT(SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, BIT(30));
}

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_period(uint32_t alarm_id, uint32_t period)
{
    REG_SET_FIELD(SYSTIMER_TARGET0_CONF_REG + alarm_id * 4, SYSTIMER_TARGET0_PERIOD, period);
}

/*******************interrupt*************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm_int(uint32_t alarm_id)
{
    REG_SET_BIT(SYSTIMER_INT_ENA_REG, 1 << alarm_id);
}

__attribute__((always_inline)) static inline void systimer_ll_disable_alarm_int(uint32_t alarm_id)
{
    REG_CLR_BIT(SYSTIMER_INT_ENA_REG, 1 << alarm_id);
}

__attribute__((always_inline)) static inline bool systimer_ll_is_alarm_int_fired(uint32_t alarm_id)
{
    return REG_GET_BIT(SYSTIMER_INT_RAW_REG, 1 << alarm_id);
}

__attribute__((always_inline)) static inline void systimer_ll_clear_alarm_int(uint32_t alarm_id)
{
    REG_SET_BIT(SYSTIMER_INT_CLR_REG, 1 << alarm_id);
}

#ifdef __cplusplus
}
#endif
