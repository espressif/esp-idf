/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include <stdbool.h>
#include "soc/dport_reg.h"
#include "hal/cache_types.h"
#include "hal/assert.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable the Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be enabled
 * @param enable      1: enable; 0: disable
 */
#if !BOOTLOADER_BUILD
__attribute__((always_inline))
#endif
static inline void cache_ll_l1_enable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
    (void) mask;
    HAL_ASSERT(cache_id == 0 || cache_id == 1);

    uint32_t bus_mask = 0;
    if (cache_id == 0) {
        bus_mask |= (mask & CACHE_BUS_IBUS0) ? DPORT_PRO_CACHE_MASK_IRAM0 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS1) ? DPORT_PRO_CACHE_MASK_IRAM1 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS2) ? DPORT_PRO_CACHE_MASK_IROM0 : 0;

        bus_mask |= (mask & CACHE_BUS_DBUS0) ? DPORT_PRO_CACHE_MASK_DROM0 : 0;
        bus_mask |= (mask & CACHE_BUS_DBUS1) ? DPORT_PRO_CACHE_MASK_DRAM1 : 0;

        DPORT_REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, bus_mask);
    } else {
        bus_mask |= (mask & CACHE_BUS_IBUS0) ? DPORT_APP_CACHE_MASK_IRAM0 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS1) ? DPORT_APP_CACHE_MASK_IRAM1 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS2) ? DPORT_APP_CACHE_MASK_IROM0 : 0;

        bus_mask |= (mask & CACHE_BUS_DBUS0) ? DPORT_APP_CACHE_MASK_DROM0 : 0;
        bus_mask |= (mask & CACHE_BUS_DBUS1) ? DPORT_APP_CACHE_MASK_DRAM1 : 0;

        DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, bus_mask);
    }
}

/**
 * Disable the Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be enabled
 * @param enable      1: enable; 0: disable
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
    (void) mask;
    HAL_ASSERT(cache_id == 0 || cache_id == 1);

    uint32_t bus_mask = 0;
    if (cache_id == 0) {
        bus_mask |= (mask & CACHE_BUS_IBUS0) ? DPORT_PRO_CACHE_MASK_IRAM0 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS1) ? DPORT_PRO_CACHE_MASK_IRAM1 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS2) ? DPORT_PRO_CACHE_MASK_IROM0 : 0;

        bus_mask |= (mask & CACHE_BUS_DBUS0) ? DPORT_PRO_CACHE_MASK_DROM0 : 0;
        bus_mask |= (mask & CACHE_BUS_DBUS1) ? DPORT_PRO_CACHE_MASK_DRAM1 : 0;

        DPORT_REG_SET_BIT(DPORT_PRO_CACHE_CTRL1_REG, bus_mask);
    } else {
        bus_mask |= (mask & CACHE_BUS_IBUS0) ? DPORT_APP_CACHE_MASK_IRAM0 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS1) ? DPORT_APP_CACHE_MASK_IRAM1 : 0;
        bus_mask |= (mask & CACHE_BUS_IBUS2) ? DPORT_APP_CACHE_MASK_IROM0 : 0;

        bus_mask |= (mask & CACHE_BUS_DBUS0) ? DPORT_APP_CACHE_MASK_DROM0 : 0;
        bus_mask |= (mask & CACHE_BUS_DBUS1) ? DPORT_APP_CACHE_MASK_DRAM1 : 0;

        DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL1_REG, bus_mask);
    }
}

#ifdef __cplusplus
}
#endif
