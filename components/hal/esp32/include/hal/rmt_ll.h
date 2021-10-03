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

#include <stdbool.h>
#include <stddef.h>
#include "hal/misc.h"
#include "soc/rmt_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RMT_LL_HW_BASE  (&RMT)
#define RMT_LL_MEM_BASE (&RMTMEM)

// Note: TX and RX channel number are all index from zero in the LL driver
// i.e. tx_channel belongs to [0,7], and rx_channel belongs to [0,7]

static inline void rmt_ll_enable_drive_clock(rmt_dev_t *dev, bool enable)
{
    dev->conf_ch[0].conf0.clk_en = enable;
}

static inline void rmt_ll_power_down_mem(rmt_dev_t *dev, bool enable)
{
    dev->conf_ch[0].conf0.mem_pd = enable; // Only conf0 register of channel0 has `mem_pd`
}

static inline bool rmt_ll_is_mem_power_down(rmt_dev_t *dev)
{
    return dev->conf_ch[0].conf0.mem_pd; // Only conf0 register of channel0 has `mem_pd`
}

static inline void rmt_ll_enable_mem_access(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.fifo_mask = enable;
}

static inline void rmt_ll_set_group_clock_src(rmt_dev_t *dev, uint32_t channel, uint8_t src, uint8_t div_num, uint8_t div_a, uint8_t div_b)
{
    dev->conf_ch[channel].conf1.ref_always_on = src;
}

static inline uint32_t rmt_ll_get_group_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.ref_always_on;
}

static inline void rmt_ll_tx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.ref_cnt_rst = 1;
}

static inline void rmt_ll_rx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.ref_cnt_rst = 1;
}

static inline void rmt_ll_tx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_rd_rst = 1;
    dev->conf_ch[channel].conf1.mem_rd_rst = 0;
}

static inline void rmt_ll_rx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_wr_rst = 1;
    dev->conf_ch[channel].conf1.mem_wr_rst = 0;
}

static inline void rmt_ll_tx_start(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.tx_start = 1;
}

static inline void rmt_ll_tx_stop(rmt_dev_t *dev, uint32_t channel)
{
    RMTMEM.chan[channel].data32[0].val = 0;
    dev->conf_ch[channel].conf1.tx_start = 0;
    dev->conf_ch[channel].conf1.mem_rd_rst = 1;
    dev->conf_ch[channel].conf1.mem_rd_rst = 0;
}

static inline void rmt_ll_rx_enable(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_en = enable;
}

static inline void rmt_ll_tx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

static inline void rmt_ll_rx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

static inline uint32_t rmt_ll_tx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

static inline uint32_t rmt_ll_rx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

static inline void rmt_ll_tx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt, div);
}

static inline void rmt_ll_rx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt, div);
}

static inline uint32_t rmt_ll_tx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt);
    return div == 0 ? 256 : div;
}

static inline uint32_t rmt_ll_rx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt);
    return div == 0 ? 256 : div;
}

static inline void rmt_ll_tx_enable_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->apb_conf.mem_tx_wrap_en = enable;
}

static inline void rmt_ll_rx_set_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, idle_thres, thres);
}

static inline uint32_t rmt_ll_rx_get_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, idle_thres);
}

static inline void rmt_ll_rx_set_mem_owner(rmt_dev_t *dev, uint32_t channel, uint8_t owner)
{
    dev->conf_ch[channel].conf1.mem_owner = owner;
}

static inline uint32_t rmt_ll_rx_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.mem_owner;
}

static inline void rmt_ll_tx_enable_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.tx_conti_mode = enable;
}

static inline bool rmt_ll_is_tx_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.tx_conti_mode;
}

static inline void rmt_ll_tx_reset_loop(rmt_dev_t *dev, uint32_t channel)
{
    // RMT on esp32 doesn't support loop count, adding this only for HAL API consistency
}

static inline void rmt_ll_rx_enable_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_filter_en = enable;
}

static inline void rmt_ll_rx_set_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf1, rx_filter_thres, thres);
}

static inline void rmt_ll_tx_enable_idle(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.idle_out_en = enable;
}

static inline bool rmt_ll_is_tx_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_en;
}

static inline void rmt_ll_tx_set_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf1.idle_out_lv = level;
}

static inline uint32_t rmt_ll_tx_get_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_lv;
}

static inline uint32_t rmt_ll_rx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel];
}

static inline uint32_t rmt_ll_tx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel];
}

static inline void rmt_ll_tx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim_ch[channel].limit = limit;
}

static inline void rmt_ll_enable_interrupt(rmt_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena.val |= mask;
    } else {
        dev->int_ena.val &= ~mask;
    }
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

static inline void rmt_ll_enable_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 2));
    dev->int_ena.val |= (enable << (channel * 3 + 2));
}

static inline void rmt_ll_enable_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 2));
    dev->int_ena.val |= (enable << (channel * 3 + 2));
}

static inline void rmt_ll_enable_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel + 24));
    dev->int_ena.val |= (enable << (channel + 24));
}

static inline void rmt_ll_clear_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3));
}

static inline void rmt_ll_clear_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 1));
}

static inline void rmt_ll_clear_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 2));
}

static inline void rmt_ll_clear_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 2));
}

static inline void rmt_ll_clear_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 24));
}

static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x01) >> 0) | ((status & 0x08) >> 2) | ((status & 0x40) >> 4) | ((status & 0x200) >> 6) |
           ((status & 0x1000) >> 8) | ((status & 0x8000) >> 10) | ((status & 0x40000) >> 12) | ((status & 0x200000) >> 14);
}

static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x02) >> 1) | ((status & 0x10) >> 3) | ((status & 0x80) >> 5) | ((status & 0x400) >> 7) |
           ((status & 0x2000) >> 9) | ((status & 0x10000) >> 11) | ((status & 0x80000) >> 13) | ((status & 0x400000) >> 15);
}

static inline uint32_t rmt_ll_get_tx_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8) |
           ((status & 0x4000) >> 10) | ((status & 0x20000) >> 12) | ((status & 0x100000) >> 14) | ((status & 0x800000) >> 16);
}

static inline uint32_t rmt_ll_get_rx_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8) |
           ((status & 0x4000) >> 10) | ((status & 0x20000) >> 12) | ((status & 0x100000) >> 14) | ((status & 0x800000) >> 16);
}

static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xFF000000) >> 24;
}

static inline void rmt_ll_tx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->carrier_duty_ch[channel], high, high_ticks);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->carrier_duty_ch[channel], low, low_ticks);
}

static inline void rmt_ll_tx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->carrier_duty_ch[channel], high);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->carrier_duty_ch[channel], low);
}

static inline void rmt_ll_tx_enable_carrier_modulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_en = enable;
}

static inline void rmt_ll_tx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf0.carrier_out_lv = level;
}

//Writes items to the specified TX channel memory with the given offset and length.
//the caller should ensure that (length + off) <= (memory block * SOC_RMT_MEM_WORDS_PER_CHANNEL)
static inline void rmt_ll_write_memory(rmt_mem_t *mem, uint32_t channel, const void *data, size_t length_in_words, size_t off)
{
    volatile uint32_t *to = (volatile uint32_t *)&mem->chan[channel].data32[off];
    uint32_t *from = (uint32_t *)data;
    while (length_in_words--) {
        *to++ = *from++;
    }
}

#ifdef __cplusplus
}
#endif
