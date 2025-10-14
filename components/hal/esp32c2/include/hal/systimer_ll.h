/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/systimer_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/system_struct.h"
#include "hal/assert.h"

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

static inline void systimer_ll_set_clock_source(soc_periph_systimer_clk_src_t clk_src)
{
    (void)clk_src;
}

static inline soc_periph_systimer_clk_src_t systimer_ll_get_clock_source(void)
{
    return SYSTIMER_CLK_SRC_XTAL;
}

/**
 * @brief Enable the bus clock for systimer module
 *
 * @param enable true to enable, false to disable
 */
static inline void systimer_ll_enable_bus_clock(bool enable)
{
    SYSTEM.perip_clk_en0.systimer_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define systimer_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        systimer_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the systimer module
 *
 * @param group_id Group ID
 */
static inline void systimer_ll_reset_register(void)
{
    SYSTEM.perip_rst_en0.systimer_rst = 1;
    SYSTEM.perip_rst_en0.systimer_rst = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define systimer_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        systimer_ll_reset_register(__VA_ARGS__); \
    } while(0)

/******************* Counter *************************/

__attribute__((always_inline)) static inline void systimer_ll_enable_counter(systimer_dev_t *dev, uint32_t counter_id, bool en)
{
    if (en) {
        dev->conf.val |= 1 << (30 - counter_id);
    } else {
        dev->conf.val &= ~(1 << (30 - counter_id));
    }
}

__attribute__((always_inline)) static inline void systimer_ll_counter_can_stall_by_cpu(systimer_dev_t *dev, uint32_t counter_id, uint32_t cpu_id, bool can)
{
    if (can) {
        dev->conf.val |= 1 << ((28 - counter_id * 2) - cpu_id);
    } else {
        dev->conf.val &= ~(1 << ((28 - counter_id * 2) - cpu_id));
    }
}

__attribute__((always_inline)) static inline void systimer_ll_counter_snapshot(systimer_dev_t *dev, uint32_t counter_id)
{
    dev->unit_op[counter_id].timer_unit_update = 1;
}

__attribute__((always_inline)) static inline bool systimer_ll_is_counter_value_valid(systimer_dev_t *dev, uint32_t counter_id)
{
    return dev->unit_op[counter_id].timer_unit_value_valid;
}

__attribute__((always_inline)) static inline void systimer_ll_set_counter_value(systimer_dev_t *dev, uint32_t counter_id, uint64_t value)
{
    dev->unit_load_val[counter_id].hi.timer_unit_load_hi = value >> 32;
    dev->unit_load_val[counter_id].lo.timer_unit_load_lo = value & 0xFFFFFFFF;
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_low(systimer_dev_t *dev, uint32_t counter_id)
{
    return dev->unit_val[counter_id].lo.timer_unit_value_lo;
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_counter_value_high(systimer_dev_t *dev, uint32_t counter_id)
{
    return dev->unit_val[counter_id].hi.timer_unit_value_hi;
}

__attribute__((always_inline)) static inline void systimer_ll_apply_counter_value(systimer_dev_t *dev, uint32_t counter_id)
{
    dev->unit_load[counter_id].val = 0x01;
}

/******************* Alarm *************************/

__attribute__((always_inline)) static inline void systimer_ll_set_alarm_target(systimer_dev_t *dev, uint32_t alarm_id, uint64_t value)
{
    dev->target_val[alarm_id].hi.timer_target_hi = value >> 32;
    dev->target_val[alarm_id].lo.timer_target_lo = value & 0xFFFFFFFF;
}

__attribute__((always_inline)) static inline uint64_t systimer_ll_get_alarm_target(systimer_dev_t *dev, uint32_t alarm_id)
{
    return ((uint64_t)(dev->target_val[alarm_id].hi.timer_target_hi) << 32) | dev->target_val[alarm_id].lo.timer_target_lo;
}

__attribute__((always_inline)) static inline void systimer_ll_connect_alarm_counter(systimer_dev_t *dev, uint32_t alarm_id, uint32_t counter_id)
{
    dev->target_conf[alarm_id].target_timer_unit_sel = counter_id;
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
    HAL_ASSERT(period < (1 << 26));
    dev->target_conf[alarm_id].target_period = period;
}

__attribute__((always_inline)) static inline uint32_t systimer_ll_get_alarm_period(systimer_dev_t *dev, uint32_t alarm_id)
{
    return dev->target_conf[alarm_id].target_period;
}

__attribute__((always_inline)) static inline void systimer_ll_apply_alarm_value(systimer_dev_t *dev, uint32_t alarm_id)
{
    dev->comp_load[alarm_id].val = 0x01;
}

__attribute__((always_inline)) static inline void systimer_ll_enable_alarm(systimer_dev_t *dev, uint32_t alarm_id, bool en)
{
    if (en) {
        dev->conf.val |= 1 << (24 - alarm_id);
    } else {
        dev->conf.val &= ~(1 << (24 - alarm_id));
    }
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
    return dev->int_st.val & (1 << alarm_id);
}

__attribute__((always_inline)) static inline void systimer_ll_clear_alarm_int(systimer_dev_t *dev, uint32_t alarm_id)
{
    dev->int_clr.val |= 1 << alarm_id;
}

#ifdef __cplusplus
}
#endif
