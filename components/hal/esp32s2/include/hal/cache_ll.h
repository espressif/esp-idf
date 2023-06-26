/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include <stdbool.h>
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CACHE_LL_DEFAULT_IBUS_MASK    CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK    CACHE_BUS_IBUS2

/**
 * @brief Get the status of cache if it is enabled or not
 *
 * @param   cache_id    cache ID (when l1 cache is per core)
 * @param   type        see `cache_type_t`
 * @return  enabled or not
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_cache_enabled(uint32_t cache_id, cache_type_t type)
{
    HAL_ASSERT(cache_id == 0);

    bool enabled;
    if (type == CACHE_TYPE_INSTRUCTION) {
        enabled = REG_GET_BIT(EXTMEM_PRO_ICACHE_CTRL_REG, EXTMEM_PRO_ICACHE_ENABLE);
    } else if (type == CACHE_TYPE_DATA) {
        enabled = REG_GET_BIT(EXTMEM_PRO_DCACHE_CTRL_REG, EXTMEM_PRO_DCACHE_ENABLE);
    } else {
        enabled = REG_GET_BIT(EXTMEM_PRO_ICACHE_CTRL_REG, EXTMEM_PRO_ICACHE_ENABLE);
        enabled = enabled && REG_GET_BIT(EXTMEM_PRO_DCACHE_CTRL_REG, EXTMEM_PRO_DCACHE_ENABLE);
    }
    return enabled;
}

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
    (void)cache_id;

    cache_bus_mask_t mask = 0;
    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= IRAM1_ADDRESS_LOW) {
        mask |= CACHE_BUS_IBUS1;
    } else if (vaddr_start >= IRAM0_CACHE_ADDRESS_LOW) {
        mask |= CACHE_BUS_IBUS0;
        mask |= (vaddr_end >= IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0;
    } else if (vaddr_start >= DRAM0_CACHE_ADDRESS_LOW) {
        mask |= CACHE_BUS_DBUS0;
        mask |= (vaddr_end >= IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0;
        mask |= (vaddr_end >= IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0;
    } else if (vaddr_start >= DRAM1_ADDRESS_LOW) {
        mask |= CACHE_BUS_DBUS1;
        mask |= (vaddr_end >= DRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS0 : 0;
        mask |= (vaddr_end >= IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0;
        mask |= (vaddr_end >= IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0;
    } else if (vaddr_start >= DPORT_CACHE_ADDRESS_LOW) {
        mask |= CACHE_BUS_DBUS2;
        mask |= (vaddr_end >= DRAM1_ADDRESS_LOW) ? CACHE_BUS_DBUS1 : 0;
        mask |= (vaddr_end >= DRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS0 : 0;
        mask |= (vaddr_end >= IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0;
        mask |= (vaddr_end >= IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0;
    } else if (vaddr_start >= DROM0_ADDRESS_LOW) {
        mask |= CACHE_BUS_IBUS2;
        mask |= (vaddr_end >= DPORT_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS2 : 0;
        mask |= (vaddr_end >= DRAM1_ADDRESS_LOW) ? CACHE_BUS_DBUS1 : 0;
        mask |= (vaddr_end >= DRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS0 : 0;
        mask |= (vaddr_end >= IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0;
        mask |= (vaddr_end >= IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0;
    } else {
        abort();
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
    (void)cache_id;

    uint32_t ibus_mask = 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_PRO_ICACHE_MASK_IRAM0 : 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS1) ? EXTMEM_PRO_ICACHE_MASK_IRAM1 : 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS2) ? EXTMEM_PRO_ICACHE_MASK_DROM0 : 0;
    REG_CLR_BIT(EXTMEM_PRO_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_PRO_DCACHE_MASK_DRAM0 : 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS1) ? EXTMEM_PRO_DCACHE_MASK_DRAM1 : 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS2) ? EXTMEM_PRO_DCACHE_MASK_DPORT : 0;
    REG_CLR_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, dbus_mask);
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
    (void)cache_id;

    uint32_t ibus_mask = 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_PRO_ICACHE_MASK_IRAM0 : 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS1) ? EXTMEM_PRO_ICACHE_MASK_IRAM1 : 0;
    ibus_mask |= (mask & CACHE_BUS_IBUS2) ? EXTMEM_PRO_ICACHE_MASK_DROM0 : 0;
    REG_SET_BIT(EXTMEM_PRO_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_PRO_DCACHE_MASK_DRAM0 : 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS1) ? EXTMEM_PRO_DCACHE_MASK_DRAM1 : 0;
    dbus_mask |= (mask & CACHE_BUS_DBUS2) ? EXTMEM_PRO_DCACHE_MASK_DPORT : 0;
    REG_SET_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, dbus_mask);
}


#ifdef __cplusplus
}
#endif
