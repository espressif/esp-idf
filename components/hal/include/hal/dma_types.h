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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

_Static_assert(sizeof(dma_descriptor_t) == 12, "dma_descriptor_t should occupy 12 bytes in memory");

#define DMA_DESCRIPTOR_BUFFER_OWNER_CPU (0)   /*!< DMA buffer is allowed to be accessed by CPU */
#define DMA_DESCRIPTOR_BUFFER_OWNER_DMA (1)   /*!< DMA buffer is allowed to be accessed by DMA engine */
#define DMA_DESCRIPTOR_BUFFER_MAX_SIZE (4095) /*!< Maximum size of the buffer that can be attached to descriptor */
