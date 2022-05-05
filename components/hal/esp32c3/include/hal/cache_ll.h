/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CACHE_LL_DEFAULT_IBUS_MASK    CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK    CACHE_BUS_DBUS0


/**
 * @brief Get the buses of a particular cache that are mapped to a virtual address range
 *
 * External virtual address can only be accessed when the involved cache buses are enabled.
 * This API is to get the cache buses where the memory region (from `vaddr_start` to `vaddr_start + len`) reside.
 *
 * @param cache_id          cache ID (when l1 cache is per core)
 * @param vaddr_start       virtual address start
 * @param len               vaddr length
 */
#if !BOOTLOADER_BUILD
__attribute__((always_inline))
#endif
static inline cache_bus_mask_t cache_ll_l1_get_bus(uint32_t cache_id, uint32_t vaddr_start, uint32_t len)
{
    HAL_ASSERT(cache_id == 0);
    cache_bus_mask_t mask = 0;

    uint32_t vaddr_end = vaddr_start + len;
    if (vaddr_start >= IRAM0_CACHE_ADDRESS_LOW && vaddr_end <= IRAM0_CACHE_ADDRESS_HIGH) {
        mask |= CACHE_BUS_IBUS0;
    } else if (vaddr_start >= DRAM0_CACHE_ADDRESS_LOW && vaddr_end <= DRAM0_CACHE_ADDRESS_HIGH) {
        mask |= CACHE_BUS_DBUS0;
    } else {
        HAL_ASSERT(0);          //Out of region
    }

    return mask;
}

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
    HAL_ASSERT(cache_id == 0);
    //On esp32c3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2| CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_IBUS : 0;
    REG_CLR_BIT(EXTMEM_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_ICACHE_SHUT_DBUS : 0;
    REG_CLR_BIT(EXTMEM_ICACHE_CTRL1_REG, dbus_mask);
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
    HAL_ASSERT(cache_id == 0);
    //On esp32c3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2| CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_IBUS : 0;
    REG_SET_BIT(EXTMEM_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_ICACHE_SHUT_DBUS : 0;
    REG_SET_BIT(EXTMEM_ICACHE_CTRL1_REG, dbus_mask);
}

#ifdef __cplusplus
}
#endif
