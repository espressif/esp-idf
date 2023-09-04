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
#include "esp32s2/rom/cache.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CACHE_LL_DEFAULT_IBUS_MASK    CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK    CACHE_BUS_IBUS2

#define CACHE_LL_L1_ICACHE_AUTOLOAD                      (1<<0)
#define CACHE_LL_L1_DCACHE_AUTOLOAD                      (1<<0)

/**
 * @brief Check if ICache auto preload is enabled or not
 *
 * @return true: enabled; false: disabled
*/
__attribute__((always_inline))
static inline bool cache_ll_l1_is_icache_autoload_enabled(void)
{
    bool enabled = false;
    if (REG_GET_BIT(EXTMEM_PRO_ICACHE_CTRL_REG, EXTMEM_PRO_ICACHE_AUTOLOAD_ENA)) {
        enabled = true;
    }
    return enabled;
}

/**
 * @brief Check if DCache auto preload is enabled or not
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_dcache_autoload_enabled(void)
{
    bool enabled = false;
    if (REG_GET_BIT(EXTMEM_PRO_DCACHE_CTRL_REG, EXTMEM_PRO_DCACHE_AUTOLOAD_ENA)) {
        enabled = true;
    }
    return enabled;
}

/**
 * @brief Check if ICache or DCache auto preload is enabled or not
 *
 * @param type  see `cache_type_t`
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_is_cache_autoload_enabled(cache_type_t type)
{
    bool enabled = false;
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        enabled = cache_ll_l1_is_icache_autoload_enabled();
        break;
    case CACHE_TYPE_DATA:
        enabled = cache_ll_l1_is_dcache_autoload_enabled();
        break;
    default: //CACHE_TYPE_ALL
        enabled = cache_ll_l1_is_icache_autoload_enabled() && cache_ll_l1_is_dcache_autoload_enabled();
        break;
    }
    return enabled;
}

/**
 * @brief Disable ICache
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_icache(void)
{
    Cache_Disable_ICache();
}

/**
 * @brief Disable DCache
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_dcache(void)
{
    Cache_Disable_DCache();
}

/**
 * @brief Disable ICache or DCache or both
 *
 * @param type  see `cache_type_t`
 */
__attribute__((always_inline))
static inline void cache_ll_disable_cache(cache_type_t type)
{
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        cache_ll_l1_disable_icache();
        break;
    case CACHE_TYPE_DATA:
        cache_ll_l1_disable_dcache();
        break;
    default: //CACHE_TYPE_ALL
        cache_ll_l1_disable_icache();
        cache_ll_l1_disable_dcache();
        break;
    }
}

/**
 * @brief Enable ICache
 *
 * @param inst_autoload_en ICache auto preload enabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_icache(bool inst_autoload_en)
{
    Cache_Enable_ICache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Enable DCache
 *
 * @param data_autoload_en DCache auto preload enabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_dcache(bool data_autoload_en)
{
    Cache_Enable_DCache(data_autoload_en ? CACHE_LL_L1_DCACHE_AUTOLOAD : 0);
}

/**
 * @brief Enable ICache or DCache or both
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
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        cache_ll_l1_enable_icache(inst_autoload_en);
        break;
    case CACHE_TYPE_DATA:
        cache_ll_l1_enable_dcache(data_autoload_en);
        break;
    default: //CACHE_TYPE_ALL
        cache_ll_l1_enable_icache(inst_autoload_en);
        cache_ll_l1_enable_dcache(data_autoload_en);
        break;
    }
}

/**
 * @brief Suspend ICache
 */
__attribute__((always_inline))
static inline void cache_ll_l1_suspend_icache(void)
{
    Cache_Suspend_ICache();
}

/**
 * @brief Suspend DCache
 */
__attribute__((always_inline))
static inline void cache_ll_l1_suspend_dcache(void)
{
    Cache_Suspend_DCache();
}

/**
 * @brief Suspend ICache or DCache or both
 *
 * @param type  see `cache_type_t`
 */
__attribute__((always_inline))
static inline void cache_ll_suspend_cache(cache_type_t type)
{
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        cache_ll_l1_suspend_icache();
        break;
    case CACHE_TYPE_DATA:
        cache_ll_l1_suspend_dcache();
        break;
    default: //CACHE_TYPE_ALL
        cache_ll_l1_suspend_icache();
        cache_ll_l1_suspend_dcache();
        break;
    }
}

/**
 * @brief Resume ICache
 *
 * @param inst_autoload_en  ICache auto preload enabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_resume_icache(bool inst_autoload_en)
{
    Cache_Resume_ICache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Resume DCache
 *
 * @param data_autoload_en  DCache auto preload enabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_resume_dcache(bool data_autoload_en)
{
    Cache_Resume_DCache(data_autoload_en ? CACHE_LL_L1_DCACHE_AUTOLOAD : 0);
}

/**
 * @brief Resume ICache or DCache or both
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
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        cache_ll_l1_resume_icache(inst_autoload_en);
        break;
    case CACHE_TYPE_DATA:
        cache_ll_l1_resume_dcache(data_autoload_en);
        break;
    default: //CACHE_TYPE_ALL
        cache_ll_l1_resume_icache(inst_autoload_en);
        cache_ll_l1_resume_dcache(data_autoload_en);
        break;
    }
}

/**
 * @brief Check if ICache is enabled or not
 *
 * @param   cache_id    cache ID (when l1 cache is per core)
 *
 * @return  true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_icache_enabled(uint32_t cache_id){
    HAL_ASSERT(cache_id == 0);

    bool enabled;
    enabled = REG_GET_BIT(EXTMEM_PRO_ICACHE_CTRL_REG, EXTMEM_PRO_ICACHE_ENABLE);
    return enabled;
}

/**
 * @brief Check if DCache is enabled or not
 *
 * @param   cache_id    cache ID (when l1 cache is per core)
 *
 * @return  true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_dcache_enabled(uint32_t cache_id)
{
    HAL_ASSERT(cache_id == 0);

    bool enabled;
    enabled = REG_GET_BIT(EXTMEM_PRO_DCACHE_CTRL_REG, EXTMEM_PRO_DCACHE_ENABLE);
    return enabled;
}

/**
 * @brief Check if ICache or DCache or both is enabled or not
 *
 * @param type  see `cache_type_t`
 *
 * @return  true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_is_cache_enabled(cache_type_t type)
{
    bool enabled = false;
    switch (type)
    {
    case CACHE_TYPE_DATA:
        enabled = cache_ll_l1_is_dcache_enabled(0);
        break;
    case CACHE_TYPE_INSTRUCTION:
        enabled = cache_ll_l1_is_icache_enabled(0);
        break;
    default: //CACHE_TYPE_ALL
        enabled = cache_ll_l1_is_dcache_enabled(0) && cache_ll_l1_is_icache_enabled(0);
        break;
    }
    return enabled;
}

/**
 * @brief Invalidate cache supported addr
 *
 * Invalidate a Cache item for either ICache or DCache.
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
 * @brief Writeback cache supported addr
 *
 * Writeback the DCache item to external memory
 *
 * @param vaddr  Start address of the region to writeback
 * @param size   Size of the region to writeback
 */
__attribute__((always_inline))
static inline void cache_ll_writeback_addr(uint32_t vaddr, uint32_t size)
{
    Cache_WriteBack_Addr(vaddr, size);
}

/**
 * @brief Get ICache line size, in bytes
 *
 * @return ICache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_l1_icache_get_line_size(void)
{
    uint32_t size = 0;
    size = Cache_Get_ICache_Line_Size();
    return size;
}

/**
 * @brief Get DCache line size, in bytes
 *
 * @return DCache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_l1_dcache_get_line_size(void)
{
    uint32_t size = 0;
    size = Cache_Get_DCache_Line_Size();
    return size;
}

/**
 * @brief Get ICache or DCache line size, in bytes
 *
 * @param type  see `cache_type_t`
 *
 * @return ICache/DCache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_get_line_size(cache_type_t type)
{
    uint32_t size = 0;
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        size = cache_ll_l1_icache_get_line_size();
        break;
    case CACHE_TYPE_DATA:
        size = cache_ll_l1_dcache_get_line_size();
        break;
    default: //CACHE_TYPE_ALL
        HAL_ASSERT(false);
        break;
    }
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
    (void)cache_id;

    cache_bus_mask_t mask = (cache_bus_mask_t)0;
    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= SOC_IRAM1_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS1);
    } else if (vaddr_start >= SOC_IRAM0_CACHE_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS0);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0));
    } else if (vaddr_start >= SOC_DRAM0_CACHE_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS0);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0));
    } else if (vaddr_start >= SOC_DRAM1_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS1);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_DRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0));
    } else if (vaddr_start >= SOC_DPORT_CACHE_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS2);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_DRAM1_ADDRESS_LOW) ? CACHE_BUS_DBUS1 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_DRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0));
    } else if (vaddr_start >= SOC_DROM0_ADDRESS_LOW) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS2);
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_DPORT_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS2 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_DRAM1_ADDRESS_LOW) ? CACHE_BUS_DBUS1 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_DRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_DBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM0_CACHE_ADDRESS_LOW) ? CACHE_BUS_IBUS0 : 0));
        mask = (cache_bus_mask_t)(mask | ((vaddr_end >= SOC_IRAM1_ADDRESS_LOW) ? CACHE_BUS_IBUS1 : 0));
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
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS0) ? EXTMEM_PRO_ICACHE_MASK_IRAM0 : 0);
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS1) ? EXTMEM_PRO_ICACHE_MASK_IRAM1 : 0);
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS2) ? EXTMEM_PRO_ICACHE_MASK_DROM0 : 0);
    REG_CLR_BIT(EXTMEM_PRO_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS0) ? EXTMEM_PRO_DCACHE_MASK_DRAM0 : 0);
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS1) ? EXTMEM_PRO_DCACHE_MASK_DRAM1 : 0);
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS2) ? EXTMEM_PRO_DCACHE_MASK_DPORT : 0);
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
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS0) ? EXTMEM_PRO_ICACHE_MASK_IRAM0 : 0);
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS1) ? EXTMEM_PRO_ICACHE_MASK_IRAM1 : 0);
    ibus_mask = ibus_mask | ((mask & CACHE_BUS_IBUS2) ? EXTMEM_PRO_ICACHE_MASK_DROM0 : 0);
    REG_SET_BIT(EXTMEM_PRO_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS0) ? EXTMEM_PRO_DCACHE_MASK_DRAM0 : 0);
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS1) ? EXTMEM_PRO_DCACHE_MASK_DRAM1 : 0);
    dbus_mask = dbus_mask | ((mask & CACHE_BUS_DBUS2) ? EXTMEM_PRO_DCACHE_MASK_DPORT : 0);
    REG_SET_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, dbus_mask);
}


#ifdef __cplusplus
}
#endif
