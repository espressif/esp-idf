/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/cache_reg.h"
#include "hal/cache_periph.h"

/*
 * ESP32-P4 cache profile counter units.
 *
 * Topology: per-core L1 instruction caches (ibus0/ibus1) and a shared L1
 * data cache with one port per core (dbus0/dbus1). The L2 (flash/PSRAM)
 * cache reports all instruction traffic on its ibus0 counters and all data
 * traffic on its dbus0 counters, regardless of the originating core.
 */

const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM] = {
    {
        .name = "l1-icache-core0", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_IBUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_IBUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_IBUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_IBUS0_ACS_NXTLVL_RD_CNT_REG,
        },
    },
    {
        .name = "l1-icache-core1", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 1,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_IBUS1_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_IBUS1_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_IBUS1_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_IBUS1_ACS_NXTLVL_RD_CNT_REG,
        },
    },
    {
        .name = "l1-dcache-core0", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_DBUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_DBUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_DBUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_DBUS0_ACS_NXTLVL_RD_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_WR] = CACHE_L1_DBUS0_ACS_NXTLVL_WR_CNT_REG,
        },
    },
    {
        .name = "l1-dcache-core1", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 1,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L1_DBUS1_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L1_DBUS1_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L1_DBUS1_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_WR] = CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG,
        },
    },
    {
        .name = "l2-cache-inst", .level = 2, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = -1,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L2_IBUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L2_IBUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L2_IBUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L2_IBUS0_ACS_NXTLVL_RD_CNT_REG,
        },
    },
    {
        .name = "l2-cache-data", .level = 2, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = -1,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = CACHE_L2_DBUS0_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = CACHE_L2_DBUS0_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = CACHE_L2_DBUS0_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = CACHE_L2_DBUS0_ACS_NXTLVL_RD_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_WR] = CACHE_L2_DBUS0_ACS_NXTLVL_WR_CNT_REG,
        },
    },
};
