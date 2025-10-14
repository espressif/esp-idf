/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h> /* Required for NULL constant */
#include <stdint.h>
#include <stdbool.h>
#include "hal/gdma_types.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"
#include "soc/system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GDMA_LL_GET_HW(id) (((id) == 0) ? (&GDMA) : NULL)

#define GDMA_LL_CHANNEL_MAX_PRIORITY 5 // supported priority levels: [0,5]

#define GDMA_LL_RX_EVENT_MASK       (0x06A7)
#define GDMA_LL_TX_EVENT_MASK       (0x1958)

// any "valid" peripheral ID can be used for M2M mode
#define GDMA_LL_M2M_FREE_PERIPH_ID_MASK (0x1CD)
#define GDMA_LL_INVALID_PERIPH_ID       (0x3F)

#define GDMA_LL_EVENT_TX_FIFO_UDF   (1<<12)
#define GDMA_LL_EVENT_TX_FIFO_OVF   (1<<11)
#define GDMA_LL_EVENT_RX_FIFO_UDF   (1<<10)
#define GDMA_LL_EVENT_RX_FIFO_OVF   (1<<9)
#define GDMA_LL_EVENT_TX_TOTAL_EOF  (1<<8)
#define GDMA_LL_EVENT_RX_DESC_EMPTY (1<<7)
#define GDMA_LL_EVENT_TX_DESC_ERROR (1<<6)
#define GDMA_LL_EVENT_RX_DESC_ERROR (1<<5)
#define GDMA_LL_EVENT_TX_EOF        (1<<4)
#define GDMA_LL_EVENT_TX_DONE       (1<<3)
#define GDMA_LL_EVENT_RX_ERR_EOF    (1<<2)
#define GDMA_LL_EVENT_RX_SUC_EOF    (1<<1)
#define GDMA_LL_EVENT_RX_DONE       (1<<0)

#define GDMA_LL_AHB_GROUP_START_ID         0 // AHB GDMA group ID starts from 0
#define GDMA_LL_AHB_NUM_GROUPS             1 // Number of AHB GDMA groups
#define GDMA_LL_AHB_PAIRS_PER_GROUP        3 // Number of GDMA pairs in each AHB group
#define GDMA_LL_AHB_TX_RX_SHARE_INTERRUPT  1  // TX and RX channel in the same pair will share the same interrupt source number

#define GDMA_LL_AHB_DESC_ALIGNMENT    4
#define GDMA_LL_AHB_RX_BURST_NEEDS_ALIGNMENT  1

///////////////////////////////////// Common /////////////////////////////////////////

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void _gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    SYSTEM.perip_clk_en1.reg_dma_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define gdma_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _gdma_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the DMA module
 */
static inline void _gdma_ll_reset_register(int group_id)
{
    (void)group_id;
    SYSTEM.perip_rst_en1.reg_dma_rst = 1;
    SYSTEM.perip_rst_en1.reg_dma_rst = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define gdma_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _gdma_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief Force enable register clock
 */
static inline void gdma_ll_force_enable_reg_clock(gdma_dev_t *dev, bool enable)
{
    dev->misc_conf.clk_en = enable;
}

///////////////////////////////////// RX /////////////////////////////////////////
/**
 * @brief Get DMA RX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel, bool raw)
{
    if (raw) {
        return dev->intr[channel].raw.val & GDMA_LL_RX_EVENT_MASK;
    } else {
        return dev->intr[channel].st.val & GDMA_LL_RX_EVENT_MASK;
    }
}

/**
 * @brief Enable DMA RX channel interrupt
 */
static inline void gdma_ll_rx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->intr[channel].ena.val |= (mask & GDMA_LL_RX_EVENT_MASK);
    } else {
        dev->intr[channel].ena.val &= ~(mask & GDMA_LL_RX_EVENT_MASK);
    }
}

/**
 * @brief Clear DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->intr[channel].clr.val = (mask & GDMA_LL_RX_EVENT_MASK);
}

/**
 * @brief Get DMA RX channel interrupt status register address
 */
static inline volatile void *gdma_ll_rx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->intr[channel].st);
}

/**
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_rx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf1.in_check_owner = enable;
}

/**
 * @brief Enable DMA RX channel burst reading data, disabled by default
 */
static inline void gdma_ll_rx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf0.in_data_burst_en = enable;
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_rx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_conf0.indscr_burst_en = enable;
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_conf0.in_rst = 1;
    dev->channel[channel].in.in_conf0.in_rst = 0;
}

/**
 * @brief Check if DMA RX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_rx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.val & 0x01;
}

/**
 * @brief Check if DMA RX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_rx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.val & 0x02;
}

/**
 * @brief Get number of bytes in RX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t gdma_ll_rx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].in.infifo_status.infifo_cnt;
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t gdma_ll_rx_pop_data(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_pop.infifo_pop = 1;
    return dev->channel[channel].in.in_pop.infifo_rdata;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->channel[channel].in.in_link.addr = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_start(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_link.start = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_stop(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_link.stop = 1;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_restart(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_link.restart = 1;
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void gdma_ll_rx_enable_auto_return(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].in.in_link.auto_ret = enable;
}

/**
 * @brief Check if DMA RX descriptor FSM is in IDLE state
 */
static inline bool gdma_ll_rx_is_desc_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_link.park;
}

/**
 * @brief Get RX success EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_success_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_suc_eof_des_addr;
}

/**
 * @brief Get RX error EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_error_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_err_eof_des_addr;
}

/**
 * @brief Get the pre-fetched RX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_prefetched_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].in.in_dscr;
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void gdma_ll_rx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->channel[channel].in.in_pri.rx_pri = prio;
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void gdma_ll_rx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    dev->channel[channel].in.in_peri_sel.sel = periph_id;
    dev->channel[channel].in.in_conf0.mem_trans_en = (periph == GDMA_TRIG_PERIPH_M2M);
}

/**
 * @brief Disconnect DMA RX channel from peripheral
 */
static inline void gdma_ll_rx_disconnect_from_periph(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].in.in_peri_sel.sel = GDMA_LL_INVALID_PERIPH_ID;
    dev->channel[channel].in.in_conf0.mem_trans_en = false;
}

///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Get DMA TX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel, bool raw)
{
    if (raw) {
        return dev->intr[channel].raw.val & GDMA_LL_TX_EVENT_MASK;
    } else {
        return dev->intr[channel].st.val & GDMA_LL_TX_EVENT_MASK;
    }
}

/**
 * @brief Enable DMA TX channel interrupt
 */
static inline void gdma_ll_tx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->intr[channel].ena.val |= (mask & GDMA_LL_TX_EVENT_MASK);
    } else {
        dev->intr[channel].ena.val &= ~(mask & GDMA_LL_TX_EVENT_MASK);
    }
}

/**
 * @brief Clear DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->intr[channel].clr.val = (mask & GDMA_LL_TX_EVENT_MASK);
}

/**
 * @brief Get DMA TX channel interrupt status register address
 */
static inline volatile void *gdma_ll_tx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->intr[channel].st);
}

/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_tx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf1.out_check_owner = enable;
}

/**
 * @brief Enable DMA TX channel burst sending data, disabled by default
 */
static inline void gdma_ll_tx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.out_data_burst_en = enable;
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_tx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.outdscr_burst_en = enable;
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void gdma_ll_tx_set_eof_mode(gdma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    dev->channel[channel].out.out_conf0.out_eof_mode = mode;
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void gdma_ll_tx_enable_auto_write_back(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->channel[channel].out.out_conf0.out_auto_wrback = enable;
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_conf0.out_rst = 1;
    dev->channel[channel].out.out_conf0.out_rst = 0;
}

/**
 * @brief Check if DMA TX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_tx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.val & 0x01;
}

/**
 * @brief Check if DMA TX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_tx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.val & 0x02;
}

/**
 * @brief Get number of bytes in TX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t gdma_ll_tx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    return dev->channel[channel].out.outfifo_status.outfifo_cnt;
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void gdma_ll_tx_push_data(gdma_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->channel[channel].out.out_push.outfifo_wdata = data;
    dev->channel[channel].out.out_push.outfifo_push = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->channel[channel].out.out_link.addr = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_start(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_link.start = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_stop(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_link.stop = 1;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_restart(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_link.restart = 1;
}

/**
 * @brief Check if DMA TX descriptor FSM is in IDLE state
 */
static inline bool gdma_ll_tx_is_desc_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.out_link.park;
}

/**
 * @brief Get TX EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.out_eof_des_addr;
}

/**
 * @brief Get the pre-fetched TX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_prefetched_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    return dev->channel[channel].out.out_dscr;
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void gdma_ll_tx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->channel[channel].out.out_pri.tx_pri = prio;
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void gdma_ll_tx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    (void)periph;
    dev->channel[channel].out.out_peri_sel.sel = periph_id;
}

/**
 * @brief Disconnect DMA TX channel from peripheral
 */
static inline void gdma_ll_tx_disconnect_from_periph(gdma_dev_t *dev, uint32_t channel)
{
    dev->channel[channel].out.out_peri_sel.sel = GDMA_LL_INVALID_PERIPH_ID;
}

#ifdef __cplusplus
}
#endif
