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
#include "soc/cp_dma_struct.h"

typedef struct cp_dma_descriptor {
    struct {
        uint32_t size : 12;         /*!< buffer size */
        uint32_t length : 12;       /*!< specify number of valid bytes in the buffer */
        uint32_t reversed24_27 : 4; /*!< reserved */
        uint32_t err : 1;           /*!< specify whether a received buffer contains error */
        uint32_t reserved29 : 1;    /*!< reserved */
        uint32_t eof : 1;           /*!< if this dma link is the last one, you shoule set this bit 1 */
        uint32_t owner : 1;         /*!< specify the owner of buffer that this descriptor points to, 1=DMA, 0=CPU. DMA will clear it after use. */
    } dw0;                          /*!< descriptor word 0 */
    void *buffer;                   /*!< pointer to the buffer */
    struct cp_dma_descriptor *next; /*!< pointer to the next descriptor or NULL if this descriptor is the last one */
} cp_dma_descriptor_t;

_Static_assert(sizeof(cp_dma_descriptor_t) == 12, "cp_dma_descriptor_t should occupy 12 bytes in memory");

/**
 * @brief HAL context
 *
 * @note `tx_desc` and `rx_desc` are internal state of the HAL, will be modified during the operations.
 * Upper layer of HAL should keep the buffer address themselves and make sure the buffers are freed when the HAL is no longer used.
 *
 */
typedef struct {
    cp_dma_dev_t *dev;
    cp_dma_descriptor_t *tx_desc;
    cp_dma_descriptor_t *rx_desc;
    cp_dma_descriptor_t *next_rx_desc_to_check;
} cp_dma_hal_context_t;

/**
 * @brief Initialize HAL layer context
 *
 * @param hal HAL layer context, memroy should be allocated at driver layer
 * @param tx_descriptors out link descriptor pool
 * @param tx_desc_num number of out link descriptors
 * @param rx_descriptors in line descriptor pool
 * @param rx_desc_num  number of in link descriptors
 */
void cp_dma_hal_init(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *tx_descriptors[], uint32_t tx_desc_num, cp_dma_descriptor_t *rx_descriptors[], uint32_t rx_desc_num);

/**
 * @brief Deinitialize HAL layer context
 */
void cp_dma_hal_deinit(cp_dma_hal_context_t *hal);

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

/**
 * @brief Get next RX descriptor that needs recycling
 *
 * @param eof_desc EOF descriptor for this iteration
 * @param[out] next_desc Next descriptor needs to check
 * @return Whether to continue
 */
bool cp_dma_hal_get_next_rx_descriptor(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *eof_desc, cp_dma_descriptor_t **next_desc);

/**
 * @brief Prepare buffer to be transmitted
 *
 * @param hal HAL layer context
 * @param buffer buffer address
 * @param len buffer size
 * @param[out] start_desc The first descriptor that carry the TX transaction
 * @param[out] end_desc The last descriptor that carry the TX transaction
 * @return Number of bytes has been parepared to transmit
 */
int cp_dma_hal_prepare_transmit(cp_dma_hal_context_t *hal, void *buffer, size_t len, cp_dma_descriptor_t **start_desc, cp_dma_descriptor_t **end_desc);

/**
 * @brief Prepare buffer to receive
 *
 * @param hal HAL layer context
 * @param buffer buffer address
 * @param size buffer size
 * @param[out] start_desc The first descriptor that carries the RX transaction
 * @param[out] end_desc The last descriptor that carries the RX transaction
 * @return Number of bytes has been parepared to receive
 */
int cp_dma_hal_prepare_receive(cp_dma_hal_context_t *hal, void *buffer, size_t size, cp_dma_descriptor_t **start_desc, cp_dma_descriptor_t **end_desc);

/**@{*/
/**
 * @brief Give the owner of descriptors between [start_desc, end_desc] to DMA, and restart DMA HW engine
 *
 * @param hal HAL layer context
 * @param start_desc The first descriptor that carries one transaction
 * @param end_desc The last descriptor that carries one transaction
 */
void cp_dma_hal_restart_tx(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *start_desc, cp_dma_descriptor_t *end_desc);
void cp_dma_hal_restart_rx(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *start_desc, cp_dma_descriptor_t *end_desc);
/**@}*/

#ifdef __cplusplus
}
#endif
