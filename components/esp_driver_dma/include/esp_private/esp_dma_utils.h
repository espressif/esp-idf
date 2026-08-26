/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
 * The array contains three parts: head, body and tail.
 * Length of each part will be >=0, especially, length=0 means that there is no such part.
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
 * @brief Split DMA RX buffer to cache aligned buffers
 *
 * @note After the original RX buffer is split into an array, caller should mount the buffer array to the DMA controller in scatter-gather mode.
 *       Don't read/write the aligned buffers before the DMA finished using them.
 *
 * @param[in]   rx_buffer        The origin DMA buffer used for receiving data
 * @param[in]   buffer_len       rx_buffer length
 * @param[out]  align_buf_array  Aligned DMA buffer array
 * @param[out]  ret_stash_buffer Allocated stash buffer (caller should free it after use)
 * @return
 *      - ESP_OK: Split to aligned buffer successfully
 *      - ESP_ERR_INVALID_ARG: Split to aligned buffer failed because of invalid argument
 *
 *  brief sketch:
 *                  cache alignment delimiter    cache alignment delimiter
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
esp_err_t esp_dma_split_rx_buffer_to_cache_aligned(void *rx_buffer, size_t buffer_len, dma_buffer_split_array_t *align_buf_array, uint8_t** ret_stash_buffer);

/**
 * @brief Merge aligned RX buffer array to origin buffer
 *
 * @note This function can be used in the ISR context.
 *
 * @param[in] align_buf_array Aligned DMA buffer array
 * @return
 *      - ESP_OK: Merge aligned buffer to origin buffer successfully
 *      - ESP_ERR_INVALID_ARG: Merge aligned buffer to origin buffer failed because of invalid argument
 */
esp_err_t esp_dma_merge_aligned_rx_buffers(dma_buffer_split_array_t *align_buf_array);

/**
 * @brief Calculate the number of DMA linked list nodes required for a given buffer size
 *
 * @param[in] buffer_size Total size of the buffer
 * @param[in] buffer_alignment Alignment requirement for the buffer
 * @param[in] max_buffer_size_per_node Maximum buffer size that each node can handle
 * @return Number of DMA linked list nodes required
 */
size_t esp_dma_calculate_node_count(size_t buffer_size, size_t buffer_alignment, size_t max_buffer_size_per_node);

#ifdef __cplusplus
}
#endif
