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

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GDMA_LL_GET_HW(id) (((id) == 0) ? (&GDMA) : NULL)

#define GDMA_LL_EVENT_TX_L3_FIFO_UDF (1<<17)
#define GDMA_LL_EVENT_TX_L3_FIFO_OVF (1<<16)
#define GDMA_LL_EVENT_TX_L1_FIFO_UDF (1<<15)
#define GDMA_LL_EVENT_TX_L1_FIFO_OVF (1<<14)
#define GDMA_LL_EVENT_RX_L3_FIFO_UDF (1<<13)
#define GDMA_LL_EVENT_RX_L3_FIFO_OVF (1<<12)
#define GDMA_LL_EVENT_RX_L1_FIFO_UDF (1<<11)
#define GDMA_LL_EVENT_RX_L1_FIFO_OVF (1<<10)
#define GDMA_LL_EVENT_RX_WATER_MARK  (1<<9)
#define GDMA_LL_EVENT_TX_TOTAL_EOF   (1<<8)
#define GDMA_LL_EVENT_RX_DESC_EMPTY  (1<<7)
#define GDMA_LL_EVENT_TX_DESC_ERROR  (1<<6)
#define GDMA_LL_EVENT_RX_DESC_ERROR  (1<<5)
#define GDMA_LL_EVENT_TX_EOF         (1<<4)
#define GDMA_LL_EVENT_TX_DONE        (1<<3)
#define GDMA_LL_EVENT_RX_ERR_EOF     (1<<2)
#define GDMA_LL_EVENT_RX_SUC_EOF     (1<<1)
#define GDMA_LL_EVENT_RX_DONE        (1<<0)

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Enable DMA channel M2M mode (TX channel n forward data to RX channel n), disabled by default
 */
static inline void gdma_ll_enable_m2m_mode(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf0[channel].mem_trans_en = enable;
    if (enable) {
        // to enable m2m mode, the tx chan has to be the same to rx chan, and set to a valid value
        dev->peri_sel[channel].peri_in_sel = 0;
        dev->peri_sel[channel].peri_out_sel = 0;
    }
}

/**
 * @brief Get DMA interrupt status word
 */
static inline uint32_t gdma_ll_get_interrupt_status(gdma_dev_t *dev, uint32_t channel)
{
    return dev->int_st[channel].val;
}

/**
 * @brief Enable DMA interrupt
 */
static inline void gdma_ll_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena[channel].val |= mask;
    } else {
        dev->int_ena[channel].val &= ~mask;
    }
}

/**
 * @brief Clear DMA interrupt
 */
static inline void gdma_ll_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->int_clr[channel].val = mask;
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
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_rx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf1[channel].check_owner = enable;
}

/**
 * @brief Enable DMA RX channel burst reading data, disabled by default
 */
static inline void gdma_ll_rx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf0[channel].in_data_burst_en = enable;
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_rx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf0[channel].indscr_burst_en = enable;
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
static inline void gdma_ll_rx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    dev->conf0[channel].in_rst = 1;
    dev->conf0[channel].in_rst = 0;
}

/**
 * @brief Set DMA RX channel memory block size
 * @param size_index Supported value: GDMA_IN_EXT_MEM_BK_SIZE_16B, GDMA_IN_EXT_MEM_BK_SIZE_32B
 */
static inline void gdma_ll_rx_set_block_size_psram(gdma_dev_t *dev, uint32_t channel, uint32_t size_index)
{
    dev->conf1[channel].in_ext_mem_bk_size = size_index;
}

/**
 * @brief Set the water mark for RX channel, default value is 12
 */
static inline void gdma_ll_rx_set_water_mark(gdma_dev_t *dev, uint32_t channel, uint32_t water_mark)
{
    dev->conf1[channel].infifo_full_thrs = water_mark;
}

/**
 * @brief Check if DMA RX FIFO is full
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_rx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->infifo_status[channel].val & (1 << 2 * (fifo_level - 1));
}

/**
 * @brief Check if DMA RX FIFO is empty
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_rx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->infifo_status[channel].val & (1 << (2 * (fifo_level - 1) + 1));
}

/**
 * @brief Get number of bytes in RX FIFO (L1, L2, L3)
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline uint32_t gdma_ll_rx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    switch (fifo_level) {
    case 1:
        return dev->infifo_status[channel].infifo_cnt_l1;
    case 2:
        return dev->infifo_status[channel].infifo_cnt_l2;
    case 3:
        return dev->infifo_status[channel].infifo_cnt_l3;
    }
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t gdma_ll_rx_pop_data(gdma_dev_t *dev, uint32_t channel)
{
    dev->in_pop[channel].infifo_pop = 1;
    return dev->in_pop[channel].infifo_rdata;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
static inline void gdma_ll_rx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->in_link[channel].addr = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
static inline void gdma_ll_rx_start(gdma_dev_t *dev, uint32_t channel)
{
    dev->in_link[channel].start = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
static inline void gdma_ll_rx_stop(gdma_dev_t *dev, uint32_t channel)
{
    dev->in_link[channel].stop = 1;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
static inline void gdma_ll_rx_restart(gdma_dev_t *dev, uint32_t channel)
{
    dev->in_link[channel].restart = 1;
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void gdma_ll_rx_enable_auto_return(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in_link[channel].auto_ret = enable;
}

/**
 * @brief Check if DMA RX FSM is in IDLE state
 */
static inline bool gdma_ll_rx_is_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    return dev->in_link[channel].park;
}

/**
 * @brief Get RX success EOF descriptor's address
 */
static inline uint32_t gdma_ll_rx_get_success_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->in_suc_eof_des_addr[channel];
}

/**
 * @brief Get RX error EOF descriptor's address
 */
static inline uint32_t gdma_ll_rx_get_error_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->in_err_eof_des_addr[channel];
}

/**
 * @brief Get current RX descriptor's address
 */
static inline uint32_t gdma_ll_rx_get_current_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->in_dscr[channel];
}

/**
 * @brief Set weight for DMA RX channel
 */
static inline void gdma_ll_rx_set_weight(gdma_dev_t *dev, uint32_t channel, uint32_t weight)
{
    dev->wight[channel].rx_weight = weight;
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void gdma_ll_rx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->pri[channel].rx_pri = prio;
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void gdma_ll_rx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, int periph_id)
{
    dev->peri_sel[channel].peri_in_sel = periph_id;
}

/**
 * @brief Extend the L2 FIFO size for RX channel
 * @note By default, the L2 FIFO size is SOC_GDMA_L2_FIFO_BASE_SIZE Bytes. Suggest to extend it to twice the block size when accessing PSRAM.
 * @note `size_in_bytes` should aligned to 8 and larger than SOC_GDMA_L2_FIFO_BASE_SIZE
 */
static inline void gdma_ll_rx_extend_l2_fifo_size_to(gdma_dev_t *dev, uint32_t channel, uint32_t size_in_bytes)
{
    if (size_in_bytes > SOC_GDMA_L2_FIFO_BASE_SIZE) {
        dev->sram_size[channel].in_size = (size_in_bytes - SOC_GDMA_L2_FIFO_BASE_SIZE) / 8;
    }
}


///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_tx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf1[channel].check_owner = enable;
}

/**
 * @brief Enable DMA TX channel burst sending data, disabled by default
 */
static inline void gdma_ll_tx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf0[channel].out_data_burst_en = enable;
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_tx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf0[channel].outdscr_burst_en = enable;
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void gdma_ll_tx_set_eof_mode(gdma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    dev->conf0[channel].out_eof_mode = mode;
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void gdma_ll_tx_enable_auto_write_back(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf0[channel].out_auto_wrback = enable;
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
static inline void gdma_ll_tx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    dev->conf0[channel].out_rst = 1;
    dev->conf0[channel].out_rst = 0;
}

/**
 * @brief Set DMA TX channel memory block size
 * @param size_index Supported value: GDMA_OUT_EXT_MEM_BK_SIZE_16B, GDMA_OUT_EXT_MEM_BK_SIZE_32B
 */
static inline void gdma_ll_tx_set_block_size_psram(gdma_dev_t *dev, uint32_t channel, uint32_t size_index)
{
    dev->conf1[channel].out_ext_mem_bk_size = size_index;
}

/**
 * @brief Check if DMA TX FIFO is full
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_tx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->outfifo_status[channel].val & (1 << 2 * (fifo_level - 1));
}

/**
 * @brief Check if DMA TX FIFO is empty
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline bool gdma_ll_tx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->outfifo_status[channel].val & (1 << (2 * (fifo_level - 1) + 1));
}

/**
 * @brief Get number of bytes in TX FIFO (L1, L2, L3)
 * @param fifo_level (1,2,3) <=> (L1, L2, L3)
 */
static inline uint32_t gdma_ll_tx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    switch (fifo_level) {
    case 1:
        return dev->outfifo_status[channel].outfifo_cnt_l1;
    case 2:
        return dev->outfifo_status[channel].outfifo_cnt_l2;
    case 3:
        return dev->outfifo_status[channel].outfifo_cnt_l3;
    }
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void gdma_ll_tx_push_data(gdma_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->out_push[channel].outfifo_wdata = data;
    dev->out_push[channel].outfifo_push = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
static inline void gdma_ll_tx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->out_link[channel].addr = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
static inline void gdma_ll_tx_start(gdma_dev_t *dev, uint32_t channel)
{
    dev->out_link[channel].start = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
static inline void gdma_ll_tx_stop(gdma_dev_t *dev, uint32_t channel)
{
    dev->out_link[channel].stop = 1;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
static inline void gdma_ll_tx_restart(gdma_dev_t *dev, uint32_t channel)
{
    dev->out_link[channel].restart = 1;
}

/**
 * @brief Check if DMA TX FSM is in IDLE state
 */
static inline bool gdma_ll_tx_is_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    return dev->out_link[channel].park;
}

/**
 * @brief Get TX EOF descriptor's address
 */
static inline uint32_t gdma_ll_tx_get_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->out_eof_des_addr[channel];
}

/**
 * @brief Get current TX descriptor's address
 */
static inline uint32_t gdma_ll_tx_get_current_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->out_dscr[channel];
}

/**
 * @brief Set weight for DMA TX channel
 */
static inline void gdma_ll_tx_set_weight(gdma_dev_t *dev, uint32_t channel, uint32_t weight)
{
    dev->wight[channel].tx_weight = weight;
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void gdma_ll_tx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->pri[channel].tx_pri = prio;
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void gdma_ll_tx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, int periph_id)
{
    dev->peri_sel[channel].peri_out_sel = periph_id;
}

/**
 * @brief Extend the L2 FIFO size for TX channel
 * @note By default, the L2 FIFO size is SOC_GDMA_L2_FIFO_BASE_SIZE Bytes. Suggest to extend it to twice the block size when accessing PSRAM.
 * @note `size_in_bytes` should aligned to 8 and larger than SOC_GDMA_L2_FIFO_BASE_SIZE
 */
static inline void gdma_ll_tx_extend_fifo_size_to(gdma_dev_t *dev, uint32_t channel, uint32_t size_in_bytes)
{
    if (size_in_bytes > SOC_GDMA_L2_FIFO_BASE_SIZE) {
        dev->sram_size[channel].out_size =  (size_in_bytes - SOC_GDMA_L2_FIFO_BASE_SIZE) / 8;
    }
}

#ifdef __cplusplus
}
#endif
