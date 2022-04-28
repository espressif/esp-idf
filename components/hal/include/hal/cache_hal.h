
/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/cache_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cache init and cache hal context init
 *
 */
void cache_hal_init(void);

/**
 * Disable the ICache or DCache or both, all the items in the corresponding Cache(s) will be invalideated.
 * Next request to these items will trigger a transaction to the external memory (flash / psram)
 *
 * @note If the autoload feature is enabled, this API will return until the ICache autoload is disabled.
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_disable(cache_type_t type);

/**
 * Enable the ICache or DCache or both.
 *
 * @param type  see `cache_type_t`
 */
void cache_hal_enable(cache_type_t type);

#ifdef __cplusplus
}
#endif
