/*
 * SPDX-FileCopyrightText: 2010-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Kind of traffic observed by a cache profile counter unit
 */
typedef enum {
    CACHE_PROFILE_TRAFFIC_INST,     /*!< Instruction fetches */
    CACHE_PROFILE_TRAFFIC_DATA,     /*!< Data accesses */
    CACHE_PROFILE_TRAFFIC_UNIFIED,  /*!< Mixed/unknown (unified request bus) */
} cache_profile_traffic_t;

/**
 * @brief One of the counters of a cache profile counter unit
 */
typedef enum {
    CACHE_PROFILE_COUNTER_HIT,         /*!< Completed accesses ("hit" counter) */
    CACHE_PROFILE_COUNTER_MISS,        /*!< Miss stall events ("miss" counter) */
    CACHE_PROFILE_COUNTER_CONFLICT,    /*!< Requester conflicts */
    CACHE_PROFILE_COUNTER_NXTLVL_RD,   /*!< Line fills from the next level */
    CACHE_PROFILE_COUNTER_NXTLVL_WR,   /*!< Write-backs to the next level */
    CACHE_PROFILE_COUNTER_MAX,
} cache_profile_counter_t;

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
 * Therefore, use `cache_ll_l1_get_bus(bus_id, vaddr_start, len)` to convert your vaddr into buses first
 */
typedef enum {
    CACHE_BUS_IBUS0 = BIT(0),
    CACHE_BUS_IBUS1 = BIT(1),
    CACHE_BUS_IBUS2 = BIT(2),
    CACHE_BUS_DBUS0 = BIT(3),
    CACHE_BUS_DBUS1 = BIT(4),
    CACHE_BUS_DBUS2 = BIT(5),
} cache_bus_mask_t;

/**
 * @brief Preload order
 */
typedef enum {
    CACHE_PRELOAD_ORDER_ASCENDING,
    CACHE_PRELOAD_ORDER_DESCENDING,
} cache_preload_order_t;

#ifdef __cplusplus
}
#endif
