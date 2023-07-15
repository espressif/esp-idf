/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal/gdma_types.h"
#include "hal/gdma_ll.h"
#include "soc/ahb_dma_struct.h"
#include "soc/ahb_dma_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AHB_DMA_LL_GET_HW(id) (((id) == 0) ? (&AHB_DMA) : NULL)

// any "dummy" peripheral ID can be used for M2M mode
#define AHB_DMA_LL_M2M_FREE_PERIPH_ID_MASK (0xFAC2)
#define AHB_DMA_LL_INVALID_PERIPH_ID       (0x3F)

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Force enable register clock
 */
static inline void ahb_dma_ll_force_enable_reg_clock(ahb_dma_dev_t *dev, bool enable)
{
    dev->misc_conf.clk_en = enable;
}

/**
 * @brief Disable priority arbitration
 *
 * @param dev DMA register base address
 * @param dis True to disable, false to enable
 */
static inline void ahb_dma_ll_disable_prio_arb(ahb_dma_dev_t *dev, bool dis)
{
    dev->misc_conf.arb_pri_dis = dis;
}

/**
 * @brief Reset DMA FSM
 *
 * @param dev DMA register base address
 */
static inline void ahb_dma_ll_reset_fsm(ahb_dma_dev_t *dev)
{
    dev->misc_conf.ahbm_rst_inter = 1;
    dev->misc_conf.ahbm_rst_inter = 0;
}

///////////////////////////////////// RX /////////////////////////////////////////
/**
 * @brief Get DMA RX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_rx_get_interrupt_status(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->in_intr[channel].st.val;
}

/**
 * @brief Enable DMA RX channel interrupt
 */
static inline void ahb_dma_ll_rx_enable_interrupt(ahb_dma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->in_intr[channel].ena.val |= mask;
    } else {
        dev->in_intr[channel].ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_rx_clear_interrupt_status(ahb_dma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->in_intr[channel].clr.val = mask;
}

/**
 * @brief Get DMA RX channel interrupt status register address
 */
static inline volatile void *ahb_dma_ll_rx_get_interrupt_status_reg(ahb_dma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->in_intr[channel].st);
}

/**
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void ahb_dma_ll_rx_enable_owner_check(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf1.in_check_owner_chn = enable;
}

/**
 * @brief Enable DMA RX channel burst reading data, disabled by default
 */
static inline void ahb_dma_ll_rx_enable_data_burst(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf0.in_data_burst_en_chn = enable;
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void ahb_dma_ll_rx_enable_descriptor_burst(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf0.indscr_burst_en_chn = enable;
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_rx_reset_channel(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_conf0.in_rst_chn = 1;
    dev->channel[channel].in.in_conf0.in_rst_chn = 0;
}

/**
 * @brief Check if DMA RX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool ahb_dma_ll_rx_is_fifo_full(ahb_dma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.val & 0x01;
}

/**
 * @brief Check if DMA RX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool ahb_dma_ll_rx_is_fifo_empty(ahb_dma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.val & 0x02;
}

/**
 * @brief Get number of bytes remained in the L1 RX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t ahb_dma_ll_rx_get_fifo_bytes(ahb_dma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.infifo_cnt_chn;
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t ahb_dma_ll_rx_pop_data(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_pop.infifo_pop_chn = 1;
    return dev->channel[channel].in.in_pop.infifo_rdata_chn;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_rx_set_desc_addr(ahb_dma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->in_link_addr[channel].inlink_addr_chn = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_rx_start(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_link.inlink_start_chn = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_rx_stop(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_link.inlink_stop_chn = 1;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_rx_restart(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_link.inlink_restart_chn = 1;
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void ahb_dma_ll_rx_enable_auto_return(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_link.inlink_auto_ret_chn = enable;
}

/**
 * @brief Check if DMA RX FSM is in IDLE state
 */
static inline bool ahb_dma_ll_rx_is_fsm_idle(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_link.inlink_park_chn;
}

/**
 * @brief Get RX success EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_rx_get_success_eof_desc_addr(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_suc_eof_des_addr.val;
}

/**
 * @brief Get RX error EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_rx_get_error_eof_desc_addr(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_err_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched RX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_rx_get_prefetched_desc_addr(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_dscr.val;
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void ahb_dma_ll_rx_set_priority(ahb_dma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->channel[channel].in.in_pri.rx_pri_chn = prio;
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void ahb_dma_ll_rx_connect_to_periph(ahb_dma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    dev->channel[channel].in.in_peri_sel.peri_in_sel_chn = periph_id;
    dev->channel[channel].in.in_conf0.mem_trans_en_chn = (periph == GDMA_TRIG_PERIPH_M2M);
}

/**
 * @brief Disconnect DMA RX channel from peripheral
 */
static inline void ahb_dma_ll_rx_disconnect_from_periph(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_peri_sel.peri_in_sel_chn = GDMA_LL_INVALID_PERIPH_ID;
    dev->channel[channel].in.in_conf0.mem_trans_en_chn = false;
}

/**
 * @brief Whether to enable the ETM subsystem for RX channel
 *
 * @note When ETM_EN is 1, only ETM tasks can be used to configure the transfer direction and enable the channel.
 */
static inline void ahb_dma_ll_rx_enable_etm_task(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf0.in_etm_en_chn = enable;
}

///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Get DMA TX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_tx_get_interrupt_status(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->out_intr[channel].st.val;
}

/**
 * @brief Enable DMA TX channel interrupt
 */
static inline void ahb_dma_ll_tx_enable_interrupt(ahb_dma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->out_intr[channel].ena.val |= mask;
    } else {
        dev->out_intr[channel].ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_tx_clear_interrupt_status(ahb_dma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->out_intr[channel].clr.val = mask;
}

/**
 * @brief Get DMA TX channel interrupt status register address
 */
static inline volatile void *ahb_dma_ll_tx_get_interrupt_status_reg(ahb_dma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->out_intr[channel].st);
}

/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void ahb_dma_ll_tx_enable_owner_check(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf1.out_check_owner_chn = enable;
}

/**
 * @brief Enable DMA TX channel burst sending data, disabled by default
 */
static inline void ahb_dma_ll_tx_enable_data_burst(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.out_data_burst_en_chn = enable;
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void ahb_dma_ll_tx_enable_descriptor_burst(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.outdscr_burst_en_chn = enable;
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void ahb_dma_ll_tx_set_eof_mode(ahb_dma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    dev->channel[channel].out.out_conf0.out_eof_mode_chn = mode;
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void ahb_dma_ll_tx_enable_auto_write_back(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.out_auto_wrback_chn = enable;
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_tx_reset_channel(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_conf0.out_rst_chn = 1;
    dev->channel[channel].out.out_conf0.out_rst_chn = 0;
}

/**
 * @brief Check if DMA TX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool ahb_dma_ll_tx_is_fifo_full(ahb_dma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.val & 0x01;
}

/**
 * @brief Check if DMA TX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool ahb_dma_ll_tx_is_fifo_empty(ahb_dma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.val & 0x02;
}

/**
 * @brief Get number of bytes in TX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t ahb_dma_ll_tx_get_fifo_bytes(ahb_dma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.outfifo_cnt_chn;
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void ahb_dma_ll_tx_push_data(ahb_dma_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->channel[channel].out.out_push.outfifo_wdata_chn = data;
    dev->channel[channel].out.out_push.outfifo_push_chn = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_tx_set_desc_addr(ahb_dma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->out_link_addr[channel].outlink_addr_chn = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_tx_start(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_link.outlink_start_chn = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_tx_stop(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_link.outlink_stop_chn = 1;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void ahb_dma_ll_tx_restart(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_link.outlink_restart_chn = 1;
}

/**
 * @brief Check if DMA TX FSM is in IDLE state
 */
static inline bool ahb_dma_ll_tx_is_fsm_idle(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.out_link.outlink_park_chn;
}

/**
 * @brief Get TX EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_tx_get_eof_desc_addr(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.out_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched TX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t ahb_dma_ll_tx_get_prefetched_desc_addr(ahb_dma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.out_dscr.val;
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void ahb_dma_ll_tx_set_priority(ahb_dma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->channel[channel].out.out_pri.tx_pri_chn = prio;
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void ahb_dma_ll_tx_connect_to_periph(ahb_dma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    (void)periph;
    dev->channel[channel].out.out_peri_sel.peri_out_sel_chn = periph_id;
}

/**
 * @brief Disconnect DMA TX channel from peripheral
 */
static inline void ahb_dma_ll_tx_disconnect_from_periph(ahb_dma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_peri_sel.peri_out_sel_chn = GDMA_LL_INVALID_PERIPH_ID;
}

/**
 * @brief Whether to enable the ETM subsystem for TX channel
 *
 * @note When ETM_EN is 1, only ETM tasks can be used to configure the transfer direction and enable the channel.
 */
static inline void ahb_dma_ll_tx_enable_etm_task(ahb_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.out_etm_en_chn = enable;
}

#ifdef __cplusplus
}
#endif
