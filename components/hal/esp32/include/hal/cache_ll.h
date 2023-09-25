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

#define CACHE_LL_ID_ALL                                  2   //All of the caches in a type and level, make this value greater than any ID
#define CACHE_LL_LEVEL_INT_MEM                           0   //Cache level for accessing internal mem
#define CACHE_LL_LEVEL_EXT_MEM                           1   //Cache level for accessing external mem
#define CACHE_LL_LEVEL_ALL                               2   //All of the cache levels, make this value greater than any level
#define CACHE_LL_LEVEL_NUMS                              1   //Number of cache levels

/**
 * @brief enable a cache unit
 *
 * @param cache_id  cache ID (when l1 cache is per core)
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_cache(uint32_t cache_id)
{
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);

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
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);
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
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);
    cache_bus_mask_t mask = (cache_bus_mask_t)0;

    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= SOC_IROM0_CACHE_ADDRESS_HIGH) {
        HAL_ASSERT(false);      //out of range
    } else if (vaddr_start >= SOC_IROM0_CACHE_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS2);
    } else if (vaddr_start >= SOC_IRAM1_CACHE_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS1);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IROM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS2 : 0));
    } else if (vaddr_start >= SOC_IRAM0_CACHE_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS0);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM1_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IROM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS2 : 0));
    } else if (vaddr_start >= SOC_DRAM1_CACHE_ADDRESS_LOW) {
        HAL_ASSERT(vaddr_end < SOC_DRAM1_CACHE_ADDRESS_HIGH);  //out of range, vaddr should be consecutive, see `ext_mem_defs.h`
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS1);
    } else if (vaddr_start >= SOC_DROM0_CACHE_ADDRESS_LOW) {
        HAL_ASSERT(vaddr_end < SOC_DROM0_CACHE_ADDRESS_HIGH);  //out of range, vaddr should be consecutive, see `ext_mem_defs.h`
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS0);
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
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);

    uint32_t bus_mask = 0;
    if (cache_id == 0) {
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS0) ? DPORT_PRO_CACHE_MASK_IRAM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS1) ? DPORT_PRO_CACHE_MASK_IRAM1 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS2) ? DPORT_PRO_CACHE_MASK_IROM0 : 0);

        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS0) ? DPORT_PRO_CACHE_MASK_DROM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS1) ? DPORT_PRO_CACHE_MASK_DRAM1 : 0);

        DPORT_REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, bus_mask);
    } else {
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS0) ? DPORT_APP_CACHE_MASK_IRAM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS1) ? DPORT_APP_CACHE_MASK_IRAM1 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS2) ? DPORT_APP_CACHE_MASK_IROM0 : 0);

        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS0) ? DPORT_APP_CACHE_MASK_DROM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS1) ? DPORT_APP_CACHE_MASK_DRAM1 : 0);

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
    cache_bus_mask_t mask = (cache_bus_mask_t)0;
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);
    if (cache_id == 0) {
        uint32_t bus_mask= DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG);
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_PRO_CACHE_MASK_IRAM0)) ? CACHE_BUS_IBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_PRO_CACHE_MASK_IRAM1)) ? CACHE_BUS_IBUS1 : 0));
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_PRO_CACHE_MASK_IROM0)) ? CACHE_BUS_IBUS2 : 0));

        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_PRO_CACHE_MASK_DROM0)) ? CACHE_BUS_DBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_PRO_CACHE_MASK_DRAM1)) ? CACHE_BUS_DBUS1 : 0));
    } else {
        uint32_t bus_mask= DPORT_REG_READ(DPORT_APP_CACHE_CTRL1_REG);
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_APP_CACHE_MASK_IRAM0)) ? CACHE_BUS_IBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_APP_CACHE_MASK_IRAM1)) ? CACHE_BUS_IBUS1 : 0));
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_APP_CACHE_MASK_IROM0)) ? CACHE_BUS_IBUS2 : 0));

        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_APP_CACHE_MASK_DROM0)) ? CACHE_BUS_DBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((!(bus_mask & DPORT_APP_CACHE_MASK_DRAM1)) ? CACHE_BUS_DBUS1 : 0));
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
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);

    uint32_t bus_mask = 0;
    if (cache_id == 0) {
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS0) ? DPORT_PRO_CACHE_MASK_IRAM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS1) ? DPORT_PRO_CACHE_MASK_IRAM1 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS2) ? DPORT_PRO_CACHE_MASK_IROM0 : 0);

        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS0) ? DPORT_PRO_CACHE_MASK_DROM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS1) ? DPORT_PRO_CACHE_MASK_DRAM1 : 0);

        DPORT_REG_SET_BIT(DPORT_PRO_CACHE_CTRL1_REG, bus_mask);
    } else {
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS0) ? DPORT_APP_CACHE_MASK_IRAM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS1) ? DPORT_APP_CACHE_MASK_IRAM1 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_IBUS2) ? DPORT_APP_CACHE_MASK_IROM0 : 0);

        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS0) ? DPORT_APP_CACHE_MASK_DROM0 : 0);
        bus_mask = bus_mask | ((mask & CACHE_BUS_DBUS1) ? DPORT_APP_CACHE_MASK_DRAM1 : 0);

        DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL1_REG, bus_mask);
    }
}

/**
 * @brief Get Cache level and the ID of the vaddr
 *
 * @param vaddr_start       virtual address start
 * @param len               vaddr length
 * @param out_level         cache level
 * @param out_id            cache id
 *
 * @return true for valid
 */
__attribute__((always_inline))
static inline bool cache_ll_vaddr_to_cache_level_id(uint32_t vaddr_start, uint32_t len, uint32_t *out_level, uint32_t *out_id)
{
    bool valid = false;
    uint32_t vaddr_end = vaddr_start + len - 1;

    valid |= ((vaddr_start >= SOC_DROM0_CACHE_ADDRESS_LOW) && (vaddr_end < SOC_DROM0_CACHE_ADDRESS_HIGH)) || ((vaddr_start >= SOC_DRAM1_CACHE_ADDRESS_LOW) && (vaddr_end < SOC_DRAM1_CACHE_ADDRESS_HIGH));
    valid |= ((vaddr_start >= SOC_IRAM0_CACHE_ADDRESS_LOW) && (vaddr_end < SOC_IRAM0_CACHE_ADDRESS_HIGH));

    if (valid) {
        *out_level = 1;
        *out_id = 0;
    }

    return valid;
}

#ifdef __cplusplus
}
#endif
