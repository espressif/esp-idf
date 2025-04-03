/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Suspend external memory cache
 *
 * @note CPU branch predictor should be disabled before calling this API
 * @note This API is only for internal usage, no thread safety guaranteed
 * @note This API is Non-Nestable and Non-Reentrant
 * @note Call to this API must be followed by a `esp_cache_resume_ext_mem_cache`
 */
void esp_cache_suspend_ext_mem_cache(void);

/**
 * @brief Resume external memory cache
 *
 * @note This API is only for internal usage, no thread safety guaranteed
 * @note This API is Non-Nestable and Non-Reentrant
 * @note This API must be called after a `esp_cache_suspend_ext_mem_cache`
 */
void esp_cache_resume_ext_mem_cache(void);

#if SOC_CACHE_FREEZE_SUPPORTED
/**
 * @brief Freeze external memory cache
 *
 * @note This API is only for internal usage, no thread safety guaranteed
 * @note This API is Non-Nestable and Non-Reentrant
 * @note Call to this API must be followed by a `esp_cache_unfreeze_ext_mem_cache`
 */
void esp_cache_freeze_ext_mem_cache(void);

/**
 * @brief Unfreeze external memory cache
 *
 * @note This API is only for internal usage, no thread safety guaranteed
 * @note This API is Non-Nestable and Non-Reentrant
 * @note This API must be called after a `esp_cache_freeze_ext_mem_cache`
 */
void esp_cache_unfreeze_ext_mem_cache(void);
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
 * @deprecated This function is deprecated and will be removed in the future.
 *             Use 'heap_caps_malloc' with MALLOC_CAP_CACHE_ALIGNED caps instead
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
__attribute__((deprecated("Use 'heap_caps_malloc' with MALLOC_CAP_CACHE_ALIGNED caps instead")))
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
 * @deprecated This function is deprecated and will be removed in the future.
 *             Use 'heap_caps_malloc_prefer' with MALLOC_CAP_CACHE_ALIGNED caps instead
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
__attribute__((deprecated("Use 'heap_caps_malloc_prefer' with MALLOC_CAP_CACHE_ALIGNED caps instead")))
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
 * @deprecated This function is deprecated and will be removed in the future.
 *             Use 'heap_caps_calloc' with MALLOC_CAP_CACHE_ALIGNED caps instead
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
__attribute__((deprecated("Use 'heap_caps_calloc' with MALLOC_CAP_CACHE_ALIGNED caps instead")))
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
 * @deprecated This function is deprecated and will be removed in the future.
 *             Use 'heap_caps_calloc_prefer' with MALLOC_CAP_CACHE_ALIGNED caps instead
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
__attribute__((deprecated("Use 'heap_caps_calloc' with MALLOC_CAP_CACHE_ALIGNED caps instead")))
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
