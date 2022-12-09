// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"

void rmt_hal_init(rmt_hal_context_t *hal)
{
    hal->regs = &RMT;
    hal->mem = &RMTMEM;
}

void rmt_hal_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_reset_tx_pointer(hal->regs, channel);
    rmt_ll_reset_rx_pointer(hal->regs, channel);
    rmt_ll_enable_err_interrupt(hal->regs, channel, false);
    rmt_ll_enable_tx_end_interrupt(hal->regs, channel, false);
    rmt_ll_enable_tx_thres_interrupt(hal->regs, channel, false);
    rmt_ll_enable_rx_end_interrupt(hal->regs, channel, false);
    rmt_ll_clear_err_interrupt(hal->regs, channel);
    rmt_ll_clear_tx_end_interrupt(hal->regs, channel);
    rmt_ll_clear_tx_thres_interrupt(hal->regs, channel);
    rmt_ll_clear_rx_end_interrupt(hal->regs, channel);
}

void rmt_hal_set_counter_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t counter_clk_hz)
{
    rmt_ll_reset_counter_clock_div(hal->regs, channel);
    uint32_t counter_div = (base_clk_hz + counter_clk_hz / 2) / counter_clk_hz;
    rmt_ll_set_counter_clock_div(hal->regs, channel, counter_div);
}

uint32_t rmt_hal_get_counter_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz)
{
    return base_clk_hz / rmt_ll_get_counter_clock_div(hal->regs, channel);
}

void rmt_hal_set_carrier_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t carrier_clk_hz, float carrier_clk_duty)
{
    uint32_t carrier_div = (base_clk_hz + carrier_clk_hz / 2) / carrier_clk_hz;
    uint32_t div_high = (uint32_t)(carrier_div * carrier_clk_duty);
    uint32_t div_low = carrier_div - div_high;
    rmt_ll_set_tx_carrier_high_low_ticks(hal->regs, channel, div_high, div_low);
}

void rmt_hal_get_carrier_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t *carrier_clk_hz, float *carrier_clk_duty)
{
    uint32_t div_high = 0;
    uint32_t div_low = 0;
    rmt_ll_get_carrier_high_low_ticks(hal->regs, channel, &div_high, &div_low);
    *carrier_clk_hz = base_clk_hz / (div_high + div_low);
    *carrier_clk_duty = (float)div_high / (div_high + div_low);
}

void rmt_hal_set_rx_filter_thres(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t thres_us)
{
    uint32_t thres = (uint32_t)(base_clk_hz / 1e6 * thres_us);
    rmt_ll_set_rx_filter_thres(hal->regs, channel, thres);
}

void rmt_hal_set_rx_idle_thres(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t thres_us)
{
    uint32_t thres = (uint32_t)(base_clk_hz / 1e6 * thres_us);
    rmt_ll_set_rx_idle_thres(hal->regs, channel, thres);
}

uint32_t rmt_hal_receive(rmt_hal_context_t *hal, uint32_t channel, rmt_item32_t *buf)
{
    uint32_t len = 0;
    rmt_ll_set_mem_owner(hal->regs, channel, RMT_MEM_OWNER_SW);
    for (len = 0; len < SOC_RMT_CHANNEL_MEM_WORDS; len++) {
        buf[len].val = hal->mem->chan[channel].data32[len].val;
        if (!(buf[len].val & 0x7FFF)) {
            break;
        } else if (!(buf[len].val & 0x7FFF0000)) {
            len++;
            break;
        }
    }
    rmt_ll_set_mem_owner(hal->regs, channel, RMT_MEM_OWNER_HW);
    rmt_ll_reset_rx_pointer(hal->regs, channel);
    return len;
}

void rmt_hal_transmit(rmt_hal_context_t *hal, uint32_t channel, const rmt_item32_t *src, uint32_t length, uint32_t offset)
{
    rmt_ll_set_mem_owner(hal->regs, channel, RMT_MEM_OWNER_SW);
    rmt_ll_write_memory(hal->mem, channel, src, length, offset);
    rmt_ll_set_mem_owner(hal->regs, channel, RMT_MEM_OWNER_HW);
}