// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The LL layer for Timer Group register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/timer_types.h"
#include "soc/timer_periph.h"

//Helper macro to get corresponding interrupt of a timer
#define TIMER_LL_GET_INTR(TIMER_IDX) ((TIMER_IDX)==TIMER_0? TIMER_INTR_T0: TIMER_INTR_T1)

#define TIMER_LL_GET_HW(TIMER_GROUP) ((TIMER_GROUP)==0? &TIMERG0: &TIMERG1)

_Static_assert(TIMER_INTR_T0 == TIMG_T0_INT_CLR, "Add mapping to LL interrupt handling, since it's no longer naturally compatible with the timer_intr_t");
_Static_assert(TIMER_INTR_T1 == TIMG_T1_INT_CLR, "Add mapping to LL interrupt handling, since it's no longer naturally compatible with the timer_intr_t");
_Static_assert(TIMER_INTR_WDT == TIMG_WDT_INT_CLR, "Add mapping to LL interrupt handling, since it's no longer naturally compatible with the timer_intr_t");

/**
 * @brief Enable timer interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param intr_mask Interrupt enable mask
 *
 * @return None
 */
static inline void timer_ll_intr_enable(timg_dev_t *hw, timer_intr_t intr_mask)
{
    hw->int_ena.val |= intr_mask;
}

/**
 * @brief Disable timer interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param intr_mask Interrupt disable mask
 *
 * @return None
 */
static inline void timer_ll_intr_disable(timg_dev_t *hw, timer_intr_t intr_mask)
{
    hw->int_ena.val &= (~intr_mask);
}

/**
 * @brief Get timer interrupt status.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return Masked interrupt status
 */
static inline timer_intr_t timer_ll_intr_status_get(timg_dev_t *hw)
{
    return hw->int_raw.val;
}

/**
 * @brief Clear timer interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param intr_mask Interrupt mask to clear
 *
 * @return None
 */
static inline void timer_ll_intr_status_clear(timg_dev_t *hw, timer_intr_t intr_mask)
{
    hw->int_clr_timers.val = intr_mask;
}

/**
 * @brief Get counter vaule from time-base counter
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param timer_val Pointer to accept the counter value
 *
 * @return None
 */
static inline void timer_ll_get_counter_value(timg_dev_t *hw, timer_idx_t timer_num, uint64_t *timer_val)
{
    hw->hw_timer[timer_num].update = 1;
    *timer_val = ((uint64_t) hw->hw_timer[timer_num].cnt_high << 32) | (hw->hw_timer[timer_num].cnt_low);
}

/**
 * @brief Set counter status, enable or disable counter.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param counter_en Counter enable status
 *
 * @return None
 */
static inline void timer_ll_set_counter_enable(timg_dev_t *hw, timer_idx_t timer_num, timer_start_t counter_en)
{
    hw->hw_timer[timer_num].config.enable = counter_en;
}

/**
 * @brief Get auto reload mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param reload Pointer to accept the auto reload mode
 *
 * @return None
 */
static inline bool timer_ll_get_auto_reload(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.autoreload;
}

/**
 * @brief Set the counter value to trigger the alarm.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param alarm_value Counter value to trigger the alarm
 *
 * @return None
 */
static inline void timer_ll_set_alarm_value(timg_dev_t *hw, timer_idx_t timer_num, uint64_t alarm_value)
{
    hw->hw_timer[timer_num].alarm_high = (uint32_t) (alarm_value >> 32);
    hw->hw_timer[timer_num].alarm_low = (uint32_t) alarm_value;
}

/**
 * @brief Get the counter value to trigger the alarm.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param alarm_value Pointer to accept the counter value to trigger the alarm
 *
 * @return None
 */
static inline void timer_ll_get_alarm_value(timg_dev_t *hw, timer_idx_t timer_num, uint64_t *alarm_value)
{
    *alarm_value = ((uint64_t) hw->hw_timer[timer_num].alarm_high << 32) | (hw->hw_timer[timer_num].alarm_low);
}

/**
 * @brief Set the alarm status, enable or disable the alarm.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param alarm_en true to enable, false to disable
 *
 * @return None
 */
static inline void timer_ll_set_alarm_enable(timg_dev_t *hw, timer_idx_t timer_num, bool alarm_en)
{
    hw->hw_timer[timer_num].config.alarm_en = alarm_en;
}

/**
 * @brief Get the alarm status.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param alarm_en Pointer to accept the alarm status
 *
 * @return None
 */
static inline void timer_ll_get_alarm_enable(timg_dev_t *hw, timer_idx_t timer_num, bool *alarm_en)
{
    *alarm_en = hw->hw_timer[timer_num].config.alarm_en;
}

/* WDT operations */

/**
 * Unlock/lock the WDT register in case of mis-operations.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param protect true to lock, false to unlock before operations.
 */

FORCE_INLINE_ATTR void timer_ll_wdt_set_protect(timg_dev_t* hw, bool protect)
{
    hw->wdt_wprotect=(protect? 0: TIMG_WDT_WKEY_VALUE);
}

/**
 * Initialize WDT.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @note Call ``timer_ll_wdt_set_protect first``
 */
FORCE_INLINE_ATTR void timer_ll_wdt_init(timg_dev_t* hw)
{
    hw->wdt_config0.sys_reset_length=7;                 //3.2uS
    hw->wdt_config0.cpu_reset_length=7;                 //3.2uS
    //currently only level interrupt is supported
    hw->wdt_config0.level_int_en = 1;
    hw->wdt_config0.edge_int_en = 0;
}

FORCE_INLINE_ATTR void timer_ll_wdt_set_tick(timg_dev_t* hw, int tick_time_us)
{
    hw->wdt_config1.clk_prescale=80*tick_time_us;
}

FORCE_INLINE_ATTR void timer_ll_wdt_feed(timg_dev_t* hw)
{
    hw->wdt_feed = 1;
}

FORCE_INLINE_ATTR void timer_ll_wdt_set_timeout(timg_dev_t* hw, int stage, uint32_t timeout_tick)
{
    switch (stage) {
    case 0:
        hw->wdt_config2=timeout_tick;
        break;
    case 1:
        hw->wdt_config3=timeout_tick;
        break;
    case 2:
        hw->wdt_config4=timeout_tick;
        break;
    case 3:
        hw->wdt_config5=timeout_tick;
        break;
    default:
        abort();
    }
}

_Static_assert(TIMER_WDT_OFF == TIMG_WDT_STG_SEL_OFF, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with the timer_wdt_behavior_t");
_Static_assert(TIMER_WDT_INT == TIMG_WDT_STG_SEL_INT, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with the timer_wdt_behavior_t");
_Static_assert(TIMER_WDT_RESET_CPU == TIMG_WDT_STG_SEL_RESET_CPU, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with the timer_wdt_behavior_t");
_Static_assert(TIMER_WDT_RESET_SYSTEM == TIMG_WDT_STG_SEL_RESET_SYSTEM, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with the timer_wdt_behavior_t");

FORCE_INLINE_ATTR void timer_ll_wdt_set_timeout_behavior(timg_dev_t* hw, int stage, timer_wdt_behavior_t behavior)
{
    switch (stage) {
    case 0:
        hw->wdt_config0.stg0 = behavior;
        break;
    case 1:
        hw->wdt_config0.stg1 = behavior;
        break;
    case 2:
        hw->wdt_config0.stg2 = behavior;
        break;
    case 3:
        hw->wdt_config0.stg3 = behavior;
        break;
    default:
        abort();
    }
}

FORCE_INLINE_ATTR void timer_ll_wdt_set_enable(timg_dev_t* hw, bool enable)
{
    hw->wdt_config0.en = enable;
}

FORCE_INLINE_ATTR void timer_ll_wdt_flashboot_en(timg_dev_t* hw, bool enable)
{
    hw->wdt_config0.flashboot_mod_en = enable;
}


#ifdef __cplusplus
}
#endif
