/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp32p4/rom/cache.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Given a L2MEM cached address, get the corresponding non-cacheable address
 * @example 0x4FF0_0000 => 0x8FF0_0000
 */
#define CACHE_LL_L2MEM_NON_CACHE_ADDR(addr) ((uintptr_t)(addr) + SOC_NON_CACHEABLE_OFFSET_SRAM)

/**
 * @brief Given a non-cacheable address, get the corresponding L2MEM cached address
 * @example 0x8FF0_0000 => 0x4FF0_0000
 */
#define CACHE_LL_L2MEM_CACHE_ADDR(non_cache_addr) ((uintptr_t)(non_cache_addr) - SOC_NON_CACHEABLE_OFFSET_SRAM)

/**
 * Cache capabilities
 */
#define CACHE_LL_ENABLE_DISABLE_STATE_SW            1   //There's no register indicating cache enable/disable state, we need to use software way for this state.
#define CACHE_LL_EXT_MEM_VIA_L2CACHE                1

#define CACHE_LL_ID_ALL                             3   //All of the caches in a type and level, make this value greater than any ID
#define CACHE_LL_LEVEL_INT_MEM                      1   //Cache level for accessing internal mem
#define CACHE_LL_LEVEL_EXT_MEM                      2   //Cache level for accessing external mem
#define CACHE_LL_LEVEL_ALL                          3   //All of the cache levels, make this value greater than any level
#define CACHE_LL_LEVEL_NUMS                         2   //Number of cache levels
#define CACHE_LL_CACHE_AUTOLOAD                     (1<<0)

#define CACHE_LL_DEFAULT_IBUS_MASK                  (CACHE_BUS_IBUS0 | CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2)
#define CACHE_LL_DEFAULT_DBUS_MASK                  (CACHE_BUS_DBUS0 | CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)

#define CACHE_LL_L1_ACCESS_EVENT_MASK               (0x1f)
#define CACHE_LL_L2_ACCESS_EVENT_MASK               (1<<6)
#define CACHE_LL_L1_CORE0_EVENT_MASK                (1<<0)
#define CACHE_LL_L1_CORE1_EVENT_MASK                (1<<1)

/*------------------------------------------------------------------------------
 * Autoload
 *----------------------------------------------------------------------------*/
/**
 * @brief Check if L1 ICache autoload is enabled or not
 *
 * @param cache_id  id of the cache in this type and level
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_icache_autoload_enabled(uint32_t cache_id)
{
    bool enabled = false;

    if (cache_id == 0) {
        enabled = REG_GET_BIT(CACHE_L1_ICACHE0_AUTOLOAD_CTRL_REG, CACHE_L1_ICACHE0_AUTOLOAD_ENA);
    } else if (cache_id == 1) {
        enabled = REG_GET_BIT(CACHE_L1_ICACHE1_AUTOLOAD_CTRL_REG, CACHE_L1_ICACHE1_AUTOLOAD_ENA);
    } else if (cache_id == CACHE_LL_ID_ALL) {
        enabled = REG_GET_BIT(CACHE_L1_ICACHE0_AUTOLOAD_CTRL_REG, CACHE_L1_ICACHE0_AUTOLOAD_ENA) && REG_GET_BIT(CACHE_L1_ICACHE1_AUTOLOAD_CTRL_REG, CACHE_L1_ICACHE1_AUTOLOAD_ENA);
    }

    return enabled;
}

/**
 * @brief Check if L1 DCache autoload is enabled or not
 *
 * @param cache_id  id of the cache in this type and level
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_dcache_autoload_enabled(uint32_t cache_id)
{
    bool enabled = false;
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        enabled = REG_GET_BIT(CACHE_L1_DCACHE_AUTOLOAD_CTRL_REG, CACHE_L1_DCACHE_AUTOLOAD_ENA);
    }
    return enabled;
}

/**
 * @brief Check if L2 Cache auto preload is enabled or not
 *
 * @param cache_id  id of the cache in this type and level
 *
 * @return true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool cache_ll_l2_is_cache_autoload_enabled(uint32_t cache_id)
{
    bool enabled = false;
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        enabled = REG_GET_BIT(CACHE_L2_CACHE_AUTOLOAD_CTRL_REG, CACHE_L2_CACHE_AUTOLOAD_ENA);
    }
    return enabled;
}

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
    bool enabled = false;

    if (cache_level == 2) {
        enabled = cache_ll_l2_is_cache_autoload_enabled(cache_id);
    } else if (cache_level == 1) {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            enabled = cache_ll_l1_is_icache_autoload_enabled(cache_id);
            break;
        case CACHE_TYPE_DATA:
            enabled = cache_ll_l1_is_dcache_autoload_enabled(cache_id);
            break;
        case CACHE_TYPE_ALL:
        default: //CACHE_TYPE_ALL
            enabled = cache_ll_l1_is_icache_autoload_enabled(CACHE_LL_ID_ALL) && cache_ll_l1_is_dcache_autoload_enabled(cache_id);
            break;
        }
    }

    return enabled;
}

/*------------------------------------------------------------------------------
 * Disable
 *----------------------------------------------------------------------------*/
/**
 * @brief Disable L1 ICache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_icache(uint32_t cache_id)
{
    if (cache_id == 0) {
        Cache_Disable_L1_CORE0_ICache();
    } else if (cache_id == 1) {
        Cache_Disable_L1_CORE1_ICache();
    } else if (cache_id == CACHE_LL_ID_ALL) {
        Cache_Disable_L1_CORE0_ICache();
        Cache_Disable_L1_CORE1_ICache();
    }
}

/**
 * @brief Disable L1 DCache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_dcache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Disable_L1_DCache();
    }
}

/**
 * @brief Disable L2 Cache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l2_disable_cache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Disable_L2_Cache();
    }
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    if (cache_level == 1) {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_disable_icache(cache_id);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_disable_dcache(cache_id);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_disable_icache(CACHE_LL_ID_ALL);
            cache_ll_l1_disable_dcache(cache_id);
            break;
        }
    } else {
        cache_ll_l2_disable_cache(cache_id);
    }
}

/*------------------------------------------------------------------------------
 * Enable
 *----------------------------------------------------------------------------*/
/**
 * @brief Enable L1 ICache
 *
 * @param cache_id          id of the cache in this type and level
 * @param inst_autoload_en  icache autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_icache(uint32_t cache_id, bool inst_autoload_en)
{
    if (cache_id == 0) {
        Cache_Enable_L1_CORE0_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    } else if (cache_id == 1) {
        Cache_Enable_L1_CORE1_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    } else if (cache_id == CACHE_LL_ID_ALL) {
        Cache_Enable_L1_CORE1_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
        Cache_Enable_L1_CORE0_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    }
}

/**
 * @brief Enable L1 DCache
 *
 * @param cache_id          id of the cache in this type and level
 * @param data_autoload_en  dcache autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_dcache(uint32_t cache_id, bool data_autoload_en)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Enable_L1_DCache(data_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    }
}

/**
 * @brief Enable L2 Cache
 *
 * @param cache_id          id of the cache in this type and level
 * @param inst_autoload_en  autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_l2_enable_cache(uint32_t cache_id, bool autoload_en)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Enable_L2_Cache(autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    }
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    if (cache_level == 2) {
        cache_ll_l2_enable_cache(cache_id, inst_autoload_en);
    } else {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_enable_icache(cache_id, inst_autoload_en);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_enable_dcache(cache_id, data_autoload_en);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_enable_dcache(cache_id, data_autoload_en);
            cache_ll_l1_enable_icache(cache_id, inst_autoload_en);
            break;
        }
    }
}

/*------------------------------------------------------------------------------
 * Suspend
 *----------------------------------------------------------------------------*/
/**
 * @brief Suspend L1 ICache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_suspend_icache(uint32_t cache_id)
{
    if (cache_id == 0) {
        Cache_Suspend_L1_CORE0_ICache();
    } else if (cache_id == 1) {
        Cache_Suspend_L1_CORE1_ICache();
    } else if (cache_id == CACHE_LL_ID_ALL) {
        Cache_Suspend_L1_CORE0_ICache();
        Cache_Suspend_L1_CORE1_ICache();
    }
}

/**
 * @brief Suspend L1 DCache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_suspend_dcache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Suspend_L1_DCache();
    }
}

/**
 * @brief Suspend L2 Cache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l2_suspend_cache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Suspend_L2_Cache();
    }
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    if (cache_level == 1) {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_suspend_icache(cache_id);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_suspend_dcache(cache_id);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_suspend_icache(CACHE_LL_ID_ALL);
            cache_ll_l1_suspend_dcache(cache_id);
            break;
        }
    } else {
        cache_ll_l2_suspend_cache(cache_id);
    }
}

/*------------------------------------------------------------------------------
 * Resume
 *----------------------------------------------------------------------------*/
/**
 * @brief Resume L1 ICache
 *
 * @param cache_id          id of the cache in this type and level
 * @param inst_autoload_en  icache autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_l1_resume_icache(uint32_t cache_id, bool inst_autoload_en)
{
    if (cache_id == 0) {
        Cache_Resume_L1_CORE0_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    } else if (cache_id == 1) {
        Cache_Resume_L1_CORE1_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    } else if (cache_id == CACHE_LL_ID_ALL) {
        Cache_Resume_L1_CORE1_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
        Cache_Resume_L1_CORE0_ICache(inst_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    }
}

/**
 * @brief Resume L1 DCache
 *
 * @param cache_id          id of the cache in this type and level
 * @param data_autoload_en  dcache autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_l1_resume_dcache(uint32_t cache_id, bool data_autoload_en)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Resume_L1_DCache(data_autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    }
}

/**
 * @brief Resume L2 Cache
 *
 * @param cache_id          id of the cache in this type and level
 * @param inst_autoload_en  autoload enabled or not
 */
__attribute__((always_inline))
static inline void cache_ll_l2_resume_cache(uint32_t cache_id, bool autoload_en)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Resume_L2_Cache(autoload_en ? CACHE_LL_CACHE_AUTOLOAD : 0);
    }
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    if (cache_level == 2) {
        cache_ll_l2_resume_cache(cache_id, inst_autoload_en);
    } else {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_resume_icache(cache_id, inst_autoload_en);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_resume_dcache(cache_id, data_autoload_en);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_resume_dcache(cache_id, data_autoload_en);
            cache_ll_l1_resume_icache(CACHE_LL_ID_ALL, inst_autoload_en);
            break;
        }
    }
}

/*------------------------------------------------------------------------------
 * Invalidate
 *----------------------------------------------------------------------------*/
/**
 * @brief Invalidate L1 ICache addr
 *
 * @param cache_id     id of the cache in this type and level
 * @param vaddr        start address of the region to be invalidated
 * @param size         size of the region to be invalidated
 */
__attribute__((always_inline))
static inline void cache_ll_l1_invalidate_icache_addr(uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    if (cache_id == 0) {
        Cache_Invalidate_Addr(CACHE_MAP_L1_ICACHE_0, vaddr, size);
    } else if (cache_id == 1) {
        Cache_Invalidate_Addr(CACHE_MAP_L1_ICACHE_1, vaddr, size);
    } else if (cache_id == CACHE_LL_ID_ALL) {
        Cache_Invalidate_Addr(CACHE_MAP_L1_ICACHE_MASK, vaddr, size);
    }
}

/**
 * @brief Invalidate L1 DCache addr
 *
 * @param cache_id     id of the cache in this type and level
 * @param vaddr        start address of the region to be invalidated
 * @param size         size of the region to be invalidated
 */
__attribute__((always_inline))
static inline void cache_ll_l1_invalidate_dcache_addr(uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Invalidate_Addr(CACHE_MAP_L1_DCACHE, vaddr, size);
    }
}

/**
 * @brief Invalidate L2 Cache addr
 *
 * @param cache_id     id of the cache in this type and level
 * @param vaddr        start address of the region to be invalidated
 * @param size         size of the region to be invalidated
 */
__attribute__((always_inline))
static inline void cache_ll_l2_invalidate_cache_addr(uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Invalidate_Addr(CACHE_MAP_L2_CACHE, vaddr, size);
    }
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
    if (cache_level == 1 || cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_invalidate_icache_addr(cache_id, vaddr, size);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_invalidate_dcache_addr(cache_id, vaddr, size);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_invalidate_icache_addr(cache_id, vaddr, size);
            cache_ll_l1_invalidate_dcache_addr(cache_id, vaddr, size);
            break;
        }
    }

    if (cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        cache_ll_l2_invalidate_cache_addr(cache_id, vaddr, size);
    }
}

/**
 * @brief Invalidate L1 ICache all
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_invalidate_icache_all(uint32_t cache_id)
{
    if (cache_id == 0) {
        Cache_Invalidate_All(CACHE_MAP_L1_ICACHE_0);
    } else if (cache_id == 1) {
        Cache_Invalidate_All(CACHE_MAP_L1_ICACHE_1);
    } else if (cache_id == CACHE_LL_ID_ALL) {
        Cache_Invalidate_All(CACHE_MAP_L1_ICACHE_MASK);
    }
}

/**
 * @brief Invalidate L1 DCache all
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_invalidate_dcache_all(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Invalidate_All(CACHE_MAP_L1_DCACHE);
    }
}

/**
 * @brief Invalidate L2 Cache all
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l2_invalidate_cache_all(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_Invalidate_All(CACHE_MAP_L2_CACHE);
    }
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
    if (cache_level == 1 || cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_invalidate_icache_all(cache_id);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_invalidate_dcache_all(cache_id);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_invalidate_icache_all(cache_id);
            cache_ll_l1_invalidate_dcache_all(cache_id);
            break;
        }
    }

    if (cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        cache_ll_l2_invalidate_cache_all(cache_id);
    }
}

/*------------------------------------------------------------------------------
 * Writeback
 *----------------------------------------------------------------------------*/
/**
 * @brief Writeback L1 DCache addr
 *
 * @param cache_id     id of the cache in this type and level
 * @param vaddr        start address of the region to be written back
 * @param size         size of the region to be written back
 */
__attribute__((always_inline))
static inline void cache_ll_l1_writeback_dcache_addr(uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, vaddr, size);
    }
}

/**
 * @brief Writeback L2 Cache addr
 *
 * @param cache_id     id of the cache in this type and level
 * @param vaddr        start address of the region to be written back
 * @param size         size of the region to be written back
 */
__attribute__((always_inline))
static inline void cache_ll_l2_writeback_cache_addr(uint32_t cache_id, uint32_t vaddr, uint32_t size)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_WriteBack_Addr(CACHE_MAP_L2_CACHE, vaddr, size);
    }
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
    if (cache_level == 1 || cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        switch (type) {
        case CACHE_TYPE_DATA:
            cache_ll_l1_writeback_dcache_addr(cache_id, vaddr, size);
            break;
        default:
            HAL_ASSERT(false);
        }
    }

    if (cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        cache_ll_l2_writeback_cache_addr(cache_id, vaddr, size);
    }
}

/**
 * @brief Writeback L1 DCache all
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_writeback_dcache_all(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_WriteBack_All(CACHE_MAP_L1_DCACHE);
    }
}

/**
 * @brief Writeback L2 Cache all
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l2_writeback_cache_all(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        Cache_WriteBack_All(CACHE_MAP_L2_CACHE);
    }
}

/**
 * @brief Writeback all
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_writeback_all(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    if (cache_level == 1 || cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        switch (type) {
        case CACHE_TYPE_DATA:
            cache_ll_l1_writeback_dcache_all(cache_id);
            break;
        default:
            HAL_ASSERT(false);
        }
    }

    if (cache_level == 2 || cache_level == CACHE_LL_LEVEL_ALL) {
        cache_ll_l2_writeback_cache_all(cache_id);
    }
}

/*------------------------------------------------------------------------------
 * Freeze
 *----------------------------------------------------------------------------*/
/**
 * @brief Freeze L1 ICache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_freeze_icache(uint32_t cache_id)
{
    if (cache_id == 0) {
        rom_cache_internal_table_ptr->freeze_l1_icache0_enable(CACHE_FREEZE_ACK_BUSY);
    } else if (cache_id == 1) {
        rom_cache_internal_table_ptr->freeze_l1_icache1_enable(CACHE_FREEZE_ACK_BUSY);
    } else if (cache_id == CACHE_LL_ID_ALL) {
        rom_cache_internal_table_ptr->freeze_l1_icache0_enable(CACHE_FREEZE_ACK_BUSY);
        rom_cache_internal_table_ptr->freeze_l1_icache1_enable(CACHE_FREEZE_ACK_BUSY);
    }
}

/**
 * @brief Freeze L1 DCache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_freeze_dcache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        rom_cache_internal_table_ptr->freeze_l1_dcache_enable(CACHE_FREEZE_ACK_BUSY);
    }
}

/**
 * @brief Freeze L2 Cache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l2_freeze_cache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        rom_cache_internal_table_ptr->freeze_l2_cache_enable(CACHE_FREEZE_ACK_BUSY);
    }
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    if (cache_level == 1) {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_freeze_icache(cache_id);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_freeze_dcache(cache_id);
            break;
        default:
            cache_ll_l1_freeze_icache(CACHE_LL_ID_ALL);
            cache_ll_l1_freeze_dcache(cache_id);
            break;
        }
    } else {
        cache_ll_l2_freeze_cache(cache_id);
    }
}

/*------------------------------------------------------------------------------
 * Unfreeze
 *----------------------------------------------------------------------------*/
/**
 * @brief Unfreeze L1 ICache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_unfreeze_icache(uint32_t cache_id)
{
    if (cache_id == 0) {
        rom_cache_internal_table_ptr->freeze_l1_icache0_disable();
    } else if (cache_id == 1) {
        rom_cache_internal_table_ptr->freeze_l1_icache1_disable();
    } else if (cache_id == CACHE_LL_ID_ALL) {
        rom_cache_internal_table_ptr->freeze_l1_icache1_disable();
        rom_cache_internal_table_ptr->freeze_l1_icache0_disable();
    }
}

/**
 * @brief Unfreeze L1 DCache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l1_unfreeze_dcache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        rom_cache_internal_table_ptr->freeze_l1_dcache_disable();
    }
}

/**
 * @brief Unfreeze L2 Cache
 *
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline))
static inline void cache_ll_l2_unfreeze_cache(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        rom_cache_internal_table_ptr->freeze_l2_cache_disable();
    }
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    if (cache_level == 2) {
        cache_ll_l2_unfreeze_cache(cache_id);
    } else {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            cache_ll_l1_unfreeze_icache(cache_id);
            break;
        case CACHE_TYPE_DATA:
            cache_ll_l1_unfreeze_dcache(cache_id);
            break;
        case CACHE_TYPE_ALL:
        default:
            cache_ll_l1_unfreeze_dcache(cache_id);
            cache_ll_l1_unfreeze_icache(CACHE_LL_ID_ALL);
            break;
        }
    }
}

/*------------------------------------------------------------------------------
 * Cache Line Size
 *----------------------------------------------------------------------------*/
/**
 * @brief Get ICache line size, in bytes
 *
 * @param cache_id  id of the cache in this type and level
 *
 * @return ICache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_l1_icache_get_line_size(uint32_t cache_id)
{
    uint32_t size = 0;

    if (cache_id == 0 || cache_id == 1 || cache_id == CACHE_LL_ID_ALL) {
        size = 64;
    }

    return size;
}

/**
 * @brief Get DCache line size, in bytes
 *
 * @param cache_id  id of the cache in this type and level
 *
 * @return DCache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_l1_dcache_get_line_size(uint32_t cache_id)
{
    uint32_t size = 0;

    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        size = 64;
    }

    return size;
}

/**
 * @brief Get L2 Cache line size, in bytes
 *
 * @param cache_id  id of the cache in this type and level
 *
 * @return L2 Cache line size, in bytes
 */
__attribute__((always_inline))
static inline uint32_t cache_ll_l2_cache_get_line_size(uint32_t cache_id)
{
    uint32_t size = 0;

    if (cache_id == 0 || cache_id == CACHE_LL_ID_ALL) {
        size = Cache_Get_L2_Cache_Line_Size();
    }

    return size;
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
    HAL_ASSERT(cache_level == 1 || cache_level == 2);

    uint32_t size = 0;
    if (cache_level == 2) {
        size = cache_ll_l2_cache_get_line_size(cache_id);
    } else {
        switch (type) {
        case CACHE_TYPE_INSTRUCTION:
            size = cache_ll_l1_icache_get_line_size(cache_id);
            break;
        case CACHE_TYPE_DATA:
            size = cache_ll_l1_dcache_get_line_size(cache_id);
            break;
        case CACHE_TYPE_ALL:
        default:
            break;
        }
    }

    return size;
}

/*------------------------------------------------------------------------------
 * Cache Bus
 *----------------------------------------------------------------------------*/
/**
 * @brief Get the buses of a particular L1 Cache that are mapped to a virtual address range
 *
 * External virtual address can only be accessed when the involved cache buses are enabled.
 * This API is to get the cache buses where the memory region (from `vaddr_start` to `vaddr_start + len`) reside.
 *
 * @param cache_id          cache ID (when l1 cache is per core)
 * @param vaddr_start       virtual address start
 * @param len               vaddr length
 */
__attribute__((always_inline))
static inline cache_bus_mask_t cache_ll_l1_get_bus(uint32_t bus_id, uint32_t vaddr_start, uint32_t len)
{
    return (cache_bus_mask_t)(CACHE_LL_DEFAULT_IBUS_MASK | CACHE_LL_DEFAULT_DBUS_MASK);
}

/**
 * Enable the L1 Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be enabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_enable_bus(uint32_t bus_id, cache_bus_mask_t mask)
{
    //not used, for compatibility
}

/**
 * Disable the Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be disabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_bus(uint32_t bus_id, cache_bus_mask_t mask)
{
    //not used, for compatibility
}

/**
 * @brief Get the buses of a particular cache that are mapped to a virtual address range
 *
 * @param cache_id          cache ID (when l1 cache is per core)
 * @param vaddr_start       virtual address start
 * @param len               vaddr length
 */
__attribute__((always_inline))
static inline cache_bus_mask_t cache_ll_l2_get_bus(uint32_t cache_id, uint32_t vaddr_start, uint32_t len)
{
    cache_bus_mask_t mask = (cache_bus_mask_t)0;

    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= SOC_DRAM_FLASH_ADDRESS_LOW && vaddr_end < SOC_DRAM_FLASH_ADDRESS_HIGH) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS0);
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS0);
    } else if (vaddr_start >= SOC_DRAM_PSRAM_ADDRESS_LOW && vaddr_end < SOC_DRAM_PSRAM_ADDRESS_HIGH) {
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_IBUS1);
        mask = (cache_bus_mask_t)(mask | CACHE_BUS_DBUS1);
    } else {
        HAL_ASSERT(0);
    }

    return mask;
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

    if (vaddr_start >= SOC_IRAM0_ADDRESS_LOW && vaddr_end < SOC_IRAM0_ADDRESS_HIGH) {
        *out_level = 1;
        *out_id = CACHE_LL_ID_ALL;
        valid = true;
    } else if (vaddr_start >= SOC_DRAM_FLASH_ADDRESS_LOW && vaddr_end < SOC_DRAM_PSRAM_ADDRESS_HIGH) {
        //PSRAM vaddr is right after the FLASH vaddr
        *out_level = 2;
        *out_id = CACHE_LL_ID_ALL;
        valid = true;
    }

    return valid;
}

/*------------------------------------------------------------------------------
 * Interrupt
 *----------------------------------------------------------------------------*/
/**
 * @brief Enable L1 Cache access error interrupt
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_enable_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    CACHE.l1_cache_acs_fail_int_ena.val |= mask;
}

/**
 * @brief Clear L1 Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_clear_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    CACHE.l1_cache_acs_fail_int_clr.val = mask;
}

/**
 * @brief Get L1 Cache access error interrupt status
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

/**
 * @brief Enable L2 Cache access error interrupt
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l2_enable_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    CACHE.l2_cache_acs_fail_int_ena.val |= mask;
}

/**
 * @brief Clear L2 Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l2_clear_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    CACHE.l2_cache_acs_fail_int_clr.val = mask;
}

/**
 * @brief Get L2 Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 *
 * @return            Status mask
 */
static inline uint32_t cache_ll_l2_get_access_error_intr_status(uint32_t cache_id, uint32_t mask)
{
    return CACHE.l2_cache_acs_fail_int_st.val & mask;
}

#ifdef __cplusplus
}
#endif
