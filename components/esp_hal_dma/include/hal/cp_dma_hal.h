/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cp_dma_dev_t *cp_dma_soc_handle_t; // CP DMA SOC layer handle

/**
 * @brief HAL context
 */
typedef struct {
    cp_dma_soc_handle_t dev; // CP DMA SOC layer handle (i.e. register base address)
} cp_dma_hal_context_t;

typedef struct {
} cp_dma_hal_config_t;

/**
 * @brief Initialize HAL layer context
 *
 * @param hal HAL layer context, whose memory should be allocated at driver layer
 * @param config configuration for the HAL layer
 */
void cp_dma_hal_init(cp_dma_hal_context_t *hal, const cp_dma_hal_config_t *config);

/**
 * @brief Deinitialize HAL layer context
 *
 * @param hal HAL layer context
 */
void cp_dma_hal_deinit(cp_dma_hal_context_t *hal);

/**
 * @brief Set descriptor base address
 *
 * @param hal HAL layer context
 * @param outlink_base Base address of the first outlink descriptor
 * @param inlink_base Base address of the first inlink descriptor
 */
void cp_dma_hal_set_desc_base_addr(cp_dma_hal_context_t *hal, uint32_t outlink_base, uint32_t inlink_base);

/**
 * @brief Start mem2mem DMA state machine
 *
 * @param hal HAL layer context
 */
void cp_dma_hal_start(cp_dma_hal_context_t *hal);

/**
 * @brief Stop mem2mem DMA state machine
 *
 * @param hal HAL layer context
 */
void cp_dma_hal_stop(cp_dma_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
