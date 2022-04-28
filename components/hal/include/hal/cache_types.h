/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CACHE_TYPE_DATA,
    CACHE_TYPE_INSTRUCTION,
    CACHE_TYPE_ALL  //This means both ICache and DCache will be used. On some chips, I/D are controlled by a shared Cache. Also use this enum under this condition. See `SOC_SHARED_IDCACHE_SUPPORTED`.
} cache_type_t;

/**
 * @brief Ibuses and Dbuses.
 *
 * @note
 * These enumurations are abstract concepts. Virtual address reside in one of these buses.
 * Therefore, use `cache_ll_l1_get_bus(cache_id, vaddr_start, len)` to convert your vaddr into buses first
 */
typedef enum {
    CACHE_BUS_IBUS0 = BIT(0),
    CACHE_BUS_IBUS1 = BIT(1),
    CACHE_BUS_IBUS2 = BIT(2),
    CACHE_BUS_DBUS0 = BIT(3),
    CACHE_BUS_DBUS1 = BIT(4),
    CACHE_BUS_DBUS2 = BIT(5),
} cache_bus_mask_t;

#ifdef __cplusplus
}
#endif
