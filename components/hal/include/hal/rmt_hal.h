/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rmt_dev_t *rmt_soc_handle_t; // RMT SOC layer handle

/**
 * @brief HAL context type of RMT driver
 */
typedef struct {
    rmt_soc_handle_t regs; /*!< RMT Register base address */
} rmt_hal_context_t;

/**
 * @brief Initialize the RMT HAL driver
 *
 * @param hal: RMT HAL context
 */
void rmt_hal_init(rmt_hal_context_t *hal);

/**
 * @brief Deinitialize the RMT HAL driver
 *
 * @param hal: RMT HAL context
 */
void rmt_hal_deinit(rmt_hal_context_t *hal);

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

#ifdef __cplusplus
}
#endif
