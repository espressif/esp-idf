// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
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

// The LL layer for ESP32 PCNT register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/pcnt_struct.h"
#include "hal/pcnt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCNT_LL_GET_HW(num) (((num) == 0) ? (&PCNT) : NULL)
#define PCNT_LL_MAX_GLITCH_WIDTH 1023

typedef enum {
    PCNT_LL_EVENT_THRES1,
    PCNT_LL_EVENT_THRES0,
    PCNT_LL_EVENT_LOW_LIMIT,
    PCNT_LL_EVENT_HIGH_LIMIT,
    PCNT_LL_EVENT_ZERO_CROSS,
    PCNT_LL_EVENT_MAX
} pcnt_ll_event_id_t;

#define PCNT_LL_EVENT_MASK ((1 << PCNT_LL_EVENT_MAX) - 1)

/**
 * @brief Set PCNT channel edge action
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param channel PCNT channel number
 * @param pos_act Counter action when detecting positive edge
 * @param neg_act Counter action when detecting negative edge
 */
static inline void pcnt_ll_set_edge_action(pcnt_dev_t *hw, uint32_t unit, uint32_t channel, pcnt_channel_edge_action_t pos_act, pcnt_channel_edge_action_t neg_act)
{
    if (channel == 0) {
        hw->conf_unit[unit].conf0.ch0_pos_mode = pos_act;
        hw->conf_unit[unit].conf0.ch0_neg_mode = neg_act;
    } else {
        hw->conf_unit[unit].conf0.ch1_pos_mode = pos_act;
        hw->conf_unit[unit].conf0.ch1_neg_mode = neg_act;
    }
}

/**
 * @brief Set PCNT channel level action
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param channel PCNT channel number
 * @param high_act Counter action when control signal is high level
 * @param low_act Counter action when control signal is low level
 */
static inline void pcnt_ll_set_level_action(pcnt_dev_t *hw, uint32_t unit, uint32_t channel, pcnt_channel_level_action_t high_act, pcnt_channel_level_action_t low_act)
{
    if (channel == 0) {
        hw->conf_unit[unit].conf0.ch0_hctrl_mode = high_act;
        hw->conf_unit[unit].conf0.ch0_lctrl_mode = low_act;
    } else {
        hw->conf_unit[unit].conf0.ch1_hctrl_mode = high_act;
        hw->conf_unit[unit].conf0.ch1_lctrl_mode = low_act;
    }
}

/**
 * @brief Get pulse counter value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit  Pulse Counter unit number
 * @return PCNT count value (a signed integer)
 */
static inline int pcnt_ll_get_count(pcnt_dev_t *hw, uint32_t unit)
{
    typeof(hw->cnt_unit[unit]) cnt_reg = hw->cnt_unit[unit];
    int16_t value = cnt_reg.cnt_val;
    return value;
}

/**
 * @brief Pause PCNT counter of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
static inline void pcnt_ll_stop_count(pcnt_dev_t *hw, uint32_t unit)
{
    hw->ctrl.val |= 1 << (2 * unit + 1);
}

/**
 * @brief Resume counting for PCNT counter
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number, select from uint32_t
 */
static inline void pcnt_ll_start_count(pcnt_dev_t *hw, uint32_t unit)
{
    hw->ctrl.val &= ~(1 << (2 * unit + 1));
}

/**
 * @brief Clear PCNT counter value to zero
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param  unit PCNT unit number, select from uint32_t
 */
static inline void pcnt_ll_clear_count(pcnt_dev_t *hw, uint32_t unit)
{
    hw->ctrl.val |= 1 << (2 * unit);
    hw->ctrl.val &= ~(1 << (2 * unit));
}

/**
 * @brief Enable PCNT interrupt for PCNT unit
 * @note  Each PCNT unit has five watch point events that share the same interrupt bit.
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit_mask PCNT units mask
 * @param enable True to enable interrupt, False to disable interrupt
 */
static inline void pcnt_ll_enable_intr(pcnt_dev_t *hw, uint32_t unit_mask, bool enable)
{
    if (enable) {
        hw->int_ena.val |= unit_mask;
    } else {
        hw->int_ena.val &= ~unit_mask;
    }
}

/**
 * @brief Get PCNT interrupt status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @return Interrupt status word
 */
__attribute__((always_inline)) static inline uint32_t pcnt_ll_get_intr_status(pcnt_dev_t *hw)
{
    return hw->int_st.val;
}

/**
 * @brief Clear PCNT interrupt status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param status value to clear interrupt status
 */
__attribute__((always_inline)) static inline void pcnt_ll_clear_intr_status(pcnt_dev_t *hw, uint32_t status)
{
    hw->int_clr.val = status;
}

/**
 * @brief Enable PCNT high limit event
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param enable true to enable, false to disable
 */
static inline void pcnt_ll_enable_high_limit_event(pcnt_dev_t *hw, uint32_t unit, bool enable)
{
    hw->conf_unit[unit].conf0.thr_h_lim_en = enable;
}

/**
 * @brief Enable PCNT low limit event
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param enable true to enable, false to disable
 */
static inline void pcnt_ll_enable_low_limit_event(pcnt_dev_t *hw, uint32_t unit, bool enable)
{
    hw->conf_unit[unit].conf0.thr_l_lim_en = enable;
}

/**
 * @brief Enable PCNT zero cross event
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param enable true to enable, false to disable
 */
static inline void pcnt_ll_enable_zero_cross_event(pcnt_dev_t *hw, uint32_t unit, bool enable)
{
    hw->conf_unit[unit].conf0.thr_zero_en = enable;
}

/**
 * @brief Enable PCNT threshold event
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param thres Threshold ID
 * @param enable true to enable, false to disable
 */
static inline void pcnt_ll_enable_thres_event(pcnt_dev_t *hw, uint32_t unit, uint32_t thres, bool enable)
{
    if (thres == 0) {
        hw->conf_unit[unit].conf0.thr_thres0_en = enable;
    } else {
        hw->conf_unit[unit].conf0.thr_thres1_en = enable;
    }
}

/**
 * @brief Disable all PCNT threshold events
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit unit number
 */
static inline void pcnt_ll_disable_all_events(pcnt_dev_t *hw, uint32_t unit)
{
    hw->conf_unit[unit].conf0.val &= ~(PCNT_LL_EVENT_MASK << 11);
}

/**
 * @brief Set PCNT high limit value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param value PCNT high limit value
 */
static inline void pcnt_ll_set_high_limit_value(pcnt_dev_t *hw, uint32_t unit, int value)
{
    typeof(hw->conf_unit[unit].conf2) conf2_reg = hw->conf_unit[unit].conf2;
    conf2_reg.cnt_h_lim = value;
    hw->conf_unit[unit].conf2 = conf2_reg;
}

/**
 * @brief Set PCNT low limit value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param value PCNT low limit value
 */
static inline void pcnt_ll_set_low_limit_value(pcnt_dev_t *hw, uint32_t unit, int value)
{
    typeof(hw->conf_unit[unit].conf2) conf2_reg = hw->conf_unit[unit].conf2;
    conf2_reg.cnt_l_lim = value;
    hw->conf_unit[unit].conf2 = conf2_reg;
}

/**
 * @brief Set PCNT threshold value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param thres Threshold ID
 * @param value PCNT threshold value
 */
static inline void pcnt_ll_set_thres_value(pcnt_dev_t *hw, uint32_t unit, uint32_t thres, int value)
{
    typeof(hw->conf_unit[unit].conf1) conf1_reg = hw->conf_unit[unit].conf1;
    if (thres == 0) {
        conf1_reg.cnt_thres0 = value;
    } else {
        conf1_reg.cnt_thres1 = value;
    }
    hw->conf_unit[unit].conf1 = conf1_reg;
}

/**
 * @brief Get PCNT high limit value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return PCNT high limit value
 */
static inline int pcnt_ll_get_high_limit_value(pcnt_dev_t *hw, uint32_t unit)
{
    typeof(hw->conf_unit[unit].conf2) conf2_reg = hw->conf_unit[unit].conf2;
    int16_t value = conf2_reg.cnt_h_lim;
    return value;
}

/**
 * @brief Get PCNT low limit value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return PCNT high limit value
 */
static inline int pcnt_ll_get_low_limit_value(pcnt_dev_t *hw, uint32_t unit)
{
    typeof(hw->conf_unit[unit].conf2) conf2_reg = hw->conf_unit[unit].conf2;
    int16_t value = conf2_reg.cnt_l_lim;
    return value;
}

/**
 * @brief Get PCNT threshold value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param thres Threshold ID
 * @return PCNT threshold value
 */
static inline int pcnt_ll_get_thres_value(pcnt_dev_t *hw, uint32_t unit, uint32_t thres)
{
    int16_t value;
    typeof(hw->conf_unit[unit].conf1) conf1_reg = hw->conf_unit[unit].conf1;
    if (thres == 0) {
        value = conf1_reg.cnt_thres0;
    } else {
        value = conf1_reg.cnt_thres1;
    }
    return value;
}

/**
 * @brief Get PCNT unit runtime status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return PCNT unit runtime status
 */
static inline uint32_t pcnt_ll_get_unit_status(pcnt_dev_t *hw, uint32_t unit)
{
    return hw->status_unit[unit].val;
}

/**
 * @brief Get PCNT count sign
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return Count sign
 */
static inline pcnt_unit_count_sign_t pcnt_ll_get_count_sign(pcnt_dev_t *hw, uint32_t unit)
{
    return hw->status_unit[unit].val & 0x03;
}

/**
 * @brief Get PCNT event status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return Event status word
 */
static inline uint32_t pcnt_ll_get_event_status(pcnt_dev_t *hw, uint32_t unit)
{
    return hw->status_unit[unit].val >> 2;
}

/**
 * @brief Set PCNT glitch filter threshold
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param filter_val PCNT signal filter value, counter in APB_CLK cycles.
 *        Any pulses lasting shorter than this will be ignored when the filter is enabled.
 */
static inline void pcnt_ll_set_glitch_filter_thres(pcnt_dev_t *hw, uint32_t unit, uint32_t filter_val)
{
    hw->conf_unit[unit].conf0.filter_thres = filter_val;
}

/**
 * @brief Get PCNT glitch filter threshold
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return glitch filter threshold
 */
static inline uint32_t pcnt_ll_get_glitch_filter_thres(pcnt_dev_t *hw, uint32_t unit)
{
    return hw->conf_unit[unit].conf0.filter_thres;
}

/**
 * @brief Enable PCNT glitch filter
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param enable True to enable the filter, False to disable the filter
 */
static inline void pcnt_ll_enable_glitch_filter(pcnt_dev_t *hw, uint32_t unit, bool enable)
{
    hw->conf_unit[unit].conf0.filter_en = enable;
}

#ifdef __cplusplus
}
#endif
