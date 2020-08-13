// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdbool.h>
#include "soc/rmt_struct.h"
#include "soc/soc_caps.h"

#define RMT_LL_HW_BASE  (&RMT)
#define RMT_LL_MEM_BASE (&RMTMEM)

#define RMT_LL_TX_CHAN_NUM  (4)
#define RMT_LL_TX_CHAN_MASK (0x0F)
#define RMT_LL_RX_CHAN_MASK (0xF0)

static inline void rmt_ll_set_sclk(rmt_dev_t *dev, uint32_t source, uint32_t div_num, uint32_t div_frac_a, uint32_t div_frac_b)
{
    dev->sys_conf.sclk_active = 0;
    dev->sys_conf.sclk_sel = source;
    dev->sys_conf.sclk_div_num = div_num;
    dev->sys_conf.sclk_div_a = div_frac_a;
    dev->sys_conf.sclk_div_b = div_frac_b;
    dev->sys_conf.sclk_active = 1;
}

static inline void rmt_ll_enable_drive_clock(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.clk_en = enable; // register clock gating
    dev->sys_conf.mem_clk_force_on = enable; // memory clock gating
}

static inline void rmt_ll_reset_counter_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << channel);
    dev->ref_cnt_rst.val &= ~(1 << channel);
}

static inline void rmt_ll_reset_tx_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_conf[channel].mem_rd_rst = 1;
    dev->tx_conf[channel].mem_rd_rst = 0;
    dev->tx_conf[channel].apb_mem_rst = 1;
    dev->tx_conf[channel].apb_mem_rst = 0;
}

static inline void rmt_ll_reset_rx_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.mem_wr_rst = 1;
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.mem_wr_rst = 0;
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.apb_mem_rst = 1;
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.apb_mem_rst = 0;
}

static inline void rmt_ll_start_tx(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_conf[channel].conf_update = 1;
    dev->tx_conf[channel].tx_start = 1;
}

static inline void rmt_ll_stop_tx(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_conf[channel].tx_stop = 1;
    dev->tx_conf[channel].conf_update = 1;
}

static inline void rmt_ll_enable_rx(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.rx_en = enable;
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.conf_update = 1;
}

static inline void rmt_ll_power_down_mem(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.mem_force_pu = !enable;
    dev->sys_conf.mem_force_pd = enable;
}

static inline bool rmt_ll_is_mem_power_down(rmt_dev_t *dev)
{
    // the RTC domain can also power down RMT memory
    // so it's probably not enough to detect whether it's powered down or not
    // mem_force_pd has higher priority than mem_force_pu
    return (dev->sys_conf.mem_force_pd) || !(dev->sys_conf.mem_force_pu);
}

static inline void rmt_ll_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    if(channel < RMT_LL_TX_CHAN_NUM) {
        dev->tx_conf[channel].mem_size = block_num;
    } else {
        dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.mem_size = block_num;
    }
}

static inline uint32_t rmt_ll_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return (channel < RMT_LL_TX_CHAN_NUM) ? (dev->tx_conf[channel].mem_size) : (dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.mem_size);
}

static inline void rmt_ll_set_counter_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    if(channel < RMT_LL_TX_CHAN_NUM) {
        dev->tx_conf[channel].div_cnt = div;
    } else {
        dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.div_cnt = div;
    }
}

static inline uint32_t rmt_ll_get_counter_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    return (channel < RMT_LL_TX_CHAN_NUM) ? (dev->tx_conf[channel].div_cnt) : (dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.div_cnt);
}

static inline void rmt_ll_enable_tx_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].mem_tx_wrap_en = enable;
}

static inline void rmt_ll_enable_mem_access(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.fifo_mask = enable;
}

static inline void rmt_ll_set_rx_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.idle_thres = thres;
}

static inline uint32_t rmt_ll_get_rx_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.idle_thres;
}

static inline void rmt_ll_set_mem_owner(rmt_dev_t *dev, uint32_t channel, uint8_t owner)
{
    if (channel >= RMT_LL_TX_CHAN_NUM) {
        dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.mem_owner = owner;
    }
}

static inline uint32_t rmt_ll_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.mem_owner;
}

static inline void rmt_ll_enable_tx_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].tx_conti_mode = enable;
}

static inline bool rmt_ll_is_tx_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].tx_conti_mode;
}

static inline void rmt_ll_set_tx_loop_count(rmt_dev_t *dev, uint32_t channel, uint32_t count)
{
    dev->tx_lim[channel].tx_loop_num = count;
}

static inline void rmt_ll_reset_tx_loop(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_lim[channel].loop_count_reset = 1;
    dev->tx_lim[channel].loop_count_reset = 0;
}

static inline void rmt_ll_enable_tx_loop_count(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_lim[channel].tx_loop_cnt_en = enable;
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
    return dev->tx_sim.val & RMT_LL_TX_CHAN_MASK;
}

static inline void rmt_ll_enable_rx_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.rx_filter_en = enable;
}

static inline void rmt_ll_set_rx_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.rx_filter_thres = thres;
}

static inline void rmt_ll_set_counter_clock_src(rmt_dev_t *dev, uint32_t channel, uint8_t src)
{
    dev->sys_conf.sclk_sel = src;
    dev->sys_conf.sclk_div_num = 0;
    dev->sys_conf.sclk_div_a = 0;
    dev->sys_conf.sclk_div_b = 0;
}

static inline uint32_t rmt_ll_get_counter_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    return dev->sys_conf.sclk_sel;
}

static inline void rmt_ll_enable_tx_idle(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].idle_out_en = enable;
}

static inline bool rmt_ll_is_tx_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].idle_out_en;
}

static inline void rmt_ll_set_tx_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->tx_conf[channel].idle_out_lv = level;
}

static inline uint32_t rmt_ll_get_tx_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].idle_out_lv;
}

static inline uint32_t rmt_ll_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return (channel < RMT_LL_TX_CHAN_NUM) ? (dev->tx_status[channel].val) : (dev->rx_status[channel - RMT_LL_TX_CHAN_NUM].val);
}

static inline void rmt_ll_set_tx_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim[channel].limit = limit;
}

static inline void rmt_ll_set_rx_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->rx_lim[channel - RMT_LL_TX_CHAN_NUM].rx_lim = limit;
}

static inline uint32_t rmt_ll_get_rx_limit(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_lim[channel - RMT_LL_TX_CHAN_NUM].rx_lim;
}

static inline void rmt_ll_enable_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (BIT(0) << (channel));
    } else {
        dev->int_ena.val &= ~(BIT(0) << (channel));
    }
}

static inline void rmt_ll_enable_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (BIT(16) << (channel - RMT_LL_TX_CHAN_NUM));
    } else {
        dev->int_ena.val &= ~(BIT(16) << (channel - RMT_LL_TX_CHAN_NUM));
    }
}

static inline void rmt_ll_enable_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if(channel < RMT_LL_TX_CHAN_NUM) {
        if (enable) {
            dev->int_ena.val |= (BIT(4) << (channel));
        } else {
            dev->int_ena.val &= ~(BIT(4) << (channel));
        }
    } else {
        if (enable) {
            dev->int_ena.val |= (BIT(20) << (channel - RMT_LL_TX_CHAN_NUM));
        } else {
            dev->int_ena.val &= ~(BIT(20) << (channel - RMT_LL_TX_CHAN_NUM));
        }
    }
}

static inline void rmt_ll_enable_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (BIT(8) << (channel));
    } else {
        dev->int_ena.val &= ~(BIT(8) << (channel));
    }
}

static inline void rmt_ll_enable_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (BIT(12) << (channel));
    } else {
        dev->int_ena.val &= ~(BIT(12) << (channel));
    }
}

static inline void rmt_ll_enable_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (BIT(24) << (channel - RMT_LL_TX_CHAN_NUM));
    } else {
        dev->int_ena.val &= ~(BIT(24) << (channel - RMT_LL_TX_CHAN_NUM));
    }
}

static inline void rmt_ll_clear_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (BIT(0) << (channel));
}

static inline void rmt_ll_clear_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (BIT(16) << (channel - RMT_LL_TX_CHAN_NUM));
}

static inline void rmt_ll_clear_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    if(channel < RMT_LL_TX_CHAN_NUM) {
        dev->int_clr.val = (BIT(4) << (channel));
    } else {
        dev->int_clr.val = (BIT(20) << (channel - RMT_LL_TX_CHAN_NUM));
    }
}

static inline void rmt_ll_clear_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (BIT(8) << (channel));
}

static inline void rmt_ll_clear_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (BIT(12) << (channel));
}

static inline void rmt_ll_clear_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (BIT(24) << (channel -RMT_LL_TX_CHAN_NUM));
}

static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    return dev->int_st.val & RMT_LL_TX_CHAN_MASK;
}

static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 12) & RMT_LL_RX_CHAN_MASK;
}

static inline uint32_t rmt_ll_get_tx_err_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 4) & RMT_LL_TX_CHAN_MASK;
}

static inline uint32_t rmt_ll_get_rx_err_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 16) & RMT_LL_RX_CHAN_MASK;
}

static inline uint32_t rmt_ll_get_err_interrupt_status(rmt_dev_t *dev)
{
    return ((dev->int_st.val >> 4) & RMT_LL_TX_CHAN_MASK) | ((dev->int_st.val >> 16) & RMT_LL_RX_CHAN_MASK);
}

static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 8) & RMT_LL_TX_CHAN_MASK;
}

static inline uint32_t rmt_ll_get_rx_thres_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 20) & RMT_LL_RX_CHAN_MASK;
}

static inline uint32_t rmt_ll_get_tx_loop_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 12) & RMT_LL_TX_CHAN_MASK;
}

static inline void rmt_ll_set_tx_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    // In case the compiler optimise a 32bit instruction (e.g. s32i) into two 16bit instruction (e.g. s16i, which is not allowed to access a register)
    // We take care of the "read-modify-write" procedure by ourselves.
    typeof(dev->tx_carrier[0]) reg;
    reg.high = high_ticks;
    reg.low = low_ticks;
    dev->tx_carrier[channel].val = reg.val;
}

static inline void rmt_ll_set_rx_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    typeof(dev->rx_carrier[0]) reg;
    reg.high_thres = high_ticks;
    reg.low_thres = low_ticks;
    dev->rx_carrier[channel - RMT_LL_TX_CHAN_NUM].val = reg.val;
}

static inline void rmt_ll_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    if (channel < RMT_LL_TX_CHAN_NUM) {
        *high_ticks = dev->tx_carrier[channel].high;
        *low_ticks = dev->tx_carrier[channel].low;
    } else {
        *high_ticks = dev->rx_carrier[channel - RMT_LL_TX_CHAN_NUM].high_thres;
        *low_ticks = dev->rx_carrier[channel - RMT_LL_TX_CHAN_NUM].low_thres;
    }
}

// This function has different meaning for TX and RX
// TX: enable to modulate carrier
// RX: enable to demodulate carrier
static inline void rmt_ll_enable_carrier(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (channel < RMT_LL_TX_CHAN_NUM) {
        dev->tx_conf[channel].carrier_en = enable;
    } else {
        dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.carrier_en = enable;
    }
}

static inline void rmt_ll_set_carrier_on_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    if (channel < RMT_LL_TX_CHAN_NUM) {
        dev->tx_conf[channel].carrier_out_lv = level;
    } else {
        dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf0.carrier_out_lv = level;
    }
}

// set true, enable carrier in all RMT state (idle, reading, sending)
// set false, enable carrier only in sending state (i.e. there're effective data in RAM to be sent)
static inline void rmt_ll_tx_set_carrier_always_on(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].carrier_eff_en = !enable;
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
    dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.mem_rx_wrap_en = enable;
}

static inline void rmt_write_fifo(rmt_dev_t *dev, uint32_t channel, const rmt_item32_t *data, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        dev->data_ch[channel] = data[i].val;
    }
}

static inline void rmt_ll_config_update(rmt_dev_t *dev, uint32_t channel)
{
    if(channel < RMT_LL_TX_CHAN_NUM) {
        dev->tx_conf[channel].conf_update = 1;
    } else {
        dev->rx_conf[channel - RMT_LL_TX_CHAN_NUM].conf1.conf_update = 1;
    }
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
