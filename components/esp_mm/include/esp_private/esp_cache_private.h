/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cache malloc flags
 */
/**
 * @brief Memory is in PSRAM
 */
#define ESP_CACHE_MALLOC_FLAG_PSRAM        BIT(0)

/**
 * @brief Memory is capable to be accessed by DMA
 */
#define ESP_CACHE_MALLOC_FLAG_DMA          BIT(1)

/**
 * @brief Helper function for malloc a cache aligned data memory buffer
 *
 * @param[in]  size         Size in bytes, the amount of memory to allocate
 * @param[in]  flags        Flags, see `ESP_CACHE_MALLOC_FLAG_x`
 * @param[out] out_ptr      A pointer to the memory allocated successfully
 * @param[out] actual_size  Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_cache_aligned_malloc(size_t size, uint32_t flags, void **out_ptr, size_t *actual_size);

/**
 * @brief Helper function for calloc a cache aligned data memory buffer
 *
 * @param[in]  n            Number of continuing chunks of memory to allocate
 * @param[in]  size         Size of one chunk, in bytes
 * @param[in]  flags        Flags, see `ESP_CACHE_MALLOC_FLAG_x`
 * @param[out] out_ptr      A pointer to the memory allocated successfully
 * @param[out] actual_size  Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_cache_aligned_calloc(size_t n, size_t size, uint32_t flags, void **out_ptr, size_t *actual_size);

/**
 * @brief Get Cache alignment requirement for data
 *
 * @param[in]  flags          Flags, see `ESP_CACHE_MALLOC_FLAG_x`
 * @param[out] out_alignment  Alignment
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_cache_get_alignment(uint32_t flags, size_t *out_alignment);

#ifdef __cplusplus
}
#endif
