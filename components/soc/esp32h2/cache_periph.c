/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "soc/cache_reg.h"
#include "soc/cache_periph.h"

/*
 * ESP32-H2 cache profile counter units.
 *
 * Single cache level shared by instructions and data (the flash cache,
 * 32-byte lines), with two request buses: bus0 carries instruction
 * fetches, bus1 carries data accesses. There is one next-level counter
 * per bus (no read/write split), mapped here to line_fills. The cache is
 * read-only (no PSRAM, no write-back — see SOC_CACHE_WRITEBACK_SUPPORTED),
 * so there are no write-back counters.
 */

const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM] = {
    {
        .name = "l1-cache-ibus", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_BUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_BUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_BUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_BUS0_ACS_NXTLVL_CNT_REG,
        },
    },
    {
        .name = "l1-cache-dbus", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_BUS1_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_BUS1_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_BUS1_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_BUS1_ACS_NXTLVL_CNT_REG,
        },
    },
};
