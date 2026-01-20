/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
 * Cache msync flags
 */
/**
 * @brief Do an invalidation
 * - For cache-to-memory (C2M) direction: setting this flag will start an invalidation after the cache writeback operation
 * - For memory-to-cache (M2C) direction: setting / unsetting this flag will behave similarly, trigger an invalidation
 */
#define ESP_CACHE_MSYNC_FLAG_INVALIDATE    BIT(0)
/**
 * @brief Allow msync to a address block that are not aligned to the data cache line size
 */
#define ESP_CACHE_MSYNC_FLAG_UNALIGNED     BIT(1)
/**
 * @brief Cache msync direction: from Cache to memory
 * @note If you don't set direction (ESP_CACHE_MSYNC_FLAG_DIR_x flags), it is by default cache-to-memory (C2M) direction
 */
#define ESP_CACHE_MSYNC_FLAG_DIR_C2M       BIT(2)
/**
 * @brief Cache msync direction: from memory to Cache
 */
#define ESP_CACHE_MSYNC_FLAG_DIR_M2C       BIT(3)
/**
 * @brief Cache msync type: data
 * @note If you don't set type (ESP_CACHE_MSYNC_FLAG_TYPE_x flags), it is by default data type
 */
#define ESP_CACHE_MSYNC_FLAG_TYPE_DATA     BIT(4)
/**
 * @brief Cache msync type: instruction
 */
#define ESP_CACHE_MSYNC_FLAG_TYPE_INST     BIT(5)

/**
 * @brief Memory sync between Cache and storage memory
 *
 *
 * For cache-to-memory (C2M) direction:
 * - For cache writeback supported chips (you can refer to SOC_CACHE_WRITEBACK_SUPPORTED in soc_caps.h)
 *   - This API will do a writeback to synchronise between cache and storage memory
 *   - With ESP_CACHE_MSYNC_FLAG_INVALIDATE, this API will also invalidate the values that just written
 *   - Note: although ESP32 is with PSRAM, but cache writeback isn't supported, so this API will do nothing on ESP32
 * - For other chips, this API will do nothing. The out-of-sync should be already dealt by the SDK
 *
 * For memory-to-cache (M2C) direction:
 * - This API will by default do an invalidation
 *
 * This API is cache-safe and thread-safe
 *
 * @note If you don't set direction (ESP_CACHE_MSYNC_FLAG_DIR_x flags), this API is by default C2M direction
 * @note If you don't set type (ESP_CACHE_MSYNC_FLAG_TYPE_x flags), this API is by default doing msync for data
 * @note You should not call this during any Flash operations (e.g. esp_flash APIs, nvs and some other APIs that are based on esp_flash APIs)
 * @note If XIP_From_PSRAM is enabled (by enabling both CONFIG_SPIRAM_FETCH_INSTRUCTIONS and CONFIG_SPIRAM_RODATA), you can call this API during Flash operations
 *
 * @param[in] addr   Starting address to do the msync
 * @param[in] size   Size to do the msync
 * @param[in] flags  Flags, see `ESP_CACHE_MSYNC_FLAG_x`
 *
 * @return
 *        - ESP_OK:
 *                  - Successful msync
 *                  - For C2M direction, if this chip doesn't support cache writeback, if the input addr is a cache supported one, this API will return ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument, not cache supported addr, see printed logs
 *        - ESP_ERR_NOT_SUPPORTED: Vaddr is not in cacheable range, API will do nothing
 */
esp_err_t esp_cache_msync(void *addr, size_t size, int flags);

/**
 * @brief Get the cache line size by address
 *
 * @param[in] addr The buffer address to examine
 *
 * @return The cache line size in bytes.
 *         Return 0 if the memory (that the address points to) is not cacheable.
 */
size_t esp_cache_get_line_size_by_addr(const void *addr);

#ifdef __cplusplus
}
#endif
