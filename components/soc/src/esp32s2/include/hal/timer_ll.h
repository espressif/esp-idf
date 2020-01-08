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

#include <stdlib.h>
#include "hal/timer_types.h"
#include "soc/timer_periph.h"

_Static_assert(TIMER_INTR_T0 == TIMG_T0_INT_CLR, "Add mapping to LL interrupt handling, since it's no longer naturally compatible with the timer_intr_t");
_Static_assert(TIMER_INTR_T1 == TIMG_T1_INT_CLR, "Add mapping to LL interrupt handling, since it's no longer naturally compatible with the timer_intr_t");
_Static_assert(TIMER_INTR_WDT == TIMG_WDT_INT_CLR, "Add mapping to LL interrupt handling, since it's no longer naturally compatible with the timer_intr_t");

typedef struct {
    timg_dev_t *dev;
    timer_idx_t idx;
} timer_ll_context_t;

// Get timer group instance with giving group number
#define TIMER_LL_GET_HW(num) ((num == 0) ? (&TIMERG0) : (&TIMERG1))

/**
 * @brief Set timer clock prescale value
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param divider Prescale value
 *
 * @return None
 */
static inline void timer_ll_set_divider(timg_dev_t *hw, timer_idx_t timer_num, uint16_t divider)
{
    int timer_en = hw->hw_timer[timer_num].config.enable;
    hw->hw_timer[timer_num].config.enable = 0;
    hw->hw_timer[timer_num].config.divider = divider;
    hw->hw_timer[timer_num].config.enable = timer_en;
}

/**
 * @brief Get timer clock prescale value
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param divider Pointer to accept the prescale value
 *
 * @return None
 */
static inline void timer_ll_get_divider(timg_dev_t *hw, timer_idx_t timer_num, uint16_t *divider)
{
    *divider = hw->hw_timer[timer_num].config.divider;
}

/**
 * @brief Load counter value into time-base counter
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param load_val Counter value
 *
 * @return None
 */
static inline void timer_ll_set_counter_value(timg_dev_t *hw, timer_idx_t timer_num, uint64_t load_val)
{
    hw->hw_timer[timer_num].load_high = (uint32_t) (load_val >> 32);
    hw->hw_timer[timer_num].load_low = (uint32_t) load_val;
    hw->hw_timer[timer_num].reload = 1;
}

/**
 * @brief Get counter value from time-base counter
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param timer_val Pointer to accept the counter value
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_get_counter_value(timg_dev_t *hw, timer_idx_t timer_num, uint64_t *timer_val)
{
    hw->hw_timer[timer_num].update.update = 1;
    *timer_val = ((uint64_t) hw->hw_timer[timer_num].cnt_high << 32) | (hw->hw_timer[timer_num].cnt_low);
}

/**
 * @brief Set counter mode, include increment mode and decrement mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param increase_en True to increment mode, fasle to decrement mode
 *
 * @return None
 */
static inline void timer_ll_set_counter_increase(timg_dev_t *hw, timer_idx_t timer_num, bool increase_en)
{
    hw->hw_timer[timer_num].config.increase = increase_en;
}

/**
 * @brief Get counter mode, include increment mode and decrement mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return
 *     - true Increment mode
 *     - false Decrement mode
 */
static inline bool timer_ll_get_counter_increase(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.increase;
}

/**
 * @brief Set counter status, enable or disable counter.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param counter_en True to enable counter, false to disable counter
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_set_counter_enable(timg_dev_t *hw, timer_idx_t timer_num, bool counter_en)
{
    hw->hw_timer[timer_num].config.enable = counter_en;
}

/**
 * @brief Get counter status.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return
 *     - true Enable counter
 *     - false Disable conuter
 */
static inline bool timer_ll_get_counter_enable(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.enable;
}

/**
 * @brief Set auto reload mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param auto_reload_en True to enable auto reload mode, flase to disable auto reload mode
 *
 * @return None
 */
static inline void timer_ll_set_auto_reload(timg_dev_t *hw, timer_idx_t timer_num, bool auto_reload_en)
{
    hw->hw_timer[timer_num].config.autoreload = auto_reload_en;
}

/**
 * @brief Get auto reload mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return
 *     - true Enable auto reload mode
 *     - false Disable auto reload mode
 */
FORCE_INLINE_ATTR bool timer_ll_get_auto_reload(timg_dev_t *hw, timer_idx_t timer_num)
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
FORCE_INLINE_ATTR void timer_ll_set_alarm_value(timg_dev_t *hw, timer_idx_t timer_num, uint64_t alarm_value)
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
 * @param alarm_en True to enable alarm, false to disable alarm
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_set_alarm_enable(timg_dev_t *hw, timer_idx_t timer_num, bool alarm_en)
{
    hw->hw_timer[timer_num].config.alarm_en = alarm_en;
}

/**
 * @brief Get the alarm status.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return
 *     - true Enable alarm
 *     - false Disable alarm
 */
static inline bool timer_ll_get_alarm_enable(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.alarm_en;
}

/**
 * @brief Enable timer interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_intr_enable(timg_dev_t *hw, timer_idx_t timer_num)
{
    hw->int_ena.val |= BIT(timer_num);
}

/**
 * @brief Disable timer interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_intr_disable(timg_dev_t *hw, timer_idx_t timer_num)
{
    hw->int_ena.val &= (~BIT(timer_num));
}

/**
 * @brief Disable timer interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_clear_intr_status(timg_dev_t *hw, timer_idx_t timer_num)
{
    hw->int_clr.val |= BIT(timer_num);
}

/**
 * @brief Get interrupt status.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param intr_status Interrupt status
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_get_intr_status(timg_dev_t *hw, uint32_t *intr_status)
{
    *intr_status = hw->int_st.val;
}

/**
 * @brief Get interrupt raw status.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param intr_raw_status Interrupt raw status
 *
 * @return None
 */
FORCE_INLINE_ATTR void timer_ll_get_intr_raw_status(timer_group_t group_num, uint32_t *intr_raw_status)
{
    timg_dev_t *hw = TIMER_LL_GET_HW(group_num);
    *intr_raw_status = hw->int_raw.val;
}

/**
 * @brief Set the level interrupt status, enable or disable the level interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param level_int_en True to enable level interrupt, false to disable level interrupt
 *
 * @return None
 */
static inline void timer_ll_set_level_int_enable(timg_dev_t *hw, timer_idx_t timer_num, bool level_int_en)
{
    hw->hw_timer[timer_num].config.level_int_en = level_int_en;
}

/**
 * @brief Get the level interrupt status.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return
 *     - true Enable level interrupt
 *     - false Disable level interrupt
 */
static inline bool timer_ll_get_level_int_enable(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.level_int_en;
}

/**
 * @brief Set the edge interrupt status, enable or disable the edge interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 * @param edge_int_en True to enable edge interrupt, false to disable edge interrupt
 *
 * @return None
 */
static inline void timer_ll_set_edge_int_enable(timg_dev_t *hw, timer_idx_t timer_num, bool edge_int_en)
{
    hw->hw_timer[timer_num].config.edge_int_en = edge_int_en;
}

/**
 * @brief Get the edge interrupt status.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param timer_num The timer number
 *
 * @return
 *     - true Enable edge interrupt
 *     - false Disable edge interrupt
 */
static inline bool timer_ll_get_edge_int_enable(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.edge_int_en;
}

/**
 * @brief Get interrupt status register address.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param intr_status_reg Interrupt status register address
 *
 * @return None
 */
static inline void timer_ll_get_intr_status_reg(timg_dev_t *hw, uint32_t *intr_status_reg)
{
    *intr_status_reg = (uint32_t)&(hw->int_st.val);
}

/**
 * @brief Set clock source.
 *
 * @param hal Context of the HAL layer
 * @param use_xtal_en True to use XTAL clock, flase to use APB clock
 *
 * @return None
 */
static inline void timer_ll_set_use_xtal(timg_dev_t *hw, timer_idx_t timer_num, bool use_xtal_en)
{
    hw->hw_timer[timer_num].config.use_xtal = use_xtal_en;
}

/**
 * @brief Get clock source.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Use XTAL clock
 *     - false Use APB clock
 */
static inline bool timer_ll_get_use_xtal(timg_dev_t *hw, timer_idx_t timer_num)
{
    return hw->hw_timer[timer_num].config.use_xtal;
}

/* WDT operations */

/**
 * @brief Unlock/lock the WDT register in case of mis-operations.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param protect true to lock, false to unlock before operations.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_set_protect(timg_dev_t* hw, bool protect)
{
    hw->wdt_wprotect=(protect? 0: TIMG_WDT_WKEY_VALUE);
}

/**
 * @brief Initialize WDT.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @note Call `timer_ll_wdt_set_protect` first
 */
FORCE_INLINE_ATTR void timer_ll_wdt_init(timg_dev_t* hw)
{
    hw->wdt_config0.sys_reset_length=7;                 //3.2uS
    hw->wdt_config0.cpu_reset_length=7;                 //3.2uS
    //currently only level interrupt is supported
    hw->wdt_config0.level_int_en = 1;
    hw->wdt_config0.edge_int_en = 0;
}

/**
 * @brief Set the WDT tick time.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param tick_time_us Tick time.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_set_tick(timg_dev_t* hw, int tick_time_us)
{
    hw->wdt_config1.clk_prescale=80*tick_time_us;
}

/**
 * @brief Feed the WDT.
 *
 * @param hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_feed(timg_dev_t* hw)
{
    hw->wdt_feed = 1;
}

/**
 * @brief Set the WDT timeout.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param stage Stage number of WDT.
 * @param timeout_Tick tick threshold of timeout.
 */
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

/**
 * @brief Set the WDT timeout behavior.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param stage Stage number of WDT.
 * @param behavior Behavior of WDT, please see enum timer_wdt_behavior_t.
 */
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

/**
 * @brief Enable/Disable the WDT enable.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param enable True to enable WDT, false to disable WDT.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_set_enable(timg_dev_t* hw, bool enable)
{
    hw->wdt_config0.en = enable;
}

/**
 * @brief Enable/Disable the WDT flashboot mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param enable True to enable WDT flashboot mode, false to disable WDT flashboot mode.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_flashboot_en(timg_dev_t* hw, bool enable)
{
    hw->wdt_config0.flashboot_mod_en = enable;
}

/**
 * @brief Clear the WDT interrupt status.
 *
 * @param hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_clear_intr_status(timg_dev_t* hw)
{
    hw->int_clr.wdt = 1;
}

/**
 * @brief Enable the WDT interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR void timer_ll_wdt_enable_intr(timg_dev_t* hw)
{
    hw->int_ena.wdt = 1;
}

#ifdef __cplusplus
}
#endif
