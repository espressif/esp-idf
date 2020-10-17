// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for Timer Group.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/soc_caps.h"
#include "hal/timer_ll.h"
#include "hal/timer_types.h"

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    timg_dev_t *dev;
    timer_idx_t idx;
} timer_hal_context_t;

/**
 * @brief Init the timer hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param group_num The timer group number
 * @param timer_num The timer number
 *
 * @return None
 */
void timer_hal_init(timer_hal_context_t *hal, timer_group_t group_num, timer_idx_t timer_num);

/**
 * @brief Get interrupt status register address and corresponding control bits mask
 *
 * @param hal Context of the HAL layer
 * @param status_reg[out] interrupt status register address
 * @param mask_bit[out] control bits mask
 */
void timer_hal_get_status_reg_mask_bit(timer_hal_context_t *hal, uint32_t *status_reg, uint32_t *mask_bit);

/**
 * @brief Set timer clock prescale value
 *
 * @param hal Context of the HAL layer
 * @param divider Prescale value
 *
 * @return None
 */
#define timer_hal_set_divider(hal, divider)  timer_ll_set_divider((hal)->dev, (hal)->idx, divider)

/**
 * @brief Get timer clock prescale value
 *
 * @param hal Context of the HAL layer
 * @param divider Pointer to accept the prescale value
 *
 * @return None
 */
#define timer_hal_get_divider(hal, divider)  timer_ll_get_divider((hal)->dev, (hal)->idx, divider)

/**
 * @brief Load counter value into time-base counter
 *
 * @param hal Context of the HAL layer
 * @param load_val Counter value
 *
 * @return None
 */
#define timer_hal_set_counter_value(hal, load_val)  timer_ll_set_counter_value((hal)->dev, (hal)->idx, load_val)

/**
 * @brief Get counter value from time-base counter
 *
 * @param hal Context of the HAL layer
 * @param timer_val Pointer to accept the counter value
 *
 * @return None
 */
#define timer_hal_get_counter_value(hal, timer_val)  timer_ll_get_counter_value((hal)->dev, (hal)->idx, timer_val)

/**
 * @brief Set counter mode, include increment mode and decrement mode.
 *
 * @param hal Context of the HAL layer
 * @param increase_en True to increment mode, fasle to decrement mode
 *
 * @return None
 */
#define timer_hal_set_counter_increase(hal, increase_en)  timer_ll_set_counter_increase((hal)->dev, (hal)->idx, increase_en)

/**
 * @brief Get counter mode, include increment mode and decrement mode.
 *
 * @param hal Context of the HAL layer
 * @param counter_dir Pointer to accept the counter mode
 *
 * @return
 *     - true Increment mode
 *     - false Decrement mode
 */
#define timer_hal_get_counter_increase(hal)  timer_ll_get_counter_increase((hal)->dev, (hal)->idx)

/**
 * @brief Set counter status, enable or disable counter.
 *
 * @param hal Context of the HAL layer
 * @param counter_en True to enable counter, false to disable counter
 *
 * @return None
 */
#define timer_hal_set_counter_enable(hal, counter_en)  timer_ll_set_counter_enable((hal)->dev, (hal)->idx, counter_en)

/**
 * @brief Get counter status.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Enable counter
 *     - false Disable conuter
 */
#define timer_hal_get_counter_enable(hal)  timer_ll_get_counter_enable((hal)->dev, (hal)->idx)

/**
 * @brief Set auto reload mode.
 *
 * @param hal Context of the HAL layer
 * @param auto_reload_en True to enable auto reload mode, flase to disable auto reload mode
 *
 * @return None
 */
#define timer_hal_set_auto_reload(hal, auto_reload_en)  timer_ll_set_auto_reload((hal)->dev, (hal)->idx, auto_reload_en)

/**
 * @brief Get auto reload mode.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Enable auto reload mode
 *     - false Disable auto reload mode
 */
#define timer_hal_get_auto_reload(hal)  timer_ll_get_auto_reload((hal)->dev, (hal)->idx)

/**
 * @brief Set the counter value to trigger the alarm.
 *
 * @param hal Context of the HAL layer
 * @param alarm_value Counter value to trigger the alarm
 *
 * @return None
 */
#define timer_hal_set_alarm_value(hal, alarm_value)  timer_ll_set_alarm_value((hal)->dev, (hal)->idx, alarm_value)

/**
 * @brief Get the counter value to trigger the alarm.
 *
 * @param hal Context of the HAL layer
 * @param alarm_value Pointer to accept the counter value to trigger the alarm
 *
 * @return None
 */
#define timer_hal_get_alarm_value(hal, alarm_value)  timer_ll_get_alarm_value((hal)->dev, (hal)->idx, alarm_value)

/**
 * @brief Set the alarm status, enable or disable the alarm.
 *
 * @param hal Context of the HAL layer
 * @param alarm_en True to enable alarm, false to disable alarm
 *
 * @return None
 */
#define timer_hal_set_alarm_enable(hal, alarm_en)  timer_ll_set_alarm_enable((hal)->dev, (hal)->idx, alarm_en)

/**
 * @brief Get the alarm status.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Enable alarm
 *     - false Disable alarm
 */
#define timer_hal_get_alarm_enable(hal)  timer_ll_get_alarm_enable((hal)->dev, (hal)->idx)

/**
 * @brief Set the level interrupt status, enable or disable the level interrupt.
 *
 * @param hal Context of the HAL layer
 * @param level_int_en True to enable level interrupt, false to disable level interrupt
 *
 * @return None
 */
#define timer_hal_set_level_int_enable(hal, level_int_en)  timer_ll_set_level_int_enable((hal)->dev, (hal)->idx, level_int_en)

/**
 * @brief Get the level interrupt status.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Enable level interrupt
 *     - false Disable level interrupt
 */
#define timer_hal_get_level_int_enable(hal)  timer_ll_get_level_int_enable((hal)->dev, (hal)->idx)

/**
 * @brief Set the edge interrupt status, enable or disable the edge interrupt.
 *
 * @param hal Context of the HAL layer
 * @param edge_int_en True to enable edge interrupt, false to disable edge interrupt
 *
 * @return None
 */
#define timer_hal_set_edge_int_enable(hal, edge_int_en)  timer_ll_set_edge_int_enable((hal)->dev, (hal)->idx, edge_int_en)

/**
 * @brief Get the edge interrupt status.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Enable edge interrupt
 *     - false Disable edge interrupt
 */
#define timer_hal_get_edge_int_enable(hal)  timer_ll_get_edge_int_enable((hal)->dev, (hal)->idx)

/**
 * @brief Enable timer interrupt.
 *
 * @param hal Context of the HAL layer
 *
 * @return None
 */
#define timer_hal_intr_enable(hal)  timer_ll_intr_enable((hal)->dev, (hal)->idx)

/**
 * @brief Disable timer interrupt.
 *
 * @param hal Context of the HAL layer
 *
 * @return None
 */
#define timer_hal_intr_disable(hal)  timer_ll_intr_disable((hal)->dev, (hal)->idx)

/**
 * @brief Clear interrupt status.
 *
 * @param hal Context of the HAL layer
 *
 * @return None
 */
#define timer_hal_clear_intr_status(hal)  timer_ll_clear_intr_status((hal)->dev, (hal)->idx)

/**
 * @brief Get interrupt status.
 *
 * @param hal Context of the HAL layer
 * @param intr_status Interrupt status
 *
 * @return None
 */
#define timer_hal_get_intr_status(hal, intr_status)  timer_ll_get_intr_status((hal)->dev, intr_status)

/**
 * @brief Get interrupt raw status.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param intr_raw_status Interrupt raw status
 *
 * @return None
 */
#define timer_hal_get_intr_raw_status(group_num, intr_raw_status)  timer_ll_get_intr_raw_status(group_num, intr_raw_status)

/**
 * @brief Get interrupt status register address.
 *
 * @param hal Context of the HAL layer
 *
 * @return Interrupt status register address
 */
#define timer_hal_get_intr_status_reg(hal)  timer_ll_get_intr_status_reg((hal)->dev)

#if SOC_TIMER_GROUP_SUPPORT_XTAL
/**
 * @brief Set clock source.
 *
 * @param hal Context of the HAL layer
 * @param use_xtal_en True to use XTAL clock, flase to use APB clock
 *
 * @return None
 */
#define timer_hal_set_use_xtal(hal, use_xtal_en)  timer_ll_set_use_xtal((hal)->dev, (hal)->idx, use_xtal_en)

/**
 * @brief Get clock source.
 *
 * @param hal Context of the HAL layer
 *
 * @return
 *     - true Use XTAL clock
 *     - false Use APB clock
 */
#define timer_hal_get_use_xtal(hal)  timer_ll_get_use_xtal((hal)->dev, (hal)->idx)
#endif

#ifdef __cplusplus
}
#endif
