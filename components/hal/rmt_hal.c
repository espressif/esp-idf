/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"
#include "soc/soc_caps.h"

void rmt_hal_init(rmt_hal_context_t *hal)
{
    hal->regs = &RMT;
    hal->mem = &RMTMEM;
}

void rmt_hal_tx_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_tx_reset_pointer(hal->regs, channel);
    rmt_ll_tx_reset_loop(hal->regs, channel);
    rmt_ll_enable_tx_err_interrupt(hal->regs, channel, false);
    rmt_ll_enable_tx_end_interrupt(hal->regs, channel, false);
    rmt_ll_enable_tx_thres_interrupt(hal->regs, channel, false);
    rmt_ll_clear_tx_err_interrupt(hal->regs, channel);
    rmt_ll_clear_tx_end_interrupt(hal->regs, channel);
    rmt_ll_clear_tx_thres_interrupt(hal->regs, channel);
}

void rmt_hal_rx_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_rx_reset_pointer(hal->regs, channel);
    rmt_ll_enable_rx_err_interrupt(hal->regs, channel, false);
    rmt_ll_enable_rx_end_interrupt(hal->regs, channel, false);
    rmt_ll_clear_rx_err_interrupt(hal->regs, channel);
    rmt_ll_clear_rx_end_interrupt(hal->regs, channel);
}

void rmt_hal_tx_set_channel_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t counter_clk_hz)
{
    rmt_ll_tx_reset_channel_clock_div(hal->regs, channel);
    uint32_t counter_div = (base_clk_hz + counter_clk_hz / 2) / counter_clk_hz;
    rmt_ll_tx_set_channel_clock_div(hal->regs, channel, counter_div);
}

void rmt_hal_set_carrier_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t carrier_clk_hz, float carrier_clk_duty)
{
    uint32_t carrier_div = (base_clk_hz + carrier_clk_hz / 2) / carrier_clk_hz;
    uint32_t div_high = (uint32_t)(carrier_div * carrier_clk_duty);
    uint32_t div_low = carrier_div - div_high;
    rmt_ll_tx_set_carrier_high_low_ticks(hal->regs, channel, div_high, div_low);
}

void rmt_hal_set_rx_filter_thres(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t thres_us)
{
    uint32_t thres = (uint32_t)(base_clk_hz / 1e6 * thres_us);
    rmt_ll_rx_set_filter_thres(hal->regs, channel, thres);
}

void rmt_hal_set_rx_idle_thres(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t thres_us)
{
    uint32_t thres = (uint32_t)(base_clk_hz / 1e6 * thres_us);
    rmt_ll_rx_set_idle_thres(hal->regs, channel, thres);
}
