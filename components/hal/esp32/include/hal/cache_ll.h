/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include <stdbool.h>
#include "soc/dport_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/assert.h"


#ifdef __cplusplus
extern "C" {
#endif

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
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    cache_bus_mask_t mask = 0;

    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= IROM0_CACHE_ADDRESS_HIGH) {
        HAL_ASSERT(false);      //out of range
    } else if (vaddr_start >= IROM0_CACHE_ADDRESS_LOW) {
        mask |= CACHE_BUS_IBUS2;
    } else if (vaddr_start >= IRAM1_CACHE_ADDRESS_LOW) {
        mask |= CACHE_BUS_IBUS1;
        mask |= (vaddr_end >= IROM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS2 : 0;
    } else if (vaddr_start >= IRAM0_CACHE_ADDRESS_LOW) {
        mask |= CACHE_BUS_IBUS0;
        mask |= (vaddr_end >= IRAM1_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0;
        mask |= (vaddr_end >= IROM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS2 : 0;
    } else if (vaddr_start >= DRAM1_CACHE_ADDRESS_LOW) {
        HAL_ASSERT(vaddr_end < DRAM1_CACHE_ADDRESS_HIGH);  //out of range, vaddr should be consecutive, see `ext_mem_defs.h`
        mask |= CACHE_BUS_DBUS1;
    } else if (vaddr_start >= DROM0_CACHE_ADDRESS_LOW) {
        HAL_ASSERT(vaddr_end < DROM0_CACHE_ADDRESS_HIGH);  //out of range, vaddr should be consecutive, see `ext_mem_defs.h`
        mask |= CACHE_BUS_DBUS0;
    } else {
        HAL_ASSERT(false);
    }

    return mask;
}

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
