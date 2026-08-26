/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dma2d_dev_t *dma2d_soc_handle_t; // 2D-DMA SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    dma2d_soc_handle_t dev; // 2D-DMA SOC layer handle (i.e. register base address)
} dma2d_hal_context_t;

/**
 * @brief Init the 2D-DMA hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param group_id The 2D-DMA group number
 */
void dma2d_hal_init(dma2d_hal_context_t *hal, int group_id);

/**
 * @brief Reset 2D-DMA TX channel
 *
 * @param hal Context of the HAL layer
 * @param channel TX channel ID
 */
void dma2d_hal_tx_reset_channel(dma2d_hal_context_t *hal, uint32_t channel);

/**
 * @brief Reset 2D-DMA RX channel
 *
 * @param hal Context of the HAL layer
 * @param channel RX channel ID
 */
void dma2d_hal_rx_reset_channel(dma2d_hal_context_t *hal, uint32_t channel);

#ifdef __cplusplus
}
#endif
