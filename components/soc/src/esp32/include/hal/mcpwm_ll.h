// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

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

// The LL layer for ESP32 MCPWM register operations

#pragma once

#include "hal/hal_defs.h"
#include "soc/mcpwm_periph.h"
#include "soc/mcpwm_struct.h"
#include "hal/mcpwm_types.h"
#include "soc/mcpwm_caps.h"
#include "hal/hal_defs.h"

#include "esp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Get the address of peripheral registers
#define MCPWM_LL_GET_HW(ID) (((ID)==0)? &MCPWM0: &MCPWM1)


/********************* Global *******************/
/**
 * Initialize common registers.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 */
static inline void mcpwm_ll_init(mcpwm_dev_t *mcpwm)
{
    mcpwm->update_cfg.global_up_en = 1;
    mcpwm->update_cfg.global_force_up = 1;
    mcpwm->update_cfg.global_force_up = 0;
}

/**
 * Set the prescale of the PWM main clock to the input clock.
 *
 * Input clock is 160MHz, PWM main clock cycle = 6.25ns*(prescale + 1).
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param prescale Prescale factor, 0-255.
 */
static inline void mcpwm_ll_set_clock_prescale(mcpwm_dev_t *mcpwm, int prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->clk_cfg, prescale, prescale);
}

STATIC_HAL_REG_CHECK(MCPWM, MCPWM_LL_INTR_CAP0, MCPWM_CAP0_INT_RAW);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_LL_INTR_CAP1, MCPWM_CAP1_INT_RAW);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_LL_INTR_CAP2, MCPWM_CAP2_INT_RAW);

/**
 * Get raw interrupt status.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @return The triggered interrupts, ORed by active interrupts.
 */
static inline mcpwm_intr_t mcpwm_ll_get_intr(mcpwm_dev_t *mcpwm)
{
    return mcpwm->int_raw.val;
}

/**
 * Clear the interrupts.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param intr Bitwise ORed interrupts to clear.
 */
static inline void mcpwm_ll_clear_intr(mcpwm_dev_t *mcpwm, mcpwm_intr_t intr)
{
    mcpwm->int_clr.val = intr;
}

/********************* Timer *******************/
/**
 * Set the prescale of the Timer_x clock to the PWM main clock.
 *
 * Timer clock frequency = PWM main clock frequency/(prescale + 1).
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to set the prescale, 0-2.
 * @param prescale Prescale factor, 0-255.
 */
static inline void mcpwm_ll_timer_set_prescale(mcpwm_dev_t *mcpwm, int timer, uint32_t prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer].period, prescale, prescale);
}


STATIC_HAL_REG_CHECK(MCPWM, MCPWM_UP_COUNTER,        1);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_DOWN_COUNTER,      2);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_UP_DOWN_COUNTER,   3);

/**
 * Set the counting mode for the PWM timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to change counting mode, 0-2.
 * @param mode Counting mode to use.
 */
static inline void mcpwm_ll_timer_set_count_mode(mcpwm_dev_t *mcpwm, int timer, mcpwm_counter_type_t mode)
{
    mcpwm->timer[timer].mode.mode = mode;
}

/**
 * Start a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to start, 0-2.
 */
static inline void mcpwm_ll_timer_start(mcpwm_dev_t *mcpwm, int timer)
{
    mcpwm->timer[timer].mode.start = 2;
}

/**
 * Stop a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to stop, 0-2.
 */
static inline void mcpwm_ll_timer_stop(mcpwm_dev_t *mcpwm, int timer)
{
    mcpwm->timer[timer].mode.start = 0;
}

/**
 * Set the overflow period of a timer.
 *
 * The overflow rate will be Frequency of timer / period.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to set period, 0-2.
 * @param period Total timer count of each period, 0-65535.
 */
static inline void mcpwm_ll_timer_set_period(mcpwm_dev_t *mcpwm, int timer, uint32_t period)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer].period, period, period - 1);
    mcpwm->timer[timer].period.upmethod = 0;
}

/**
 * Get the period setting of a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to get the period, 0-2.
 * @return Period setting value.
 */
static inline uint32_t mcpwm_ll_timer_get_period(mcpwm_dev_t *mcpwm, int timer)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer].period, period) + 1;
}

/********************* Sync *******************/
/**
 * Enable the synchronization feature for a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer Timer to set, 0-2.
 * @param enable true to enable, otherwise false.
 */
static inline void mcpwm_ll_sync_enable(mcpwm_dev_t *mcpwm, int timer, bool enable)
{
    if (enable) {
        mcpwm->timer[timer].sync.out_sel = 0;
        mcpwm->timer[timer].sync.in_en = 1;
    } else {
        mcpwm->timer[timer].sync.in_en = 0;
    }
}

/**
 * Set the phase (counter value) to reload when the sync signal triggers.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer Timer to set, 0-2.
 * @param reload_val The reloaded value.
 */
static inline void mcpwm_ll_sync_set_phase(mcpwm_dev_t *mcpwm, int timer, uint32_t reload_val)
{
    mcpwm->timer[timer].sync.timer_phase = reload_val;
}

STATIC_HAL_REG_CHECK(MCPWM, MCPWM_SELECT_SYNC0, 4);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_SELECT_SYNC1, 5);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_SELECT_SYNC2, 6);
/**
 * Set the sync signal source for a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param timer The timer to set, 0-2.
 * @param sync_sig The synchronization signal to use.
 */
static inline void mcpwm_ll_sync_set_input(mcpwm_dev_t *mcpwm, int timer, mcpwm_sync_signal_t sync_sig)
{
    if (timer == 0) {
        mcpwm->timer_synci_cfg.t0_in_sel = sync_sig;
    } else if (timer == 1) {
        mcpwm->timer_synci_cfg.t1_in_sel = sync_sig;
    } else {   //MCPWM_TIMER_2
        mcpwm->timer_synci_cfg.t2_in_sel = sync_sig;
    }
}

/********************* Comparator *******************/
/**
 * Select a timer for the specified operator to use.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    The operator to choose timer, 0-2.
 * @param timer The timer to use, 0-2.
 */
static inline void mcpwm_ll_operator_select_timer(mcpwm_dev_t *mcpwm, int op, int timer)
{
    if (op == 0) {
        mcpwm->timer_sel.operator0_sel = timer;
    } else if (op == 1) {
        mcpwm->timer_sel.operator1_sel = timer;
    } else {
        mcpwm->timer_sel.operator2_sel = timer;
    }
}

/**
 * Set the update method of the compare value of a timer
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    Operator to set, 0-2.
 */
static inline void mcpwm_ll_operator_set_compare_upmethod(mcpwm_dev_t *mcpwm, int op)
{
    mcpwm->channel[op].cmpr_cfg.a_upmethod = BIT(0);
    mcpwm->channel[op].cmpr_cfg.b_upmethod = BIT(0);
}

/**
 * Get one of the compare value of a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    The operator to get, 0-2.
 * @param cmp_n Comparer id to get, 0-1.
 * @return The set compare value.
 */
static inline uint32_t mcpwm_ll_operator_get_compare(mcpwm_dev_t *mcpwm, int op, int cmp_n)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->channel[op].cmpr_value[cmp_n], cmpr_val);
}

/**
 * Set one of the compare value of a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    The operator to set, 0-2.
 * @param cmp_n The comparer to set value, 0-1.
 * @param compare The compare value, 0-65535.
 */
static inline void mcpwm_ll_operator_set_compare(mcpwm_dev_t *mcpwm, int op, int cmp_n, uint32_t compare)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->channel[op].cmpr_value[cmp_n], cmpr_val, compare);
}

/********************* Generator *******************/

STATIC_HAL_REG_CHECK(MCPWM, MCPWM_ACTION_NO_CHANGE, 0);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_ACTION_FORCE_LOW, 1);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_ACTION_FORCE_HIGH, 2);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_ACTION_TOGGLE,     3);
/**
 * Set the action will be taken by a operator when its timer counts to zero.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to set action, 0-2.
 * @param gen    One generator of the operator to take the action, 0-1.
 * @param action Action to take.
 */
static inline void mcpwm_ll_gen_set_zero_action(mcpwm_dev_t *mcpwm, int op, int gen, mcpwm_output_action_t action)
{
    mcpwm->channel[op].generator[gen].utez = action;
    mcpwm->channel[op].generator[gen].dtez = action;
}

/**
 * Set the action will be taken by a operator when its timer counts to the period value.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to set action, 0-2.
 * @param gen    One generator of the operator to take the action, 0-1.
 * @param action Action to take.
 */
static inline void mcpwm_ll_gen_set_period_action(mcpwm_dev_t *mcpwm, int op, int gen, mcpwm_output_action_t action)
{
    mcpwm->channel[op].generator[gen].utep = action;
    mcpwm->channel[op].generator[gen].dtep = action;
}

/**
 * Set the action will be taken by a operator when its timer counts to the compare value.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op        The operator to set action, 0-2.
 * @param gen       One generator of the operator to take the action, 0-1.
 * @param cmp_n     The comparer to use.
 * @param up_action The action to take when the counter is counting up.
 * @param down_action The action to take when the counter is counting down.
 */
static inline void mcpwm_ll_gen_set_cmp_action(mcpwm_dev_t *mcpwm, int op, int gen,
        int cmp_n, mcpwm_output_action_t up_action, mcpwm_output_action_t down_action)
{
    if (cmp_n == 0) {
        mcpwm->channel[op].generator[gen].utea = up_action;
        mcpwm->channel[op].generator[gen].dtea = down_action;
    } else {
        mcpwm->channel[op].generator[gen].uteb = up_action;
        mcpwm->channel[op].generator[gen].dteb = down_action;
    }
}

/********************* Fault *******************/
/**
 * Enable the fault detection feature for an input signal.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param fault_sig One of the signals to select, 0-2.
 * @param level The active level of the fault-detection signal.
 */
static inline void mcpwm_ll_fault_enable(mcpwm_dev_t *mcpwm, int fault_sig, bool level)
{
    if (fault_sig == 0) {
        mcpwm->fault_detect.f0_en = 1;
        mcpwm->fault_detect.f0_pole = level;
    } else if (fault_sig == 1) {
        mcpwm->fault_detect.f1_en = 1;
        mcpwm->fault_detect.f1_pole = level;
    } else {   //MCPWM_SELECT_F2
        mcpwm->fault_detect.f2_en = 1;
        mcpwm->fault_detect.f2_pole = level;
    }
}

/**
 * Disable the fault detection of an input signal.
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param fault_sig The signal to disable, 0-2.
 */
static inline void mcpwm_ll_fault_disable(mcpwm_dev_t *mcpwm, int fault_sig)
{
    if (fault_sig == 0) {
        mcpwm->fault_detect.f0_en = 0;
    } else if (fault_sig == 1) {
        mcpwm->fault_detect.f1_en = 0;
    } else {   //MCPWM_SELECT_F2
        mcpwm->fault_detect.f2_en = 0;
    }
}

/**
 * Clear the oneshot fault status of an operator.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to clear, 0-2.
 */
static inline void mcpwm_ll_fault_clear_ost(mcpwm_dev_t *mcpwm, int op)
{
    mcpwm->channel[op].tz_cfg1.clr_ost = 1;
    mcpwm->channel[op].tz_cfg1.clr_ost = 0;
}

/**
 * Use the oneshot mode to handle the fault when it occurs
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to handle the fault signal, 0-2.
 * @param signal The fault signal to set, 0-2.
 * @param enable true to enable oneshot, otherwise false.
 */
static inline void mcpwm_ll_fault_oneshot_enable_signal(mcpwm_dev_t *mcpwm, int op, int signal, bool enable)
{
    if (signal == 0) {
        mcpwm->channel[op].tz_cfg0.f0_ost = enable;
    } else if (signal == 1) {
        mcpwm->channel[op].tz_cfg0.f1_ost = enable;
    } else {   //MCPWM_SELECT_F2
        mcpwm->channel[op].tz_cfg0.f2_ost = enable;
    }
}

/**
 * @brief Get the oneshot enabled status of the operator
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to check, 0-2.
 * @param signal The fault signal to get, 0-2.
 */
static inline bool mcpwm_ll_fault_oneshot_signal_enabled(mcpwm_dev_t *mcpwm, int op, int signal)
{
    if (signal == 0) {
        return mcpwm->channel[op].tz_cfg0.f0_ost;
    } else if (signal == 1) {
        return mcpwm->channel[op].tz_cfg0.f1_ost;
    } else {   //MCPWM_SELECT_F2
        return mcpwm->channel[op].tz_cfg0.f2_ost;
    }
}

/**
 * Use the CBC (cycle-by-cycle) mode to handle the fault when it occurs.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to handle the fault signal, 0-2.
 * @param signal The fault signal to set, 0-2.
 * @param enable true to enable cbc mode, otherwise false.
 */
static inline void mcpwm_ll_fault_cbc_enable_signal(mcpwm_dev_t *mcpwm, int op, int signal, bool enable)
{
    if (signal == 0) {
        mcpwm->channel[op].tz_cfg0.f0_cbc = enable;
    } else if (signal == 1) {
        mcpwm->channel[op].tz_cfg0.f1_cbc = enable;
    } else {   //MCPWM_SELECT_F2
        mcpwm->channel[op].tz_cfg0.f2_cbc = enable;
    }
}

/**
 * Set the action that will be taken when the fault is handled by oneshot.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to handle the fault signal, 0-2.
 * @param gen    The generator to take the action, 0-1.
 * @param up_action     Action to take when fault happens when counting up.
 * @param down_action   Action to take when fault happens when counting down.
 */
static inline void mcpwm_ll_fault_set_oneshot_action(mcpwm_dev_t *mcpwm, int op, int gen,
        mcpwm_output_action_t up_action, mcpwm_output_action_t down_action)
{
    if (gen == 0) {
        mcpwm->channel[op].tz_cfg0.a_ost_u = up_action;
        mcpwm->channel[op].tz_cfg0.a_ost_d = down_action;
    } else {
        mcpwm->channel[op].tz_cfg0.b_ost_u = up_action;
        mcpwm->channel[op].tz_cfg0.b_ost_d = down_action;
    }
}

/**
 * Set the action that will be taken when the fault is handled cycle by cycle.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op     The operator to handle the fault signal, 0-2.
 * @param gen    The generator to take the action, 0-1.
 * @param up_action     Action to take when fault happens when counting up.
 * @param down_action   Action to take when fault happens when counting down.
 */
static inline void mcpwm_ll_fault_set_cyc_action(mcpwm_dev_t *mcpwm, int op, int gen,
        mcpwm_output_action_t up_action, mcpwm_output_action_t down_action)
{
    mcpwm->channel[op].tz_cfg1.cbcpulse = BIT(0);    //immediately
    if (gen == 0) {
        mcpwm->channel[op].tz_cfg0.a_cbc_u = up_action;
        mcpwm->channel[op].tz_cfg0.a_cbc_d = down_action;
    } else {
        mcpwm->channel[op].tz_cfg0.b_cbc_u = up_action;
        mcpwm->channel[op].tz_cfg0.b_cbc_d = down_action;
    }
}

/********************* Dead Zone (deadtime) *******************/
/**
 * Initialize the dead zone feature.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to initialize, 0-2.
 */
static inline void mcpwm_ll_deadtime_init(mcpwm_dev_t *mcpwm, int op)
{
    mcpwm->channel[op].db_cfg.fed_upmethod = BIT(0);
    mcpwm->channel[op].db_cfg.red_upmethod = BIT(0);
    mcpwm->channel[op].db_cfg.clk_sel = 0;
}

/**
 * Set the output dead zone mode applying to the outputs of a timer.
 *
 * If the desired internal connection is not provided, you can write your own inside this function.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    The operator to set, 0-2.
 * @param mode  Dead zone mode to use.
 */
static inline void mcpwm_ll_set_deadtime_mode(mcpwm_dev_t *mcpwm,
        int op, mcpwm_deadtime_type_t mode)
{
#define MCPWM_LL_DEADTIME_REG_MASK (MCPWM_DT0_DEB_MODE_M | MCPWM_DT0_A_OUTSWAP_M | MCPWM_DT0_B_OUTSWAP_M | \
    MCPWM_DT0_RED_INSEL_M | MCPWM_DT0_FED_INSEL_M | MCPWM_DT0_RED_OUTINVERT_M | MCPWM_DT0_FED_OUTINVERT_M | \
    MCPWM_DT0_A_OUTBYPASS_M | MCPWM_DT0_B_OUTBYPASS_M)

    static uint32_t deadtime_mode_settings[MCPWM_DEADTIME_TYPE_MAX] = {
        [MCPWM_BYPASS_RED] =                    0b010000000 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_BYPASS_FED] =                    0b100000000 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_ACTIVE_HIGH_MODE] =              0b000000000 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_ACTIVE_LOW_MODE] =               0b001100000 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE] =   0b001000000 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_ACTIVE_LOW_COMPLIMENT_MODE] =    0b000100000 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_ACTIVE_RED_FED_FROM_PWMXA] =     0b000000011 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_ACTIVE_RED_FED_FROM_PWMXB] =     0b000001011 << MCPWM_DT0_DEB_MODE_S,
        [MCPWM_DEADTIME_BYPASS] =               0b110000000 << MCPWM_DT0_DEB_MODE_S,
    };
    mcpwm->channel[op].db_cfg.val =
        (mcpwm->channel[op].db_cfg.val & (~MCPWM_LL_DEADTIME_REG_MASK)) | deadtime_mode_settings[mode];

#undef MCPWM_LL_DEADTIME_REG_MASK
}

/**
 * Set the delay of the falling edge on the output.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    The operator to set, 0-2.
 * @param fed   Falling delay, by PWM main clock.
 */
static inline void mcpwm_ll_deadtime_set_falling_delay(mcpwm_dev_t *mcpwm, int op, uint32_t fed)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->channel[op].db_fed_cfg, fed, fed);
}

/**
 * Set the delay of the rising edge on the output.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op    The operator to set, 0-2.
 * @param fed   Rising delay, by PWM main clock.
 */
static inline void mcpwm_ll_deadtime_set_rising_delay(mcpwm_dev_t *mcpwm, int op, uint32_t red)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->channel[op].db_red_cfg, red, red);
}

/**
 * Disable (bypass) the dead zone feature.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 */
static inline void mcpwm_ll_deadtime_bypass(mcpwm_dev_t *mcpwm, int op)
{
    mcpwm_ll_set_deadtime_mode(mcpwm, op, MCPWM_DEADTIME_BYPASS);
}

/********************* Carrier *******************/
/**
 * Initialize the carrier feature.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 */
static inline void mcpwm_ll_carrier_init(mcpwm_dev_t *mcpwm, int op)
{
    mcpwm->channel[op].carrier_cfg.in_invert = 0;
}

/**
 * Enable the carrier feature for a timer.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 * @param enable true to enable, otherwise false.
 */
static inline void mcpwm_ll_carrier_enable(mcpwm_dev_t *mcpwm, int op, bool enable)
{
    mcpwm->channel[op].carrier_cfg.en = enable;
}

/**
 * Set the prescale of the carrier timer.
 *
 * The carrier period will be Frequency of PWM main clock/(carrier_period+1).
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 * @param carrier_period The prescale of the carrier clock, 0-15.
 */
static inline void mcpwm_ll_carrier_set_prescale(mcpwm_dev_t *mcpwm, int op, uint8_t carrier_period)
{
    mcpwm->channel[op].carrier_cfg.prescale = carrier_period;
}

/**
 * Set the duty rate of the carrier.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 * @param carrier_duty Duty rate will be (carrier_duty/8)*100%. 0-7.
 */
static inline void mcpwm_ll_carrier_set_duty(mcpwm_dev_t *mcpwm, int op, uint8_t carrier_duty)
{
    mcpwm->channel[op].carrier_cfg.duty = carrier_duty;
}

/**
 * Invert output of the carrier.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 * @param invert true to invert, otherwise false.
 */
static inline void mcpwm_ll_carrier_out_invert(mcpwm_dev_t *mcpwm, int op, bool invert)
{
    mcpwm->channel[op].carrier_cfg.out_invert = invert;
}

/**
 * Set the oneshot pulse width of the carrier.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param op The operator to set, 0-2.
 * @param pulse_width The width of the oneshot pulse, by carrier period. 0 to disable the oneshot pulse.
 */
static inline void mcpwm_ll_carrier_set_oneshot_width(mcpwm_dev_t *mcpwm, int op, uint8_t pulse_width)
{
    mcpwm->channel[op].carrier_cfg.oshtwth = pulse_width;
}

/********************* Capture *******************/
/**
 * Enable the capture feature for a signal
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param cap_sig Signal to enable, 0-2.
 * @param enable true to enable, otherwise false.
 */
static inline void mcpwm_ll_capture_enable(mcpwm_dev_t *mcpwm, int cap_sig, int enable)
{
    if (enable) {
        mcpwm->cap_timer_cfg.timer_en = 1;
        mcpwm->cap_cfg_ch[cap_sig].en = 1;
    } else {
        mcpwm->cap_cfg_ch[cap_sig].en = 0;
    }
}

/**
 * Get the captured value.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param cap_sig Of which signal to get the captured value.
 * @return The captured value
 */
static inline uint32_t mcpwm_ll_get_capture_val(mcpwm_dev_t *mcpwm, int cap_sig)
{
    return mcpwm->cap_val_ch[cap_sig];
}

/**
 * Get the set capture edge.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param cap_sig Which signal the edge capture is applied.
 * @return Capture signal edge: 1 - positive edge, 2 - negtive edge
 */
static inline mcpwm_capture_on_edge_t mcpwm_ll_get_captured_edge(mcpwm_dev_t *mcpwm, int cap_sig)
{
    bool edge;
    if (cap_sig == 0) {
        edge = mcpwm->cap_status.cap0_edge;
    } else if (cap_sig == 1) {
        edge = mcpwm->cap_status.cap1_edge;
    } else {   //2
        edge = mcpwm->cap_status.cap2_edge;
    }
    return (edge ? MCPWM_NEG_EDGE : MCPWM_POS_EDGE);
}

STATIC_HAL_REG_CHECK(MCPWM, MCPWM_NEG_EDGE, BIT(0));
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_POS_EDGE, BIT(1));

/**
 * Select the edge to capture.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param cap_sig   The signal to capture, 0-2.
 * @param cap_edge  The edge to capture, bitwise.
 */
static inline void mcpwm_ll_capture_select_edge(mcpwm_dev_t *mcpwm, int cap_sig,
        mcpwm_capture_on_edge_t cap_edge)
{
    mcpwm->cap_cfg_ch[cap_sig].mode = cap_edge;
}

/**
 * Set the prescale of the input signal to capture.
 *
 * @param mcpwm Address of the MCPWM peripheral registers.
 * @param cap_sig   The prescaled signal to capture, 0-2.
 * @param prescale  Prescal value, 0 to disable.
 */
static inline void mcpwm_ll_capture_set_prescale(mcpwm_dev_t *mcpwm, int cap_sig, uint32_t prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->cap_cfg_ch[cap_sig], prescale, prescale);
}

/**
 * Utility function, get the `mcpwm_intr_t` interrupt enum of a specific capture signal.
 *
 * @param bit x for CAPx.
 * @return the corresponding `mcpwm_intr_t`.
 */
static inline mcpwm_intr_t mcpwm_ll_get_cap_intr_def(int bit)
{
    return BIT(bit + MCPWM_CAP0_INT_RAW_S);
}

#ifdef __cplusplus
}
#endif
