/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Helper function for malloc a cache aligned data memory buffer
 *
 * @note Now only support 'MALLOC_CAP_INTERNAL', 'MALLOC_CAP_DMA' and 'MALLOC_CAP_SPIRAM'
 *
 * @param[in]  size             Size in bytes, the amount of memory to allocate
 * @param[in]  heap_caps        Flags, see `MALLOC_CAP_x`
 * @param[out] out_ptr          A pointer to the memory allocated successfully
 * @param[out] actual_size      Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_cache_aligned_malloc(size_t size, uint32_t heap_caps, void **out_ptr, size_t *actual_size);

/**
 * @brief Helper function for malloc a cache aligned data memory buffer as preference in decreasing order.
 *
 * @note Now only support 'MALLOC_CAP_INTERNAL', 'MALLOC_CAP_DMA' and 'MALLOC_CAP_SPIRAM'
 *
 * @param[in]  size         Size in bytes, the amount of memory to allocate
 * @param[out] out_ptr      A pointer to the memory allocated successfully
 * @param[out] actual_size  Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 * @param[in]  flag_nums    Number of variable parameters
 * @param[in]  spread param The spread params are bitwise OR of Flags, see `MALLOC_CAP_x`. This API prefers to allocate memory with the first parameter. If failed, allocate memory with
 *                          the next parameter. It will try in this order until allocating a chunk of memory successfully
 *                          or fail to allocate memories with any of the parameters.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_cache_aligned_malloc_prefer(size_t size, void **out_ptr, size_t *actual_size, size_t flag_nums, ...);

/**
 * @brief Helper function for calloc a cache aligned data memory buffer
 *
 * @note Now only support 'MALLOC_CAP_INTERNAL', 'MALLOC_CAP_DMA' and 'MALLOC_CAP_SPIRAM'
 *
 * @param[in]  n                Number of continuing chunks of memory to allocate
 * @param[in]  size             Size of one chunk, in bytes
 * @param[in]  heap_caps        Flags, see `MALLOC_CAP_x`
 * @param[out] out_ptr          A pointer to the memory allocated successfully
 * @param[out] actual_size      Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_cache_aligned_calloc(size_t n, size_t size, uint32_t heap_caps, void **out_ptr, size_t *actual_size);

/**
 * @brief Helper function for calloc a cache aligned data memory buffer as preference in decreasing order.
 *
 * @note Now only support 'MALLOC_CAP_INTERNAL', 'MALLOC_CAP_DMA' and 'MALLOC_CAP_SPIRAM'
 *
 * @param[in]  n            Number of continuing chunks of memory to allocate
 * @param[in]  size         Size in bytes, the amount of memory to allocate
 * @param[out] out_ptr      A pointer to the memory allocated successfully
 * @param[out] actual_size  Actual size for allocation in bytes, when the size you specified doesn't meet the cache alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 * @param[in]  flag_nums    Number of variable parameters
 * @param[in]  spread param The spread params are bitwise OR of Flags, see `MALLOC_CAP_x`. This API prefers to allocate memory with the first parameter. If failed, allocate memory with
 *                          the next parameter. It will try in this order until allocating a chunk of memory successfully
 *                          or fail to allocate memories with any of the parameters.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_cache_aligned_calloc_prefer(size_t n, size_t size, void **out_ptr, size_t *actual_size, size_t flag_nums, ...);

/**
 * @brief Get Cache alignment requirement for data
 *
 * @note Now only support 'MALLOC_CAP_INTERNAL', 'MALLOC_CAP_DMA' and 'MALLOC_CAP_SPIRAM'
 *
 * @param[in]  heap_caps        Flags, see `MALLOC_CAP_x`
 * @param[out] out_alignment    Alignment
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_cache_get_alignment(uint32_t heap_caps, size_t *out_alignment);

#ifdef __cplusplus
}
#endif
