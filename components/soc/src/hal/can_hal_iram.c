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

#include <stdint.h>
#include "hal/can_hal.h"

uint32_t can_hal_decode_interrupt_events(can_hal_context_t *hal_ctx, bool bus_recovering)
{
    uint32_t events = 0;
    //Read interrupt, status
    uint32_t interrupts = can_ll_get_and_clear_intrs(hal_ctx->dev);
    uint32_t status = can_ll_get_status(hal_ctx->dev);
    uint32_t tec = can_ll_get_tec(hal_ctx->dev);
    uint32_t rec = can_ll_get_rec(hal_ctx->dev);

    //Receive Interrupt set whenever RX FIFO  is not empty
    if (interrupts & CAN_LL_INTR_RI) {
        events |= CAN_HAL_EVENT_RX_BUFF_FRAME;
    }
    //Transmit interrupt set whenever TX buffer becomes free
    if (interrupts & CAN_LL_INTR_TI) {
        events |= CAN_HAL_EVENT_TX_BUFF_FREE;
    }
    //Error Warning Interrupt set whenever Error or Bus Status bit changes
    if (interrupts & CAN_LL_INTR_EI) {
        if (status & CAN_LL_STATUS_BS) {
            //Currently in BUS OFF state
            //EWL is exceeded, thus must have entered BUS OFF
            //Below EWL. Therefore TEC is counting down in bus recovery
            events |= (status & CAN_LL_STATUS_ES) ? CAN_HAL_EVENT_BUS_OFF : CAN_HAL_EVENT_BUS_RECOV_PROGRESS;
        } else {
            //Not in BUS OFF
            events |= (status & CAN_LL_STATUS_ES) ? CAN_HAL_EVENT_ABOVE_EWL :   //Just Exceeded EWL
                      ((bus_recovering) ?  //If previously undergoing bus recovery
                      CAN_HAL_EVENT_BUS_RECOV_CPLT :
                      CAN_HAL_EVENT_BELOW_EWL);
        }
    }
    //Error Passive Interrupt on transition from error active to passive or vice versa
    if (interrupts & CAN_LL_INTR_EPI) {
        events |= (tec >= CAN_ERR_PASS_THRESH || rec >= CAN_ERR_PASS_THRESH) ? CAN_HAL_EVENT_ERROR_PASSIVE : CAN_HAL_EVENT_ERROR_ACTIVE;
    }
    //Arbitration Lost Interrupt triggered on losing arbitration
    if (interrupts & CAN_LL_INTR_ALI) {
        events |= CAN_HAL_EVENT_ARB_LOST;
    }
    //Bus error interrupt triggered on a bus error (e.g. bit, ACK, stuff etc)
    if (interrupts & CAN_LL_INTR_BEI) {
        events |= CAN_HAL_EVENT_BUS_ERR;
    }
    return events;
}

void can_hal_set_tx_buffer_and_transmit(can_hal_context_t *hal_ctx, can_hal_frame_t *tx_frame)
{
    //Copy frame into tx buffer
    can_ll_set_tx_buffer(hal_ctx->dev, tx_frame);
    //Hit the send command
    if (tx_frame->self_reception) {
        if (tx_frame->single_shot) {
            can_ll_set_cmd_self_rx_single_shot(hal_ctx->dev);
        } else {
            can_ll_set_cmd_self_rx_request(hal_ctx->dev);
        }
    } else if (tx_frame->single_shot){
        can_ll_set_cmd_tx_single_shot(hal_ctx->dev);
    } else {
        can_ll_set_cmd_tx(hal_ctx->dev);
    }
}