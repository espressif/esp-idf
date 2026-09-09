/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/cache_reg.h"
#include "hal/cache_periph.h"

/*
 * ESP32-H4 cache profile counter units.
 *
 * Single cache level (the flash/PSRAM cache); each core accesses it
 * through an instruction bus (ibus0/ibus1) and a data bus (dbus0/dbus1),
 * each with hit/miss/conflict/next-level counters (next-level write
 * counters on the data buses only).
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
