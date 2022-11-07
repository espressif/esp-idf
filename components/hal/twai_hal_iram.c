/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include "sdkconfig.h"
#include "hal/twai_hal.h"

#ifdef CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT
//Errata condition occurs at 64 messages. Threshold set to 62 to prevent the chance of failing to detect errata condition.
#define TWAI_RX_FIFO_CORRUPT_THRESH     62
#endif

/* ----------------------------- Event Handling ----------------------------- */

/**
 * Helper functions that can decode what events have been triggered based on
 * the values of the interrupt, status, TEC and REC registers. The HAL context's
 * state flags are also updated based on the events that have triggered.
 */
static inline uint32_t twai_hal_decode_interrupt(twai_hal_context_t *hal_ctx)
{
    uint32_t events = 0;
    uint32_t interrupts = twai_ll_get_and_clear_intrs(hal_ctx->dev);
    uint32_t status = twai_ll_get_status(hal_ctx->dev);
    uint32_t tec = twai_ll_get_tec(hal_ctx->dev);
    uint32_t rec = twai_ll_get_rec(hal_ctx->dev);
    uint32_t state_flags = hal_ctx->state_flags;

    //Error Warning Interrupt set whenever Error or Bus Status bit changes
    if (interrupts & TWAI_LL_INTR_EI) {
        if (status & TWAI_LL_STATUS_BS) {       //Currently in BUS OFF state
            if (status & TWAI_LL_STATUS_ES) {    //EWL is exceeded, thus must have entered BUS OFF
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_OFF);
                TWAI_HAL_SET_BITS(state_flags, TWAI_HAL_STATE_FLAG_BUS_OFF);
                //Any TX would have been halted by entering bus off. Reset its flag
                TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_RUNNING | TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
            } else {
                //Below EWL. Therefore TEC is counting down in bus recovery
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_RECOV_PROGRESS);
            }
        } else {    //Not in BUS OFF
            if (status & TWAI_LL_STATUS_ES) {       //Just Exceeded EWL
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_ABOVE_EWL);
                TWAI_HAL_SET_BITS(state_flags, TWAI_HAL_STATE_FLAG_ERR_WARN);
            } else if (hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_RECOVERING) {
                //Previously undergoing bus recovery. Thus means bus recovery complete
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_RECOV_CPLT);
                TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_RECOVERING | TWAI_HAL_STATE_FLAG_BUS_OFF);
            } else {        //Just went below EWL
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BELOW_EWL);
                TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_ERR_WARN);
            }
        }
    }
    //Receive Interrupt set whenever RX FIFO is not empty
    if (interrupts & TWAI_LL_INTR_RI) {
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_RX_BUFF_FRAME);
    }
    //Transmit interrupt set whenever TX buffer becomes free
#ifdef CONFIG_TWAI_ERRATA_FIX_TX_INTR_LOST
    if ((interrupts & TWAI_LL_INTR_TI || hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED) && status & TWAI_LL_STATUS_TBS) {
#else
    if (interrupts & TWAI_LL_INTR_TI) {
#endif
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_TX_BUFF_FREE);
        TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
    }
    //Error Passive Interrupt on transition from error active to passive or vice versa
    if (interrupts & TWAI_LL_INTR_EPI) {
        if (tec >= TWAI_ERR_PASS_THRESH || rec >= TWAI_ERR_PASS_THRESH) {
            TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_ERROR_PASSIVE);
            TWAI_HAL_SET_BITS(state_flags, TWAI_HAL_STATE_FLAG_ERR_PASSIVE);
        } else {
            TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_ERROR_ACTIVE);
            TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_ERR_PASSIVE);
        }
    }
    //Bus error interrupt triggered on a bus error (e.g. bit, ACK, stuff etc)
    if (interrupts & TWAI_LL_INTR_BEI) {
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_ERR);
    }
    //Arbitration Lost Interrupt triggered on losing arbitration
    if (interrupts & TWAI_LL_INTR_ALI) {
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_ARB_LOST);
    }
    hal_ctx->state_flags = state_flags;
    return events;
}

uint32_t twai_hal_get_events(twai_hal_context_t *hal_ctx)
{
    uint32_t events = twai_hal_decode_interrupt(hal_ctx);

    //Handle low latency events
    if (events & TWAI_HAL_EVENT_BUS_OFF) {
        twai_ll_set_mode(hal_ctx->dev, TWAI_MODE_LISTEN_ONLY);  //Freeze TEC/REC by entering LOM
#ifdef CONFIG_TWAI_ERRATA_FIX_BUS_OFF_REC
        //Errata workaround: Force REC to 0 by re-triggering bus-off (by setting TEC to 0 then 255)
        twai_ll_set_tec(hal_ctx->dev, 0);
        twai_ll_set_tec(hal_ctx->dev, 255);
        (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear the re-triggered bus-off interrupt
#endif
    }
    if (events & TWAI_HAL_EVENT_BUS_RECOV_CPLT) {
        twai_ll_enter_reset_mode(hal_ctx->dev);     //Enter reset mode to stop the controller
    }
    if (events & TWAI_HAL_EVENT_BUS_ERR) {
#ifdef CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID
        twai_ll_err_type_t type;
        twai_ll_err_dir_t dir;
        twai_ll_err_seg_t seg;
        twai_ll_parse_err_code_cap(hal_ctx->dev, &type, &dir, &seg);    //Decode error interrupt
        //Check for errata condition (RX message has bus error at particular segments)
        if (dir == TWAI_LL_ERR_DIR_RX &&
            ((seg == TWAI_LL_ERR_SEG_DATA || seg == TWAI_LL_ERR_SEG_CRC_SEQ) ||
             (seg == TWAI_LL_ERR_SEG_ACK_DELIM && type == TWAI_LL_ERR_OTHER))) {
            TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_NEED_PERIPH_RESET);
        }
#endif
        twai_ll_clear_err_code_cap(hal_ctx->dev);
    }
    if (events & TWAI_HAL_EVENT_ARB_LOST) {
        twai_ll_clear_arb_lost_cap(hal_ctx->dev);
    }
#ifdef CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT
    //Check for errata condition (rx_msg_count >= corruption_threshold)
    if (events & TWAI_HAL_EVENT_RX_BUFF_FRAME && twai_ll_get_rx_msg_count(hal_ctx->dev) >= TWAI_RX_FIFO_CORRUPT_THRESH) {
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_NEED_PERIPH_RESET);
    }
#endif
#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
    if (events & TWAI_HAL_EVENT_NEED_PERIPH_RESET) {
        //A peripheral reset will invalidate an RX event;
        TWAI_HAL_CLEAR_BITS(events, (TWAI_HAL_EVENT_RX_BUFF_FRAME));
    }
#endif
    return events;
}

#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
void twai_hal_prepare_for_reset(twai_hal_context_t *hal_ctx)
{
    uint32_t status = twai_ll_get_status(hal_ctx->dev);
    if (!(status & TWAI_LL_STATUS_TBS)) {   //Transmit buffer is NOT free, indicating an Ongoing TX will be cancelled by the HW reset
        TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_NEED_RETRY);
        //Note: Even if the TX completes right after this, we still consider it will be retried.
        //Worst case the same message will get sent twice.
    }
    //Some register must saved before entering reset mode
    hal_ctx->rx_msg_cnt_save = (uint8_t) twai_ll_get_rx_msg_count(hal_ctx->dev);
    twai_ll_enter_reset_mode(hal_ctx->dev);     //Enter reset mode to stop the controller
    twai_ll_save_reg(hal_ctx->dev, &hal_ctx->reg_save); //Save remaining registers after entering reset mode
}

void twai_hal_recover_from_reset(twai_hal_context_t *hal_ctx)
{
    twai_ll_enter_reset_mode(hal_ctx->dev);
    twai_ll_enable_extended_reg_layout(hal_ctx->dev);
    twai_ll_restore_reg(hal_ctx->dev, &hal_ctx->reg_save);
    twai_ll_exit_reset_mode(hal_ctx->dev);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);

    if (hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_TX_NEED_RETRY) {
        //HW reset has cancelled a TX. Re-transmit here
        twai_hal_set_tx_buffer_and_transmit(hal_ctx, &hal_ctx->tx_frame_save);
        TWAI_HAL_CLEAR_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_NEED_RETRY);
    }
}
#endif  //defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)

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
    TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
    //Save transmitted frame in case we need to retry
    memcpy(&hal_ctx->tx_frame_save, tx_frame, sizeof(twai_hal_frame_t));
#endif  //defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
}
