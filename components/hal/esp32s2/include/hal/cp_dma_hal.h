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

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

// CP DMA HAL usages:
// 1. Initialize HAL layer by cp_dma_hal_init, pass in the allocated descriptors for TX and RX
// 2. Enable DMA and interrupt by cp_dma_hal_start
// 3. Prepare descriptors used for TX and RX
// 4. Restart the DMA engine in case it's not in working

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "hal/dma_types.h"
#include "soc/cp_dma_struct.h"

/**
 * @brief HAL context
 *
 */
typedef struct {
    cp_dma_dev_t *dev;
} cp_dma_hal_context_t;

typedef struct {
} cp_dma_hal_config_t;

/**
 * @brief Initialize HAL layer context
 *
 * @param hal HAL layer context, whose memroy should be allocated at driver layer
 * @param config configuration for the HAL layer
 */
void cp_dma_hal_init(cp_dma_hal_context_t *hal, const cp_dma_hal_config_t *config);

/**
 * @brief Deinitialize HAL layer context
 */
void cp_dma_hal_deinit(cp_dma_hal_context_t *hal);

/**
 * @brief Set descriptor base address
 */
void cp_dma_hal_set_desc_base_addr(cp_dma_hal_context_t *hal, intptr_t outlink_base, intptr_t inlink_base);

/**
 * @brief Start mem2mem DMA state machine
 */
void cp_dma_hal_start(cp_dma_hal_context_t *hal);

/**
 * @brief Stop mem2mem DMA state machine
 */
void cp_dma_hal_stop(cp_dma_hal_context_t *hal);

/**
 * @brief Get interrupt status word
 *
 * @return uint32_t Interrupt status
 */
uint32_t cp_dma_hal_get_intr_status(cp_dma_hal_context_t *hal) IRAM_ATTR;

/**
 * @brief Clear interrupt mask
 *
 * @param mask interrupt mask
 */
void cp_dma_hal_clear_intr_status(cp_dma_hal_context_t *hal, uint32_t mask) IRAM_ATTR;

/**@{*/
/**
 * @brief Give the owner of descriptors between [start_desc, end_desc] to DMA, and restart DMA HW engine
 *
 * @param hal HAL layer context
 * @param start_desc The first descriptor that carries one transaction
 * @param end_desc The last descriptor that carries one transaction
 */
void cp_dma_hal_restart_tx(cp_dma_hal_context_t *hal);
void cp_dma_hal_restart_rx(cp_dma_hal_context_t *hal);
/**@}*/

#ifdef __cplusplus
}
#endif
