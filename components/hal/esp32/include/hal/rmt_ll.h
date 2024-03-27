/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @note TX and RX channels are index from 0 in the LL driver, i.e. tx_channel = [0,7], rx_channel = [0,7]
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/rmt_struct.h"
#include "soc/dport_reg.h"
#include "hal/rmt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RMT_LL_EVENT_TX_DONE(channel)     (1 << ((channel) * 3))
#define RMT_LL_EVENT_TX_THRES(channel)    (1 << ((channel) + 24))
#define RMT_LL_EVENT_TX_LOOP_END(channel) (0) // esp32 doesn't support tx loop count
#define RMT_LL_EVENT_TX_ERROR(channel)    (1 << ((channel) * 3 + 2))
#define RMT_LL_EVENT_RX_DONE(channel)     (1 << ((channel) * 3 + 1))
#define RMT_LL_EVENT_RX_THRES(channel)    (0) // esp32 doesn't support rx wrap
#define RMT_LL_EVENT_RX_ERROR(channel)    (1 << ((channel) * 3 + 2))
#define RMT_LL_EVENT_TX_MASK(channel)     (RMT_LL_EVENT_TX_DONE(channel) | RMT_LL_EVENT_TX_THRES(channel) | RMT_LL_EVENT_TX_LOOP_END(channel))
#define RMT_LL_EVENT_RX_MASK(channel)     (RMT_LL_EVENT_RX_DONE(channel) | RMT_LL_EVENT_RX_THRES(channel))

#define RMT_LL_MAX_FILTER_VALUE           255
#define RMT_LL_MAX_IDLE_VALUE             65535

typedef enum {
    RMT_LL_MEM_OWNER_SW = 0,
    RMT_LL_MEM_OWNER_HW = 1,
} rmt_ll_mem_owner_t;

/**
 * @brief Enable the bus clock for RMT module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void rmt_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    uint32_t reg_val = DPORT_READ_PERI_REG(DPORT_PERIP_CLK_EN_REG);
    reg_val &= ~DPORT_RMT_CLK_EN;
    reg_val |= enable << 9;
    DPORT_WRITE_PERI_REG(DPORT_PERIP_CLK_EN_REG, reg_val);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define rmt_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; rmt_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the RMT module
 *
 * @param group_id Group ID
 */
static inline void rmt_ll_reset_register(int group_id)
{
    (void)group_id;
    DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);
    DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, 0);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define rmt_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; rmt_ll_reset_register(__VA_ARGS__)

/**
 * @brief Enable clock gate for register and memory
 *
 * @param dev Peripheral instance address
 * @param enable True to enable, False to disable
 */
static inline void rmt_ll_enable_periph_clock(rmt_dev_t *dev, bool enable)
{
    dev->conf_ch[0].conf0.clk_en = enable; // register clock gating
}

/**
 * @brief Power down memory
 *
 * @param dev Peripheral instance address
 * @param enable True to power down, False to power up
 */
static inline void rmt_ll_power_down_mem(rmt_dev_t *dev, bool enable)
{
    dev->conf_ch[0].conf0.mem_pd = enable; // Only conf0 register of channel0 has `mem_pd`
}

/**
 * @brief Enable APB accessing RMT memory in nonfifo mode
 *
 * @param dev Peripheral instance address
 * @param enable True to enable, False to disable
 */
static inline void rmt_ll_enable_mem_access_nonfifo(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.fifo_mask = enable;
}

/**
 * @brief Set clock source and divider for RMT channel group
 *
 * @param dev Peripheral instance address
 * @param channel not used as clock source is set for all channels
 * @param src Clock source
 * @param divider_integral Integral part of the divider
 * @param divider_denominator Denominator part of the divider
 * @param divider_numerator Numerator part of the divider
 */
static inline void rmt_ll_set_group_clock_src(rmt_dev_t *dev, uint32_t channel, rmt_clock_source_t src,
        uint32_t divider_integral, uint32_t divider_denominator, uint32_t divider_numerator)
{
    (void)divider_integral;
    (void)divider_denominator;
    (void)divider_numerator;
    switch (src) {
    case RMT_CLK_SRC_APB:
        dev->conf_ch[channel].conf1.ref_always_on = 1;
        break;
    case RMT_CLK_SRC_REF_TICK:
        dev->conf_ch[channel].conf1.ref_always_on = 0;
        break;
    default:
        HAL_ASSERT(false && "unsupported RMT clock source");
        break;
    }
}

/**
 * @brief Enable RMT peripheral source clock
 *
 * @note RMT doesn't support enable/disable clock source, this function is only for compatibility
 *
 * @param dev Peripheral instance address
 * @param en True to enable, False to disable
 */
static inline void rmt_ll_enable_group_clock(rmt_dev_t *dev, bool en)
{
    (void)dev;
    (void)en;
}

////////////////////////////////////////TX Channel Specific/////////////////////////////////////////////////////////////

/**
 * @brief Reset clock divider for TX channels by mask
 *
 * @param dev Peripheral instance address
 * @param channel_mask Mask of TX channels
 */
static inline void rmt_ll_tx_reset_channels_clock_div(rmt_dev_t *dev, uint32_t channel_mask)
{
    for (int i = 0; i < 8; i++) {
        if (channel_mask & (1 << i)) {
            dev->conf_ch[i].conf1.ref_cnt_rst = 1;
        }
    }
}

/**
 * @brief Set TX channel clock divider
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param div Division value
 */
static inline void rmt_ll_tx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_ASSERT(div >= 1 && div <= 256 && "divider out of range");
    // limit the maximum divider to 256
    if (div >= 256) {
        div = 0; // 0 means 256 division
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt, div);
}

/**
 * @brief Reset RMT reading pointer for TX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 */
__attribute__((always_inline))
static inline void rmt_ll_tx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_rd_rst = 1;
    dev->conf_ch[channel].conf1.mem_rd_rst = 0;
    dev->conf_ch[channel].conf1.apb_mem_rst = 1;
    dev->conf_ch[channel].conf1.apb_mem_rst = 0;
}

/**
 * @brief Start transmitting for TX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 */
__attribute__((always_inline))
static inline void rmt_ll_tx_start(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.tx_start = 1;
}

/**
 * @brief Set memory block number for TX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param block_num memory block number
 */
static inline void rmt_ll_tx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

/**
 * @brief Enable TX wrap
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param enable True to enable, False to disable
 */
static inline void rmt_ll_tx_enable_wrap(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->apb_conf.mem_tx_wrap_en = enable;
}

/**
 * @brief Enable transmitting in a loop
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param enable True to enable, False to disable
 */
__attribute__((always_inline))
static inline void rmt_ll_tx_enable_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.tx_conti_mode = enable;
}

/**
 * @brief Fix the output level when TX channel is in IDLE state
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param level IDLE level (1 => high, 0 => low)
 * @param enable True to fix the IDLE level, otherwise the IDLE level is determined by EOF encoder
 */
__attribute__((always_inline))
static inline void rmt_ll_tx_fix_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level, bool enable)
{
    dev->conf_ch[channel].conf1.idle_out_en = enable;
    dev->conf_ch[channel].conf1.idle_out_lv = level;
}

/**
 * @brief Set the amount of RMT symbols that can trigger the limitation interrupt
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param limit Specify the number of symbols
 */
static inline void rmt_ll_tx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim_ch[channel].limit = limit;
}

/**
 * @brief Set high and low duration of carrier signal
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param high_ticks Duration of high level
 * @param low_ticks Duration of low level
 */
static inline void rmt_ll_tx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    HAL_ASSERT(high_ticks >= 1 && high_ticks <= 65536 && low_ticks >= 1 && low_ticks <= 65536 && "out of range high/low ticks");
    // ticks=0 means 65536 in hardware
    if (high_ticks >= 65536) {
        high_ticks = 0;
    }
    if (low_ticks >= 65536) {
        low_ticks = 0;
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->carrier_duty_ch[channel], high, high_ticks);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->carrier_duty_ch[channel], low, low_ticks);
}

/**
 * @brief Enable modulating carrier signal to TX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param enable True to enable, False to disable
 */
static inline void rmt_ll_tx_enable_carrier_modulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_en = enable;
}

/**
 * @brief Set on high or low to modulate the carrier signal
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @param level Which level to modulate on (0=>low level, 1=>high level)
 */
static inline void rmt_ll_tx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf0.carrier_out_lv = level;
}

////////////////////////////////////////RX Channel Specific/////////////////////////////////////////////////////////////

/**
 * @brief Reset clock divider for RX channels by mask
 *
 * @param dev Peripheral instance address
 * @param channel_mask Mask of RX channels
 */
static inline void rmt_ll_rx_reset_channels_clock_div(rmt_dev_t *dev, uint32_t channel_mask)
{
    for (int i = 0; i < 8; i++) {
        if (channel_mask & (1 << i)) {
            dev->conf_ch[i].conf1.ref_cnt_rst = 1;
        }
    }
}

/**
 * @brief Set RX channel clock divider
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @param div Division value
 */
static inline void rmt_ll_rx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_ASSERT(div >= 1 && div <= 256 && "divider out of range");
    // limit the maximum divider to 256
    if (div >= 256) {
        div = 0; // 0 means 256 division
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt, div);
}

/**
 * @brief Reset RMT writing pointer for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 */
__attribute__((always_inline))
static inline void rmt_ll_rx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_wr_rst = 1;
    dev->conf_ch[channel].conf1.mem_wr_rst = 0;
    dev->conf_ch[channel].conf1.apb_mem_rst = 1;
    dev->conf_ch[channel].conf1.apb_mem_rst = 0;
}

/**
 * @brief Enable receiving for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @param enable True to enable, False to disable
 */
__attribute__((always_inline))
static inline void rmt_ll_rx_enable(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_en = enable;
}

/**
 * @brief Set memory block number for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @param block_num memory block number
 */
static inline void rmt_ll_rx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

/**
 * @brief Set the time length for RX channel before going into IDLE state
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @param thres Time length threshold
 */
__attribute__((always_inline))
static inline void rmt_ll_rx_set_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, idle_thres, thres);
}

/**
 * @brief Set RMT memory owner for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @param owner Memory owner
 */
__attribute__((always_inline))
static inline void rmt_ll_rx_set_mem_owner(rmt_dev_t *dev, uint32_t channel, rmt_ll_mem_owner_t owner)
{
    dev->conf_ch[channel].conf1.mem_owner = owner;
}

/**
 * @brief Enable filter for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX chanenl number
 * @param enable True to enable, False to disable
 */
__attribute__((always_inline))
static inline void rmt_ll_rx_enable_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_filter_en = enable;
}

/**
 * @brief Set RX channel filter threshold (i.e. the maximum width of one pulse signal that would be treated as a noise)
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @param thres Filter threshold
 */
__attribute__((always_inline))
static inline void rmt_ll_rx_set_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf1, rx_filter_thres, thres);
}

/**
 * @brief Get RMT memory write cursor offset
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @return writer offset
 */
__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_memory_writer_offset(rmt_dev_t *dev, uint32_t channel)
{
    return (dev->status_ch[channel] & 0x3FF) - (channel) * 64;
}

//////////////////////////////////////////Interrupt Specific////////////////////////////////////////////////////////////

/**
 * @brief Enable RMT interrupt for specific event mask
 *
 * @param dev Peripheral instance address
 * @param mask Event mask
 * @param enable True to enable, False to disable
 */
__attribute__((always_inline))
static inline void rmt_ll_enable_interrupt(rmt_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena.val |= mask;
    } else {
        dev->int_ena.val &= ~mask;
    }
}

/**
 * @brief Clear RMT interrupt status by mask
 *
 * @param dev Peripheral instance address
 * @param mask Interupt status mask
 */
__attribute__((always_inline))
static inline void rmt_ll_clear_interrupt_status(rmt_dev_t *dev, uint32_t mask)
{
    dev->int_clr.val = mask;
}

/**
 * @brief Get interrupt status register address
 *
 * @param dev Peripheral instance address
 * @return Register address
 */
static inline volatile void *rmt_ll_get_interrupt_status_reg(rmt_dev_t *dev)
{
    return &dev->int_st;
}

/**
 * @brief Get interrupt status for TX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_interrupt_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->int_st.val & RMT_LL_EVENT_TX_MASK(channel);
}

/**
 * @brief Get interrupt raw status for TX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT TX channel number
 * @return Interrupt raw status
 */
static inline uint32_t rmt_ll_tx_get_interrupt_status_raw(rmt_dev_t *dev, uint32_t channel)
{
    return dev->int_raw.val & (RMT_LL_EVENT_TX_MASK(channel) | RMT_LL_EVENT_TX_ERROR(channel));
}

/**
 * @brief Get interrupt raw status for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @return Interrupt raw status
 */
__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_interrupt_status_raw(rmt_dev_t *dev, uint32_t channel)
{
    return dev->int_raw.val & (RMT_LL_EVENT_RX_MASK(channel) | RMT_LL_EVENT_RX_ERROR(channel));
}

/**
 * @brief Get interrupt status for RX channel
 *
 * @param dev Peripheral instance address
 * @param channel RMT RX channel number
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_interrupt_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->int_st.val & RMT_LL_EVENT_RX_MASK(channel);
}

//////////////////////////////////////////Deprecated Functions//////////////////////////////////////////////////////////
/////////////////////////////The following functions are only used by the legacy driver/////////////////////////////////
/////////////////////////////They might be removed in the next major release (ESP-IDF 6.0)//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_status_word(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel];
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_status_word(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel];
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt);
    return div == 0 ? 256 : div;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt);
    return div == 0 ? 256 : div;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, idle_thres);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

__attribute__((always_inline))
static inline bool rmt_ll_tx_is_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.tx_conti_mode;
}

__attribute__((always_inline))
static inline rmt_clock_source_t rmt_ll_get_group_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    if (dev->conf_ch[channel].conf1.ref_always_on) {
        return RMT_CLK_SRC_APB;
    }
    return RMT_CLK_SRC_REF_TICK;
}

__attribute__((always_inline))
static inline bool rmt_ll_tx_is_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_en;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_lv;
}

static inline bool rmt_ll_is_mem_powered_down(rmt_dev_t *dev)
{
    // Only conf0 register of channel0 has `mem_pd`
    return dev->conf_ch[0].conf0.mem_pd;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.mem_owner;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x01) >> 0) | ((status & 0x08) >> 2) | ((status & 0x40) >> 4) | ((status & 0x200) >> 6) |
           ((status & 0x1000) >> 8) | ((status & 0x8000) >> 10) | ((status & 0x40000) >> 12) | ((status & 0x200000) >> 14);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x02) >> 1) | ((status & 0x10) >> 3) | ((status & 0x80) >> 5) | ((status & 0x400) >> 7) |
           ((status & 0x2000) >> 9) | ((status & 0x10000) >> 11) | ((status & 0x80000) >> 13) | ((status & 0x400000) >> 15);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8) |
           ((status & 0x4000) >> 10) | ((status & 0x20000) >> 12) | ((status & 0x100000) >> 14) | ((status & 0x800000) >> 16);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8) |
           ((status & 0x4000) >> 10) | ((status & 0x20000) >> 12) | ((status & 0x100000) >> 14) | ((status & 0x800000) >> 16);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xFF000000) >> 24;
}

#ifdef __cplusplus
}
#endif
