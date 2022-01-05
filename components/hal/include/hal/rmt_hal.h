/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct rmt_dev_t *rmt_soc_handle_t; // RMT SOC layer handle
typedef struct rmt_mem_t *rmt_mem_handle_t; // RMT memory handle

/**
 * @brief HAL context type of RMT driver
 *
 */
typedef struct {
    rmt_soc_handle_t regs; /*!< RMT Register base address */
    rmt_mem_handle_t mem;  /*!< RMT Memory base address */
} rmt_hal_context_t;

#define RMT_MEM_OWNER_SW (0) /*!< RMT Memory ownership belongs to software side */
#define RMT_MEM_OWNER_HW (1) /*!< RMT Memory ownership belongs to hardware side */

/**
 * @brief Initialize the RMT HAL driver
 *
 * @param hal: RMT HAL context
 */
void rmt_hal_init(rmt_hal_context_t *hal);

/**
 * @brief Reset RMT TX Channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 */
void rmt_hal_tx_channel_reset(rmt_hal_context_t *hal, uint32_t channel);

/**
 * @brief Reset RMT TX Channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 */
void rmt_hal_rx_channel_reset(rmt_hal_context_t *hal, uint32_t channel);

/**
 * @brief Set counter clock for RMT channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 * @param base_clk_hz: base clock for RMT internal channel (counter clock will divide from it)
 * @param counter_clk_hz: target counter clock
 */
void rmt_hal_tx_set_channel_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t counter_clk_hz);

/**
 * @brief Set carrier clock for RMT channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 * @param base_clk_hz: base clock for RMT carrier generation (carrier clock will divide from it)
 * @param carrier_clk_hz: target carrier clock
 * @param carrier_clk_duty: duty ratio of carrier clock
 */
void rmt_hal_set_carrier_clock(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t carrier_clk_hz, float carrier_clk_duty);

/**
 * @brief Set filter threshold for RMT Receive channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 * @param base_clk_hz: base clock for RMT receive filter
 * @param thres_us: threshold of RMT receive filter, in us
 */
void rmt_hal_set_rx_filter_thres(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t thres_us);

/**
 * @brief Set idle threshold for RMT Receive channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 * @param base_clk_hz: base clock for RMT receive channel
 * @param thres_us: IDLE threshold for RMT receive channel
 */
void rmt_hal_set_rx_idle_thres(rmt_hal_context_t *hal, uint32_t channel, uint32_t base_clk_hz, uint32_t thres_us);

#ifdef __cplusplus
}
#endif
