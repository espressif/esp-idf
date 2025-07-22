/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include <stdbool.h>
#include "soc/cache_reg.h"
#include "soc/cache_struct.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/assert.h"
#include "esp32c61/rom/cache.h"

#ifdef __cplusplus
extern "C" {
#endif
#define CACHE_LL_ENABLE_DISABLE_STATE_SW            1   //There's no register indicating cache enable/disable state, we need to use software way for this state.

#define CACHE_LL_DEFAULT_IBUS_MASK                  CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK                  CACHE_BUS_DBUS0

#define CACHE_LL_ID_ALL                             1   //All of the caches in a type and level, make this value greater than any ID
#define CACHE_LL_LEVEL_INT_MEM                      0   //Cache level for accessing internal mem
#define CACHE_LL_LEVEL_EXT_MEM                      1   //Cache level for accessing external mem
#define CACHE_LL_LEVEL_ALL                          2   //All of the cache levels, make this value greater than any level
#define CACHE_LL_LEVEL_NUMS                         1   //Number of cache levels
#define CACHE_LL_L1_ICACHE_AUTOLOAD                 (1<<0)

#define CACHE_LL_L1_ACCESS_EVENT_MASK               (0x1f)


/**
 * @brief Check if Cache auto preload is enabled or not.
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_is_cache_autoload_enabled(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);
    bool enabled = false;
    if (REG_GET_BIT(CACHE_L1_CACHE_AUTOLOAD_CTRL_REG, CACHE_L1_CACHE_AUTOLOAD_ENA)) {
        enabled = true;
    }
    return enabled;
}

/**
 * @brief Disable Cache
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_disable_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    (void) type;
    Cache_Disable_Cache();
}

/**
 * @brief Enable Cache
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 * @param data_autoload_en  data autoload enabled or not
 * @param inst_autoload_en  inst autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_enable_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id, bool inst_autoload_en, bool data_autoload_en)
{
    Cache_Enable_Cache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Suspend Cache
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_suspend_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    Cache_Suspend_Cache();
}

/**
 * @brief Resume Cache
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 * @param data_autoload_en  data autoload enabled or not
 * @param inst_autoload_en  inst autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_resume_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id, bool inst_autoload_en, bool data_autoload_en)
{
    Cache_Resume_Cache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Invalidate cache supported addr
 *
 * Invalidate a cache item
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 * @param vaddr             start address of the region to be invalidated
 * @param size              size of the region to be invalidated
 */
__attribute__((always_inline))
static inline void cache_ll_invalidate_addr(uint32_t cache_level, cache_type_t type, uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    Cache_Invalidate_Addr(vaddr, size);
}

/**
 * @brief Invalidate all
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_invalidate_all(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    Cache_Invalidate_All();
}

/**
 * @brief Writeback cache supported addr
 *
 * Writeback a cache item
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 * @param vaddr             start address of the region to be written back
 * @param size              size of the region to be written back
 */
__attribute__((always_inline))
static inline void cache_ll_writeback_addr(uint32_t cache_level, cache_type_t type, uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    Cache_WriteBack_Addr(vaddr, size);
}

/**
 * @brief Freeze Cache
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_freeze_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    Cache_Freeze_Enable(CACHE_FREEZE_ACK_BUSY);
}

/**
 * @brief Unfreeze Cache
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_unfreeze_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    Cache_Freeze_Disable();
}

/**
 * @brief Get Cache line size, in bytes
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 *
 * @return Cache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_get_line_size(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    uint32_t size = 0;
    size = Cache_Get_Line_Size();
    return size;
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
    if (vaddr_start >= SOC_IRAM0_CACHE_ADDRESS_LOW && vaddr_end < SOC_IRAM0_CACHE_ADDRESS_HIGH) {
        //c61 the I/D bus memory are shared, so we always return `CACHE_BUS_IBUS0 | CACHE_BUS_DBUS0`
        mask = (cache_bus_mask_t)(mask | (CACHE_BUS_IBUS0 | CACHE_BUS_DBUS0));
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
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);
    //On esp32c61, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2 | CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS0) ? CACHE_L1_CACHE_SHUT_BUS0 : 0);
    REG_CLR_BIT(CACHE_L1_CACHE_CTRL_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS0) ? CACHE_L1_CACHE_SHUT_BUS1 : 0);
    REG_CLR_BIT(CACHE_L1_CACHE_CTRL_REG, dbus_mask);
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
    HAL_ASSERT(cache_id <= CACHE_LL_ID_ALL);
    //On esp32c61, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2 | CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS0) ? CACHE_L1_CACHE_SHUT_BUS0 : 0);
    REG_SET_BIT(CACHE_L1_CACHE_CTRL_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS0) ? CACHE_L1_CACHE_SHUT_BUS1 : 0);
    REG_SET_BIT(CACHE_L1_CACHE_CTRL_REG, dbus_mask);
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

    valid |= (SOC_ADDRESS_IN_IRAM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_IRAM0_CACHE(vaddr_end));
    valid |= (SOC_ADDRESS_IN_DRAM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_DRAM0_CACHE(vaddr_end));

    if (valid) {
        *out_level = 1;
        *out_id = 0;
    }

    return valid;
}

/**
 * Enable the Cache fail tracer
 *
 * @param cache_id    cache ID
 * @param en          enable / disable
 */
static inline void cache_ll_l1_enable_fail_tracer(uint32_t cache_id, bool en)
{
    CACHE.trace_ena.l1_cache_trace_ena = en;
}

/*------------------------------------------------------------------------------
 * Interrupt
 *----------------------------------------------------------------------------*/
/**
 * @brief Enable Cache access error interrupt
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_enable_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    CACHE.l1_cache_acs_fail_int_ena.val |= mask;
}

/**
 * @brief Clear Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_clear_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    CACHE.l1_cache_acs_fail_int_clr.val = mask;
}

/**
 * @brief Get Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 *
 * @return            Status mask
 */
static inline uint32_t cache_ll_l1_get_access_error_intr_status(uint32_t cache_id, uint32_t mask)
{
    return CACHE.l1_cache_acs_fail_int_st.val & mask;
}

#ifdef __cplusplus
}
#endif
