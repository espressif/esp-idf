/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GDMA_LL_GET_HW(id) (((id) == 0) ? (&GDMA) : NULL)

#define GDMA_LL_RX_EVENT_MASK       (0x06A7)
#define GDMA_LL_TX_EVENT_MASK       (0x1958)

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

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Enable DMA channel M2M mode (TX channel n forward data to RX channel n), disabled by default
 */
static inline void gdma_ll_enable_m2m_mode(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA clock gating
 */
static inline void gdma_ll_enable_clock(gdma_dev_t *dev, bool enable)
{
    abort(); //TODO IDF-3817
}

///////////////////////////////////// RX /////////////////////////////////////////
/**
 * @brief Get DMA RX channel interrupt status word
 */
static inline uint32_t gdma_ll_rx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA RX channel interrupt
 */
static inline void gdma_ll_rx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Clear DMA RX channel interrupt
 */
static inline void gdma_ll_rx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get DMA RX channel interrupt status register address
 */
static inline volatile void *gdma_ll_rx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_rx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA RX channel burst reading data, disabled by default
 */
static inline void gdma_ll_rx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_rx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
static inline void gdma_ll_rx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Check if DMA RX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_rx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Check if DMA RX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_rx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get number of bytes in RX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t gdma_ll_rx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t gdma_ll_rx_pop_data(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
static inline void gdma_ll_rx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Start dealing with RX descriptors
 */
static inline void gdma_ll_rx_start(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Stop dealing with RX descriptors
 */
static inline void gdma_ll_rx_stop(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
static inline void gdma_ll_rx_restart(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void gdma_ll_rx_enable_auto_return(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Check if DMA RX FSM is in IDLE state
 */
static inline bool gdma_ll_rx_is_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get RX success EOF descriptor's address
 */
static inline uint32_t gdma_ll_rx_get_success_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get RX error EOF descriptor's address
 */
static inline uint32_t gdma_ll_rx_get_error_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get current RX descriptor's address
 */
static inline uint32_t gdma_ll_rx_get_current_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void gdma_ll_rx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void gdma_ll_rx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, int periph_id)
{
    abort(); //TODO IDF-3817
}

///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Get DMA TX channel interrupt status word
 */
static inline uint32_t gdma_ll_tx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA TX channel interrupt
 */
static inline void gdma_ll_tx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Clear DMA TX channel interrupt
 */
static inline void gdma_ll_tx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get DMA TX channel interrupt status register address
 */
static inline volatile void *gdma_ll_tx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_tx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA TX channel burst sending data, disabled by default
 */
static inline void gdma_ll_tx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_tx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void gdma_ll_tx_set_eof_mode(gdma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void gdma_ll_tx_enable_auto_write_back(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
static inline void gdma_ll_tx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Check if DMA TX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_tx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Check if DMA TX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_tx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get number of bytes in TX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t gdma_ll_tx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void gdma_ll_tx_push_data(gdma_dev_t *dev, uint32_t channel, uint32_t data)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
static inline void gdma_ll_tx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Start dealing with TX descriptors
 */
static inline void gdma_ll_tx_start(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Stop dealing with TX descriptors
 */
static inline void gdma_ll_tx_stop(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
static inline void gdma_ll_tx_restart(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Check if DMA TX FSM is in IDLE state
 */
static inline bool gdma_ll_tx_is_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get TX EOF descriptor's address
 */
static inline uint32_t gdma_ll_tx_get_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Get current TX descriptor's address
 */
static inline uint32_t gdma_ll_tx_get_current_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void gdma_ll_tx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    abort(); //TODO IDF-3817
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void gdma_ll_tx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, int periph_id)
{
    abort(); //TODO IDF-3817
}

#ifdef __cplusplus
}
#endif
