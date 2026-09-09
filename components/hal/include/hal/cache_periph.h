/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/cache_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CACHE_CNT_SUPPORTED
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

extern const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM];
#endif

#ifdef __cplusplus
}
#endif
