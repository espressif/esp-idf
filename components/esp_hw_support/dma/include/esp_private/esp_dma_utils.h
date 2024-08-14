/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DMA buffer information
 */
typedef struct {
    void *aligned_buffer;   //!< Buffer address
    void *recovery_address; //!< Origin buffer address that aligned buffer should be recovered
    size_t length;          //!< Buffer length
} dma_buffer_split_info_t;

/**
 * @brief DMA buffer aligned array
 */
typedef struct {
    union {
        struct {
            dma_buffer_split_info_t head;               //!< Aligned head part. Corresponds to the part of the original buffer where the head is not aligned
            dma_buffer_split_info_t body;               //!< Aligned body part. Corresponds to the part of the original aligned buffer
            dma_buffer_split_info_t tail;               //!< Aligned tail part. Corresponds to the part of the original buffer where the tail is not aligned
        } buf;
        dma_buffer_split_info_t aligned_buffer[3];      //!< DMA aligned buffer array, consist of `head`, `body` and `tail`
    };
} dma_buffer_split_array_t;

/**
 * @brief Split unaligned DMA buffer to aligned DMA buffer or aligned DMA buffer array
 *
 * @note Returned align array contains three parts: head, body and tail. Length of each buffer will be >=0, length 0 means that there is no such part
 *
 * @param[in]   buffer           Origin DMA buffer address
 * @param[in]   buffer_len       Origin DMA buffer length
 * @param[in]   stash_buffer     Needed extra buffer to stash aligned buffer, should be allocated with DMA capable memory and aligned to split_alignment
 * @param[in]   stash_buffer_len stash_buffer length
 * @param[in]   split_alignment  Alignment of each buffer required by the DMA
 * @param[out]  align_array      Aligned DMA buffer array
 * @return
 *      - ESP_OK: Split to aligned buffer successfully
 *      - ESP_ERR_INVALID_ARG: Split to aligned buffer failed because of invalid argument
 *
 *  brief sketch:
 *                  buffer alignment delimiter    buffer alignment delimiter
 *                              │                             │
 *     Origin Buffer            │        Origin Buffer        │
 *           │                  │              │              │
 *           │                  ▼              ▼              ▼
 *           │       ...---xxxxx|xxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxx----...
 *           │               │                 │                 │
 *           │               │                 ▼                 │
 *           │               │  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxx|  │
 *           │               │                 ▲                 │
 *           ▼               │                 │                 │
 *     Aligned buffers       └──► Head        Body   Tail ◄──────┘
 *                                 │                  │
 *                                 ▼                  ▼
 *                              |xxxxx......|     |xxxxx......|
 */
esp_err_t esp_dma_split_buffer_to_aligned(void *buffer, size_t buffer_len, void *stash_buffer, size_t stash_buffer_len, size_t split_alignment, dma_buffer_split_array_t *align_array);

/**
 * @brief Merge aligned buffer array to origin buffer
 *
 * @param[in]   align_array       Aligned DMA buffer array
 * @return
 *      - ESP_OK: Merge aligned buffer to origin buffer successfully
 *      - ESP_ERR_INVALID_ARG: Merge aligned buffer to origin buffer failed because of invalid argument
 */
esp_err_t esp_dma_merge_aligned_buffers(dma_buffer_split_array_t *align_array);

#ifdef __cplusplus
}
#endif
