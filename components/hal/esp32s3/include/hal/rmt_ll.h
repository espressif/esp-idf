/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "soc/rmt_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RMT_LL_MAX_LOOP_COUNT           (1023)/*!< Max loop count that hardware is supported */
#define RMT_LL_HW_BASE  (&RMT)
#define RMT_LL_MEM_BASE (&RMTMEM)

// Note: TX and RX channel number are all index from zero in the LL driver
// i.e. tx_channel belongs to [0,3], and rx_channel belongs to [0,3]

static inline void rmt_ll_enable_drive_clock(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.clk_en = enable; // register clock gating
    dev->sys_conf.mem_clk_force_on = enable; // memory clock gating
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

static inline void rmt_ll_enable_mem_access(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.apb_fifo_mask = enable;
}

static inline void rmt_ll_set_group_clock_src(rmt_dev_t *dev, uint32_t channel, uint8_t src, uint8_t div_num, uint8_t div_a, uint8_t div_b)
{
    // Formula: rmt_sclk = module_clock_src / (1 + div_num + div_a / div_b)
    dev->sys_conf.sclk_active = 0;
    dev->sys_conf.sclk_sel = src;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->sys_conf, sclk_div_num, div_num);
    dev->sys_conf.sclk_div_a = div_a;
    dev->sys_conf.sclk_div_b = div_b;
    dev->sys_conf.sclk_active = 1;
}

static inline uint32_t rmt_ll_get_group_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    return dev->sys_conf.sclk_sel;
}

static inline void rmt_ll_tx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << channel);
}

static inline void rmt_ll_tx_reset_channels_clock_div(rmt_dev_t *dev, uint32_t channel_mask)
{
    dev->ref_cnt_rst.val |= channel_mask;
}

static inline void rmt_ll_rx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << (channel + 4));
}

static inline void rmt_ll_tx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->chnconf0[channel].mem_rd_rst_n = 1;
    dev->chnconf0[channel].mem_rd_rst_n = 0;
    dev->chnconf0[channel].apb_mem_rst_n = 1;
    dev->chnconf0[channel].apb_mem_rst_n = 0;
}

static inline void rmt_ll_rx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->chmconf[channel].conf1.mem_wr_rst_m = 1;
    dev->chmconf[channel].conf1.mem_wr_rst_m = 0;
    dev->chmconf[channel].conf1.apb_mem_rst_m = 1;
    dev->chmconf[channel].conf1.apb_mem_rst_m = 0;
}

static inline void rmt_ll_tx_start(rmt_dev_t *dev, uint32_t channel)
{
    dev->chnconf0[channel].conf_update_n = 1;
    dev->chnconf0[channel].tx_start_n = 1;
}

static inline void rmt_ll_tx_stop(rmt_dev_t *dev, uint32_t channel)
{
    dev->chnconf0[channel].tx_stop_n = 1;
    dev->chnconf0[channel].conf_update_n = 1;
}

static inline void rmt_ll_rx_enable(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chmconf[channel].conf1.rx_en_m = enable;
    dev->chmconf[channel].conf1.conf_update_m = 1;
}

static inline void rmt_ll_tx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->chnconf0[channel].mem_size_n = block_num;
}

static inline void rmt_ll_rx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->chmconf[channel].conf0.mem_size_m = block_num;
}

static inline uint32_t rmt_ll_tx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chnconf0[channel].mem_size_n;
}

static inline uint32_t rmt_ll_rx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chmconf[channel].conf0.mem_size_m;
}

static inline void rmt_ll_tx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->chnconf0[channel], div_cnt_n, div);
}

static inline void rmt_ll_rx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->chmconf[channel].conf0, div_cnt_m, div);
}

static inline uint32_t rmt_ll_tx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->chnconf0[channel], div_cnt_n);
}

static inline uint32_t rmt_ll_rx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->chmconf[channel].conf0, div_cnt_m);
}

static inline void rmt_ll_tx_enable_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chnconf0[channel].mem_tx_wrap_en_n = enable;
}

static inline void rmt_ll_rx_set_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    dev->chmconf[channel].conf0.idle_thres_m = thres;
}

static inline uint32_t rmt_ll_rx_get_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chmconf[channel].conf0.idle_thres_m;
}

static inline void rmt_ll_rx_set_mem_owner(rmt_dev_t *dev, uint32_t channel, uint8_t owner)
{
    dev->chmconf[channel].conf1.mem_owner_m = owner;
}

static inline uint32_t rmt_ll_rx_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chmconf[channel].conf1.mem_owner_m;
}

static inline void rmt_ll_tx_enable_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chnconf0[channel].tx_conti_mode_n = enable;
}

static inline bool rmt_ll_is_tx_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chnconf0[channel].tx_conti_mode_n;
}

static inline void rmt_ll_tx_enable_loop_autostop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chn_tx_lim[channel].loop_stop_en_chn = enable;
}

static inline void rmt_ll_tx_set_loop_count(rmt_dev_t *dev, uint32_t channel, uint32_t count)
{
    dev->chn_tx_lim[channel].tx_loop_num_chn = count;
}

static inline void rmt_ll_tx_reset_loop(rmt_dev_t *dev, uint32_t channel)
{
    dev->chn_tx_lim[channel].loop_count_reset_chn = 1;
    dev->chn_tx_lim[channel].loop_count_reset_chn = 0;
}

static inline void rmt_ll_tx_enable_loop_count(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chn_tx_lim[channel].tx_loop_cnt_en_chn = enable;
}

static inline void rmt_ll_tx_enable_sync(rmt_dev_t *dev, bool enable)
{
    dev->tx_sim.tx_sim_en = enable;
}

static inline void rmt_ll_tx_add_to_sync_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val |= 1 << channel;
}

static inline void rmt_ll_tx_remove_from_sync_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val &= ~(1 << channel);
}

static inline void rmt_ll_rx_enable_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chmconf[channel].conf1.rx_filter_en_m = enable;
}

static inline void rmt_ll_rx_set_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->chmconf[channel].conf1, rx_filter_thres_m, thres);
}

static inline void rmt_ll_tx_enable_idle(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chnconf0[channel].idle_out_en_n = enable;
}

static inline bool rmt_ll_is_tx_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chnconf0[channel].idle_out_en_n;
}

static inline void rmt_ll_tx_set_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->chnconf0[channel].idle_out_lv_n = level;
}

static inline uint32_t rmt_ll_tx_get_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chnconf0[channel].idle_out_lv_n;
}

static inline uint32_t rmt_ll_rx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chmstatus[channel].val;
}

static inline uint32_t rmt_ll_tx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chnstatus[channel].val;
}

static inline void rmt_ll_tx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->chn_tx_lim[channel].tx_lim_chn = limit;
}

static inline void rmt_ll_rx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->chm_rx_lim[channel].chm_rx_lim_reg = limit;
}

static inline uint32_t rmt_ll_rx_get_limit(rmt_dev_t *dev, uint32_t channel)
{
    return dev->chm_rx_lim[channel].chm_rx_lim_reg;
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
    if (enable) {
        dev->int_ena.val |= (1 << channel);
    } else {
        dev->int_ena.val &= ~(1 << channel);
    }
}

static inline void rmt_ll_enable_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 4));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 4));
    }
}

static inline void rmt_ll_enable_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 16));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 16));
    }
}

static inline void rmt_ll_enable_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 20));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 20));
    }
}

static inline void rmt_ll_enable_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 8));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 8));
    }
}

static inline void rmt_ll_enable_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 12));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 12));
    }
}

static inline void rmt_ll_enable_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 24));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 24));
    }
}

static inline void rmt_ll_clear_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel));
}

static inline void rmt_ll_clear_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 16));
}

static inline void rmt_ll_clear_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 4));
}

static inline void rmt_ll_clear_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 20));
}

static inline void rmt_ll_clear_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 8));
}

static inline void rmt_ll_clear_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 12));
}

static inline void rmt_ll_clear_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 24));
}

static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    return dev->int_st.val & 0x0F;
}

static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 16) & 0x0F;
}

static inline uint32_t rmt_ll_get_tx_err_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 4) & 0x0F;
}

static inline uint32_t rmt_ll_get_rx_err_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 20) & 0x0F;
}

static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 8) & 0x0F;
}

static inline uint32_t rmt_ll_get_rx_thres_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 24) & 0x0F;
}

static inline uint32_t rmt_ll_get_tx_loop_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 12) & 0x0F;
}

static inline void rmt_ll_tx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    // In case the compiler optimise a 32bit instruction (e.g. s32i) into two 16bit instruction (e.g. s16i, which is not allowed to access a register)
    // We take care of the "read-modify-write" procedure by ourselves.
    rmt_chncarrier_duty_reg_t reg;
    reg.carrier_high_chn = high_ticks;
    reg.carrier_low_chn = low_ticks;
    dev->chncarrier_duty[channel].val = reg.val;
}

static inline void rmt_ll_rx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    rmt_chm_rx_carrier_rm_reg_t reg;
    reg.carrier_high_thres_chm = high_ticks;
    reg.carrier_low_thres_chm = low_ticks;
    dev->chm_rx_carrier_rm[channel].val = reg.val;
}

static inline void rmt_ll_tx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks )
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->chncarrier_duty[channel], carrier_high_chn);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->chncarrier_duty[channel], carrier_low_chn);
}

static inline void rmt_ll_rx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->chm_rx_carrier_rm[channel], carrier_high_thres_chm);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->chm_rx_carrier_rm[channel], carrier_low_thres_chm);
}

static inline void rmt_ll_tx_enable_carrier_modulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chnconf0[channel].carrier_en_n = enable;
}

static inline void rmt_ll_rx_enable_carrier_demodulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chmconf[channel].conf0.carrier_en_m = enable;
}

static inline void rmt_ll_tx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->chnconf0[channel].carrier_out_lv_n = level;
}

static inline void rmt_ll_rx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->chmconf[channel].conf0.carrier_out_lv_m = level;
}

// set true, enable carrier in all RMT state (idle, reading, sending)
// set false, enable carrier only in sending state (i.e. there're effective data in RAM to be sent)
static inline void rmt_ll_tx_set_carrier_always_on(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chnconf0[channel].carrier_eff_en_n = !enable;
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

static inline void rmt_ll_rx_enable_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->chmconf[channel].conf1.mem_rx_wrap_en_m = enable;
}

#ifdef __cplusplus
}
#endif
