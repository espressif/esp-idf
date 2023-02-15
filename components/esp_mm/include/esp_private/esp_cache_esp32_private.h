/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private header for cache drivers, where cache functionality requires other components
 *
 * @note Now only esp32, can be applied to other similar chips
 */
typedef struct cache_driver_s cache_driver_t;

/**
 * @brief Cache driver
 */
struct cache_driver_s {

    /**
     * @brief Cache flush
     *
     * @param[in] cpu_no    CPU id
     */
    void (*cache_flush)(int cpu_no);

    /**
     * @brief Cache writeback to psram
     */
    void (*cache_writeback_psram)(void);
};

/**
 * @brief Register cache writeback
 *
 * @param[in] func    Cache driver
 */
void cache_register_writeback(cache_driver_t *func);

/**
 * @brief Cache sync
 *
 * @note This API only do cache sync, but doesn't guarantee concurrent access to cache
 * @note Do not use in your application
 */
void cache_sync(void);


#ifdef __cplusplus
}
#endif
