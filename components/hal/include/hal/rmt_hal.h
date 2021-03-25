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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/rmt_struct.h"

/**
 * @brief HAL context type of RMT driver
 *
 */
typedef struct {
    rmt_dev_t *regs;     /*!< RMT Register base address */
    rmt_mem_t *mem;      /*!< RMT Memory base address */
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

/**
 * @brief Receive a frame from RMT channel
 *
 * @param hal: RMT HAL context
 * @param channel: RMT channel number
 * @param buf: buffer to store received RMT frame
 * @return number of items that get received
 */
uint32_t rmt_hal_receive(rmt_hal_context_t *hal, uint32_t channel, rmt_item32_t *buf);

#ifdef __cplusplus
}
#endif
