/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

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

/**
 * @brief Description of one cache profile counter unit
 *
 * A unit is one set of counters observing one traffic stream, e.g. the
 * instruction fetches of core 0 into the L1 cache.
 */
typedef struct {
    const char *name;             /*!< Short human-readable name */
    uint8_t level;                /*!< Cache level (1 or 2) */
    cache_profile_traffic_t traffic;  /*!< Kind of traffic observed */
    int8_t core_id;               /*!< Originating core, -1 if unknown/mixed */
    uint32_t counter_reg[CACHE_PROFILE_COUNTER_MAX];  /*!< Counter registers; 0 if the unit
                                                           does not have that counter */
} cache_profile_counter_unit_t;

#if SOC_CACHE_CNT_SUPPORTED
extern const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM];
#endif

#ifdef __cplusplus
}
#endif
