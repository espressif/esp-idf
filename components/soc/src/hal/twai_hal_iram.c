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
#include "hal/twai_hal.h"

uint32_t twai_hal_decode_interrupt_events(twai_hal_context_t *hal_ctx, bool bus_recovering)
{
    uint32_t events = 0;
    //Read interrupt, status
    uint32_t interrupts = twai_ll_get_and_clear_intrs(hal_ctx->dev);
    uint32_t status = twai_ll_get_status(hal_ctx->dev);
    uint32_t tec = twai_ll_get_tec(hal_ctx->dev);
    uint32_t rec = twai_ll_get_rec(hal_ctx->dev);

    //Receive Interrupt set whenever RX FIFO  is not empty
    if (interrupts & TWAI_LL_INTR_RI) {
        events |= TWAI_HAL_EVENT_RX_BUFF_FRAME;
    }
    //Transmit interrupt set whenever TX buffer becomes free
    if (interrupts & TWAI_LL_INTR_TI) {
        events |= TWAI_HAL_EVENT_TX_BUFF_FREE;
    }
    //Error Warning Interrupt set whenever Error or Bus Status bit changes
    if (interrupts & TWAI_LL_INTR_EI) {
        if (status & TWAI_LL_STATUS_BS) {
            //Currently in BUS OFF state
            //EWL is exceeded, thus must have entered BUS OFF
            //Below EWL. Therefore TEC is counting down in bus recovery
            //Todo: Check if BUS Recov can be removed for esp32s2
            events |= (status & TWAI_LL_STATUS_ES) ? TWAI_HAL_EVENT_BUS_OFF : TWAI_HAL_EVENT_BUS_RECOV_PROGRESS;
        } else {
            //Not in BUS OFF
            events |= (status & TWAI_LL_STATUS_ES) ? TWAI_HAL_EVENT_ABOVE_EWL :   //Just Exceeded EWL
                      ((bus_recovering) ?  //If previously undergoing bus recovery
                      TWAI_HAL_EVENT_BUS_RECOV_CPLT :
                      TWAI_HAL_EVENT_BELOW_EWL);
        }
    }
    //Error Passive Interrupt on transition from error active to passive or vice versa
    if (interrupts & TWAI_LL_INTR_EPI) {
        events |= (tec >= TWAI_ERR_PASS_THRESH || rec >= TWAI_ERR_PASS_THRESH) ? TWAI_HAL_EVENT_ERROR_PASSIVE : TWAI_HAL_EVENT_ERROR_ACTIVE;
    }
    //Arbitration Lost Interrupt triggered on losing arbitration
    if (interrupts & TWAI_LL_INTR_ALI) {
        events |= TWAI_HAL_EVENT_ARB_LOST;
    }
    //Bus error interrupt triggered on a bus error (e.g. bit, ACK, stuff etc)
    if (interrupts & TWAI_LL_INTR_BEI) {
        events |= TWAI_HAL_EVENT_BUS_ERR;
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
}