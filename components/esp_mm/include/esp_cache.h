/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
 * @brief Do an invalidation with the values that just written
 */
#define ESP_CACHE_MSYNC_FLAG_INVALIDATE    BIT(0)
/**
 * @brief Allow writeback a block that are not aligned to the data cache line size
 */
#define ESP_CACHE_MSYNC_FLAG_UNALIGNED     BIT(1)


/**
 * @brief Memory sync between Cache and external memory
 *
 * - For cache writeback supported chips (you can refer to SOC_CACHE_WRITEBACK_SUPPORTED in soc_caps.h)
 *   - this API will do a writeback to synchronise between cache and the PSRAM
 *   - with ESP_CACHE_MSYNC_FLAG_INVALIDATE, this API will also invalidate the values that just written
 *   - note: although ESP32 is with PSRAM, but cache writeback isn't supported, so this API will do nothing on ESP32
 * - For other chips, this API will do nothing. The out-of-sync should be already dealt by the SDK
 *
 * This API is cache-safe and thread-safe
 *
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
 *                  - If this chip doesn't support cache writeback, if the input addr is a cache supported one, this API will return ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument, not cache supported addr, see printed logs
 */
esp_err_t esp_cache_msync(void *addr, size_t size, int flags);

#ifdef __cplusplus
}
#endif
