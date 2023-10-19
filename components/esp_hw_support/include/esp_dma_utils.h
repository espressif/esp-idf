/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DMA malloc flags
 */
/**
 * @brief Memory is in PSRAM
 */
#define ESP_DMA_MALLOC_FLAG_PSRAM        BIT(0)

/**
 * @brief Helper function for malloc a DMA capable memory buffer
 *
 * @param[in]  size         Size in bytes, the amount of memory to allocate
 * @param[in]  flags        Flags, see `ESP_DMA_MALLOC_FLAG_x`
 * @param[out] out_ptr      A pointer to the memory allocated successfully
 * @param[out] actual_size  Actual size for allocation in bytes, when the size you specified doesn't meet the DMA alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_dma_malloc(size_t size, uint32_t flags, void **out_ptr, size_t *actual_size);

/**
 * @brief Helper function for calloc a DMA capable memory buffer
 *
 * @param[in]  n            Number of continuing chunks of memory to allocate
 * @param[in]  size         Size of one chunk, in bytes
 * @param[in]  flags        Flags, see `ESP_DMA_MALLOC_FLAG_x`
 * @param[out] out_ptr      A pointer to the memory allocated successfully
 * @param[out] actual_size  Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_dma_calloc(size_t n, size_t size, uint32_t flags, void **out_ptr, size_t *actual_size);

/**
 * @brief DMA buffer location
 */
typedef enum {
    ESP_DMA_BUF_LOCATION_INTERNAL,    ///< DMA buffer is in internal memory
    ESP_DMA_BUF_LOCATION_PSRAM,       ///< DMA buffer is in PSRAM
} esp_dma_buf_location_t;

/**
 * @brief Helper function to check if a buffer meets DMA alignment requirements
 *
 * @param[in]  ptr       Pointer to the buffer
 * @param[in]  size      Size of the buffer
 * @param[in]  location  Location of the DMA buffer, see `esp_dma_buf_location_t`
 *
 * @return
 *        - True:  Buffer is aligned
 *        - False: Buffer is not aligned, or buffer is not DMA capable
 */
bool esp_dma_is_buffer_aligned(const void *ptr, size_t size, esp_dma_buf_location_t location);

#ifdef __cplusplus
}
#endif
