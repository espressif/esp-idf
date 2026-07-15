/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "soc/cache_reg.h"
#include "soc/cache_periph.h"

/*
 * ESP32-S31 cache profile counter units.
 *
 * The register layout matches the ESP32-P4: per-bus hit/miss/conflict and
 * next-level read/write counters, with the same counter semantics. There
 * is a single cache level (the flash/PSRAM cache, 64-byte lines); each
 * core accesses it through an instruction bus (ibus0/ibus1) and a data
 * bus (dbus0/dbus1). Only the CPU request buses are exposed here.
 */

const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM] = {
    {
        .name = "l1-cache-inst-core0", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_IBUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_IBUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_IBUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_IBUS0_ACS_NXTLVL_RD_CNT_REG,
        },
    },
    {
        .name = "l1-cache-inst-core1", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 1,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_IBUS1_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_IBUS1_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_IBUS1_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_IBUS1_ACS_NXTLVL_RD_CNT_REG,
        },
    },
    {
        .name = "l1-cache-data-core0", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_DBUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_DBUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_DBUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_DBUS0_ACS_NXTLVL_RD_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_WR] = CACHE_L1_DBUS0_ACS_NXTLVL_WR_CNT_REG,
        },
    },
    {
        .name = "l1-cache-data-core1", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 1,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_DBUS1_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_DBUS1_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_DBUS1_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_WR] = CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG,
        },
    },
};
