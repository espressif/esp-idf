/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "hal/twai_hal.h"
#include "hal/twaifd_ll.h"

size_t twai_hal_get_mem_requirment(void)
{
    return sizeof(twai_hal_context_t);
}

bool twai_hal_init(twai_hal_context_t *hal_ctx, const twai_hal_config_t *config)
{
    hal_ctx->dev = TWAIFD_LL_GET_HW(config->controller_id);
    hal_ctx->retry_cnt = config->retry_cnt;
    hal_ctx->enable_self_test = config->enable_self_test;
    hal_ctx->enable_loopback = config->enable_loopback;
    hal_ctx->enable_listen_only = config->enable_listen_only;

    twaifd_ll_reset(hal_ctx->dev);
    twaifd_ll_enable_hw(hal_ctx->dev, false);   //mode should be changed under disabled
    twaifd_ll_set_mode(hal_ctx->dev, config->enable_listen_only, config->enable_self_test, config->enable_loopback);
    twaifd_ll_set_tx_retrans_limit(hal_ctx->dev, config->retry_cnt);
    twaifd_ll_filter_drop_rtr(hal_ctx->dev, config->no_receive_rtr);
    twaifd_ll_enable_filter_mode(hal_ctx->dev, true);  // each filter still has independent enable control
    twaifd_ll_enable_fd_mode(hal_ctx->dev, true);      // fd frame still controlled by `header.fdf`
    twaifd_ll_enable_bus_off_tx_fail_mode(hal_ctx->dev, true); // all buffers go to "TX failed" state upon bus-off
    twaifd_ll_enable_rxfifo_auto_increase(hal_ctx->dev, true);
    twaifd_ll_ts_set_sample_point(hal_ctx->dev, TWAIFD_LL_TS_POINT_EOF);
    twaifd_ll_enable_intr(hal_ctx->dev, config->intr_mask);
    if (config->timer_freq) {
        twaifd_ll_timer_set_clkdiv(hal_ctx->dev, config->clock_source_hz / config->timer_freq);
        twaifd_ll_timer_enable_intr(hal_ctx->dev, 1, true);
    }
    return true;
}

void twai_hal_deinit(twai_hal_context_t *hal_ctx)
{
    twaifd_ll_set_operate_cmd(hal_ctx->dev, TWAIFD_LL_HW_CMD_RST_TX_CNT);
    twaifd_ll_set_operate_cmd(hal_ctx->dev, TWAIFD_LL_HW_CMD_RST_RX_CNT);
    memset(hal_ctx, 0, sizeof(twai_hal_context_t));
}

bool twai_hal_check_brp_validation(twai_hal_context_t *hal_ctx, uint32_t brp)
{
    return twaifd_ll_check_brp_validation(brp);
}

void twai_hal_configure_timing(twai_hal_context_t *hal_ctx, const twai_timing_advanced_config_t *t_config)
{
    twaifd_ll_set_nominal_bitrate(hal_ctx->dev, t_config);
    if (t_config->ssp_offset) {
        // the underlying hardware calculates the ssp in system clock cycles, not in quanta time
        twaifd_ll_config_secondary_sample_point(hal_ctx->dev, TWAIFD_LL_SSP_SRC_MEAS_OFFSET, t_config->ssp_offset * t_config->brp);
    }
}

void twai_hal_configure_timing_fd(twai_hal_context_t *hal_ctx, const twai_timing_advanced_config_t *t_config_fd)
{
    twaifd_ll_set_fd_bitrate(hal_ctx->dev, t_config_fd);
    if (t_config_fd->ssp_offset) {
        // the underlying hardware calculates the ssp in system clock cycles, not in quanta time
        twaifd_ll_config_secondary_sample_point(hal_ctx->dev, TWAIFD_LL_SSP_SRC_MEAS_OFFSET, t_config_fd->ssp_offset * t_config_fd->brp);
    }
}

void twai_hal_configure_mask_filter(twai_hal_context_t *hal_ctx, uint8_t filter_id, const twai_mask_filter_config_t *f_config)
{
    uint32_t id = f_config->num_of_ids ? f_config->id_list[0] : f_config->id;
    bool full_open = (f_config->mask == 0) && (id == 0);
    bool full_close = (f_config->mask == UINT32_MAX) && (id == UINT32_MAX);
    bool cc_ext = full_open || (!full_close && f_config->is_ext && !f_config->no_classic);
    bool fd_ext = full_open || (!full_close && f_config->is_ext && !f_config->no_fd);
    bool cc_std = full_open || (!full_close && !f_config->is_ext && !f_config->no_classic);
    bool fd_std = full_open || (!full_close && !f_config->is_ext && !f_config->no_fd);
    twaifd_ll_mask_filter_accept_classic_std(hal_ctx->dev, filter_id, cc_std);
    twaifd_ll_mask_filter_accept_classic_ext(hal_ctx->dev, filter_id, cc_ext);
    twaifd_ll_mask_filter_accept_fd_std(hal_ctx->dev, filter_id, fd_std);
    twaifd_ll_mask_filter_accept_fd_ext(hal_ctx->dev, filter_id, fd_ext);
    twaifd_ll_mask_filter_set_id_mask(hal_ctx->dev, filter_id, f_config->is_ext, id, f_config->mask);
}

void twai_hal_configure_range_filter(twai_hal_context_t *hal_ctx, uint8_t filter_id, const twai_range_filter_config_t *f_config)
{
    bool cc_ext = f_config->is_ext && !f_config->no_classic;
    bool fd_ext = f_config->is_ext && !f_config->no_fd;
    bool cc_std = !f_config->is_ext && !f_config->no_classic;
    bool fd_std = !f_config->is_ext && !f_config->no_fd;
    twaifd_ll_range_filter_accept_classic_std(hal_ctx->dev, filter_id, cc_std);
    twaifd_ll_range_filter_accept_classic_ext(hal_ctx->dev, filter_id, cc_ext);
    twaifd_ll_range_filter_accept_fd_std(hal_ctx->dev, filter_id, fd_std);
    twaifd_ll_range_filter_accept_fd_ext(hal_ctx->dev, filter_id, fd_ext);
    twaifd_ll_range_filter_set_id_thres(hal_ctx->dev, 0, f_config->is_ext, f_config->range_high, f_config->range_low);
}

void twai_hal_start(twai_hal_context_t *hal_ctx)
{
    twaifd_ll_enable_hw(hal_ctx->dev, true);
    twaifd_ll_waiting_state_change(hal_ctx->dev);
}

void twai_hal_stop(twai_hal_context_t *hal_ctx)
{
    twaifd_ll_enable_hw(hal_ctx->dev, false);
}

void twai_hal_timer_start_with(twai_hal_context_t *hal_ctx, uint64_t preload_value)
{
    hal_ctx->timer_overflow_cnt = preload_value >> 32;
    twaifd_ll_timer_set_preload_value(hal_ctx->dev, preload_value);
    twaifd_ll_timer_apply_preload_value(hal_ctx->dev);
    twaifd_ll_timer_set_preload_value(hal_ctx->dev, 0); // set load value back to 0
    twaifd_ll_timer_enable(hal_ctx->dev, true);
}

void twai_hal_timer_stop(twai_hal_context_t *hal_ctx)
{
    twaifd_ll_timer_enable(hal_ctx->dev, false);
}

twai_error_state_t twai_hal_get_err_state(twai_hal_context_t *hal_ctx)
{
    return twaifd_ll_get_fault_state(hal_ctx->dev);
}

void twai_hal_start_bus_recovery(twai_hal_context_t *hal_ctx)
{
    twaifd_ll_set_operate_cmd(hal_ctx->dev, TWAIFD_LL_HW_CMD_RST_ERR_CNT);
}

uint16_t twai_hal_get_tec(twai_hal_context_t *hal_ctx)
{
    return twaifd_ll_get_tec((hal_ctx)->dev);
}

uint16_t twai_hal_get_rec(twai_hal_context_t *hal_ctx)
{
    return twaifd_ll_get_rec((hal_ctx)->dev);
}

/* ------------------------------------ IRAM Content ------------------------------------ */

void twai_hal_format_frame(const twai_hal_trans_desc_t *trans_desc, twai_hal_frame_t *frame)
{
    const twai_frame_header_t *header = trans_desc->frame.header;
    int final_dlc = (header->dlc) ? header->dlc : twaifd_len2dlc(trans_desc->frame.buffer_len);
    twaifd_ll_format_frame_header(header, final_dlc, frame);
    if (!header->rtr) {
        int data_len = (header->dlc) ? twaifd_dlc2len(header->dlc) : trans_desc->frame.buffer_len;
        data_len = (header->fdf) ? MIN(data_len, TWAIFD_FRAME_MAX_LEN) : MIN(data_len, TWAI_FRAME_MAX_LEN);
        twaifd_ll_format_frame_data(trans_desc->frame.buffer, data_len, frame);
    }
}

void twai_hal_parse_frame(twai_hal_context_t *hal_ctx, twai_hal_frame_t *frame, twai_frame_header_t *header, uint8_t *buffer, uint8_t buffer_len)
{
    // compensate the timer overflow before parsing it
    frame->timestamp_high = hal_ctx->timer_overflow_cnt;

    twaifd_ll_parse_frame_header(frame, header);
    if (!header->rtr) {
        int frame_data_len = twaifd_dlc2len(header->dlc);
        // limit data_len for twai classic non-iso mode.
        frame_data_len = (header->fdf) ? MIN(frame_data_len, TWAIFD_FRAME_MAX_LEN) : MIN(frame_data_len, TWAI_FRAME_MAX_LEN);
        uint8_t final_len = MIN(frame_data_len, buffer_len);
        twaifd_ll_parse_frame_data(frame, buffer, final_len);
    }
}

void twai_hal_set_tx_buffer_and_transmit(twai_hal_context_t *hal_ctx, twai_hal_frame_t *tx_frame, uint8_t buffer_idx)
{
    twaifd_ll_mount_tx_buffer(hal_ctx->dev, tx_frame, buffer_idx);
    twaifd_ll_set_tx_buffer_cmd(hal_ctx->dev, buffer_idx, TWAIFD_LL_TX_CMD_READY);
}

uint32_t twai_hal_get_rx_msg_count(twai_hal_context_t *hal_ctx)
{
    return twaifd_ll_get_rx_frame_count(hal_ctx->dev);
}

bool twai_hal_read_rx_fifo(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame)
{
    twaifd_ll_get_rx_frame(hal_ctx->dev, rx_frame);
    // 'not_empty' interrupt can only be cleared after reading the frame
    twaifd_ll_clr_intr_status(hal_ctx->dev, TWAIFD_LL_INTR_RX_NOT_EMPTY);
    return true;
}

uint32_t twai_hal_get_events(twai_hal_context_t *hal_ctx)
{
    uint32_t hal_events = 0;
    uint32_t int_stat = twaifd_ll_get_intr_status(hal_ctx->dev);
    uint32_t timer_int_stat = twaifd_ll_timer_get_intr_status(hal_ctx->dev, 0xffffffff);
    uint32_t tec = twaifd_ll_get_tec(hal_ctx->dev);
    uint32_t rec = twaifd_ll_get_rec(hal_ctx->dev);
    twaifd_ll_clr_intr_status(hal_ctx->dev, int_stat);

    // check timer event before other events, to avoid race condition that receive frame right on timer overflow
    if (timer_int_stat) {
        twaifd_ll_timer_clr_intr_status(hal_ctx->dev, timer_int_stat);
        hal_ctx->timer_overflow_cnt ++;
    }
    if (int_stat & (TWAIFD_LL_INTR_TX_DONE)) {
        hal_events |= TWAI_HAL_EVENT_TX_BUFF_FREE;
        if (int_stat & TWAIFD_LL_INTR_TX_FRAME) {
            hal_events |= TWAI_HAL_EVENT_TX_SUCCESS;
        }
    }
    if (int_stat & TWAIFD_LL_INTR_RX_NOT_EMPTY) {
        hal_events |= TWAI_HAL_EVENT_RX_BUFF_FRAME;
    }
    if (int_stat & (TWAIFD_LL_INTR_BUS_ERR | TWAIFD_LL_INTR_DATA_OVERRUN | TWAIFD_LL_INTR_RX_FULL)) {
        hal_ctx->errors = twaifd_ll_get_err_reason(hal_ctx->dev);
        hal_events |= TWAI_HAL_EVENT_BUS_ERR;
    }
    if (int_stat & TWAIFD_LL_INTR_ARBIT_LOST) {
        hal_events |= TWAI_HAL_EVENT_ARB_LOST;
    }
    if (int_stat & (TWAIFD_LL_INTR_FSM_CHANGE | TWAIFD_LL_INTR_ERR_WARN)) {
        twai_error_state_t curr_sta = twaifd_ll_get_fault_state(hal_ctx->dev);
        uint32_t limit = twaifd_ll_get_err_warn_limit(hal_ctx->dev);
        if (curr_sta == TWAI_ERROR_BUS_OFF) {
            hal_events |= TWAI_HAL_EVENT_BUS_OFF;
        } else if (curr_sta == TWAI_ERROR_PASSIVE) {
            hal_events |= TWAI_HAL_EVENT_ERROR_PASSIVE;
        } else {
            if ((tec >= limit) || (rec >= limit)) {
                hal_events |= TWAI_HAL_EVENT_ERROR_WARNING;
            } else {
                hal_events |= TWAI_HAL_EVENT_ERROR_ACTIVE;
            }
        }
    }
    return hal_events;
}
