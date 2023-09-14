
/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/cache_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cache init and cache hal context init
 */
void cache_hal_init(void);

/**
 * @brief Disable Cache
 *
 * Disable the ICache or DCache or both, of a certain level or all levels.
 * All the items in the corresponding Cache(s) will be invalideated.
 * Next request to these items will trigger a transaction to the physical memory
 *
 * @note If the autoload feature is enabled, this API will return until the ICache autoload is disabled.
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 */
void cache_hal_disable(cache_type_t type, uint32_t cache_level);

/**
 * @brief Enable Cache
 *
 * Enable the ICache or DCache or both, of a certain level or all levels.
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 */
void cache_hal_enable(cache_type_t type, uint32_t cache_level);

/**
 * @brief Suspend Cache
 *
 * Suspend the ICache or DCache or both, of a certain level or all levels.
 * This API suspends the CPU access to cache for a while, without invalidation.
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 */
void cache_hal_suspend(cache_type_t type, uint32_t cache_level);

/**
 * @brief Resume Cache
 *
 * Resume the ICache or DCache or both, of a certain level or all levels.
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 */
void cache_hal_resume(cache_type_t type, uint32_t cache_level);

/**
 * @brief Check if corresponding cache is enabled or not
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 *
 * @return true: enabled; false: disabled
 */
bool cache_hal_is_cache_enabled(cache_type_t type, uint32_t cache_level);

/**
 * @brief Invalidate Cache supported addr
 *
 * Invalidate a Cache item for either ICache or DCache.
 *
 * @param vaddr  Start address of the region to be invalidated
 * @param size   Size of the region to be invalidated
 *
 * @return       True for valid address. No operation if invalid
 */
bool cache_hal_invalidate_addr(uint32_t vaddr, uint32_t size);

#if SOC_CACHE_WRITEBACK_SUPPORTED
/**
 * @brief Writeback Cache supported addr
 *
 * Writeback the DCache item to external memory
 *
 * @param vaddr  Start address of the region to writeback
 * @param size   Size of the region to writeback
 *
 * @return       True for valid address. No operation if invalid
 */
bool cache_hal_writeback_addr(uint32_t vaddr, uint32_t size);
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED

#if SOC_CACHE_FREEZE_SUPPORTED
/**
 * @brief Freeze Cache
 *
 * Freeze cache, CPU access to cache will be suspended, until the cache is unfrozen.
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 */
void cache_hal_freeze(cache_type_t type, uint32_t cache_level);

/**
 * @brief Unfreeze cache
 *
 * Unfreeze cache, CPU access to cache will be restored
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s)
 */
void cache_hal_unfreeze(cache_type_t type, uint32_t cache_level);
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

/**
 * @brief Get cache line size, in bytes
 *
 * @param type         see `cache_type_t`
 * @param cache_level  Level of the Cache(s) *
 * @return cache line size, in bytes
 */
uint32_t cache_hal_get_cache_line_size(cache_type_t type, uint32_t cache_level);

/**
 * @brief Get Cache level and the ID of the vaddr
 *
 * @param vaddr_start       virtual address start
 * @param len               vaddr length
 * @param out_level         cache level
 * @param out_id            cache id
 *
 * @return true for valid, false for invalid addr or null pointer
 */
bool cache_hal_vaddr_to_cache_level_id(uint32_t vaddr_start, uint32_t len, uint32_t *out_level, uint32_t *out_id);

#ifdef __cplusplus
}
#endif
