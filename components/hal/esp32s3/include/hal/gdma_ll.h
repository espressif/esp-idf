/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GDMA_LL_GET_HW(id) (((id) == 0) ? (&GDMA) : NULL)

#define GDMA_LL_RX_EVENT_MASK        (0x3FF)
#define GDMA_LL_TX_EVENT_MASK        (0xFF)

#define GDMA_LL_EVENT_TX_L3_FIFO_UDF (1<<7)
#define GDMA_LL_EVENT_TX_L3_FIFO_OVF (1<<6)
#define GDMA_LL_EVENT_TX_L1_FIFO_UDF (1<<5)
#define GDMA_LL_EVENT_TX_L1_FIFO_OVF (1<<4)
#define GDMA_LL_EVENT_TX_TOTAL_EOF   (1<<3)
#define GDMA_LL_EVENT_TX_DESC_ERROR  (1<<2)
#define GDMA_LL_EVENT_TX_EOF         (1<<1)
#define GDMA_LL_EVENT_TX_DONE        (1<<0)

#define GDMA_LL_EVENT_RX_L3_FIFO_UDF (1<<9)
#define GDMA_LL_EVENT_RX_L3_FIFO_OVF (1<<8)
#define GDMA_LL_EVENT_RX_L1_FIFO_UDF (1<<7)
#define GDMA_LL_EVENT_RX_L1_FIFO_OVF (1<<6)
#define GDMA_LL_EVENT_RX_WATER_MARK  (1<<5)
#define GDMA_LL_EVENT_RX_DESC_EMPTY  (1<<4)
#define GDMA_LL_EVENT_RX_DESC_ERROR  (1<<3)
#define GDMA_LL_EVENT_RX_ERR_EOF     (1<<2)
#define GDMA_LL_EVENT_RX_SUC_EOF     (1<<1)
#define GDMA_LL_EVENT_RX_DONE        (1<<0)

#define GDMA_LL_L2FIFO_BASE_SIZE (16) // Basic size of GDMA Level 2 FIFO

/* Memory block size value supported by channel */
#define GDMA_LL_EXT_MEM_BK_SIZE_16B (0)
#define GDMA_LL_EXT_MEM_BK_SIZE_32B (1)
#define GDMA_LL_EXT_MEM_BK_SIZE_64B (2)

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Enable DMA channel M2M mode (TX channel n forward data to RX channel n), disabled by default
 */
static inline void gdma_ll_enable_m2m_mode(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.conf0.mem_trans_en = enable;
    if (enable) {
        // to enable m2m mode, the tx chan has to be the same to rx chan, and set to a valid value
        dev->channel[channel].in.peri_sel.sel = 0;
        dev->channel[channel].out.peri_sel.sel = 0;
    }
}

/**
 * @brief Enable DMA clock gating
 */
static inline void gdma_ll_enable_clock(gdma_dev_t *dev, bool enable)
{
    dev->misc_conf.clk_en = enable;
}

///////////////////////////////////// RX /////////////////////////////////////////
/**
 * @brief Get DMA RX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.int_st.val;
}

/**
 * @brief Enable DMA RX channel interrupt
 */
static inline void gdma_ll_rx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->channel[channel].in.int_ena.val |= mask;
    } else {
        dev->channel[channel].in.int_ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->channel[channel].in.int_clr.val = mask;
}

/**
 * @brief Get DMA RX channel interrupt status register address
 */
static inline volatile void *gdma_ll_rx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->channel[channel].in.int_st);
}

/**
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_rx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.conf1.in_check_owner = enable;
}

/**
 * @brief Enable DMA RX channel burst reading data, disabled by default
 */
static inline void gdma_ll_rx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.conf0.in_data_burst_en = enable;
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_rx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.conf0.indscr_burst_en = enable;
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.conf0.in_rst = 1;
    dev->channel[channel].in.conf0.in_rst = 0;
}

/**
 * @brief Set DMA RX channel memory block size
 * @param size_index Supported value: GDMA_LL_EXT_MEM_BK_SIZE_16B/32B/64B
 */
static inline void gdma_ll_rx_set_block_size_psram(gdma_dev_t *dev, uint32_t channel, uint32_t size_index)
{
    dev->channel[channel].in.conf1.in_ext_mem_bk_size = size_index;
}

/**
 * @brief Set the water mark for RX channel, default value is 12
 */
static inline void gdma_ll_rx_set_water_mark(gdma_dev_t *dev, uint32_t channel, uint32_t water_mark)
{
    dev->channel[channel].in.conf1.dma_infifo_full_thrs = water_mark;
}

/**
 * @brief Check if DMA RX FIFO is full
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_rx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.val & (1 << 2 * (fifo_level - 1));
}

/**
 * @brief Check if DMA RX FIFO is empty
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_rx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.val & (1 << (2 * (fifo_level - 1) + 1));
}

/**
 * @brief Get number of bytes in RX FIFO (L1, L2, L3)
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline uint32_t gdma_ll_rx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    switch (fifo_level) {
    case 1:
        return dev->channel[channel].in.infifo_status.infifo_cnt_l1;
    case 2:
        return dev->channel[channel].in.infifo_status.infifo_cnt_l2;
    case 3:
        return dev->channel[channel].in.infifo_status.infifo_cnt_l3;
    }
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t gdma_ll_rx_pop_data(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.pop.infifo_pop = 1;
    return dev->channel[channel].in.pop.infifo_rdata;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->channel[channel].in.link.addr = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_start(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.link.start = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_stop(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.link.stop = 1;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_restart(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.link.restart = 1;
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void gdma_ll_rx_enable_auto_return(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.link.auto_ret = enable;
}

/**
 * @brief Check if DMA RX FSM is in IDLE state
 */
static inline bool gdma_ll_rx_is_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.link.park;
}

/**
 * @brief Get RX success EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_success_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.suc_eof_des_addr;
}

/**
 * @brief Get RX error EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_error_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.err_eof_des_addr;
}

/**
 * @brief Get current RX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_current_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.dscr;
}

/**
 * @brief Set weight for DMA RX channel
 */
static inline void gdma_ll_rx_set_weight(gdma_dev_t *dev, uint32_t channel, uint32_t weight)
{
    dev->channel[channel].in.wight.rx_weight = weight;
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void gdma_ll_rx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->channel[channel].in.pri.rx_pri = prio;
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void gdma_ll_rx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, int periph_id)
{
    dev->channel[channel].in.peri_sel.sel = periph_id;
}

///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Get DMA TX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.int_st.val;
}

/**
 * @brief Enable DMA TX channel interrupt
 */
static inline void gdma_ll_tx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->channel[channel].out.int_ena.val |= mask;
    } else {
        dev->channel[channel].out.int_ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->channel[channel].out.int_clr.val = mask;
}

/**
 * @brief Get DMA TX channel interrupt status register address
 */
static inline volatile void *gdma_ll_tx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->channel[channel].out.int_st);
}

/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_tx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.conf1.out_check_owner = enable;
}

/**
 * @brief Enable DMA TX channel burst sending data, disabled by default
 */
static inline void gdma_ll_tx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.conf0.out_data_burst_en = enable;
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_tx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.conf0.outdscr_burst_en = enable;
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void gdma_ll_tx_set_eof_mode(gdma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    dev->channel[channel].out.conf0.out_eof_mode = mode;
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void gdma_ll_tx_enable_auto_write_back(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.conf0.out_auto_wrback = enable;
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.conf0.out_rst = 1;
    dev->channel[channel].out.conf0.out_rst = 0;
}

/**
 * @brief Set DMA TX channel memory block size
 * @param size_index Supported value: GDMA_LL_EXT_MEM_BK_SIZE_16B/32B/64B
 */
static inline void gdma_ll_tx_set_block_size_psram(gdma_dev_t *dev, uint32_t channel, uint32_t size_index)
{
    dev->channel[channel].out.conf1.out_ext_mem_bk_size = size_index;
}

/**
 * @brief Check if DMA TX FIFO is full
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_tx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.val & (1 << 2 * (fifo_level - 1));
}

/**
 * @brief Check if DMA TX FIFO is empty
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_tx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.val & (1 << (2 * (fifo_level - 1) + 1));
}

/**
 * @brief Get number of bytes in TX FIFO (L1, L2, L3)
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline uint32_t gdma_ll_tx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    switch (fifo_level) {
    case 1:
        return dev->channel[channel].out.outfifo_status.outfifo_cnt_l1;
    case 2:
        return dev->channel[channel].out.outfifo_status.outfifo_cnt_l2;
    case 3:
        return dev->channel[channel].out.outfifo_status.outfifo_cnt_l3;
    }
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void gdma_ll_tx_push_data(gdma_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->channel[channel].out.push.outfifo_wdata = data;
    dev->channel[channel].out.push.outfifo_push = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->channel[channel].out.link.addr = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_start(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.link.start = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_stop(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.link.stop = 1;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_restart(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.link.restart = 1;
}

/**
 * @brief Check if DMA TX FSM is in IDLE state
 */
static inline bool gdma_ll_tx_is_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.link.park;
}

/**
 * @brief Get TX EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.eof_des_addr;
}

/**
 * @brief Get current TX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_current_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.dscr;
}

/**
 * @brief Set weight for DMA TX channel
 */
static inline void gdma_ll_tx_set_weight(gdma_dev_t *dev, uint32_t channel, uint32_t weight)
{
    dev->channel[channel].out.wight.tx_weight = weight;
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void gdma_ll_tx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->channel[channel].out.pri.tx_pri = prio;
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void gdma_ll_tx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, int periph_id)
{
    dev->channel[channel].out.peri_sel.sel = periph_id;
}

#ifdef __cplusplus
}
#endif
