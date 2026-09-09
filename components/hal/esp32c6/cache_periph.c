/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/extmem_reg.h"
#include "hal/cache_periph.h"

/*
 * ESP32-C6 cache profile counter units.
 *
 * Single cache level shared by instructions and data (the flash cache,
 * 32-byte lines), with one instruction bus and one data bus. Counter
 * semantics match the ESP32-P4. There is one next-level counter per bus
 * (no read/write split), mapped here to line_fills. The cache is
 * read-only (no PSRAM, no write-back — see SOC_CACHE_WRITEBACK_SUPPORTED),
 * so there are no write-back counters.
 */

const cache_profile_counter_unit_t cache_periph_profile_counter_units[SOC_CACHE_CNT_UNITS_NUM] = {
    {
        .name = "l1-cache-ibus", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_INST, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = EXTMEM_L1_IBUS_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = EXTMEM_L1_IBUS_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = EXTMEM_L1_IBUS_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = EXTMEM_L1_IBUS_ACS_NXTLVL_CNT_REG,
        },
    },
    {
        .name = "l1-cache-dbus", .level = 1, .traffic = CACHE_PROFILE_TRAFFIC_DATA, .core_id = 0,
        .counter_reg = {
            [CACHE_PROFILE_COUNTER_HIT] = EXTMEM_L1_DBUS_ACS_HIT_CNT_REG,
            [CACHE_PROFILE_COUNTER_MISS] = EXTMEM_L1_DBUS_ACS_MISS_CNT_REG,
            [CACHE_PROFILE_COUNTER_CONFLICT] = EXTMEM_L1_DBUS_ACS_CONFLICT_CNT_REG,
            [CACHE_PROFILE_COUNTER_NXTLVL_RD] = EXTMEM_L1_DBUS_ACS_NXTLVL_CNT_REG,
        },
    },
};
