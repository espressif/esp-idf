/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include "soc/extmem_reg.h"
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
 */
#if !BOOTLOADER_BUILD
__attribute__((always_inline))
#endif
static inline void cache_ll_l1_enable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    //On esp32s3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2| CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    if (cache_id == 0) {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE0_BUS : 0;
    } else {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE1_BUS : 0;
    }
    REG_CLR_BIT(EXTMEM_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    if (cache_id == 1) {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE0_BUS : 0;
    } else {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE1_BUS : 0;
    }
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, dbus_mask);
}

/**
 * Returns enabled buses for a given core
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 *
 * @return State of enabled buses
 */
__attribute__((always_inline))
static inline cache_bus_mask_t cache_ll_l1_get_enabled_bus(uint32_t cache_id)
{
    cache_bus_mask_t mask = 0;
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    //On esp32s3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first

    uint32_t ibus_mask = REG_READ(EXTMEM_ICACHE_CTRL1_REG);
    if (cache_id == 0) {
        mask |= (!(ibus_mask & EXTMEM_ICACHE_SHUT_CORE0_BUS)) ? CACHE_BUS_IBUS0 : 0;
    } else {
        mask |= (!(ibus_mask & EXTMEM_ICACHE_SHUT_CORE1_BUS)) ? CACHE_BUS_IBUS0 : 0;
    }

    uint32_t dbus_mask = REG_READ(EXTMEM_DCACHE_CTRL1_REG);
    if (cache_id == 1) {
        mask |= (!(dbus_mask & EXTMEM_DCACHE_SHUT_CORE0_BUS)) ? CACHE_BUS_DBUS0 : 0;
    } else {
        mask |= (!(dbus_mask & EXTMEM_DCACHE_SHUT_CORE1_BUS)) ? CACHE_BUS_DBUS0 : 0;
    }

    return mask;
}

/**
 * Disable the Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be disabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    //On esp32s3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2| CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    if (cache_id == 0) {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE0_BUS : 0;
    } else {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE1_BUS : 0;
    }
    REG_SET_BIT(EXTMEM_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    if (cache_id == 1) {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE0_BUS : 0;
    } else {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE1_BUS : 0;
    }
    REG_SET_BIT(EXTMEM_DCACHE_CTRL1_REG, dbus_mask);
}

#ifdef __cplusplus
}
#endif
