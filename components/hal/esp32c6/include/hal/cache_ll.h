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
#include "esp32c6/rom/cache.h"

#ifdef __cplusplus
extern "C" {
#endif
#define CACHE_LL_ENABLE_DISABLE_STATE_SW            1   //There's no register indicating cache enable/disable state, we need to use software way for this state.

#define CACHE_LL_DEFAULT_IBUS_MASK                  CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK                  CACHE_BUS_DBUS0

#define CACHE_LL_L1_ACCESS_EVENT_MASK               (1<<4)
#define CACHE_LL_L1_ACCESS_EVENT_CACHE_FAIL         (1<<4)

#define CACHE_LL_L1_ICACHE_AUTOLOAD                 (1<<0)

/**
 * @brief Check if Cache auto preload is enabled or not. On ESP32C6, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_is_cache_autoload_enabled(cache_type_t type)
{
    bool enabled = false;
    if (REG_GET_BIT(EXTMEM_L1_CACHE_AUTOLOAD_CTRL_REG, EXTMEM_L1_CACHE_AUTOLOAD_ENA)) {
        enabled = true;
    }
    return enabled;
}

/**
 * @brief Disable Cache. On ESP32C6, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 */
__attribute__((always_inline))
static inline void cache_ll_disable_cache(cache_type_t type)
{
    (void) type;
    Cache_Disable_ICache();
}

/**
 * @brief Enable Cache. On ESP32C6, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 *
 * @param data_autoload_en Dcache auto preload enabled
 *
 * @param inst_autoload_en Icache auto preload enabled
 */
__attribute__((always_inline))
static inline void cache_ll_enable_cache(cache_type_t type, bool inst_autoload_en, bool data_autoload_en)
{
    Cache_Enable_ICache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Suspend Cache. On ESP32C6, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 */
__attribute__((always_inline))
static inline void cache_ll_suspend_cache(cache_type_t type)
{
    Cache_Suspend_ICache();
}

/**
 * @brief Resume Cache. On ESP326, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 *
 * @param data_autoload_en Dcache auto preload enabled
 *
 * @param inst_autoload_en Icache auto preload enabled
 */
__attribute__((always_inline))
static inline void cache_ll_resume_cache(cache_type_t type, bool inst_autoload_en, bool data_autoload_en)
{
    Cache_Resume_ICache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Invalidate cache supported addr
 *
 * Invalidate a Cache item
 *
 * @param vaddr  Start address of the region to be invalidated
 * @param size   Size of the region to be invalidated
 */
__attribute__((always_inline))
static inline void cache_ll_invalidate_addr(uint32_t vaddr, uint32_t size)
{
    Cache_Invalidate_Addr(vaddr, size);
}

/**
 * @brief Freeze Cache. On ESP32C6, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 */
__attribute__((always_inline))
static inline void cache_ll_freeze_cache(cache_type_t type)
{
    Cache_Freeze_ICache_Enable(CACHE_FREEZE_ACK_BUSY);
}

/**
 * @brief Unfreeze Cache. On ESP32C6, instructions and data share Cache
 *
 * @param type  see `cache_type_t`
 */
__attribute__((always_inline))
static inline void cache_ll_unfreeze_cache(cache_type_t type)
{
    Cache_Freeze_ICache_Disable();
}

/**
 * @brief Get cache line size, in bytes
 *
 * @param type  see `cache_type_t`
 *
 * @return cache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_get_line_size(cache_type_t type)
{
    uint32_t size = 0;
    size = Cache_Get_ICache_Line_Size();
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
    HAL_ASSERT(cache_id == 0);
    cache_bus_mask_t mask = (cache_bus_mask_t)0;

    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= SOC_IRAM0_CACHE_ADDRESS_LOW && vaddr_end < SOC_IRAM0_CACHE_ADDRESS_HIGH) {
        //c6 the I/D bus memory are shared, so we always return `CACHE_BUS_IBUS0 | CACHE_BUS_DBUS0`
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
    HAL_ASSERT(cache_id == 0);
    //On esp32c6, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2 | CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS0) ? EXTMEM_L1_CACHE_SHUT_IBUS : 0);
    REG_CLR_BIT(EXTMEM_L1_CACHE_CTRL_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS0) ? EXTMEM_L1_CACHE_SHUT_DBUS : 0);
    REG_CLR_BIT(EXTMEM_L1_CACHE_CTRL_REG, dbus_mask);
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
    //On esp32c6, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2 | CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS0) ? EXTMEM_L1_CACHE_SHUT_IBUS : 0);
    REG_SET_BIT(EXTMEM_L1_CACHE_CTRL_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS0) ? EXTMEM_L1_CACHE_SHUT_DBUS : 0);
    REG_SET_BIT(EXTMEM_L1_CACHE_CTRL_REG, dbus_mask);
}

/*------------------------------------------------------------------------------
 * Interrupt
 *----------------------------------------------------------------------------*/
/**
 * @brief Enable Cache access error interrupt
 *
 * @param cache_id    Cache ID, not used on C3. For compabitlity
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_enable_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    SET_PERI_REG_MASK(EXTMEM_L1_CACHE_ACS_FAIL_INT_ENA_REG, mask);
}

/**
 * @brief Clear Cache access error interrupt status
 *
 * @param cache_id    Cache ID, not used on C3. For compabitlity
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_clear_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    SET_PERI_REG_MASK(EXTMEM_L1_CACHE_ACS_FAIL_INT_CLR_REG, mask);
}

/**
 * @brief Get Cache access error interrupt status
 *
 * @param cache_id    Cache ID, not used on C3. For compabitlity
 * @param mask        Interrupt mask
 *
 * @return            Status mask
 */
static inline uint32_t cache_ll_l1_get_access_error_intr_status(uint32_t cache_id, uint32_t mask)
{
    return GET_PERI_REG_MASK(EXTMEM_L1_CACHE_ACS_FAIL_INT_ST_REG, mask);
}

#ifdef __cplusplus
}
#endif
