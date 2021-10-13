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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 PCNT register operations

#pragma once

#include "hal/hal_defs.h"
#include "soc/pcnt_periph.h"
#include "soc/pcnt_struct.h"
#include "hal/pcnt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get PCNT hardware instance with giving pcnt num
#define PCNT_LL_GET_HW(num) (((num) == 0) ? (&PCNT) : NULL)

/**
 * @brief Set PCNT counter mode
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param channel PCNT channel number
 * @param pos_mode Counter mode when detecting positive edge
 * @param neg_mode Counter mode when detecting negative edge
 * @param hctrl_mode Counter mode when control signal is high level
 * @param lctrl_mode Counter mode when control signal is low level
 */
static inline void pcnt_ll_set_mode(pcnt_dev_t *hw, pcnt_unit_t unit, pcnt_channel_t channel, pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode)
{
    typeof(hw->conf_unit[unit].conf0) conf0_reg = hw->conf_unit[unit].conf0;
    if (channel == 0) {
        conf0_reg.ch0_pos_mode = pos_mode;
        conf0_reg.ch0_neg_mode = neg_mode;
        conf0_reg.ch0_hctrl_mode = hctrl_mode;
        conf0_reg.ch0_lctrl_mode = lctrl_mode;
    } else {
        conf0_reg.ch1_pos_mode = pos_mode;
        conf0_reg.ch1_neg_mode = neg_mode;
        conf0_reg.ch1_hctrl_mode = hctrl_mode;
        conf0_reg.ch1_lctrl_mode = lctrl_mode;
    }
    hw->conf_unit[unit].conf0 = conf0_reg;
}

/**
 * @brief Get pulse counter value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit  Pulse Counter unit number
 * @param count Pointer to accept counter value
 */
static inline void pcnt_ll_get_counter_value(pcnt_dev_t *hw, pcnt_unit_t unit, int16_t *count)
{
    *count = (int16_t) hw->cnt_unit[unit].cnt_val;
}

/**
 * @brief Pause PCNT counter of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
static inline void pcnt_ll_counter_pause(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    hw->ctrl.val |= BIT(PCNT_CNT_PAUSE_U0_S + (unit * 2));
}

/**
 * @brief Resume counting for PCNT counter
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number, select from pcnt_unit_t
 */
static inline void pcnt_ll_counter_resume(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    hw->ctrl.val &= (~(BIT(PCNT_CNT_PAUSE_U0_S + (unit * 2))));
}

/**
 * @brief Clear and reset PCNT counter value to zero
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param  unit PCNT unit number, select from pcnt_unit_t
 */
static inline void pcnt_ll_counter_clear(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    uint32_t reset_bit = BIT(PCNT_PLUS_CNT_RST_U0_S + (unit * 2));
    hw->ctrl.val |= reset_bit;
    hw->ctrl.val &= ~reset_bit;
}

/**
 * @brief Enable PCNT interrupt for PCNT unit
 *        @note
 *        Each Pulse counter unit has five watch point events that share the same interrupt.
 *        Configure events with pcnt_event_enable() and pcnt_event_disable()
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
static inline void pcnt_ll_intr_enable(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    hw->int_ena.val |= BIT(PCNT_CNT_THR_EVENT_U0_INT_ENA_S + unit);
}

/**
 * @brief Disable PCNT interrupt for PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
static inline void pcnt_ll_intr_disable(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    hw->int_ena.val &= (~(BIT(PCNT_CNT_THR_EVENT_U0_INT_ENA_S + unit)));
}

/**
 * @brief Get PCNT interrupt status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param status Pointer to accept value
 */
static inline void pcnt_ll_get_intr_status(pcnt_dev_t *hw, uint32_t *status)
{
    *status = hw->int_st.val;
}

/**
 * @brief Clear PCNT interrupt status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param status value to clear interrupt status
 */
static inline void pcnt_ll_clear_intr_status(pcnt_dev_t *hw, uint32_t status)
{
    hw->int_clr.val = status;
}

/**
 * @brief Enable PCNT event of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 */
static inline void pcnt_ll_event_enable(pcnt_dev_t *hw, pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    if (evt_type == PCNT_EVT_L_LIM) {
        hw->conf_unit[unit].conf0.thr_l_lim_en = 1;
    } else if (evt_type == PCNT_EVT_H_LIM) {
        hw->conf_unit[unit].conf0.thr_h_lim_en = 1;
    } else if (evt_type == PCNT_EVT_THRES_0) {
        hw->conf_unit[unit].conf0.thr_thres0_en = 1;
    } else if (evt_type == PCNT_EVT_THRES_1) {
        hw->conf_unit[unit].conf0.thr_thres1_en = 1;
    } else if (evt_type == PCNT_EVT_ZERO) {
        hw->conf_unit[unit].conf0.thr_zero_en = 1;
    }
}

/**
 * @brief Disable PCNT event of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 */
static inline void pcnt_ll_event_disable(pcnt_dev_t *hw, pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    if (evt_type == PCNT_EVT_L_LIM) {
        hw->conf_unit[unit].conf0.thr_l_lim_en = 0;
    } else if (evt_type == PCNT_EVT_H_LIM) {
        hw->conf_unit[unit].conf0.thr_h_lim_en = 0;
    } else if (evt_type == PCNT_EVT_THRES_0) {
        hw->conf_unit[unit].conf0.thr_thres0_en = 0;
    } else if (evt_type == PCNT_EVT_THRES_1) {
        hw->conf_unit[unit].conf0.thr_thres1_en = 0;
    } else if (evt_type == PCNT_EVT_ZERO) {
        hw->conf_unit[unit].conf0.thr_zero_en = 0;
    }
}

/**
 * @brief Set PCNT event value of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 *
 * @param value Counter value for PCNT event
 */
static inline void pcnt_ll_set_event_value(pcnt_dev_t *hw, pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t value)
{
    if (evt_type == PCNT_EVT_L_LIM) {
        hw->conf_unit[unit].conf2.cnt_l_lim = value;
    } else if (evt_type == PCNT_EVT_H_LIM) {
        hw->conf_unit[unit].conf2.cnt_h_lim = value;
    } else if (evt_type == PCNT_EVT_THRES_0) {
        hw->conf_unit[unit].conf1.cnt_thres0 = value;
    } else if (evt_type == PCNT_EVT_THRES_1) {
        hw->conf_unit[unit].conf1.cnt_thres1 = value;
    }
}

/**
 * @brief Get PCNT event value of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 * @param value Pointer to accept counter value for PCNT event
 */
static inline void pcnt_ll_get_event_value(pcnt_dev_t *hw, pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t *value)
{
    if (evt_type == PCNT_EVT_L_LIM) {
        *value = (int16_t) hw->conf_unit[unit].conf2.cnt_l_lim;
    } else if (evt_type == PCNT_EVT_H_LIM) {
        *value = (int16_t) hw->conf_unit[unit].conf2.cnt_h_lim;
    } else if (evt_type == PCNT_EVT_THRES_0) {
        *value = (int16_t) hw->conf_unit[unit].conf1.cnt_thres0;
    } else if (evt_type == PCNT_EVT_THRES_1) {
        *value = (int16_t) hw->conf_unit[unit].conf1.cnt_thres1;
    } else {
        *value = 0;
    }
}

/**
 * @brief Set PCNT filter value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param filter_val PCNT signal filter value, counter in APB_CLK cycles.
 *        Any pulses lasting shorter than this will be ignored when the filter is enabled.
 *        @note
 *        filter_val is a 10-bit value, so the maximum filter_val should be limited to 1023.
 */
static inline void pcnt_ll_set_filter_value(pcnt_dev_t *hw, pcnt_unit_t unit, uint16_t filter_val)
{
    hw->conf_unit[unit].conf0.filter_thres = filter_val;
}

/**
 * @brief Get PCNT filter value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param filter_val Pointer to accept PCNT filter value.
 */
static inline void pcnt_ll_get_filter_value(pcnt_dev_t *hw, pcnt_unit_t unit, uint16_t *filter_val)
{
    *filter_val = hw->conf_unit[unit].conf0.filter_thres;
}

/**
 * @brief Enable PCNT input filter
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
static inline void pcnt_ll_filter_enable(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    hw->conf_unit[unit].conf0.filter_en = 1;
}

/**
 * @brief Disable PCNT input filter
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
static inline void pcnt_ll_filter_disable(pcnt_dev_t *hw, pcnt_unit_t unit)
{
    hw->conf_unit[unit].conf0.filter_en = 0;
}

#ifdef __cplusplus
}
#endif
