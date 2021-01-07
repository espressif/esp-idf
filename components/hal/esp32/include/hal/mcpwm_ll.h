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

// The LL layer for ESP32 MCPWM register operations

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/mcpwm_struct.h"
#include "hal/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Get the address of peripheral registers
#define MCPWM_LL_GET_HW(ID)     (((ID) == 0) ? &MCPWM0 : &MCPWM1)
#define MCPWM_LL_MAX_PRESCALE   255

/********************* Group registers *******************/

// Set/Get group clock: PWM_clk = CLK_160M / (prescale + 1)
static inline void mcpwm_ll_group_set_clock(mcpwm_dev_t *mcpwm, unsigned long long group_clk_hz)
{
    mcpwm->clk_cfg.prescale = (SOC_MCPWM_BASE_CLK_HZ / group_clk_hz) - 1;
}

static inline unsigned long long mcpwm_ll_group_get_clock(mcpwm_dev_t *mcpwm)
{
    return SOC_MCPWM_BASE_CLK_HZ / (mcpwm->clk_cfg.prescale + 1);
}

static inline void mcpwm_ll_group_enable_shadow_mode(mcpwm_dev_t *mcpwm)
{
    mcpwm->update_cfg.global_up_en = 1;
    mcpwm->update_cfg.op0_up_en = 1;
    mcpwm->update_cfg.op1_up_en = 1;
    mcpwm->update_cfg.op2_up_en = 1;
}

static inline void mcpwm_ll_group_flush_shadow(mcpwm_dev_t *mcpwm)
{
    mcpwm->update_cfg.val ^= (1 << 1);
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

static inline uint32_t mcpwm_ll_intr_get_capture_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 27) & 0x07;
}

//////////// clear interrupt status for each event ////////////////

static inline void mcpwm_ll_intr_clear_timer_stop_status(mcpwm_dev_t *mcpwm, uint32_t timer_mask)
{
    mcpwm->int_clr.val |= (timer_mask & 0x07) << 0;
}

static inline void mcpwm_ll_intr_clear_timer_tez_status(mcpwm_dev_t *mcpwm, uint32_t timer_mask)
{
    mcpwm->int_clr.val |= (timer_mask & 0x07) << 3;
}

static inline void mcpwm_ll_intr_clear_timer_tep_status(mcpwm_dev_t *mcpwm, uint32_t timer_mask)
{
    mcpwm->int_clr.val |= (timer_mask & 0x07) << 6;
}

static inline void mcpwm_ll_intr_clear_fault_enter_status(mcpwm_dev_t *mcpwm, uint32_t fault_mask)
{
    mcpwm->int_clr.val |= (fault_mask & 0x07) << 9;
}

static inline void mcpwm_ll_intr_clear_fault_exit_status(mcpwm_dev_t *mcpwm, uint32_t fault_mask)
{
    mcpwm->int_clr.val |= (fault_mask & 0x07) << 12;
}

static inline void mcpwm_ll_intr_clear_compare_status(mcpwm_dev_t *mcpwm, uint32_t operator_mask, uint32_t cmp_id)
{
    mcpwm->int_clr.val |= (operator_mask & 0x07) << (15 + cmp_id * 3);
}

static inline void mcpwm_ll_intr_clear_trip_cbc_status(mcpwm_dev_t *mcpwm, uint32_t cbc_mask)
{
    mcpwm->int_clr.val |= (cbc_mask & 0x07) << 21;
}

static inline void mcpwm_ll_intr_clear_trip_ost_status(mcpwm_dev_t *mcpwm, uint32_t ost_mask)
{
    mcpwm->int_clr.val |= (ost_mask & 0x07) << 24;
}

static inline void mcpwm_ll_intr_clear_capture_status(mcpwm_dev_t *mcpwm, uint32_t capture_mask)
{
    mcpwm->int_clr.val |= (capture_mask & 0x07) << 27;
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

static inline void mcpwm_ll_intr_enable_fault(mcpwm_dev_t *mcpwm, uint32_t fault_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= 1 << (9 + fault_id);  // enter fault interrupt
        mcpwm->int_ena.val |= 1 << (12 + fault_id); // exit fault interrupt
    } else {
        mcpwm->int_ena.val &= ~(1 << (9 + fault_id));
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

static inline void mcpwm_ll_intr_enable_trip(mcpwm_dev_t *mcpwm, uint32_t operator_id, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= (1 << (21 + operator_id));
        mcpwm->int_ena.val |= (1 << (24 + operator_id));
    } else {
        mcpwm->int_ena.val &= ~(1 << (21 + operator_id));
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

static inline void mcpwm_ll_timer_set_clock(mcpwm_dev_t *mcpwm, int timer_id, unsigned long long group_clock, unsigned long long timer_clock)
{
    mcpwm->timer[timer_id].period.prescale = group_clock / timer_clock - 1;
}

static inline unsigned long long mcpwm_ll_timer_get_clock(mcpwm_dev_t *mcpwm, int timer_id, unsigned long long group_clock)
{
    return group_clock / (mcpwm->timer[timer_id].period.prescale + 1);
}

static inline void mcpwm_ll_timer_set_peak(mcpwm_dev_t *mcpwm, int timer_id, uint32_t peak, bool symmetric)
{
    if (!symmetric) { // in asymmetric mode, period = [0,peak-1]
        mcpwm->timer[timer_id].period.period = peak - 1;
    } else { // in symmetric mode, period = [0,peak-1] + [peak,1]
        mcpwm->timer[timer_id].period.period = peak;
    }
}

static inline uint32_t mcpwm_ll_timer_get_peak(mcpwm_dev_t *mcpwm, int timer_id, bool symmetric)
{
    // asymmetric mode
    if (!symmetric) {
        return mcpwm->timer[timer_id].period.period + 1;
    }
    // symmetric mode
    return mcpwm->timer[timer_id].period.period;
}

static inline void mcpwm_ll_timer_update_period_at_once(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].period.upmethod = 0;
}

static inline void mcpwm_ll_timer_enable_update_period_on_tez(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    if (enable) {
        mcpwm->timer[timer_id].period.upmethod |= 0x01;
    } else {
        mcpwm->timer[timer_id].period.upmethod &= ~0x01;
    }
}

static inline void mcpwm_ll_timer_enable_update_period_on_sync(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    if (enable) {
        mcpwm->timer[timer_id].period.upmethod |= 0x02;
    } else {
        mcpwm->timer[timer_id].period.upmethod &= ~0x02;
    }
}

static inline void mcpwm_ll_timer_set_count_mode(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_count_mode_t mode)
{
    switch (mode) {
    case MCPWM_TIMER_COUNT_MODE_PAUSE:
        mcpwm->timer[timer_id].mode.mode = 0;
        break;
    case MCPWM_TIMER_COUNT_MODE_UP:
        mcpwm->timer[timer_id].mode.mode = 1;
        break;
    case MCPWM_TIMER_COUNT_MODE_DOWN:
        mcpwm->timer[timer_id].mode.mode = 2;
        break;
    case MCPWM_TIMER_COUNT_MODE_UP_DOWN:
        mcpwm->timer[timer_id].mode.mode = 3;
        break;
    }
}

static inline mcpwm_timer_count_mode_t mcpwm_ll_timer_get_count_mode(mcpwm_dev_t *mcpwm, int timer_id)
{
    switch (mcpwm->timer[timer_id].mode.mode) {
    case 0:
        return MCPWM_TIMER_COUNT_MODE_PAUSE;
    case 1:
        return MCPWM_TIMER_COUNT_MODE_UP;
    case 2:
        return MCPWM_TIMER_COUNT_MODE_DOWN;
    case 3:
        return MCPWM_TIMER_COUNT_MODE_UP_DOWN;
    }
}

static inline void mcpwm_ll_timer_set_operate_command(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_operate_cmd_t mode)
{
    switch (mode) {
    case MCPWM_TIMER_STOP_AT_ZERO:
        mcpwm->timer[timer_id].mode.start = 0;
        break;
    case MCPWM_TIMER_STOP_AT_PEAK:
        mcpwm->timer[timer_id].mode.start = 1;
        break;
    case MCPWM_TIMER_START_NO_STOP:
        mcpwm->timer[timer_id].mode.start = 2;
        break;
    case MCPWM_TIMER_START_STOP_AT_ZERO:
        mcpwm->timer[timer_id].mode.start = 3;
        break;
    case MCPWM_TIMER_START_STOP_AT_PEAK:
        mcpwm->timer[timer_id].mode.start = 4;
        break;
    }
}

static inline void mcpwm_ll_timer_set_count_value(mcpwm_dev_t *mcpwm, int timer_id, uint32_t value)
{
    // we use software sync to set count value
    int previous_phase = mcpwm->timer[timer_id].sync.timer_phase;
    mcpwm->timer[timer_id].sync.timer_phase = value;
    mcpwm->timer[timer_id].sync.sync_sw = ~mcpwm->timer[timer_id].sync.sync_sw;
    mcpwm->timer[timer_id].sync.timer_phase = previous_phase;
}

static inline uint32_t mcpwm_ll_timer_get_count_value(mcpwm_dev_t *mcpwm, int timer_id)
{
    return mcpwm->timer[timer_id].status.value;
}

static inline bool mcpwm_ll_is_timer_decreasing(mcpwm_dev_t *mcpwm, int timer_id)
{
    return mcpwm->timer[timer_id].status.direction;
}

static inline void mcpwm_ll_timer_enable_sync_input(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    mcpwm->timer[timer_id].sync.in_en = enable;
}

static inline void mcpwm_ll_timer_sync_out_same_in(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].sync.out_sel = 0;
}

static inline void mcpwm_ll_timer_sync_out_on_timer_event(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_event_t event)
{
    if (event == MCPWM_TIMER_EVENT_ZERO) {
        mcpwm->timer[timer_id].sync.out_sel = 1;
    } else if (event == MCPWM_TIMER_EVENT_PEAK) {
        mcpwm->timer[timer_id].sync.out_sel = 2;
    }
}

static inline void mcpwm_ll_timer_disable_sync_out(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].sync.out_sel = 3;
}

static inline void mcpwm_ll_timer_trigger_sw_sync(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].sync.sync_sw = ~mcpwm->timer[timer_id].sync.sync_sw;
}

static inline void mcpwm_ll_timer_set_sync_phase_value(mcpwm_dev_t *mcpwm, int timer_id, uint32_t reload_val)
{
    mcpwm->timer[timer_id].sync.timer_phase = reload_val;
}

static inline uint32_t mcpwm_ll_timer_get_sync_phase_value(mcpwm_dev_t *mcpwm, int timer_id)
{
    return mcpwm->timer[timer_id].sync.timer_phase;
}

static inline void mcpwm_ll_timer_set_sync_phase_direction(mcpwm_dev_t *mcpwm, int timer_id, bool decrease)
{
    mcpwm->timer[timer_id].sync.phase_direct = decrease;
}

static inline void mcpwm_ll_timer_enable_sync_from_internal_timer(mcpwm_dev_t *mcpwm, int this_timer, int internal_sync_timer)
{
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (this_timer * 3));
    mcpwm->timer_synci_cfg.val |= (internal_sync_timer + 1) << (this_timer * 3);
}

static inline void mcpwm_ll_timer_enable_sync_from_external(mcpwm_dev_t *mcpwm, int this_timer, int extern_syncer)
{
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (this_timer * 3));
    mcpwm->timer_synci_cfg.val |= (extern_syncer + 4) << (this_timer * 3);
}

static inline void mcpwm_ll_invert_external_syncer(mcpwm_dev_t *mcpwm, int sync_id, bool invert)
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
        mcpwm->timer_sel.operator0_sel = timer_id;
    } else if (operator_id == 1) {
        mcpwm->timer_sel.operator1_sel = timer_id;
    } else {
        mcpwm->timer_sel.operator2_sel = timer_id;
    }
}

static inline void mcpwm_ll_operator_update_compare_at_once(mcpwm_dev_t *mcpwm, int operator_id, int compare_id)
{
    mcpwm->channel[operator_id].cmpr_cfg.val &= ~(0x0F << (4 * compare_id));
}

static inline void mcpwm_ll_operator_enable_update_compare_on_tez(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].cmpr_cfg.val |= (1 << 0) << (4 * compare_id);
    } else {
        mcpwm->channel[operator_id].cmpr_cfg.val &= ~((1 << 0) << (4 * compare_id));
    }
}

static inline void mcpwm_ll_operator_enable_update_compare_on_tep(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].cmpr_cfg.val |= (1 << 1) << (4 * compare_id);
    } else {
        mcpwm->channel[operator_id].cmpr_cfg.val &= ~((1 << 1) << (4 * compare_id));
    }
}

static inline void mcpwm_ll_operator_enable_update_compare_on_sync(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].cmpr_cfg.val |= (1 << 2) << (4 * compare_id);
    } else {
        mcpwm->channel[operator_id].cmpr_cfg.val &= ~((1 << 2) << (4 * compare_id));
    }
}

static inline void mcpwm_ll_operator_set_compare_value(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, uint32_t compare_value)
{
    mcpwm->channel[operator_id].cmpr_value[compare_id].cmpr_val = compare_value;
}

static inline uint32_t mcpwm_ll_operator_get_compare_value(mcpwm_dev_t *mcpwm, int operator_id, int compare_id)
{
    return mcpwm->channel[operator_id].cmpr_value[compare_id].cmpr_val;
}

static inline void mcpwm_ll_operator_update_action_at_once(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->channel[operator_id].gen_cfg0.upmethod = 0;
}

static inline void mcpwm_ll_operator_enable_update_action_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].gen_cfg0.upmethod |= 1 << 0;
    } else {
        mcpwm->channel[operator_id].gen_cfg0.upmethod &= ~(1 << 0);
    }
}

static inline void mcpwm_ll_operator_enable_update_action_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].gen_cfg0.upmethod |= 1 << 1;
    } else {
        mcpwm->channel[operator_id].gen_cfg0.upmethod &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_operator_enable_update_action_on_sync(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].gen_cfg0.upmethod |= 1 << 2;
    } else {
        mcpwm->channel[operator_id].gen_cfg0.upmethod &= ~(1 << 2);
    }
}

/********************* Generator registers *******************/

static inline void mcpwm_ll_generator_reset_actions(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    mcpwm->channel[operator_id].generator[generator_id].val = 0;
}

static inline void mcpwm_ll_generator_set_action_on_timer_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, mcpwm_timer_event_t event, mcpwm_generator_action_t action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // utez, utep
        mcpwm->channel[operator_id].generator[generator_id].val &= ~(0x03 << (event * 2));
        mcpwm->channel[operator_id].generator[generator_id].val |= action << (event * 2);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dtez, dtep
        mcpwm->channel[operator_id].generator[generator_id].val &= ~(0x03 << (event * 2 + 12));
        mcpwm->channel[operator_id].generator[generator_id].val |= action << (event * 2 + 12);
    }
}

static inline void mcpwm_ll_generator_set_action_on_compare_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, int cmp_id, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // utea, uteb
        mcpwm->channel[operator_id].generator[generator_id].val &= ~(0x03 << (cmp_id * 2 + 4));
        mcpwm->channel[operator_id].generator[generator_id].val |= action << (cmp_id * 2 + 4);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dtea, dteb
        mcpwm->channel[operator_id].generator[generator_id].val &= ~(0x03 << (cmp_id * 2 + 16));
        mcpwm->channel[operator_id].generator[generator_id].val |= action << (cmp_id * 2 + 16);
    }
}

static inline void mcpwm_ll_generator_set_action_on_trigger_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, int trig_id, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // ut0, ut1
        mcpwm->channel[operator_id].generator[generator_id].val &= ~(0x03 << (trig_id * 2 + 8));
        mcpwm->channel[operator_id].generator[generator_id].val |= action << (trig_id * 2 + 8);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dt0, dt1
        mcpwm->channel[operator_id].generator[generator_id].val &= ~(0x03 << (trig_id * 2 + 20));
        mcpwm->channel[operator_id].generator[generator_id].val |= action << (trig_id * 2 + 20);
    }
}

static inline void mcpwm_ll_gen_set_onetime_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id, int action)
{
    if (generator_id == 0) {
        mcpwm->channel[operator_id].gen_force.a_nciforce_mode = action;
        mcpwm->channel[operator_id].gen_force.a_nciforce = ~mcpwm->channel[operator_id].gen_force.a_nciforce;
    } else {
        mcpwm->channel[operator_id].gen_force.b_nciforce_mode = action;
        mcpwm->channel[operator_id].gen_force.b_nciforce = ~mcpwm->channel[operator_id].gen_force.b_nciforce;
    }
}

static inline void mcpwm_ll_gen_set_continuous_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id, int action)
{
    mcpwm->channel[operator_id].gen_force.cntu_force_upmethod = 0; // force action immediately
    if (generator_id == 0) {
        mcpwm->channel[operator_id].gen_force.a_cntuforce_mode = action;
    } else {
        mcpwm->channel[operator_id].gen_force.b_cntuforce_mode = action;
    }
}

/********************* Dead time registers *******************/

static inline void mcpwm_ll_deadtime_set_resolution_same_to_timer(mcpwm_dev_t *mcpwm, int operator_id, bool same)
{
    mcpwm->channel[operator_id].db_cfg.clk_sel = same;
}

static inline void mcpwm_ll_deadtime_red_select_generator(mcpwm_dev_t *mcpwm, int operator_id, int generator)
{
    mcpwm->channel[operator_id].db_cfg.red_insel = generator;
}

static inline void mcpwm_ll_deadtime_fed_select_generator(mcpwm_dev_t *mcpwm, int operator_id, int generator)
{
    mcpwm->channel[operator_id].db_cfg.fed_insel = generator;
}

static inline void mcpwm_ll_deadtime_bypass_path(mcpwm_dev_t *mcpwm, int operator_id, int path, bool bypass)
{
    if (bypass) {
        mcpwm->channel[operator_id].db_cfg.val |= 1 << (path + 15);
    } else {
        mcpwm->channel[operator_id].db_cfg.val &= ~(1 << (path + 15));
    }
}

static inline void mcpwm_ll_deadtime_invert_outpath(mcpwm_dev_t *mcpwm, int operator_id, int path, bool invert)
{
    if (invert) {
        mcpwm->channel[operator_id].db_cfg.val |= 1 << (path + 13);
    } else {
        mcpwm->channel[operator_id].db_cfg.val &= ~(1 << (path + 13));
    }
}

static inline void mcpwm_ll_deadtime_swap_out_path(mcpwm_dev_t *mcpwm, int operator_id, int path, bool swap)
{
    if (swap) {
        mcpwm->channel[operator_id].db_cfg.val |= 1 << (path + 9);
    } else {
        mcpwm->channel[operator_id].db_cfg.val &= ~(1 << (path + 9));
    }
}

static inline void mcpwm_ll_deadtime_enable_deb(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->channel[operator_id].db_cfg.deb_mode = enable;
}

static inline uint32_t mcpwm_ll_deadtime_get_topology_code(mcpwm_dev_t *mcpwm, int operator_id)
{
    return (mcpwm->channel[operator_id].db_cfg.deb_mode << 8) | (mcpwm->channel[operator_id].db_cfg.b_outswap << 7) |
           (mcpwm->channel[operator_id].db_cfg.a_outswap << 6) | (mcpwm->channel[operator_id].db_cfg.fed_insel << 5) |
           (mcpwm->channel[operator_id].db_cfg.red_insel << 4) | (mcpwm->channel[operator_id].db_cfg.fed_outinvert << 3) |
           (mcpwm->channel[operator_id].db_cfg.red_outinvert << 2) | (mcpwm->channel[operator_id].db_cfg.a_outbypass << 1) |
           (mcpwm->channel[operator_id].db_cfg.b_outbypass << 0);
}

static inline void mcpwm_ll_deadtime_set_falling_delay(mcpwm_dev_t *mcpwm, int operator_id, uint32_t fed)
{
    mcpwm->channel[operator_id].db_fed_cfg.fed = fed - 1;
}

static inline uint32_t mcpwm_ll_deadtime_get_falling_delay(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->channel[operator_id].db_fed_cfg.fed + 1;
}

static inline void mcpwm_ll_deadtime_set_rising_delay(mcpwm_dev_t *mcpwm, int operator_id, uint32_t red)
{
    mcpwm->channel[operator_id].db_red_cfg.red = red - 1;
}

static inline uint32_t mcpwm_ll_deadtime_get_rising_delay(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->channel[operator_id].db_red_cfg.red + 1;
}

static inline void mcpwm_ll_deadtime_update_delay_at_once(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->channel[operator_id].db_cfg.fed_upmethod = 0;
    mcpwm->channel[operator_id].db_cfg.red_upmethod = 0;
}

static inline void mcpwm_ll_deadtime_enable_update_delay_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].db_cfg.fed_upmethod |= 1 << 0;
        mcpwm->channel[operator_id].db_cfg.red_upmethod |= 1 << 0;
    } else {
        mcpwm->channel[operator_id].db_cfg.fed_upmethod &= ~(1 << 0);
        mcpwm->channel[operator_id].db_cfg.red_upmethod &= ~(1 << 0);
    }
}

static inline void mcpwm_ll_deadtime_enable_update_delay_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].db_cfg.fed_upmethod |= 1 << 1;
        mcpwm->channel[operator_id].db_cfg.red_upmethod |= 1 << 1;
    } else {
        mcpwm->channel[operator_id].db_cfg.fed_upmethod &= ~(1 << 1);
        mcpwm->channel[operator_id].db_cfg.red_upmethod &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_deadtime_enable_update_delay_on_sync(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->channel[operator_id].db_cfg.fed_upmethod |= 1 << 2;
        mcpwm->channel[operator_id].db_cfg.red_upmethod |= 1 << 2;
    } else {
        mcpwm->channel[operator_id].db_cfg.fed_upmethod &= ~(1 << 2);
        mcpwm->channel[operator_id].db_cfg.red_upmethod &= ~(1 << 2);
    }
}

/********************* Carrier registers *******************/

static inline void mcpwm_ll_carrier_enable(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->channel[operator_id].carrier_cfg.en = enable;
}

static inline void mcpwm_ll_carrier_set_prescale(mcpwm_dev_t *mcpwm, int operator_id, uint8_t prescale)
{
    mcpwm->channel[operator_id].carrier_cfg.prescale = prescale - 1;
}

static inline uint8_t mcpwm_ll_carrier_get_prescale(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->channel[operator_id].carrier_cfg.prescale + 1;
}

static inline void mcpwm_ll_carrier_set_duty(mcpwm_dev_t *mcpwm, int operator_id, uint8_t carrier_duty)
{
    mcpwm->channel[operator_id].carrier_cfg.duty = carrier_duty;
}

static inline uint8_t mcpwm_ll_carrier_get_duty(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->channel[operator_id].carrier_cfg.duty;
}

static inline void mcpwm_ll_carrier_out_invert(mcpwm_dev_t *mcpwm, int operator_id, bool invert)
{
    mcpwm->channel[operator_id].carrier_cfg.out_invert = invert;
}

static inline void mcpwm_ll_carrier_in_invert(mcpwm_dev_t *mcpwm, int operator_id, bool invert)
{
    mcpwm->channel[operator_id].carrier_cfg.in_invert = invert;
}

static inline void mcpwm_ll_carrier_set_oneshot_width(mcpwm_dev_t *mcpwm, int operator_id, uint8_t pulse_width)
{
    mcpwm->channel[operator_id].carrier_cfg.oshtwth = pulse_width - 1;
}

static inline uint8_t mcpwm_ll_carrier_get_oneshot_width(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->channel[operator_id].carrier_cfg.oshtwth + 1;
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
    mcpwm->channel[operator_id].tz_cfg1.clr_ost = 0;
    mcpwm->channel[operator_id].tz_cfg1.clr_ost = 1;
}

static inline void mcpwm_ll_fault_enable_oneshot_mode(mcpwm_dev_t *mcpwm, int operator_id, int fault_sig, bool enable)
{
    if (fault_sig == 0) {
        mcpwm->channel[operator_id].tz_cfg0.f0_ost = enable;
    } else if (fault_sig == 1) {
        mcpwm->channel[operator_id].tz_cfg0.f1_ost = enable;
    } else {
        mcpwm->channel[operator_id].tz_cfg0.f2_ost = enable;
    }
}

static inline void mcpwm_ll_fault_enable_cbc_mode(mcpwm_dev_t *mcpwm, int operator_id, int fault_sig, bool enable)
{
    if (fault_sig == 0) {
        mcpwm->channel[operator_id].tz_cfg0.f0_cbc = enable;
    } else if (fault_sig == 1) {
        mcpwm->channel[operator_id].tz_cfg0.f1_cbc = enable;
    } else {
        mcpwm->channel[operator_id].tz_cfg0.f2_cbc = enable;
    }
    mcpwm->channel[operator_id].tz_cfg1.cbcpulse = 1 << 0;
}

static inline void mcpwm_ll_fault_enable_sw_cbc(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->channel[operator_id].tz_cfg0.sw_cbc = enable;
}

static inline void mcpwm_ll_fault_enable_sw_oneshot(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->channel[operator_id].tz_cfg0.sw_ost = enable;
}

static inline void mcpwm_ll_fault_trigger_sw_cbc(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->channel[operator_id].tz_cfg1.force_cbc = ~mcpwm->channel[operator_id].tz_cfg1.force_cbc;
}

static inline void mcpwm_ll_fault_trigger_sw_oneshot(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->channel[operator_id].tz_cfg1.force_ost = ~mcpwm->channel[operator_id].tz_cfg1.force_ost;
}

static inline void mcpwm_ll_generator_set_action_on_fault_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
        mcpwm_timer_direction_t direction, mcpwm_fault_reaction_t reaction, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) {
        mcpwm->channel[operator_id].tz_cfg0.val &= ~(0x03 << (8 + 8 * generator_id + 4 * reaction + 2));
        mcpwm->channel[operator_id].tz_cfg0.val |= action << (8 + 8 * generator_id + 4 * reaction + 2);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) {
        mcpwm->channel[operator_id].tz_cfg0.val &= ~(0x03 << (8 + 8 * generator_id + 4 * reaction));
        mcpwm->channel[operator_id].tz_cfg0.val |= action << (8 + 8 * generator_id + 4 * reaction);
    }
}

static inline bool mcpwm_ll_fault_is_ost_on(mcpwm_dev_t *mcpwm, int op)
{
    return mcpwm->channel[op].tz_status.ost_on;
}

static inline bool mcpwm_ll_fault_is_cbc_on(mcpwm_dev_t *mcpwm, int op)
{
    return mcpwm->channel[op].tz_status.cbc_on;
}

/********************* Capture registers *******************/

static inline void mcpwm_ll_capture_enable_timer(mcpwm_dev_t *mcpwm, bool enable)
{
    mcpwm->cap_timer_cfg.timer_en = enable;
}

static inline void mcpwm_ll_capture_enable_channel(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    mcpwm->cap_cfg_ch[channel].en = enable;
}

static inline void mcpwm_ll_capture_set_sync_phase(mcpwm_dev_t *mcpwm, uint32_t phase_value)
{
    mcpwm->cap_timer_phase = phase_value;
}

static inline uint32_t mcpwm_ll_capture_get_sync_phase(mcpwm_dev_t *mcpwm)
{
    return mcpwm->cap_timer_phase;
}

static inline void mcpwm_ll_capture_enable_timer_sync(mcpwm_dev_t *mcpwm, bool enable)
{
    mcpwm->cap_timer_cfg.synci_en = enable;
}

static inline void mcpwm_ll_capture_set_internal_timer_syncer(mcpwm_dev_t *mcpwm, int sync_out_timer)
{
    mcpwm->cap_timer_cfg.synci_sel = sync_out_timer + 1;
}

static inline void mcpwm_ll_capture_set_external_syncer(mcpwm_dev_t *mcpwm, int extern_syncer)
{
    mcpwm->cap_timer_cfg.synci_sel = extern_syncer + 4;
}

static inline void mcpwm_ll_capture_trigger_sw_sync(mcpwm_dev_t *mcpwm)
{
    mcpwm->cap_timer_cfg.sync_sw = 1; // auto clear
}

static inline void mcpwm_ll_capture_enable_posedge(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    if (enable) {
        mcpwm->cap_cfg_ch[channel].val |= 1 << 2;
    } else {
        mcpwm->cap_cfg_ch[channel].val &= ~(1 << 2);
    }
}

static inline void mcpwm_ll_capture_enable_negedge(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    if (enable) {
        mcpwm->cap_cfg_ch[channel].val |= 1 << 1;
    } else {
        mcpwm->cap_cfg_ch[channel].val &= ~(1 << 1);
    }
}

static inline void mcpwm_ll_invert_input(mcpwm_dev_t *mcpwm, int channel, bool invert)
{
    mcpwm->cap_cfg_ch[channel].in_invert = invert;
}

static inline void mcpwm_ll_trigger_soft_capture(mcpwm_dev_t *mcpwm, int channel)
{
    mcpwm->cap_cfg_ch[channel].sw = 1; // auto clear
}

static inline uint32_t mcpwm_ll_capture_get_value(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_val_ch[channel];
}

static inline bool mcpwm_ll_capture_is_negedge(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_status.val & (1 << channel) ? true : false;
}

static inline void mcpwm_ll_capture_set_prescale(mcpwm_dev_t *mcpwm, int channel, uint32_t prescale)
{
    mcpwm->cap_cfg_ch[channel].prescale = prescale - 1;
}

static inline uint32_t mcpwm_ll_capture_get_prescale(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_cfg_ch[channel].prescale + 1;
}

#ifdef __cplusplus
}
#endif
