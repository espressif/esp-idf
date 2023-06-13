/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "hal/assert.h"
#include "hal/cache_hal.h"
#include "hal/cache_types.h"
#include "hal/cache_ll.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "soc/soc_caps.h"
#include "rom/cache.h"

/*------------------------------------------------------------------------------
 * Unified Cache Control
 * See cache_hal.h for more info about these HAL APIs
 * This file is in internal RAM.
 * Now this file doesn't compile on ESP32
 *----------------------------------------------------------------------------*/

/**
 * To know if autoload is enabled or not.
 *
 * We should have a unified flag for this aim, then we don't need to call following 2 functions
 * to know the flag.
 *
 * Suggest ROM keeping this flag value to BIT(2). Then we can replace following lines to:
 * #define DATA_AUTOLOAD_FLAG      BIT(2)
 * #define INST_AUTOLOAD_FLAG      BIT(2)
 */
#define DATA_AUTOLOAD_FLAG      Cache_Disable_DCache()
#define INST_AUTOLOAD_FLAG      Cache_Disable_ICache()

/**
 * Necessary hal contexts, could be maintained by upper layer in the future
 */
typedef struct {
    uint32_t data_autoload_flag;
    uint32_t inst_autoload_flag;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    // There's no register indicating if cache is enabled on these chips, use sw flag to save this state.
    volatile bool cache_enabled;
#endif
} cache_hal_context_t;

static cache_hal_context_t ctx;

void cache_hal_init(void)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    ctx.data_autoload_flag = INST_AUTOLOAD_FLAG;
    Cache_Enable_ICache(ctx.data_autoload_flag);
#else
    ctx.data_autoload_flag = DATA_AUTOLOAD_FLAG;
    Cache_Enable_DCache(ctx.data_autoload_flag);
    ctx.inst_autoload_flag = INST_AUTOLOAD_FLAG;
    Cache_Enable_ICache(ctx.inst_autoload_flag);
#endif

    cache_ll_l1_enable_bus(0, CACHE_LL_DEFAULT_DBUS_MASK);
    cache_ll_l1_enable_bus(0, CACHE_LL_DEFAULT_IBUS_MASK);

#if !CONFIG_FREERTOS_UNICORE
    cache_ll_l1_enable_bus(1, CACHE_LL_DEFAULT_DBUS_MASK);
    cache_ll_l1_enable_bus(1, CACHE_LL_DEFAULT_IBUS_MASK);
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 1;
#endif
}

void cache_hal_disable(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Disable_ICache();
#else
    if (type == CACHE_TYPE_DATA) {
        Cache_Disable_DCache();
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        Cache_Disable_ICache();
    } else {
        Cache_Disable_ICache();
        Cache_Disable_DCache();
    }
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 0;
#endif
}

void cache_hal_enable(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Enable_ICache(ctx.inst_autoload_flag);
#else
    if (type == CACHE_TYPE_DATA) {
        Cache_Enable_DCache(ctx.data_autoload_flag);
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        Cache_Enable_ICache(ctx.inst_autoload_flag);
    } else {
        Cache_Enable_ICache(ctx.inst_autoload_flag);
        Cache_Enable_DCache(ctx.data_autoload_flag);
    }
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 1;
#endif
}

void cache_hal_suspend(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Suspend_ICache();
#else
    if (type == CACHE_TYPE_DATA) {
        Cache_Suspend_DCache();
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        Cache_Suspend_ICache();
    } else {
        Cache_Suspend_ICache();
        Cache_Suspend_DCache();
    }
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 0;
#endif
}

void cache_hal_resume(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Resume_ICache(ctx.inst_autoload_flag);
#else
    if (type == CACHE_TYPE_DATA) {
        Cache_Resume_DCache(ctx.data_autoload_flag);
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        Cache_Resume_ICache(ctx.inst_autoload_flag);
    } else {
        Cache_Resume_ICache(ctx.inst_autoload_flag);
        Cache_Resume_DCache(ctx.data_autoload_flag);
    }
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 1;
#endif
}

bool cache_hal_is_cache_enabled(cache_type_t type)
{
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    return ctx.cache_enabled;
#else
    return cache_ll_l1_is_cache_enabled(0, type);
#endif
}

void cache_hal_invalidate_addr(uint32_t vaddr, uint32_t size)
{
    //Now only esp32 has 2 MMUs, this file doesn't build on esp32
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(0, vaddr, size, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION));
    Cache_Invalidate_Addr(vaddr, size);
}

#if SOC_CACHE_WRITEBACK_SUPPORTED
void cache_hal_writeback_addr(uint32_t vaddr, uint32_t size)
{
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(0, vaddr, size, MMU_VADDR_DATA));
    Cache_WriteBack_Addr(vaddr, size);
}
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED


#if SOC_CACHE_FREEZE_SUPPORTED
void cache_hal_freeze(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Freeze_ICache_Enable(CACHE_FREEZE_ACK_BUSY);
#else
    if (type == CACHE_TYPE_DATA) {
        Cache_Freeze_DCache_Enable(CACHE_FREEZE_ACK_BUSY);
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        Cache_Freeze_ICache_Enable(CACHE_FREEZE_ACK_BUSY);
    } else {
        Cache_Freeze_ICache_Enable(CACHE_FREEZE_ACK_BUSY);
        Cache_Freeze_DCache_Enable(CACHE_FREEZE_ACK_BUSY);
    }
#endif
}

void cache_hal_unfreeze(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Freeze_ICache_Disable();
#else
    if (type == CACHE_TYPE_DATA) {
        Cache_Freeze_DCache_Disable();
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        Cache_Freeze_ICache_Disable();
    } else {
        Cache_Freeze_DCache_Disable();
        Cache_Freeze_ICache_Disable();
    }
#endif
}
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

uint32_t cache_hal_get_cache_line_size(cache_type_t type)
{
#if SOC_SHARED_IDCACHE_SUPPORTED
    return Cache_Get_ICache_Line_Size();
#else
    uint32_t size = 0;
    if (type == CACHE_TYPE_DATA) {
        size = Cache_Get_DCache_Line_Size();
    } else if (type == CACHE_TYPE_INSTRUCTION) {
        size = Cache_Get_ICache_Line_Size();
    } else {
        HAL_ASSERT(false);
    }
    return size;
#endif
}
