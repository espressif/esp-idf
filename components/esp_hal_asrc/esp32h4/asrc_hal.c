/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "hal/asrc_ll.h"
#include "hal/asrc_hal.h"

void asrc_hal_init(asrc_hal_context_t *hal)
{
    hal->dev = ASRC_LL_GET_HW();
}

void asrc_hal_enable_asrc_module(asrc_hal_context_t *hal, bool enable)
{
    asrc_ll_enable_asrc_module(enable);
}

void asrc_hal_init_stream(asrc_hal_context_t *hal, asrc_hal_config_t *asrc_cfg, uint8_t asrc_idx)
{
    asrc_ll_stop_stream(hal->dev, asrc_idx);
    asrc_ll_reset_stream(hal->dev, asrc_idx);
    asrc_ll_reset_input_fifo(hal->dev, asrc_idx);
    asrc_ll_reset_output_fifo(hal->dev, asrc_idx);
    asrc_ll_set_channel_mode(hal->dev, asrc_idx, asrc_cfg->src_info.channel,
                             asrc_cfg->dest_info.channel, asrc_cfg->weight, asrc_cfg->weight_len);
    asrc_ll_set_rate_reg(hal->dev, asrc_idx, asrc_cfg->src_info.sample_rate, asrc_cfg->dest_info.sample_rate);
    asrc_ll_clear_incnt_counter(hal->dev, asrc_idx);
    asrc_ll_clear_outcnt_counter(hal->dev, asrc_idx);
    asrc_ll_enable_outcnt_counter(hal->dev, asrc_idx);
    asrc_ll_set_eof_mode(hal->dev, asrc_idx, 0);
    asrc_ll_enable_outsample_comp(hal->dev, asrc_idx);
}

void asrc_hal_deinit_stream(asrc_hal_context_t *hal, uint8_t asrc_idx)
{
    asrc_ll_stop_stream(hal->dev, asrc_idx);
    asrc_ll_reset_stream(hal->dev, asrc_idx);
    asrc_ll_reset_input_fifo(hal->dev, asrc_idx);
    asrc_ll_reset_output_fifo(hal->dev, asrc_idx);
    asrc_ll_clear_incnt_counter(hal->dev, asrc_idx);
    asrc_ll_clear_outcnt_counter(hal->dev, asrc_idx);
}

void asrc_hal_set_in_bytes_num(asrc_hal_context_t *hal, uint8_t asrc_idx, uint32_t in_bytes_num)
{
    asrc_ll_set_in_bytes_num(hal->dev, asrc_idx, in_bytes_num);
}

void asrc_hal_set_out_bytes_num(asrc_hal_context_t *hal, uint8_t asrc_idx, uint32_t out_bytes_num)
{
    asrc_ll_set_out_bytes_num(hal->dev, asrc_idx, out_bytes_num);
}

void asrc_hal_start(asrc_hal_context_t *hal, uint8_t asrc_idx)
{
    asrc_ll_start_stream(hal->dev, asrc_idx);
}

uint32_t asrc_hal_get_out_data_bytes(asrc_hal_context_t *hal, uint8_t asrc_idx)
{
    return asrc_ll_get_outbytes_cnt(hal->dev, asrc_idx);
}

void asrc_hal_deinit(asrc_hal_context_t *hal)
{
    hal->dev = NULL;
}
