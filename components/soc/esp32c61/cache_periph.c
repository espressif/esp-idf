/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "soc/cache_reg.h"
#include "soc/cache_periph.h"

/*
 * ESP32-C61 cache profile counter units.
 *
 * Single cache level shared by instructions and data (the flash/PSRAM
 * cache), with two request buses: bus0 carries instruction fetches, bus1
 * carries data accesses (same bus arrangement as the ESP32-C5). bus0's
 * next-level counters use the BUS0 register names while bus1's use the
 * DBUS1 names.
 */

const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM] = {
    {
        .name = "l1-cache-ibus", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_BUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_BUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_BUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_BUS0_ACS_NXTLVL_RD_CNT_REG,
        },
    },
    {
        .name = "l1-cache-dbus", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_BUS1_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_BUS1_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_BUS1_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_WR] = CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG,
        },
    },
};
