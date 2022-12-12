/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "soc/soc_caps.h"
#include "hal/mcpwm_hal.h"
#include "hal/mcpwm_ll.h"

void mcpwm_hal_init(mcpwm_hal_context_t *hal, const mcpwm_hal_init_config_t *init_config)
{
    hal->dev = MCPWM_LL_GET_HW(init_config->group_id);
    mcpwm_ll_group_enable_shadow_mode(hal->dev);
    mcpwm_ll_group_flush_shadow(hal->dev);
}

void mcpwm_hal_deinit(mcpwm_hal_context_t *hal)
{
    hal->dev = NULL;
}

void mcpwm_hal_timer_reset(mcpwm_hal_context_t *hal, int timer_id)
{
    mcpwm_ll_timer_set_count_mode(hal->dev, timer_id, MCPWM_TIMER_COUNT_MODE_PAUSE);
    mcpwm_ll_timer_update_period_at_once(hal->dev, timer_id);
    // disable sync input and output by default
    mcpwm_ll_timer_disable_sync_out(hal->dev, timer_id);
    mcpwm_ll_timer_enable_sync_input(hal->dev, timer_id, false);
    mcpwm_ll_timer_clear_sync_input(hal->dev, timer_id);
}

void mcpwm_hal_operator_reset(mcpwm_hal_context_t *hal, int oper_id)
{
    // allow to update action, compare, and dead time configuration
    mcpwm_ll_operator_stop_update_action(hal->dev, oper_id, false);
    mcpwm_ll_operator_update_action_at_once(hal->dev, oper_id);
    mcpwm_ll_deadtime_stop_update_delay(hal->dev, oper_id, false);
    mcpwm_ll_deadtime_update_delay_at_once(hal->dev, oper_id);
    for (int i = 0; i < SOC_MCPWM_COMPARATORS_PER_OPERATOR; i++) {
        mcpwm_ll_operator_stop_update_compare(hal->dev, oper_id, i, false);
        mcpwm_ll_operator_update_compare_at_once(hal->dev, oper_id, i);
    }
    mcpwm_ll_brake_enable_cbc_refresh_on_tez(hal->dev, oper_id, false);
    mcpwm_ll_fault_enable_cbc_refresh_on_tep(hal->dev, oper_id, false);
    mcpwm_ll_brake_enable_soft_cbc(hal->dev, oper_id, false);
    mcpwm_ll_brake_enable_soft_ost(hal->dev, oper_id, false);
}

void mcpwm_hal_generator_reset(mcpwm_hal_context_t *hal, int oper_id, int gen_id)
{
    mcpwm_ll_generator_reset_actions(hal->dev, oper_id, gen_id);
    mcpwm_ll_gen_disable_continue_force_action(hal->dev, oper_id, gen_id);
    mcpwm_ll_gen_disable_noncontinue_force_action(hal->dev, oper_id, gen_id);
}
