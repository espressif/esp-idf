/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Freeze external memory cache and disable non-iram interrupts
 *
 * @note This API will enter a critical section, you will need to call `esp_cache_unfreeze_caches_enable_interrupts` to exit it.
 */
void esp_cache_freeze_caches_disable_interrupts(void);

/**
 * @brief Unfreeze external memory cache and re-enable non-iram interrupts
 */
void esp_cache_unfreeze_caches_enable_interrupts(void);
#endif

/**
 * @brief Enter critical section for cache sync operations
 */
void esp_cache_sync_ops_enter_critical_section(void);

/**
 * @brief Exit critical section for cache sync operations
 */
void esp_cache_sync_ops_exit_critical_section(void);

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
