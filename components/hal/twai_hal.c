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

#include <stddef.h>
#include "hal/twai_hal.h"

//Default values written to various registers on initialization
#define TWAI_HAL_INIT_TEC    0
#define TWAI_HAL_INIT_REC    0
#define TWAI_HAL_INIT_EWL    96

/* ---------------------------- Init and Config ----------------------------- */

bool twai_hal_init(twai_hal_context_t *hal_ctx)
{
    //Initialize HAL context
    hal_ctx->dev = &TWAI;
    hal_ctx->state_flags = 0;
    //Initialize TWAI controller, and set default values to registers
    twai_ll_enter_reset_mode(hal_ctx->dev);
    if (!twai_ll_is_in_reset_mode(hal_ctx->dev)) {    //Must enter reset mode to write to config registers
        return false;
    }
#ifdef TWAI_SUPPORT_MULTI_ADDRESS_LAYOUT
    twai_ll_enable_extended_reg_layout(hal_ctx->dev);        //Changes the address layout of the registers
#endif
    twai_ll_set_mode(hal_ctx->dev, TWAI_MODE_LISTEN_ONLY);    //Freeze REC by changing to LOM mode
    //Both TEC and REC should start at 0
    twai_ll_set_tec(hal_ctx->dev, TWAI_HAL_INIT_TEC);
    twai_ll_set_rec(hal_ctx->dev, TWAI_HAL_INIT_REC);
    twai_ll_set_err_warn_lim(hal_ctx->dev, TWAI_HAL_INIT_EWL);    //Set default value of for EWL
    return true;
}

void twai_hal_deinit(twai_hal_context_t *hal_ctx)
{
    //Clear any pending registers
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);
    twai_ll_set_enabled_intrs(hal_ctx->dev, 0);
    twai_ll_clear_arb_lost_cap(hal_ctx->dev);
    twai_ll_clear_err_code_cap(hal_ctx->dev);
    hal_ctx->dev = NULL;
}

void twai_hal_configure(twai_hal_context_t *hal_ctx, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config, uint32_t intr_mask, uint32_t clkout_divider)
{
    //Configure bus timing, acceptance filter, CLKOUT, and interrupts
    twai_ll_set_bus_timing(hal_ctx->dev, t_config->brp, t_config->sjw, t_config->tseg_1, t_config->tseg_2, t_config->triple_sampling);
    twai_ll_set_acc_filter(hal_ctx->dev, f_config->acceptance_code, f_config->acceptance_mask, f_config->single_filter);
    twai_ll_set_clkout(hal_ctx->dev, clkout_divider);
    twai_ll_set_enabled_intrs(hal_ctx->dev, intr_mask);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
}

/* -------------------------------- Actions --------------------------------- */

void twai_hal_start(twai_hal_context_t *hal_ctx, twai_mode_t mode)
{
    twai_ll_set_mode(hal_ctx->dev, mode);                //Set operating mode
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
    TWAI_HAL_SET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RUNNING);
    twai_ll_exit_reset_mode(hal_ctx->dev); 
}

void twai_hal_stop(twai_hal_context_t *hal_ctx)
{
    twai_ll_enter_reset_mode(hal_ctx->dev);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);
    twai_ll_set_mode(hal_ctx->dev, TWAI_MODE_LISTEN_ONLY);    //Freeze REC by changing to LOM mode
    //Any TX is immediately halted on entering reset mode
    TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
    TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RUNNING);
}

/* ----------------------------- Event Handling ----------------------------- */

uint32_t twai_hal_decode_interrupt_events(twai_hal_context_t *hal_ctx)
{
    uint32_t events = 0;
    //Read interrupt, status
    uint32_t interrupts = twai_ll_get_and_clear_intrs(hal_ctx->dev);
    uint32_t status = twai_ll_get_status(hal_ctx->dev);
    uint32_t tec = twai_ll_get_tec(hal_ctx->dev);
    uint32_t rec = twai_ll_get_rec(hal_ctx->dev);

    //Error Warning Interrupt set whenever Error or Bus Status bit changes
    if (interrupts & TWAI_LL_INTR_EI) {
        if (status & TWAI_LL_STATUS_BS) {
            //Currently in BUS OFF state
            if (status & TWAI_LL_STATUS_ES) {    //EWL is exceeded, thus must have entered BUS OFF
                twai_ll_set_mode(hal_ctx->dev, TWAI_MODE_LISTEN_ONLY);  //Set to listen only to freeze tec and rec
                events |= TWAI_HAL_EVENT_BUS_OFF;
                TWAI_HAL_SET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_BUS_OFF);
                TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RUNNING);
                //Any TX would have been halted by entering bus off. Reset its flag
                TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
            } else {
                //Below EWL. Therefore TEC is counting down in bus recovery
                events |= TWAI_HAL_EVENT_BUS_RECOV_PROGRESS;
            }
        } else {
            //Not in BUS OFF
            if (status & TWAI_LL_STATUS_ES) {       //Just Exceeded EWL
                events |= TWAI_HAL_EVENT_ABOVE_EWL;  
                TWAI_HAL_SET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_ERR_WARN);
            } else if (hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_RECOVERING) {
                //Previously undergoing bus recovery. Thus means bus recovery complete
                twai_ll_enter_reset_mode(hal_ctx->dev);     //Enter reset mode to stop the peripheral
                events |= TWAI_HAL_EVENT_BUS_RECOV_CPLT;
                TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RECOVERING);
                TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_BUS_OFF);
            } else {        //Just went below EWL
                events |= TWAI_HAL_EVENT_BELOW_EWL;
                TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_ERR_WARN);
            }
        }
    }
    //Receive Interrupt set whenever RX FIFO  is not empty
    if (interrupts & TWAI_LL_INTR_RI) {
        events |= TWAI_HAL_EVENT_RX_BUFF_FRAME;
    }
    //Transmit interrupt set whenever TX buffer becomes free
    if (interrupts & TWAI_LL_INTR_TI) {
        events |= TWAI_HAL_EVENT_TX_BUFF_FREE;
        TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
    }
    //Error Passive Interrupt on transition from error active to passive or vice versa
    if (interrupts & TWAI_LL_INTR_EPI) {
        if (tec >= TWAI_ERR_PASS_THRESH || rec >= TWAI_ERR_PASS_THRESH) {
            events |= TWAI_HAL_EVENT_ERROR_PASSIVE;
            TWAI_HAL_SET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_ERR_PASSIVE);
        } else {
            events |= TWAI_HAL_EVENT_ERROR_ACTIVE;
            TWAI_HAL_RESET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_ERR_PASSIVE);
        }
    }
    //Bus error interrupt triggered on a bus error (e.g. bit, ACK, stuff etc)
    if (interrupts & TWAI_LL_INTR_BEI) {
        twai_ll_clear_err_code_cap(hal_ctx->dev);
        events |= TWAI_HAL_EVENT_BUS_ERR;
    }
    //Arbitration Lost Interrupt triggered on losing arbitration
    if (interrupts & TWAI_LL_INTR_ALI) {
        twai_ll_clear_arb_lost_cap(hal_ctx->dev);
        events |= TWAI_HAL_EVENT_ARB_LOST;
    }
    return events;
}

void twai_hal_set_tx_buffer_and_transmit(twai_hal_context_t *hal_ctx, twai_hal_frame_t *tx_frame)
{
    //Copy frame into tx buffer
    twai_ll_set_tx_buffer(hal_ctx->dev, tx_frame);
    //Hit the send command
    if (tx_frame->self_reception) {
        if (tx_frame->single_shot) {
            twai_ll_set_cmd_self_rx_single_shot(hal_ctx->dev);
        } else {
            twai_ll_set_cmd_self_rx_request(hal_ctx->dev);
        }
    } else if (tx_frame->single_shot){
        twai_ll_set_cmd_tx_single_shot(hal_ctx->dev);
    } else {
        twai_ll_set_cmd_tx(hal_ctx->dev);
    }
    TWAI_HAL_SET_FLAG(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
}
