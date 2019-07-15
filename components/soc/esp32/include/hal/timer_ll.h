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

#ifdef __cplusplus
}
#endif
