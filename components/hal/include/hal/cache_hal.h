
/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/cache_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cache init and cache hal context init
 */
void cache_hal_init(void);

/**
 * @brief Disable cache
 *
 * Disable the ICache or DCache or both, all the items in the corresponding Cache(s) will be invalideated.
 * Next request to these items will trigger a transaction to the external memory (flash / psram)
 *
 * @note If the autoload feature is enabled, this API will return until the ICache autoload is disabled.
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_disable(cache_type_t type);

/**
 * @brief Enable cache
 *
 * Enable the ICache or DCache or both.
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_enable(cache_type_t type);

/**
 * @brief Suspend cache
 *
 * Suspend the ICache or DCache or both，suspends the CPU access to cache for a while, without invalidation.
 *
 * @param type  see `cache_type_t`
 *
 * @return  Current status of corresponding Cache(s)
 */
void cache_hal_suspend(cache_type_t type);

/**
 * @brief Resume cache
 *
 * Resume the ICache or DCache or both.
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_resume(cache_type_t type);

/**
 * @brief Check if corresponding cache is enabled or not
 *
 * @param type  see `cache_type_t`
 *
 * @return true: enabled; false: disabled
 */
bool cache_hal_is_cache_enabled(cache_type_t type);

/**
 * @brief Invalidate cache supported addr
 *
 * Invalidate a Cache item for either ICache or DCache.
 *
 * @param vaddr  Start address of the region to be invalidated
 * @param size   Size of the region to be invalidated
 */
void cache_hal_invalidate_addr(uint32_t vaddr, uint32_t size);

#if SOC_CACHE_WRITEBACK_SUPPORTED
/**
 * @brief Writeback cache supported addr
 *
 * Writeback the DCache item to external memory
 *
 * @param vaddr  Start address of the region to writeback
 * @param size   Size of the region to writeback
 */
void cache_hal_writeback_addr(uint32_t vaddr, uint32_t size);
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED

#if SOC_CACHE_FREEZE_SUPPORTED
/**
 * @brief Freeze cache
 *
 * Freeze cache, CPU access to cache will be suspended, until the cache is unfrozen.
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_freeze(cache_type_t type);

/**
 * @brief Unfreeze cache
 *
 * Unfreeze cache, CPU access to cache will be restored
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_unfreeze(cache_type_t type);
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

/**
 * @brief Get cache line size, in bytes
 *
 * @param type  see `cache_type_t`
 *
 * @return cache line size, in bytes
 */
uint32_t cache_hal_get_cache_line_size(cache_type_t type);

#ifdef __cplusplus
}
#endif
