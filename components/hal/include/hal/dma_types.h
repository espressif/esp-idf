/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of DMA descriptor
 *
 */
typedef struct dma_descriptor_s {
    struct {
        uint32_t size : 12;         /*!< Buffer size */
        uint32_t length : 12;       /*!< Number of valid bytes in the buffer */
        uint32_t reversed24_27 : 4; /*!< Reserved */
        uint32_t err_eof : 1;       /*!< Whether the received buffer contains error */
        uint32_t reserved29 : 1;    /*!< Reserved */
        uint32_t suc_eof : 1;       /*!< Whether the descriptor is the last one in the link */
        uint32_t owner : 1;         /*!< Who is allowed to access the buffer that this descriptor points to */
    } dw0;                          /*!< Descriptor Word 0 */
    void *buffer;                   /*!< Pointer to the buffer */
    struct dma_descriptor_s *next;  /*!< Pointer to the next descriptor (set to NULL if the descriptor is the last one, e.g. suc_eof=1) */
} dma_descriptor_t;

ESP_STATIC_ASSERT(sizeof(dma_descriptor_t) == 12, "dma_descriptor_t should occupy 12 bytes in memory");

#define DMA_DESCRIPTOR_BUFFER_OWNER_CPU (0)   /*!< DMA buffer is allowed to be accessed by CPU */
#define DMA_DESCRIPTOR_BUFFER_OWNER_DMA (1)   /*!< DMA buffer is allowed to be accessed by DMA engine */
#define DMA_DESCRIPTOR_BUFFER_MAX_SIZE (4095) /*!< Maximum size of the buffer that can be attached to descriptor */
#define DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED  (4095-3)  /*!< Maximum size of the buffer that can be attached to descriptor, and aligned to 4B */


#ifdef __cplusplus
}
#endif
