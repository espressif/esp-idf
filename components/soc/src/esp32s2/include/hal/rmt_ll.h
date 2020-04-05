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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "soc/rmt_struct.h"
#include "soc/rmt_caps.h"

static inline void rmt_ll_enable_drive_clock(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.clk_en = enable; // register clock gating
    dev->apb_conf.mem_clk_force_on = enable; // memory clock gating
}

static inline void rmt_ll_reset_counter_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << channel);
    dev->ref_cnt_rst.val &= ~(1 << channel);
}

static inline void rmt_ll_reset_tx_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_rd_rst = 1;
    dev->conf_ch[channel].conf1.mem_rd_rst = 0;
}

static inline void rmt_ll_reset_rx_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_wr_rst = 1;
    dev->conf_ch[channel].conf1.mem_wr_rst = 0;
}

static inline void rmt_ll_start_tx(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.tx_start = 1;
}

static inline void rmt_ll_stop_tx(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.tx_stop = 1;
}

static inline void rmt_ll_enable_rx(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_en = enable;
}

static inline void rmt_ll_power_down_mem(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.mem_force_pu = !enable;
    dev->apb_conf.mem_force_pd = enable;
}

static inline bool rmt_ll_is_mem_power_down(rmt_dev_t *dev)
{
    // the RTC domain can also power down RMT memory
    // so it's probably not enough to detect whether it's powered down or not
    // mem_force_pd has higher priority than mem_force_pu
    return (dev->apb_conf.mem_force_pd) || !(dev->apb_conf.mem_force_pu);
}

static inline void rmt_ll_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

static inline uint32_t rmt_ll_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

static inline void rmt_ll_set_counter_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    dev->conf_ch[channel].conf0.div_cnt = div;
}

static inline uint32_t rmt_ll_get_counter_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = dev->conf_ch[channel].conf0.div_cnt;
    return div == 0 ? 256 : div;
}

static inline void rmt_ll_enable_tx_pingpong(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.mem_tx_wrap_en = enable;
}

static inline void rmt_ll_enable_mem_access(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.fifo_mask = enable;
}

static inline void rmt_ll_set_rx_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    dev->conf_ch[channel].conf0.idle_thres = thres;
}

static inline uint32_t rmt_ll_get_rx_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.idle_thres;
}

static inline void rmt_ll_set_mem_owner(rmt_dev_t *dev, uint32_t channel, uint8_t owner)
{
    dev->conf_ch[channel].conf1.mem_owner = owner;
}

static inline uint32_t rmt_ll_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.mem_owner;
}

static inline void rmt_ll_enable_tx_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.tx_conti_mode = enable;
}

static inline bool rmt_ll_is_tx_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.tx_conti_mode;
}

static inline void rmt_ll_set_tx_loop_count(rmt_dev_t *dev, uint32_t channel, uint32_t count)
{
    dev->tx_lim_ch[channel].tx_loop_num = count;
}

static inline void rmt_ll_reset_tx_loop(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_lim_ch[channel].loop_count_reset = 1;
    dev->tx_lim_ch[channel].loop_count_reset = 0;
}

static inline void rmt_ll_enable_tx_loop_count(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_lim_ch[channel].tx_loop_cnt_en = enable;
}

static inline void rmt_ll_enable_tx_sync(rmt_dev_t *dev, bool enable)
{
    dev->tx_sim.en = enable;
}

static inline void rmt_ll_add_channel_to_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val |= 1 << channel;
}

static inline uint32_t rmt_ll_remove_channel_from_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val &= ~(1 << channel);
    return dev->tx_sim.val & 0x0F;
}

static inline void rmt_ll_enable_rx_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_filter_en = enable;
}

static inline void rmt_ll_set_rx_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    dev->conf_ch[channel].conf1.rx_filter_thres = thres;
}

static inline void rmt_ll_set_counter_clock_src(rmt_dev_t *dev, uint32_t channel, uint8_t src)
{
    dev->conf_ch[channel].conf1.ref_always_on = src;
}

static inline uint32_t rmt_ll_get_counter_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.ref_always_on;
}

static inline void rmt_ll_enable_tx_idle(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.idle_out_en = enable;
}

static inline bool rmt_ll_is_tx_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_en;
}

static inline void rmt_ll_set_tx_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf1.idle_out_lv = level;
}

static inline uint32_t rmt_ll_get_tx_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_lv;
}

static inline uint32_t rmt_ll_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel].val;
}

static inline void rmt_ll_set_tx_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim_ch[channel].limit = limit;
}

static inline void rmt_ll_set_rx_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim_ch[channel].rx_lim = limit;
}

static inline uint32_t rmt_ll_get_rx_limit(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_lim_ch[channel].rx_lim;
}

static inline void rmt_ll_enable_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3));
    dev->int_ena.val |= (enable << (channel * 3));
}

static inline void rmt_ll_enable_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 1));
    dev->int_ena.val |= (enable << (channel * 3 + 1));
}

static inline void rmt_ll_enable_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 2));
    dev->int_ena.val |= (enable << (channel * 3 + 2));
}

static inline void rmt_ll_enable_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel + 12));
    dev->int_ena.val |= (enable << (channel + 12));
}

static inline void rmt_ll_enable_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel + 16));
    dev->int_ena.val |= (enable << (channel + 16));
}

static inline void rmt_ll_enable_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel + 20));
    dev->int_ena.val |= (enable << (channel + 20));
}

static inline void rmt_ll_clear_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3));
}

static inline void rmt_ll_clear_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 1));
}

static inline void rmt_ll_clear_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 2));
}

static inline void rmt_ll_clear_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 12));
}

static inline void rmt_ll_clear_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 16));
}

static inline void rmt_ll_clear_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 20));
}

static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x01) >> 0) | ((status & 0x08) >> 2) | ((status & 0x40) >> 4) | ((status & 0x200) >> 6);
}

static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x02) >> 1) | ((status & 0x10) >> 3) | ((status & 0x80) >> 5) | ((status & 0x400) >> 7);
}

static inline uint32_t rmt_ll_get_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8);
}

static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xF000) >> 12;
}

static inline uint32_t rmt_ll_get_tx_loop_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xF0000) >> 16;
}

static inline uint32_t rmt_ll_get_rx_thres_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xF00000) >> 20;
}

static inline void rmt_ll_set_tx_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    dev->carrier_duty_ch[channel].high = high_ticks;
    dev->carrier_duty_ch[channel].low = low_ticks;
}

static inline void rmt_ll_set_rx_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    dev->ch_rx_carrier_rm[channel].carrier_high_thres_ch = high_ticks;
    dev->ch_rx_carrier_rm[channel].carrier_low_thres_ch = low_ticks;
}

static inline void rmt_ll_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = dev->carrier_duty_ch[channel].high;
    *low_ticks = dev->carrier_duty_ch[channel].low;
}

// This function has different meaning for TX and RX
// TX: enable to modulate carrier
// RX: enable to demodulate carrier
static inline void rmt_ll_enable_carrier(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_en = enable;
}

static inline void rmt_ll_set_carrier_on_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf0.carrier_out_lv = level;
}

// set true, enable carrier in all RMT state (idle, reading, sending)
// set false, enable carrier only in sending state (i.e. there're effective data in RAM to be sent)
static inline void rmt_ll_tx_set_carrier_always_on(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_eff_en = !enable;
}

static inline void rmt_ll_write_memory(rmt_mem_t *mem, uint32_t channel, const rmt_item32_t *data, uint32_t length, uint32_t off)
{
    length = (off + length) > SOC_RMT_CHANNEL_MEM_WORDS ? (SOC_RMT_CHANNEL_MEM_WORDS - off) : length;
    for (uint32_t i = 0; i < length; i++) {
        mem->chan[channel].data32[i + off].val = data[i].val;
    }
}

static inline void rmt_ll_enable_rx_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.chk_rx_carrier_en = enable;
}

/************************************************************************************************
 * Following Low Level APIs only used for backward compatible, will be deprecated in the future!
 ***********************************************************************************************/

static inline void rmt_ll_set_intr_enable_mask(uint32_t mask)
{
    RMT.int_ena.val |= mask;
}

static inline void rmt_ll_clr_intr_enable_mask(uint32_t mask)
{
    RMT.int_ena.val &= (~mask);
}

#ifdef __cplusplus
}
#endif
