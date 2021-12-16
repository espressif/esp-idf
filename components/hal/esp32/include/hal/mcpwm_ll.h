/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 MCPWM register operations

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "soc/soc_caps.h"
#include "soc/mcpwm_struct.h"
#include "hal/mcpwm_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Get the address of peripheral registers
#define MCPWM_LL_GET_HW(ID) (((ID) == 0) ? &MCPWM0 : &MCPWM1)
#define MCPWM_LL_MAX_CAPTURE_PRESCALE 255
#define MCPWM_LL_MAX_COMPARE_VALUE    65535
#define MCPWM_LL_MAX_DEAD_DELAY       65535
#define MCPWM_LL_MAX_PHASE_VALUE      65535

/********************* Group registers *******************/

// Set/Get group clock: PWM_clk = CLK_160M / (prescale + 1)
static inline void mcpwm_ll_group_set_clock_prescale(mcpwm_dev_t *mcpwm, int pre_scale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->clk_cfg, clk_prescale, pre_scale - 1);
}

static inline uint32_t mcpwm_ll_group_get_clock_prescale(mcpwm_dev_t *mcpwm)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->clk_cfg, clk_prescale) + 1;
}

static inline void mcpwm_ll_group_enable_shadow_mode(mcpwm_dev_t *mcpwm)
{
    mcpwm->update_cfg.global_up_en = 1;
    // updating of active registers in MCPWM operators should be enabled
    mcpwm->update_cfg.op0_up_en = 1;
    mcpwm->update_cfg.op1_up_en = 1;
    mcpwm->update_cfg.op2_up_en = 1;
}

static inline void mcpwm_ll_group_flush_shadow(mcpwm_dev_t *mcpwm)
{
    // a toggle can trigger a forced update of all active registers in MCPWM, i.e. shadow->active
    mcpwm->update_cfg.global_force_up = ~mcpwm->update_cfg.global_force_up;
}

/********************* Interrupt registers *******************/

static inline uint32_t mcpwm_ll_intr_get_status(mcpwm_dev_t *mcpwm)
{
    return mcpwm->int_st.val;
}

static inline void mcpwm_ll_intr_clear_status(mcpwm_dev_t *mcpwm, uint32_t intr_mask)
{
    mcpwm->int_clr.val = intr_mask;
}

static inline void mcpwm_ll_intr_disable_all(mcpwm_dev_t *mcpwm)
{
    mcpwm->int_ena.val = 0;
}

//////////// get interrupt status for each event ////////////////

static inline uint32_t mcpwm_ll_intr_get_timer_stop_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 0) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_timer_tez_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 3) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_timer_tep_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 6) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_fault_enter_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 9) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_fault_exit_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 12) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_compare_status(mcpwm_dev_t *mcpwm, uint32_t cmp_id)
{
    return (mcpwm->int_st.val >> (15 + cmp_id * 3)) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_trip_cbc_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 21) & 0x07;
}

static inline uint32_t mcpwm_ll_intr_get_trip_ost_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 24) & 0x07;
}

__attribute__((always_inline))
static inline uint32_t mcpwm_ll_intr_get_capture_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 27) & 0x07;
}

//////////// clear interrupt status for each event ////////////////

static inline void mcpwm_ll_intr_clear_timer_stop_status(mcpwm_dev_t *mcpwm, uint32_t timer_mask)
{
    mcpwm->int_clr.val = (timer_mask & 0x07) << 0;
}

static inline void mcpwm_ll_intr_clear_timer_tez_status(mcpwm_dev_t *mcpwm, uint32_t timer_mask)
{
    mcpwm->int_clr.val = (timer_mask & 0x07) << 3;
}

static inline void mcpwm_ll_intr_clear_timer_tep_status(mcpwm_dev_t *mcpwm, uint32_t timer_mask)
{
    mcpwm->int_clr.val = (timer_mask & 0x07) << 6;
}

static inline void mcpwm_ll_intr_clear_fault_enter_status(mcpwm_dev_t *mcpwm, uint32_t fault_mask)
{
    mcpwm->int_clr.val = (fault_mask & 0x07) << 9;
}

static inline void mcpwm_ll_intr_clear_fault_exit_status(mcpwm_dev_t *mcpwm, uint32_t fault_mask)
{
    mcpwm->int_clr.val = (fault_mask & 0x07) << 12;
}

static inline void mcpwm_ll_intr_clear_compare_status(mcpwm_dev_t *mcpwm, uint32_t operator_mask, uint32_t cmp_id)
{
    mcpwm->int_clr.val = (operator_mask & 0x07) << (15 + cmp_id * 3);
}

static inline void mcpwm_ll_intr_clear_trip_cbc_status(mcpwm_dev_t *mcpwm, uint32_t cbc_mask)
{
    mcpwm->int_clr.val = (cbc_mask & 0x07) << 21;
}

static inline void mcpwm_ll_intr_clear_trip_ost_status(mcpwm_dev_t *mcpwm, uint32_t ost_mask)
{
    mcpwm->int_clr.val = (ost_mask & 0x07) << 24;
}

__attribute__((always_inline))
static inline void mcpwm_ll_intr_clear_capture_status(mcpwm_dev_t *mcpwm, uint32_t capture_mask)
{
    mcpwm->int_clr.val = (capture_mask & 0x07) << 27;
}

//////////// enable interrupt for each event ////////////////

static inline void mcpwm_ll_intr_enable_timer_stop(mcpwm_dev_t *mcpwm, uint32_t timer_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (timer_id + 0);
    } else {
        mcpwm->int_ena.val &= ~(1 << (timer_id + 0));
    }
}

static inline void mcpwm_ll_intr_enable_timer_tez(mcpwm_dev_t *mcpwm, uint32_t timer_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (timer_id + 3);
    } else {
        mcpwm->int_ena.val &= ~(1 << (timer_id + 3));
    }
}

static inline void mcpwm_ll_intr_enable_timer_tep(mcpwm_dev_t *mcpwm, uint32_t timer_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (timer_id + 6);
    } else {
        mcpwm->int_ena.val &= ~(1 << (timer_id + 6));
    }
}

static inline void mcpwm_ll_intr_enable_fault_enter(mcpwm_dev_t *mcpwm, uint32_t fault_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (9 + fault_id);  // enter fault interrupt
    } else {
        mcpwm->int_ena.val &= ~(1 << (9 + fault_id));
    }
}

static inline void mcpwm_ll_intr_enable_fault_exit(mcpwm_dev_t *mcpwm, uint32_t fault_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (12 + fault_id); // exit fault interrupt
    } else {
        mcpwm->int_ena.val &= ~(1 << (12 + fault_id));
    }
}

static inline void mcpwm_ll_intr_enable_compare(mcpwm_dev_t *mcpwm, uint32_t operator_id, uint32_t cmp_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= (1 << (15 + cmp_id * 3 + operator_id));
    } else {
        mcpwm->int_ena.val &= ~(1 << (15 + cmp_id * 3 + operator_id));
    }
}

static inline void mcpwm_ll_intr_enable_trip_cbc(mcpwm_dev_t *mcpwm, uint32_t operator_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= (1 << (21 + operator_id));
    } else {
        mcpwm->int_ena.val &= ~(1 << (21 + operator_id));
    }
}

static inline void mcpwm_ll_intr_enable_trip_ost(mcpwm_dev_t *mcpwm, uint32_t operator_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= (1 << (24 + operator_id));
    } else {
        mcpwm->int_ena.val &= ~(1 << (24 + operator_id));
    }
}

static inline void mcpwm_ll_intr_enable_capture(mcpwm_dev_t *mcpwm, uint32_t capture_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (27 + capture_id);
    } else {
        mcpwm->int_ena.val &= ~(1 << (27 + capture_id));
    }
}

/********************* Timer registers *******************/

static inline void mcpwm_ll_timer_set_clock_prescale(mcpwm_dev_t *mcpwm, int timer_id, uint32_t prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_prescale, prescale - 1);
}

static inline uint32_t mcpwm_ll_timer_get_clock_prescale(mcpwm_dev_t *mcpwm, int timer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_prescale) + 1;
}

static inline void mcpwm_ll_timer_set_peak(mcpwm_dev_t *mcpwm, int timer_id, uint32_t peak, bool symmetric)
{
    if (!symmetric) { // in asymmetric mode, period = [0,peak-1]
        HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period, peak - 1);
    } else { // in symmetric mode, period = [0,peak-1] + [peak,1]
        HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period, peak);
    }
}

static inline uint32_t mcpwm_ll_timer_get_peak(mcpwm_dev_t *mcpwm, int timer_id, bool symmetric)
{
    // asymmetric mode
    if (!symmetric) {
        return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period) + 1;
    }
    // symmetric mode
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period);
}

static inline void mcpwm_ll_timer_update_period_at_once(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod  = 0;
}

static inline void mcpwm_ll_timer_enable_update_period_on_tez(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    if (enable) {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod  |= 0x01;
    } else {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod  &= ~0x01;
    }
}

static inline void mcpwm_ll_timer_enable_update_period_on_sync(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    if (enable) {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod |= 0x02;
    } else {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod &= ~0x02;
    }
}

static inline void mcpwm_ll_timer_set_count_mode(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_count_mode_t mode)
{
    switch (mode) {
    case MCPWM_TIMER_COUNT_MODE_PAUSE:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 0;
        break;
    case MCPWM_TIMER_COUNT_MODE_UP:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 1;
        break;
    case MCPWM_TIMER_COUNT_MODE_DOWN:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 2;
        break;
    case MCPWM_TIMER_COUNT_MODE_UP_DOWN:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 3;
        break;
    }
}

static inline mcpwm_timer_count_mode_t mcpwm_ll_timer_get_count_mode(mcpwm_dev_t *mcpwm, int timer_id)
{
    switch (mcpwm->timer[timer_id].timer_cfg1.timer_mod) {
    case 1:
        return MCPWM_TIMER_COUNT_MODE_UP;
    case 2:
        return MCPWM_TIMER_COUNT_MODE_DOWN;
    case 3:
        return MCPWM_TIMER_COUNT_MODE_UP_DOWN;
    case 0:
    default:
        return MCPWM_TIMER_COUNT_MODE_PAUSE;
    }
}

static inline void mcpwm_ll_timer_set_execute_command(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_execute_cmd_t cmd)
{
    switch (cmd) {
    case MCPWM_TIMER_STOP_AT_ZERO:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 0;
        break;
    case MCPWM_TIMER_STOP_AT_PEAK:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 1;
        break;
    case MCPWM_TIMER_START_NO_STOP:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 2;
        break;
    case MCPWM_TIMER_START_STOP_AT_ZERO:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 3;
        break;
    case MCPWM_TIMER_START_STOP_AT_PEAK:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 4;
        break;
    }
}

static inline uint32_t mcpwm_ll_timer_get_count_value(mcpwm_dev_t *mcpwm, int timer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer_id].timer_status, timer_value);
}

static inline mcpwm_timer_direction_t mcpwm_ll_timer_get_count_direction(mcpwm_dev_t *mcpwm, int timer_id)
{
    return mcpwm->timer[timer_id].timer_status.timer_direction ? MCPWM_TIMER_DIRECTION_DOWN : MCPWM_TIMER_DIRECTION_UP;
}

static inline void mcpwm_ll_timer_enable_sync_input(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    mcpwm->timer[timer_id].timer_sync.timer_synci_en = enable;
}

static inline void mcpwm_ll_timer_sync_out_penetrate(mcpwm_dev_t *mcpwm, int timer_id)
{
    // sync_out is selected to sync_in
    mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 0;
}

static inline void mcpwm_ll_timer_sync_out_on_timer_event(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_event_t event)
{
    if (event == MCPWM_TIMER_EVENT_ZERO) {
        mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 1;
    } else if (event == MCPWM_TIMER_EVENT_PEAK) {
        mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 2;
    } else {
        HAL_ASSERT(false && "unknown sync out event");
    }
}

static inline void mcpwm_ll_timer_disable_sync_out(mcpwm_dev_t *mcpwm, int timer_id)
{
    // sync_out will always be zero
    mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 3;
}

static inline void mcpwm_ll_timer_trigger_soft_sync(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].timer_sync.timer_sync_sw = ~mcpwm->timer[timer_id].timer_sync.timer_sync_sw;
}

static inline void mcpwm_ll_timer_set_sync_phase_value(mcpwm_dev_t *mcpwm, int timer_id, uint32_t phase_value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_sync, timer_phase, phase_value);
}

static inline void mcpwm_ll_timer_set_sync_phase_direction(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_direction_t direction)
{
    mcpwm->timer[timer_id].timer_sync.timer_phase_direction = direction;
}

static inline void mcpwm_ll_timer_set_gpio_synchro(mcpwm_dev_t *mcpwm, int timer, int gpio_sync_id)
{
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (timer * 3));
    mcpwm->timer_synci_cfg.val |= (gpio_sync_id + 4) << (timer * 3);
}

static inline void mcpwm_ll_timer_set_timer_synchro(mcpwm_dev_t *mcpwm, int timer, int timer_sync_id)
{
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (timer * 3));
    mcpwm->timer_synci_cfg.val |= (timer_sync_id + 1) << (timer * 3);
}

static inline void mcpwm_ll_timer_set_soft_synchro(mcpwm_dev_t *mcpwm, int timer)
{
    // no sync input is selected, but software sync can still work
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (timer * 3));
}

static inline void mcpwm_ll_invert_gpio_synchro(mcpwm_dev_t *mcpwm, int sync_id, bool invert)
{
    if (invert) {
        mcpwm->timer_synci_cfg.val |= 1 << (sync_id + 9);
    } else {
        mcpwm->timer_synci_cfg.val &= ~(1 << (sync_id + 9));
    }
}

/********************* Operator registers *******************/

static inline void mcpwm_ll_operator_flush_shadow(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->update_cfg.val ^= (1 << (2 * operator_id + 3));
}

static inline void mcpwm_ll_operator_select_timer(mcpwm_dev_t *mcpwm, int operator_id, int timer_id)
{
    if (operator_id == 0) {
        mcpwm->operator_timersel.operator0_timersel = timer_id;
    } else if (operator_id == 1) {
        mcpwm->operator_timersel.operator1_timersel = timer_id;
    } else {
        mcpwm->operator_timersel.operator2_timersel = timer_id;
    }
}

static inline void mcpwm_ll_operator_update_compare_at_once(mcpwm_dev_t *mcpwm, int operator_id, int compare_id)
{
    mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~(0x0F << (4 * compare_id));
}

static inline void mcpwm_ll_operator_enable_update_compare_on_tez(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 0) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 0) << (4 * compare_id));
    }
}

static inline void mcpwm_ll_operator_enable_update_compare_on_tep(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 1) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 1) << (4 * compare_id));
    }
}

static inline void mcpwm_ll_operator_enable_update_compare_on_sync(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 2) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 2) << (4 * compare_id));
    }
}

static inline void mcpwm_ll_operator_set_compare_value(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, uint32_t compare_value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->operators[operator_id].timestamp[compare_id], gen, compare_value);
}

static inline uint32_t mcpwm_ll_operator_get_compare_value(mcpwm_dev_t *mcpwm, int operator_id, int compare_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->operators[operator_id].timestamp[compare_id], gen);
}

static inline void mcpwm_ll_operator_update_action_at_once(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod = 0;
}

static inline void mcpwm_ll_operator_enable_update_action_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 0;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 0);
    }
}

static inline void mcpwm_ll_operator_enable_update_action_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 1;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_operator_enable_update_action_on_sync(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 2;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 2);
    }
}

static inline void mcpwm_ll_operator_set_trigger_gpio_fault(mcpwm_dev_t *mcpwm, int operator_id, int trig_id, int fault_id)
{
    mcpwm->operators[operator_id].gen_cfg0.val &= ~(0x07 << (4 + 3 * trig_id));
    mcpwm->operators[operator_id].gen_cfg0.val |= (fault_id << (4 + 3 * trig_id));
}

static inline void mcpwm_ll_operator_set_trigger_timer_sync(mcpwm_dev_t *mcpwm, int operator_id, int trig_id)
{
    // the timer here is not selectable, must be the one connected with the operator
    mcpwm->operators[operator_id].gen_cfg0.val &= ~(0x07 << (4 + 3 * trig_id));
    mcpwm->operators[operator_id].gen_cfg0.val |= (3 << (4 + 3 * trig_id));
}

/********************* Generator registers *******************/

static inline void mcpwm_ll_generator_reset_actions(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    mcpwm->operators[operator_id].generator[generator_id].val = 0;
}

static inline void mcpwm_ll_generator_set_action_on_timer_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, mcpwm_timer_event_t event, mcpwm_generator_action_t action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // utez, utep
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (event * 2));
        mcpwm->operators[operator_id].generator[generator_id].val |= action << (event * 2);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dtez, dtep
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (event * 2 + 12));
        mcpwm->operators[operator_id].generator[generator_id].val |= action << (event * 2 + 12);
    }
}

static inline void mcpwm_ll_generator_set_action_on_compare_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, int cmp_id, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // utea, uteb
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (cmp_id * 2 + 4));
        mcpwm->operators[operator_id].generator[generator_id].val |= action << (cmp_id * 2 + 4);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dtea, dteb
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (cmp_id * 2 + 16));
        mcpwm->operators[operator_id].generator[generator_id].val |= action << (cmp_id * 2 + 16);
    }
}

static inline void mcpwm_ll_generator_set_action_on_trigger_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, int trig_id, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // ut0, ut1
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (trig_id * 2 + 8));
        mcpwm->operators[operator_id].generator[generator_id].val |= action << (trig_id * 2 + 8);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dt0, dt1
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (trig_id * 2 + 20));
        mcpwm->operators[operator_id].generator[generator_id].val |= action << (trig_id * 2 + 20);
    }
}

static inline void mcpwm_ll_gen_trigger_noncontinue_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_nciforce = ~mcpwm->operators[operator_id].gen_force.gen_a_nciforce;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_nciforce = ~mcpwm->operators[operator_id].gen_force.gen_b_nciforce;
    }
}

static inline void mcpwm_ll_gen_disable_continue_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    mcpwm->operators[operator_id].gen_force.gen_cntuforce_upmethod = 0; // update force method immediately
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_cntuforce_mode = 0;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_cntuforce_mode = 0;
    }
}

static inline void mcpwm_ll_gen_disable_noncontinue_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_nciforce_mode = 0;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_nciforce_mode = 0;
    }
}

static inline void mcpwm_ll_gen_set_continue_force_level(mcpwm_dev_t *mcpwm, int operator_id, int generator_id, int level)
{
    mcpwm->operators[operator_id].gen_force.gen_cntuforce_upmethod = 0; // update force method immediately
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_cntuforce_mode = level + 1;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_cntuforce_mode = level + 1;
    }
}

static inline void mcpwm_ll_gen_set_noncontinue_force_level(mcpwm_dev_t *mcpwm, int operator_id, int generator_id, int level)
{
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_nciforce_mode = level + 1;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_nciforce_mode = level + 1;
    }
}

/********************* Dead time registers *******************/

static inline void mcpwm_ll_deadtime_resolution_to_timer(mcpwm_dev_t *mcpwm, int operator_id, bool same)
{
    // whether to make the resolution of dead time delay module the same to the timer connected with operator
    mcpwm->operators[operator_id].dt_cfg.dt_clk_sel = same;
}

static inline void mcpwm_ll_deadtime_red_select_generator(mcpwm_dev_t *mcpwm, int operator_id, int generator)
{
    mcpwm->operators[operator_id].dt_cfg.dt_red_insel = generator;
}

static inline void mcpwm_ll_deadtime_fed_select_generator(mcpwm_dev_t *mcpwm, int operator_id, int generator)
{
    mcpwm->operators[operator_id].dt_cfg.dt_fed_insel = generator;
}

static inline void mcpwm_ll_deadtime_bypass_path(mcpwm_dev_t *mcpwm, int operator_id, int path, bool bypass)
{
    if (bypass) {
        mcpwm->operators[operator_id].dt_cfg.val |= 1 << (path + 15);
    } else {
        mcpwm->operators[operator_id].dt_cfg.val &= ~(1 << (path + 15));
    }
}

static inline void mcpwm_ll_deadtime_invert_outpath(mcpwm_dev_t *mcpwm, int operator_id, int path, bool invert)
{
    if (invert) {
        mcpwm->operators[operator_id].dt_cfg.val |= 1 << (path + 13);
    } else {
        mcpwm->operators[operator_id].dt_cfg.val &= ~(1 << (path + 13));
    }
}

static inline void mcpwm_ll_deadtime_swap_out_path(mcpwm_dev_t *mcpwm, int operator_id, int path, bool swap)
{
    if (swap) {
        mcpwm->operators[operator_id].dt_cfg.val |= 1 << (path + 9);
    } else {
        mcpwm->operators[operator_id].dt_cfg.val &= ~(1 << (path + 9));
    }
}

static inline void mcpwm_ll_deadtime_enable_deb(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].dt_cfg.dt_deb_mode = enable;
}

static inline uint32_t mcpwm_ll_deadtime_get_switch_topology(mcpwm_dev_t *mcpwm, int operator_id)
{
    return (mcpwm->operators[operator_id].dt_cfg.dt_deb_mode << 8) | (mcpwm->operators[operator_id].dt_cfg.dt_b_outswap << 7) |
           (mcpwm->operators[operator_id].dt_cfg.dt_a_outswap << 6) | (mcpwm->operators[operator_id].dt_cfg.dt_fed_insel << 5) |
           (mcpwm->operators[operator_id].dt_cfg.dt_red_insel << 4) | (mcpwm->operators[operator_id].dt_cfg.dt_fed_outinvert << 3) |
           (mcpwm->operators[operator_id].dt_cfg.dt_red_outinvert << 2) | (mcpwm->operators[operator_id].dt_cfg.dt_a_outbypass << 1) |
           (mcpwm->operators[operator_id].dt_cfg.dt_b_outbypass << 0);
}

static inline void mcpwm_ll_deadtime_set_falling_delay(mcpwm_dev_t *mcpwm, int operator_id, uint32_t fed)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->operators[operator_id].dt_fed_cfg, dt_fed, fed - 1);
}

static inline uint32_t mcpwm_ll_deadtime_get_falling_delay(mcpwm_dev_t *mcpwm, int operator_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->operators[operator_id].dt_fed_cfg, dt_fed) + 1;
}

static inline void mcpwm_ll_deadtime_set_rising_delay(mcpwm_dev_t *mcpwm, int operator_id, uint32_t red)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->operators[operator_id].dt_red_cfg, dt_red, red - 1);
}

static inline uint32_t mcpwm_ll_deadtime_get_rising_delay(mcpwm_dev_t *mcpwm, int operator_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->operators[operator_id].dt_red_cfg, dt_red) + 1;
}

static inline void mcpwm_ll_deadtime_update_delay_at_once(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod = 0;
    mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod = 0;
}

static inline void mcpwm_ll_deadtime_enable_update_delay_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 0;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 0;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 0);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 0);
    }
}

static inline void mcpwm_ll_deadtime_enable_update_delay_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 1;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 1;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 1);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_deadtime_enable_update_delay_on_sync(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 2;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 2;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 2);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 2);
    }
}

/********************* Carrier registers *******************/

static inline void mcpwm_ll_carrier_enable(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_en = enable;
}

static inline void mcpwm_ll_carrier_set_prescale(mcpwm_dev_t *mcpwm, int operator_id, uint8_t prescale)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_prescale = prescale - 1;
}

static inline uint8_t mcpwm_ll_carrier_get_prescale(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->operators[operator_id].carrier_cfg.carrier_prescale + 1;
}

static inline void mcpwm_ll_carrier_set_duty(mcpwm_dev_t *mcpwm, int operator_id, uint8_t carrier_duty)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_duty = carrier_duty;
}

static inline uint8_t mcpwm_ll_carrier_get_duty(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->operators[operator_id].carrier_cfg.carrier_duty;
}

static inline void mcpwm_ll_carrier_out_invert(mcpwm_dev_t *mcpwm, int operator_id, bool invert)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_out_invert = invert;
}

static inline void mcpwm_ll_carrier_in_invert(mcpwm_dev_t *mcpwm, int operator_id, bool invert)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_in_invert = invert;
}

static inline void mcpwm_ll_carrier_set_oneshot_width(mcpwm_dev_t *mcpwm, int operator_id, uint8_t pulse_width)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_oshtwth = pulse_width - 1;
}

static inline uint8_t mcpwm_ll_carrier_get_oneshot_width(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->operators[operator_id].carrier_cfg.carrier_oshtwth + 1;
}

/********************* Fault detector registers *******************/

static inline void mcpwm_ll_fault_enable_detection(mcpwm_dev_t *mcpwm, int fault_sig, bool enable)
{
    if (enable) {
        mcpwm->fault_detect.val |= 1 << fault_sig;
    } else {
        mcpwm->fault_detect.val &= ~(1 << fault_sig);
    }
}

static inline void mcpwm_ll_fault_set_active_level(mcpwm_dev_t *mcpwm, int fault_sig, bool level)
{
    if (level) {
        mcpwm->fault_detect.val |= 1 << (fault_sig + 3);
    } else {
        mcpwm->fault_detect.val &= ~(1 << (fault_sig + 3));
    }
}

static inline void mcpwm_ll_fault_clear_ost(mcpwm_dev_t *mcpwm, int operator_id)
{
    // a posedge can clear the ost fault status
    mcpwm->operators[operator_id].fh_cfg1.fh_clr_ost = 0;
    mcpwm->operators[operator_id].fh_cfg1.fh_clr_ost = 1;
}

static inline void mcpwm_ll_fault_enable_oneshot_mode(mcpwm_dev_t *mcpwm, int operator_id, int fault_sig, bool enable)
{
    mcpwm->operators[operator_id].fh_cfg0.val &= ~(1 << (7 - fault_sig));
    mcpwm->operators[operator_id].fh_cfg0.val |= (enable << (7 - fault_sig));
}

static inline void mcpwm_ll_fault_enable_cbc_mode(mcpwm_dev_t *mcpwm, int operator_id, int fault_sig, bool enable)
{
    mcpwm->operators[operator_id].fh_cfg0.val &= ~(1 << (3 - fault_sig));
    mcpwm->operators[operator_id].fh_cfg0.val |= (enable << (3 - fault_sig));
}

static inline void mcpwm_ll_fault_enable_cbc_refresh_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].fh_cfg1.val |= 1 << 1;
    } else {
        mcpwm->operators[operator_id].fh_cfg1.val &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_fault_enable_cbc_refresh_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].fh_cfg1.val |= 1 << 2;
    } else {
        mcpwm->operators[operator_id].fh_cfg1.val &= ~(1 << 2);
    }
}

static inline void mcpwm_ll_fault_enable_sw_cbc(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].fh_cfg0.fh_sw_cbc = enable;
}

static inline void mcpwm_ll_fault_enable_sw_oneshot(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].fh_cfg0.fh_sw_ost = enable;
}

static inline void mcpwm_ll_fault_trigger_sw_cbc(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].fh_cfg1.fh_force_cbc = ~mcpwm->operators[operator_id].fh_cfg1.fh_force_cbc;
}

static inline void mcpwm_ll_fault_trigger_sw_ost(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].fh_cfg1.fh_force_ost = ~mcpwm->operators[operator_id].fh_cfg1.fh_force_ost;
}

static inline void mcpwm_ll_generator_set_action_on_trip_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, mcpwm_trip_type_t trip, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) {
        mcpwm->operators[operator_id].fh_cfg0.val &= ~(0x03 << (8 + 8 * generator_id + 4 * trip + 2));
        mcpwm->operators[operator_id].fh_cfg0.val |= action << (8 + 8 * generator_id + 4 * trip + 2);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) {
        mcpwm->operators[operator_id].fh_cfg0.val &= ~(0x03 << (8 + 8 * generator_id + 4 * trip));
        mcpwm->operators[operator_id].fh_cfg0.val |= action << (8 + 8 * generator_id + 4 * trip);
    }
}

static inline bool mcpwm_ll_fault_is_ost_on(mcpwm_dev_t *mcpwm, int op)
{
    return mcpwm->operators[op].fh_status.fh_ost_on;
}

static inline bool mcpwm_ll_fault_is_cbc_on(mcpwm_dev_t *mcpwm, int op)
{
    return mcpwm->operators[op].fh_status.fh_cbc_on;
}

/********************* Capture registers *******************/

static inline void mcpwm_ll_capture_enable_timer(mcpwm_dev_t *mcpwm, bool enable)
{
    mcpwm->cap_timer_cfg.cap_timer_en = enable;
}

static inline void mcpwm_ll_capture_enable_channel(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    mcpwm->cap_chn_cfg[channel].capn_en = enable;
}

static inline void mcpwm_ll_capture_set_sync_phase_value(mcpwm_dev_t *mcpwm, uint32_t phase_value)
{
    mcpwm->cap_timer_phase.cap_timer_phase = phase_value;
}

static inline uint32_t mcpwm_ll_capture_get_sync_phase_value(mcpwm_dev_t *mcpwm)
{
    return mcpwm->cap_timer_phase.cap_timer_phase;
}

static inline void mcpwm_ll_capture_enable_timer_sync(mcpwm_dev_t *mcpwm, bool enable)
{
    mcpwm->cap_timer_cfg.cap_synci_en = enable;
}

static inline void mcpwm_ll_capture_set_internal_timer_synchro(mcpwm_dev_t *mcpwm, int sync_out_timer)
{
    mcpwm->cap_timer_cfg.cap_synci_sel = sync_out_timer + 1;
}

static inline void mcpwm_ll_capture_set_external_synchro(mcpwm_dev_t *mcpwm, int extern_synchro)
{
    mcpwm->cap_timer_cfg.cap_synci_sel = extern_synchro + 4;
}

static inline void mcpwm_ll_capture_trigger_sw_sync(mcpwm_dev_t *mcpwm)
{
    mcpwm->cap_timer_cfg.cap_sync_sw = 1; // auto clear
}

static inline void mcpwm_ll_capture_enable_posedge(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    if (enable) {
        mcpwm->cap_chn_cfg[channel].val |= 1 << 2;
    } else {
        mcpwm->cap_chn_cfg[channel].val &= ~(1 << 2);
    }
}

static inline void mcpwm_ll_capture_enable_negedge(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    if (enable) {
        mcpwm->cap_chn_cfg[channel].val |= 1 << 1;
    } else {
        mcpwm->cap_chn_cfg[channel].val &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_invert_input(mcpwm_dev_t *mcpwm, int channel, bool invert)
{
    mcpwm->cap_chn_cfg[channel].capn_in_invert = invert;
}

static inline void mcpwm_ll_trigger_soft_capture(mcpwm_dev_t *mcpwm, int channel)
{
    mcpwm->cap_chn_cfg[channel].capn_sw = 1; // auto clear
}

__attribute__((always_inline))
static inline uint32_t mcpwm_ll_capture_get_value(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_chn[channel].capn_value;
}

__attribute__((always_inline))
static inline bool mcpwm_ll_capture_is_negedge(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_status.val & (1 << channel) ? true : false;
}

static inline void mcpwm_ll_capture_set_prescale(mcpwm_dev_t *mcpwm, int channel, uint32_t prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->cap_chn_cfg[channel], capn_prescale, prescale - 1);
}

static inline uint32_t mcpwm_ll_capture_get_prescale(mcpwm_dev_t *mcpwm, int channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->cap_chn_cfg[channel], capn_prescale) + 1;
}

#ifdef __cplusplus
}
#endif
