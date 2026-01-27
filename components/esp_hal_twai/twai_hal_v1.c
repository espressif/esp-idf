/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "esp_compiler.h"
#include "hal/log.h"
#include "hal/twai_hal.h"
#include "hal/twai_ll.h"
#include "soc/soc_caps.h"

//Default values written to various registers on initialization
#define TWAI_HAL_INIT_TEC    0
#define TWAI_HAL_INIT_REC    0
#define TWAI_HAL_INIT_EWL    96

#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
// context for errata workarounds
typedef struct twai_hal_errata_ctx_t {
    twai_hal_frame_t tx_frame_save;
    twai_ll_reg_save_t reg_save;
    uint8_t rx_msg_cnt_save;
} twai_hal_errata_ctx_t;
#endif

size_t twai_hal_get_mem_requirment(void)
{
#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
    return sizeof(twai_hal_context_t) + sizeof(twai_hal_errata_ctx_t);
#else
    return sizeof(twai_hal_context_t);
#endif
}

/* ---------------------------- Init and Config ----------------------------- */
bool twai_hal_init(twai_hal_context_t *hal_ctx, const twai_hal_config_t *config)
{
    //Initialize HAL context
    hal_ctx->dev = TWAI_LL_GET_HW(config->controller_id);
    hal_ctx->state_flags = 0;
    hal_ctx->clock_source_hz = config->clock_source_hz;
    hal_ctx->retry_cnt = config->retry_cnt;
    hal_ctx->enable_self_test = config->enable_self_test;
    hal_ctx->enable_loopback = config->enable_loopback;
    hal_ctx->enable_listen_only = config->enable_listen_only;
    //Initialize TWAI controller, and set default values to registers
    twai_ll_enter_reset_mode(hal_ctx->dev);
    if (!twai_ll_is_in_reset_mode(hal_ctx->dev)) {    //Must enter reset mode to write to config registers
        return false;
    }
#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
    hal_ctx->errata_ctx = (twai_hal_errata_ctx_t *)(hal_ctx + 1);   //errata context is place at end of hal_ctx
#endif
#if TWAI_LL_SUPPORT(MULTI_ADDRESS_LAYOUT)
    twai_ll_enable_extended_reg_layout(hal_ctx->dev);        //Changes the address layout of the registers
#endif
    twai_ll_set_mode(hal_ctx->dev, true, false, false);      //Freeze REC by changing to LOM mode
    twai_ll_set_acc_filter(hal_ctx->dev, 0, 0xFFFFFFFF, true);  //receive all by default if no additional filter config later
    //Both TEC and REC should start at 0
    twai_ll_set_tec(hal_ctx->dev, TWAI_HAL_INIT_TEC);
    twai_ll_set_rec(hal_ctx->dev, TWAI_HAL_INIT_REC);
    twai_ll_set_err_warn_lim(hal_ctx->dev, TWAI_HAL_INIT_EWL);  //Set default value of for EWL
    twai_ll_set_enabled_intrs(hal_ctx->dev, config->intr_mask);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
    return true;
}

void twai_hal_deinit(twai_hal_context_t *hal_ctx)
{
    //Clear any pending registers
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);
    twai_ll_set_enabled_intrs(hal_ctx->dev, 0);
    twai_ll_clear_arb_lost_cap(hal_ctx->dev);
    hal_ctx->dev = NULL;
}

void twai_hal_configure(twai_hal_context_t *hal_ctx, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config, uint32_t clkout_divider)
{
    uint32_t brp = t_config->brp;
    // both quanta_resolution_hz and brp can affect the baud rate
    // but a non-zero quanta_resolution_hz takes higher priority
    if (t_config->quanta_resolution_hz) {
        brp = hal_ctx->clock_source_hz / t_config->quanta_resolution_hz;
    }

    //Configure bus timing, acceptance filter, CLKOUT, and interrupts
    twai_ll_set_bus_timing(hal_ctx->dev, brp, t_config->sjw, t_config->tseg_1, t_config->tseg_2, t_config->triple_sampling);
    twai_ll_set_acc_filter(hal_ctx->dev, f_config->acceptance_code, f_config->acceptance_mask, f_config->single_filter);
    twai_ll_set_clkout(hal_ctx->dev, clkout_divider);
}

bool twai_hal_check_brp_validation(twai_hal_context_t *hal_ctx, uint32_t brp)
{
    return twai_ll_check_brp_validation(brp);
}

void twai_hal_configure_timing(twai_hal_context_t *hal_ctx, const twai_timing_advanced_config_t *t_config)
{
    twai_ll_set_bus_timing(hal_ctx->dev, t_config->brp, t_config->sjw, t_config->tseg_1 + t_config->prop_seg, t_config->tseg_2, !!t_config->ssp_offset);
    twai_ll_set_clkout(hal_ctx->dev, t_config->brp);
}

void twai_hal_configure_mask_filter(twai_hal_context_t *hal_ctx, uint8_t filter_id, const twai_mask_filter_config_t *f_config)
{
    uint32_t id = f_config->num_of_ids ? f_config->id_list[0] : f_config->id;
    bool full_open = (f_config->mask == 0) && (id == 0);
    bool full_close = (f_config->mask == UINT32_MAX) && (id == UINT32_MAX);
    // shift id/mask bits adopt to hardware bits define (3=32-29, 21=32-11), shift for dual filter is done in `DUAL_16BIT_FILTER()`
    uint8_t bit_shift = (f_config->dual_filter || full_close) ? 0 : f_config->is_ext ? 3 : 21;
    uint32_t code = id << bit_shift;
    uint32_t mask = f_config->mask << bit_shift;
    // save the id type strategy, see comment of `sja1000_filter_id_type`
    hal_ctx->sja1000_filter_id_type = (full_open) ? 0 : f_config->is_ext ? 2 : 1;
    twai_ll_set_acc_filter(hal_ctx->dev, code, ~mask, !f_config->dual_filter);
}

twai_error_state_t twai_hal_get_err_state(twai_hal_context_t *hal_ctx)
{
    uint32_t hw_state = twai_ll_get_status(hal_ctx->dev);
    return (hw_state & TWAI_LL_STATUS_BS) ? TWAI_ERROR_BUS_OFF : (hw_state & TWAI_LL_STATUS_ES) ? TWAI_ERROR_PASSIVE : TWAI_ERROR_ACTIVE;
}

uint16_t twai_hal_get_tec(twai_hal_context_t *hal_ctx)
{
    return twai_ll_get_tec((hal_ctx)->dev);
}

uint16_t twai_hal_get_rec(twai_hal_context_t *hal_ctx)
{
    return twai_ll_get_rec((hal_ctx)->dev);
}

/* -------------------------------- Actions --------------------------------- */
void twai_hal_start(twai_hal_context_t *hal_ctx)
{
    twai_ll_set_mode(hal_ctx->dev, hal_ctx->enable_listen_only, hal_ctx->enable_self_test, hal_ctx->enable_loopback);
    //Clear the TEC and REC
    twai_ll_set_tec(hal_ctx->dev, 0);
#if TWAI_LL_HAS_LOM_DOM_ISSUE
    // Errata workaround: Prevent transmission of dominant error frame while in listen only mode by setting REC to 128
    // before exiting reset mode. This forces the controller to be error passive (thus only transmits recessive bits).
    // The TEC/REC remain frozen in listen only mode thus ensuring we remain error passive.
    if (hal_ctx->enable_listen_only) {
        twai_ll_set_rec(hal_ctx->dev, 128);
    } else
#endif
    {
        twai_ll_set_rec(hal_ctx->dev, 0);
    }
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
    TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RUNNING);
    twai_ll_exit_reset_mode(hal_ctx->dev);
}

void twai_hal_stop(twai_hal_context_t *hal_ctx)
{
    twai_ll_enter_reset_mode(hal_ctx->dev);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);
    twai_ll_set_mode(hal_ctx->dev, true, false, false);    //Freeze REC by changing to LOM mode
    //Any TX is immediately halted on entering reset mode
    TWAI_HAL_CLEAR_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED | TWAI_HAL_STATE_FLAG_RUNNING);
}

void twai_hal_start_bus_recovery(twai_hal_context_t *hal_ctx)
{
    TWAI_HAL_CLEAR_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
    TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RECOVERING);
    twai_ll_exit_reset_mode(hal_ctx->dev);
}

/* ------------------------------------ IRAM Content ------------------------------------ */

#if TWAI_LL_HAS_RX_FIFO_ISSUE
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
            if (status & TWAI_LL_STATUS_ES) {   //EWL is exceeded, thus must have entered BUS OFF
                //The last error which trigger bus_off, hardware no longer fire TWAI_HAL_EVENT_BUS_ERR, but error reason still need to be read/clear and report
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_OFF | TWAI_HAL_EVENT_BUS_ERR);
                TWAI_HAL_SET_BITS(state_flags, TWAI_HAL_STATE_FLAG_BUS_OFF);
                //Any TX would have been halted by entering bus off. Reset its flag
                TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_RUNNING | TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
            } else {
                //Below EWL. Therefore TEC is counting down in bus recovery
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_RECOV_PROGRESS);
            }
        } else {    //Not in BUS OFF
            if (status & TWAI_LL_STATUS_ES) {       //Just Exceeded EWL
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_ERROR_WARNING);
                TWAI_HAL_SET_BITS(state_flags, TWAI_HAL_STATE_FLAG_ERR_WARN);
            } else if (hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_RECOVERING) {
                //Previously undergoing bus recovery. Thus means bus recovery complete, hardware back to error_active
                TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_BUS_RECOV_CPLT | TWAI_HAL_EVENT_ERROR_ACTIVE);
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
#if TWAI_LL_HAS_INTR_LOST_ISSUE
    // Errata workaround: Check the transmit buffer status bit to recover any lost transmit interrupt.
    if ((interrupts & TWAI_LL_INTR_TI || hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED) && status & TWAI_LL_STATUS_TBS) {
#else
    if (interrupts & TWAI_LL_INTR_TI) {
#endif
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_TX_BUFF_FREE);
        TWAI_HAL_CLEAR_BITS(state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
        if (status & TWAI_LL_STATUS_TCS) {
            TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_TX_SUCCESS);
        }
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
#if TWAI_LL_HAS_BUSOFF_REC_ISSUE
        //Errata workaround: Force REC to 0 by re-triggering bus-off (by setting TEC to 0 then 255)
        twai_ll_set_tec(hal_ctx->dev, 0);
        twai_ll_set_tec(hal_ctx->dev, 255);
        (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear the re-triggered bus-off interrupt
#endif
    }
    if (events & TWAI_HAL_EVENT_BUS_ERR) {
        twai_ll_err_type_t type;
        twai_ll_err_dir_t dir;
        twai_ll_err_seg_t seg;
        twai_ll_parse_err_code_cap(hal_ctx->dev, &type, &dir, &seg);    //Decode error interrupt
        twai_error_flags_t errors = {
            .bit_err = (type == TWAI_LL_ERR_BIT),
            .form_err = (type == TWAI_LL_ERR_FORM),
            .stuff_err = (type == TWAI_LL_ERR_STUFF),
            .ack_err = (type == TWAI_LL_ERR_OTHER) && (seg == TWAI_LL_ERR_SEG_ACK_SLOT),
        };
        hal_ctx->errors = errors;
#if TWAI_LL_HAS_RX_FRAME_ISSUE
        //Check for errata condition (RX message has bus error at particular segments)
        if (dir == TWAI_LL_ERR_DIR_RX &&
                ((seg == TWAI_LL_ERR_SEG_DATA || seg == TWAI_LL_ERR_SEG_CRC_SEQ) ||
                 (seg == TWAI_LL_ERR_SEG_ACK_DELIM && type == TWAI_LL_ERR_OTHER))) {
            TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_NEED_PERIPH_RESET);
            HAL_LOGD("TWAI_HAL", "RX frame invalid detected");
        }
#endif
    }
    if (events & TWAI_HAL_EVENT_ARB_LOST) {
        twai_ll_clear_arb_lost_cap(hal_ctx->dev);
    }
#if TWAI_LL_HAS_RX_FIFO_ISSUE
    //Check for errata condition (rx_msg_count >= corruption_threshold)
    if (events & TWAI_HAL_EVENT_RX_BUFF_FRAME && twai_ll_get_rx_msg_count(hal_ctx->dev) >= TWAI_RX_FIFO_CORRUPT_THRESH) {
        TWAI_HAL_SET_BITS(events, TWAI_HAL_EVENT_NEED_PERIPH_RESET);
        HAL_LOGD("TWAI_HAL", "RX FIFO corruption detected");
    }
#endif
#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
    if (events & TWAI_HAL_EVENT_NEED_PERIPH_RESET) {
        //A peripheral reset will invalidate an RX event;
        TWAI_HAL_CLEAR_BITS(events, (TWAI_HAL_EVENT_RX_BUFF_FRAME));
    }
#endif
    return events;
}

#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
bool twai_hal_is_hw_busy(twai_hal_context_t *hal_ctx)
{
    return (TWAI_LL_STATUS_TS | TWAI_LL_STATUS_RS) & twai_ll_get_status(hal_ctx->dev);
}

void twai_hal_backup_config(twai_hal_context_t *hal_ctx)
{
    //Some register must saved before entering reset mode
    hal_ctx->errata_ctx->rx_msg_cnt_save = (uint8_t) twai_ll_get_rx_msg_count(hal_ctx->dev);
    twai_ll_enter_reset_mode(hal_ctx->dev);     //Enter reset mode to stop the controller
    twai_ll_save_reg(hal_ctx->dev, &hal_ctx->errata_ctx->reg_save); //Save remaining registers after entering reset mode
}

uint32_t twai_hal_get_reset_lost_rx_cnt(twai_hal_context_t *hal_ctx)
{
    return hal_ctx->errata_ctx->rx_msg_cnt_save;
}

void twai_hal_restore_config(twai_hal_context_t *hal_ctx)
{
    twai_ll_enter_reset_mode(hal_ctx->dev);
    twai_ll_enable_extended_reg_layout(hal_ctx->dev);
    twai_ll_restore_reg(hal_ctx->dev, &hal_ctx->errata_ctx->reg_save);
}

void twai_hal_prepare_for_reset(twai_hal_context_t *hal_ctx)
{
    uint32_t status = twai_ll_get_status(hal_ctx->dev);
    if (!(status & TWAI_LL_STATUS_TBS)) {   //Transmit buffer is NOT free, indicating an Ongoing TX will be cancelled by the HW reset
        TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_NEED_RETRY);
        //Note: Even if the TX completes right after this, we still consider it will be retried.
        //Worst case the same message will get sent twice.
    }
    twai_hal_backup_config(hal_ctx);
}

void twai_hal_recover_from_reset(twai_hal_context_t *hal_ctx)
{
    twai_hal_restore_config(hal_ctx);
    twai_ll_exit_reset_mode(hal_ctx->dev);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);

    if (hal_ctx->state_flags & TWAI_HAL_STATE_FLAG_TX_NEED_RETRY) {
        //HW reset has cancelled a TX. Re-transmit here
        twai_hal_set_tx_buffer_and_transmit(hal_ctx, &hal_ctx->errata_ctx->tx_frame_save, 0);
        TWAI_HAL_CLEAR_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_NEED_RETRY);
    }
}
#endif // TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE

void twai_hal_format_frame(const twai_hal_trans_desc_t *trans_desc, twai_hal_frame_t *frame)
{
    const twai_frame_header_t *header = trans_desc->frame.header;
    twai_message_t msg_flags = {
        .extd = header->ide,
        .rtr = header->rtr,
        .ss = trans_desc->config.retry_cnt != -1,
        .self = trans_desc->config.loopback,
    };
    uint16_t final_dlc = (header->dlc) ? header->dlc : twaifd_len2dlc(trans_desc->frame.buffer_len);
    twai_ll_format_frame_buffer(header->id, final_dlc, trans_desc->frame.buffer, msg_flags.flags, frame);
}

void twai_hal_parse_frame(twai_hal_context_t *hal_ctx, twai_hal_frame_t *frame, twai_frame_header_t *header, uint8_t *buffer, uint8_t buffer_len)
{
    twai_ll_parse_frame_header((const twai_ll_frame_buffer_t *)frame, header);
    header->timestamp = 0;  // hardware timestamp is not supported in v1
    if (!header->rtr) {
        twai_ll_parse_frame_data((const twai_ll_frame_buffer_t *)frame, buffer, buffer_len);
    }
}

void twai_hal_set_tx_buffer_and_transmit(twai_hal_context_t *hal_ctx, twai_hal_frame_t *tx_frame, uint8_t buffer_idx)
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
    } else if (tx_frame->single_shot) {
        twai_ll_set_cmd_tx_single_shot(hal_ctx->dev);
    } else {
        twai_ll_set_cmd_tx(hal_ctx->dev);
    }
    TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
    if (&hal_ctx->errata_ctx->tx_frame_save == tx_frame) {
        return;
    }
    //Save transmitted frame in case we need to retry
    ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-overlapping-buffers") // TODO IDF-11085
    memcpy(&hal_ctx->errata_ctx->tx_frame_save, tx_frame, sizeof(twai_hal_frame_t));
    ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-overlapping-buffers")
#endif  //TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
}

uint32_t twai_hal_get_rx_msg_count(twai_hal_context_t *hal_ctx)
{
    return twai_ll_get_rx_msg_count((hal_ctx)->dev);
}

bool twai_hal_read_rx_fifo(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame)
{
#if TWAI_LL_SUPPORT(RX_STATUS)
    if (twai_ll_get_status(hal_ctx->dev) & TWAI_LL_STATUS_MS) {
        //Release the buffer for this particular overrun frame
        twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
        return false;
    }
#else
    if (twai_ll_get_status(hal_ctx->dev) & TWAI_LL_STATUS_DOS) {
        //No need to release RX buffer as we'll be releasing all RX frames in continuously later
        return false;
    }
#endif
    twai_ll_get_rx_buffer(hal_ctx->dev, rx_frame);
    twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
    return true;
}

bool twai_hal_soft_filter_check_msg(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame)
{
    if (hal_ctx->sja1000_filter_id_type && (hal_ctx->sja1000_filter_id_type - 1) != twai_ll_frame_is_ext_format(rx_frame)) {
        return false;   // if filter enabled and frame don't match, drop the frame
    }
    return true;
}

bool twai_hal_check_state_flags(twai_hal_context_t *hal_ctx, uint32_t check_flags)
{
    return hal_ctx->state_flags & check_flags;
}

uint32_t twai_hal_clear_rx_fifo_overrun(twai_hal_context_t *hal_ctx)
{
    uint32_t msg_cnt = 0;
    //Note: Need to keep polling th rx message counter in case another message arrives whilst clearing
    while (twai_ll_get_rx_msg_count(hal_ctx->dev) > 0) {
        twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
        msg_cnt++;
    }
    //Set a clear data overrun command to clear the data overrun status bit
    twai_ll_set_cmd_clear_data_overrun(hal_ctx->dev);
    return msg_cnt;
}
