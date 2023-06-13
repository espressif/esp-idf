/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
 * @brief enable a cache unit
 *
 * @param cache_id  cache ID (when l1 cache is per core)
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_cache(uint32_t cache_id)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);

    if (cache_id == 0) {
        DPORT_REG_SET_BIT(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_CACHE_ENABLE);
    } else {
        DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE);
    }
}

/**
 * @brief disable a cache unit
 *
 * @param cache_id  cache ID (when l1 cache is per core)
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_cache(uint32_t cache_id)
{
    if (cache_id == 0) {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_PRO_DCACHE_DBUG0_REG, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1){
            ;
        }
        DPORT_REG_CLR_BIT(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_CACHE_ENABLE);
    } else {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_APP_DCACHE_DBUG0_REG, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1){
            ;
        }
        DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE);
    }
}

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
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    (void) type;    //On 32 it shares between I and D cache

    bool enabled;
    if (cache_id == 0) {
        enabled = DPORT_REG_GET_BIT(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_CACHE_ENABLE);
    } else {
        enabled = DPORT_REG_GET_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE);
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
    if (cache_id == 0) {
        uint32_t bus_mask= DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG);
        mask |= (!(bus_mask & DPORT_PRO_CACHE_MASK_IRAM0)) ? CACHE_BUS_IBUS0 : 0;
        mask |= (!(bus_mask & DPORT_PRO_CACHE_MASK_IRAM1)) ? CACHE_BUS_IBUS1 : 0;
        mask |= (!(bus_mask & DPORT_PRO_CACHE_MASK_IROM0)) ? CACHE_BUS_IBUS2 : 0;

        mask |= (!(bus_mask & DPORT_PRO_CACHE_MASK_DROM0)) ? CACHE_BUS_DBUS0 : 0;
        mask |= (!(bus_mask & DPORT_PRO_CACHE_MASK_DRAM1)) ? CACHE_BUS_DBUS1 : 0;
    } else {
        uint32_t bus_mask= DPORT_REG_READ(DPORT_APP_CACHE_CTRL1_REG);
        mask |= (!(bus_mask & DPORT_APP_CACHE_MASK_IRAM0)) ? CACHE_BUS_IBUS0 : 0;
        mask |= (!(bus_mask & DPORT_APP_CACHE_MASK_IRAM1)) ? CACHE_BUS_IBUS1 : 0;
        mask |= (!(bus_mask & DPORT_APP_CACHE_MASK_IROM0)) ? CACHE_BUS_IBUS2 : 0;

        mask |= (!(bus_mask & DPORT_APP_CACHE_MASK_DROM0)) ? CACHE_BUS_DBUS0 : 0;
        mask |= (!(bus_mask & DPORT_APP_CACHE_MASK_DRAM1)) ? CACHE_BUS_DBUS1 : 0;
    }
    return mask;
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
