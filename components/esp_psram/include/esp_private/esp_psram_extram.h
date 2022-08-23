/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the psram mapped vaddr range
 *
 * @param[out] out_vstart PSRAM virtual address start
 * @param[out] out_vend   PSRAM virtual address end
 *
 * @note [out_vstart, out_vend), `out_vend` isn't included.
 *
 * @return
 *        - ESP_OK                  On success
 *        - ESP_ERR_INVALID_STATE   PSRAM is not initialized successfully
 */
esp_err_t esp_psram_extram_get_mapped_range(intptr_t *out_vstart, intptr_t *out_vend);

/**
 * @brief Get the psram alloced vaddr range
 *
 * @param[out] out_vstart PSRAM virtual address start
 * @param[out] out_vend   PSRAM virtual address end
 *
 * @note [out_vstart, out_vend), `out_vend` isn't included.
 *
 * @return
 *        - ESP_OK                  On success
 *        - ESP_ERR_INVALID_STATE   PSRAM is not initialized successfully
 */
esp_err_t esp_psram_extram_get_alloced_range(intptr_t *out_vstart, intptr_t *out_vend);

/**
 * @brief Add the initialized PSRAM to the heap allocator.
 *
 * @return
 *        - ESP_OK: On success
 *        Other error type, see `heap_caps_add_region`.
 */
esp_err_t esp_psram_extram_add_to_heap_allocator(void);

/**
 * @brief Reserve a pool of internal memory for specific DMA/internal allocations
 *
 * @param size Size of reserved pool in bytes
 *
 * @return
 *          - ESP_OK:         On success
 *          - ESP_ERR_NO_MEM: When no memory available for pool
 */
esp_err_t esp_psram_extram_reserve_dma_pool(size_t size);

/**
 * @brief Memory test for PSRAM. Should be called after PSRAM is initialized and
 * (in case of a dual-core system) the app CPU is online. This test overwrites the
 * memory with crap, so do not call after e.g. the heap allocator has stored important
 * stuff in PSRAM.
 *
 * @return true on success, false on failed memory test
 */
bool esp_psram_extram_test(void);

#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief Force a writeback of the data in the PSRAM cache. This is to be called whenever
 * cache is disabled, because disabling cache on the ESP32 discards the data in the PSRAM
 * cache.
 *
 * This is meant for use from within the SPI flash code.
 */
void esp_psram_extram_writeback_cache(void);
#endif

#ifdef __cplusplus
}
#endif
