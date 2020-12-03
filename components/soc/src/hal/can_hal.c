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

//Todo: Place the implementation of all common HAL functions here

#include <stddef.h>
#include "hal/can_hal.h"

//Default values written to various registers on initialization
#define CAN_HAL_INIT_TEC    0
#define CAN_HAL_INIT_REC    0
#define CAN_HAL_INIT_EWL    96

bool can_hal_init(can_hal_context_t *hal_ctx)
{
    //Initialize HAL context
    hal_ctx->dev = &CAN;
    //Initialize CAN controller, and set default values to registers
    if (!can_ll_enter_reset_mode(hal_ctx->dev)) {    //Must enter reset mode to write to config registers
        return false;
    }
    can_ll_enable_extended_reg_layout(hal_ctx->dev);        //Set PeliCAN address layout
    can_ll_set_mode(hal_ctx->dev, CAN_MODE_LISTEN_ONLY);    //Freeze REC by changing to LOM mode
    //Both TEC and REC should start at 0
    can_ll_set_tec(hal_ctx->dev, CAN_HAL_INIT_TEC);
    can_ll_set_rec(hal_ctx->dev, CAN_HAL_INIT_REC);
    can_ll_set_err_warn_lim(hal_ctx->dev, CAN_HAL_INIT_EWL);    //Set default value of for EWL
    return true;
}

void can_hal_deinit(can_hal_context_t *hal_ctx)
{
    //Clear any pending registers
    (void) can_ll_get_and_clear_intrs(hal_ctx->dev);
    can_ll_set_enabled_intrs(hal_ctx->dev, 0);
    can_ll_clear_arb_lost_cap(hal_ctx->dev);
    can_ll_clear_err_code_cap(hal_ctx->dev);
    hal_ctx->dev = NULL;
}

void can_hal_configure(can_hal_context_t *hal_ctx, const can_timing_config_t *t_config, const can_filter_config_t *f_config, uint32_t intr_mask, uint32_t clkout_divider)
{
    //Configure bus timing, acceptance filter, CLKOUT, and interrupts
    can_ll_set_bus_timing(hal_ctx->dev, t_config->brp, t_config->sjw, t_config->tseg_1, t_config->tseg_2, t_config->triple_sampling);
    can_ll_set_acc_filter(hal_ctx->dev, f_config->acceptance_code, f_config->acceptance_mask, f_config->single_filter);
    can_ll_set_clkout(hal_ctx->dev, clkout_divider);
    can_ll_set_enabled_intrs(hal_ctx->dev, intr_mask);
    (void) can_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
}

bool can_hal_start(can_hal_context_t *hal_ctx, can_mode_t mode)
{
    can_ll_set_mode(hal_ctx->dev, mode);                //Set operating mode
    //Todo: Check if this can be removed
    (void) can_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
    return can_ll_exit_reset_mode(hal_ctx->dev);        //Return false if failed to exit reset mode
}

bool can_hal_stop(can_hal_context_t *hal_ctx)
{
    if (!can_ll_enter_reset_mode(hal_ctx->dev)) {
        return false;
    }
    //Todo: Check if this can be removed
    (void) can_ll_get_and_clear_intrs(hal_ctx->dev);
    can_ll_set_mode(hal_ctx->dev, CAN_MODE_LISTEN_ONLY);    //Freeze REC by changing to LOM mode
    return true;
}