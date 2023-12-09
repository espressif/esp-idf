/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-P4 PCNT register operations

#pragma once

#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "soc/pcnt_struct.h"
#include "hal/pcnt_types.h"
#include "hal/misc.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCNT_LL_GET_HW(num)      (((num) == 0) ? (&PCNT) : NULL)
#define PCNT_LL_MAX_GLITCH_WIDTH 1023
#define PCNT_LL_MAX_LIM          SHRT_MAX
#define PCNT_LL_MIN_LIN          SHRT_MIN

typedef enum {
    PCNT_LL_WATCH_EVENT_INVALID = -1,
    PCNT_LL_WATCH_EVENT_THRES1,
    PCNT_LL_WATCH_EVENT_THRES0,
    PCNT_LL_WATCH_EVENT_LOW_LIMIT,
    PCNT_LL_WATCH_EVENT_HIGH_LIMIT,
    PCNT_LL_WATCH_EVENT_ZERO_CROSS,
    PCNT_LL_WATCH_EVENT_MAX
} pcnt_ll_watch_event_id_t;

#define PCNT_LL_WATCH_EVENT_MASK          ((1 << PCNT_LL_WATCH_EVENT_MAX) - 1)
#define PCNT_LL_UNIT_WATCH_EVENT(unit_id) (1 << (unit_id))

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
__attribute__((always_inline))
static inline int pcnt_ll_get_count(pcnt_dev_t *hw, uint32_t unit)
{
    pcnt_un_cnt_reg_t cnt_reg;
    cnt_reg.val = hw->cnt_unit[unit].val;

    int16_t value = cnt_reg.pulse_cnt;
    return value;
}

/**
 * @brief Pause PCNT counter of PCNT unit
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 */
__attribute__((always_inline))
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
__attribute__((always_inline))
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
__attribute__((always_inline))
static inline void pcnt_ll_clear_count(pcnt_dev_t *hw, uint32_t unit)
{
    hw->ctrl.val |= 1 << (2 * unit);
    hw->ctrl.val &= ~(1 << (2 * unit));
}

/**
 * @brief Enable PCNT step comparator event
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param enable true to enable, false to disable
 */
static inline void pcnt_ll_enable_step_notify(pcnt_dev_t *hw, uint32_t unit, bool enable)
{
    if (enable) {
        hw->ctrl.val |= 1 << (8 + unit);
    } else {
        hw->ctrl.val &= ~(1 << (8 + unit));
    }
}

/**
 * @brief Set PCNT step value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param value PCNT step value
 */
static inline void pcnt_ll_set_step_value(pcnt_dev_t *hw, uint32_t unit, int value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->change_conf_unit[3 - unit], cnt_step, value);
}

/**
 * @brief Set PCNT step limit value
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @param value PCNT step limit value
 */
static inline void pcnt_ll_set_step_limit_value(pcnt_dev_t *hw, uint32_t unit, int value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->change_conf_unit[3 - unit], cnt_step_lim, value);
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
__attribute__((always_inline))
static inline uint32_t pcnt_ll_get_intr_status(pcnt_dev_t *hw)
{
    return hw->int_st.val;
}

/**
 * @brief Clear PCNT interrupt status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param status value to clear interrupt status
 */
__attribute__((always_inline))
static inline void pcnt_ll_clear_intr_status(pcnt_dev_t *hw, uint32_t status)
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
    hw->conf_unit[unit].conf0.val &= ~(PCNT_LL_WATCH_EVENT_MASK << 11);
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
    pcnt_un_conf2_reg_t conf2_reg;
    conf2_reg.val = hw->conf_unit[unit].conf2.val;

    conf2_reg.cnt_h_lim = value;
    hw->conf_unit[unit].conf2.val = conf2_reg.val;
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
    pcnt_un_conf2_reg_t conf2_reg;
    conf2_reg.val = hw->conf_unit[unit].conf2.val;

    conf2_reg.cnt_l_lim = value;
    hw->conf_unit[unit].conf2.val = conf2_reg.val;
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
    pcnt_un_conf1_reg_t conf1_reg;
    conf1_reg.val = hw->conf_unit[unit].conf1.val;

    if (thres == 0) {
        conf1_reg.cnt_thres0 = value;
    } else {
        conf1_reg.cnt_thres1 = value;
    }
    hw->conf_unit[unit].conf1.val = conf1_reg.val;
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
    pcnt_un_conf2_reg_t conf2_reg;
    conf2_reg.val = hw->conf_unit[unit].conf2.val;

    int16_t value = conf2_reg.cnt_h_lim ;
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
    pcnt_un_conf2_reg_t conf2_reg;
    conf2_reg.val = hw->conf_unit[unit].conf2.val;

    int16_t value = conf2_reg.cnt_l_lim ;
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
    pcnt_un_conf1_reg_t conf1_reg;
    conf1_reg.val = hw->conf_unit[unit].conf1.val;

    if (thres == 0) {
        value = conf1_reg.cnt_thres0 ;
    } else {
        value = conf1_reg.cnt_thres1 ;
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
 * @brief Get PCNT zero cross mode
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return Zero cross mode
 */
__attribute__((always_inline))
static inline pcnt_unit_zero_cross_mode_t pcnt_ll_get_zero_cross_mode(pcnt_dev_t *hw, uint32_t unit)
{
    return (pcnt_unit_zero_cross_mode_t)(hw->status_unit[unit].val & 0x03);
}

/**
 * @brief Get PCNT event status
 *
 * @param hw Peripheral PCNT hardware instance address.
 * @param unit PCNT unit number
 * @return Event status word
 */
__attribute__((always_inline))
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
    return hw->conf_unit[unit].conf0.filter_thres ;
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

/**
 * @brief Get interrupt status register address.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return Interrupt status register address
 */
static inline volatile void *pcnt_ll_get_intr_status_reg(pcnt_dev_t *hw)
{
    return &hw->int_st.val;
}

/**
 * @brief Enable or disable the bus clock for the PCNT module
 *
 * @param set_bit True to set bit, false to clear bit
 */
static inline void pcnt_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_pcnt_apb_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define pcnt_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; pcnt_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the PCNT module
 */
static inline void pcnt_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_pcnt = 1;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_pcnt = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define pcnt_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; pcnt_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
