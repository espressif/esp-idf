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

// The HAL layer for MCPWM (common part)

#include "hal/mcpwm_hal.h"

void mcpwm_hal_init(mcpwm_hal_context_t *hal, const mcpwm_hal_init_config_t *init_config)
{
    hal->dev = MCPWM_LL_GET_HW(init_config->host_id);
}

void mcpwm_hal_hw_init(mcpwm_hal_context_t *hal)
{
    mcpwm_ll_init(hal->dev);
    mcpwm_ll_set_clock_prescale(hal->dev, hal->prescale);
}

void mcpwm_hal_timer_start(mcpwm_hal_context_t *hal, int timer)
{
    mcpwm_ll_timer_start(hal->dev, timer);
}

void mcpwm_hal_timer_stop(mcpwm_hal_context_t *hal, int timer)
{
    mcpwm_ll_timer_stop(hal->dev, timer);
}

void mcpwm_hal_timer_update_basic(mcpwm_hal_context_t *hal, int timer)
{
    mcpwm_ll_timer_set_prescale(hal->dev, timer, hal->timer[timer].timer_prescale);

    uint32_t period = MCPWM_BASE_CLK / (hal->timer[timer].freq *
            (hal->prescale +1) * (hal->timer[timer].timer_prescale + 1));
    mcpwm_ll_timer_set_period(hal->dev, timer, period);
    //write back the actual value to the context
    hal->timer[timer].freq = MCPWM_BASE_CLK / (period *
            (hal->prescale +1) * (hal->timer[timer].timer_prescale + 1));

    mcpwm_ll_timer_set_count_mode(hal->dev, timer, hal->timer[timer].count_mode);
}

void mcpwm_hal_timer_enable_sync(mcpwm_hal_context_t *hal, int timer, const mcpwm_hal_sync_config_t *sync_conf)
{
    uint32_t set_phase = mcpwm_ll_timer_get_period(hal->dev, timer) * sync_conf->reload_permillage / 1000;
    mcpwm_ll_sync_set_phase(hal->dev, timer, set_phase);

    mcpwm_ll_sync_set_input(hal->dev, timer, sync_conf->sync_sig);
    mcpwm_ll_sync_enable(hal->dev, timer, 1);
}

void mcpwm_hal_timer_disable_sync(mcpwm_hal_context_t *hal, int timer)
{
    mcpwm_ll_sync_enable(hal->dev, timer, 0);
}

void mcpwm_hal_operator_update_basic(mcpwm_hal_context_t *hal, int op)
{
    mcpwm_hal_operator_config_t *op_conf = &hal->op[op];
    mcpwm_ll_operator_select_timer(hal->dev, op, op_conf->timer);
    for (int cmp = 0; cmp < SOC_MCPWM_COMPARATOR_NUM; cmp++) {
        mcpwm_hal_operator_update_comparator(hal, op, cmp);
    }
    for (int gen = 0; gen < SOC_MCPWM_GENERATOR_NUM; gen++) {
        mcpwm_hal_operator_update_generator(hal, op, gen);
    }
}

void mcpwm_hal_operator_update_comparator(mcpwm_hal_context_t *hal, int op, int cmp)
{
    int timer = hal->op[op].timer;
    uint32_t period = mcpwm_ll_timer_get_period(hal->dev, timer);
    mcpwm_ll_operator_set_compare(hal->dev, op, cmp, (hal->op[op].duty[cmp] * period) / 100);
    mcpwm_ll_operator_set_compare_upmethod(hal->dev, timer);
}

void mcpwm_hal_operator_update_generator(mcpwm_hal_context_t *hal, int op, int gen_num)
{
    mcpwm_hal_generator_config_t* gen_config = &(hal->op[op].gen[gen_num]);
    if (gen_config->duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH) {
        mcpwm_ll_gen_set_zero_action(hal->dev, op, gen_num, MCPWM_ACTION_FORCE_HIGH);
        mcpwm_ll_gen_set_period_action(hal->dev, op, gen_num, MCPWM_ACTION_FORCE_HIGH);
        mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, 0, MCPWM_ACTION_FORCE_HIGH, MCPWM_ACTION_FORCE_HIGH);
        mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, 1, MCPWM_ACTION_FORCE_HIGH, MCPWM_ACTION_FORCE_HIGH);
    } else if (gen_config->duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_LOW) {
        mcpwm_ll_gen_set_zero_action(hal->dev, op, gen_num, MCPWM_ACTION_FORCE_LOW);
        mcpwm_ll_gen_set_period_action(hal->dev, op, gen_num, MCPWM_ACTION_FORCE_LOW);
        mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, 0, MCPWM_ACTION_FORCE_LOW, MCPWM_ACTION_FORCE_LOW);
        mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, 1, MCPWM_ACTION_FORCE_LOW, MCPWM_ACTION_FORCE_LOW);
    } else if (gen_config->duty_type == MCPWM_DUTY_MODE_1 || gen_config->duty_type == MCPWM_DUTY_MODE_0) {
        const int timer_used = hal->op[op].timer;
        const int cmp_used = gen_config->comparator;
        const int cmp_not_used = 1 - gen_config->comparator;

        mcpwm_output_action_t inactive_action;
        mcpwm_output_action_t active_action;
        const mcpwm_output_action_t no_action = MCPWM_ACTION_NO_CHANGE;
        if (gen_config->duty_type == MCPWM_DUTY_MODE_1) {
            active_action = MCPWM_ACTION_FORCE_LOW;      //active low
            inactive_action = MCPWM_ACTION_FORCE_HIGH;    //inactive high
        } else {   //MCPWM_DUTY_MODE_0
            inactive_action = MCPWM_ACTION_FORCE_LOW;    //inactive low
            active_action = MCPWM_ACTION_FORCE_HIGH;      //active high
        }

        if (hal->timer[timer_used].count_mode == MCPWM_UP_COUNTER) {
            mcpwm_ll_gen_set_zero_action(hal->dev, op, gen_num, active_action);
            mcpwm_ll_gen_set_period_action(hal->dev, op, gen_num, no_action);
            mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, cmp_used, inactive_action, no_action);
        } else if (hal->timer[timer_used].count_mode == MCPWM_DOWN_COUNTER) {
            mcpwm_ll_gen_set_zero_action(hal->dev, op, gen_num, no_action);
            mcpwm_ll_gen_set_period_action(hal->dev, op, gen_num, inactive_action);
            mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, cmp_used, no_action, active_action);
        } else {   //Timer count up-down
            mcpwm_ll_gen_set_zero_action(hal->dev, op, gen_num, active_action);
            mcpwm_ll_gen_set_period_action(hal->dev, op, gen_num, no_action);
            mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, cmp_used, inactive_action, active_action);
        }
        mcpwm_ll_gen_set_cmp_action(hal->dev, op, gen_num, cmp_not_used, no_action, no_action);
    }
}

void mcpwm_hal_operator_enable_carrier(mcpwm_hal_context_t *hal, int op, const mcpwm_hal_carrier_conf_t *carrier_conf)
{
    mcpwm_ll_carrier_init(hal->dev, op);
    mcpwm_ll_carrier_set_prescale(hal->dev, op, carrier_conf->period);
    mcpwm_ll_carrier_set_duty(hal->dev, op, carrier_conf->duty);
    mcpwm_ll_carrier_enable(hal->dev, op, true);
    mcpwm_ll_carrier_set_oneshot_width(hal->dev, op, carrier_conf->oneshot_pulse_width);
    mcpwm_ll_carrier_out_invert(hal->dev, op, carrier_conf->inverted);
}

void mcpwm_hal_operator_disable_carrier(mcpwm_hal_context_t *hal, int op)
{
    mcpwm_ll_carrier_enable(hal->dev, op, false);
}

void mcpwm_hal_operator_update_deadzone(mcpwm_hal_context_t *hal, int op, const mcpwm_hal_deadzone_conf_t *deadzone)
{
    if (deadzone->mode != MCPWM_DEADTIME_BYPASS) {
        mcpwm_ll_deadtime_init(hal->dev, op);
        mcpwm_ll_deadtime_set_rising_delay(hal->dev, op, deadzone->red);
        mcpwm_ll_deadtime_set_falling_delay(hal->dev, op, deadzone->fed);
        mcpwm_ll_set_deadtime_mode(hal->dev, op, deadzone->mode);
    } else {
        mcpwm_ll_deadtime_bypass(hal->dev, op);
    }
}

void mcpwm_hal_fault_init(mcpwm_hal_context_t *hal, int fault_sig, bool level)
{
    mcpwm_ll_fault_enable(hal->dev, fault_sig, level);
}

void mcpwm_hal_operator_update_fault(mcpwm_hal_context_t *hal, int op, const mcpwm_hal_fault_conf_t *fault_conf)
{
    for (int fault_sig = 0; fault_sig < SOC_MCPWM_FAULT_SIG_NUM; fault_sig++) {
        bool enabled = (fault_conf->cbc_enabled_mask & BIT(fault_sig)) ? true : false;
        mcpwm_ll_fault_cbc_enable_signal(hal->dev, op, fault_sig, enabled);
    }
    for (int fault_sig = 0; fault_sig < SOC_MCPWM_FAULT_SIG_NUM; fault_sig++) {
        bool enabled = (fault_conf->ost_enabled_mask & BIT(fault_sig)) ? true : false;
        mcpwm_ll_fault_oneshot_enable_signal(hal->dev, op, fault_sig, enabled);
    }
    if (fault_conf->cbc_enabled_mask) {
        for (int gen = 0; gen < SOC_MCPWM_GENERATOR_NUM; gen++) {
            mcpwm_ll_fault_set_cyc_action(hal->dev, op, gen, fault_conf->action_on_fault[gen], fault_conf->action_on_fault[gen]);
        }
    }
    if (fault_conf->ost_enabled_mask) {
        for (int gen = 0; gen < SOC_MCPWM_GENERATOR_NUM; gen++) {
            mcpwm_ll_fault_set_oneshot_action(hal->dev, op, gen, fault_conf->action_on_fault[gen], fault_conf->action_on_fault[gen]);
        }
    }
}

void mcpwm_hal_fault_oneshot_clear(mcpwm_hal_context_t *hal, int op)
{
    mcpwm_ll_fault_clear_ost(hal->dev, op);
}

void mcpwm_hal_fault_disable(mcpwm_hal_context_t *hal, int fault_sig)
{
    for (int op = 0; op < SOC_MCPWM_OP_NUM; op++) {
        if (mcpwm_ll_fault_oneshot_signal_enabled(hal->dev, op, fault_sig)) {
            mcpwm_ll_fault_clear_ost(hal->dev, op);
        }
    }
    mcpwm_ll_fault_disable(hal->dev, fault_sig);
}

void mcpwm_hal_capture_enable(mcpwm_hal_context_t *hal, int cap_sig, const mcpwm_hal_capture_config_t *conf)
{
    mcpwm_ll_capture_enable(hal->dev, cap_sig, 1);
    mcpwm_ll_capture_select_edge(hal->dev, cap_sig, conf->cap_edge);
    mcpwm_ll_capture_set_prescale(hal->dev, cap_sig, conf->prescale);
}

esp_err_t mcpwm_hal_capture_get_result(mcpwm_hal_context_t *hal, int cap_sig, uint32_t *out_count, mcpwm_capture_on_edge_t *out_edge)
{
    const mcpwm_intr_t sig_intr = mcpwm_ll_get_cap_intr_def(cap_sig);
    //unconditionally update the output value
    if (out_edge) {
        *out_edge = mcpwm_ll_get_captured_edge(hal->dev, cap_sig);
    }
    *out_count = mcpwm_ll_get_capture_val(hal->dev, cap_sig);

    if (mcpwm_ll_get_intr(hal->dev) & sig_intr) {
        mcpwm_ll_clear_intr(hal->dev, sig_intr);
    }
    return (mcpwm_ll_get_intr(hal->dev) & sig_intr ? ESP_OK : ESP_ERR_NOT_FOUND);
}

void mcpwm_hal_capture_disable(mcpwm_hal_context_t *hal, int cap_sig)
{
    mcpwm_ll_capture_enable(hal->dev, cap_sig, 0);
}

