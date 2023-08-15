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
#if CONFIG_IDF_TARGET_ESP32P4  //TODO: IDF-7516
#define DATA_AUTOLOAD_ENABLE      Cache_Disable_L2_Cache()
#define INST_AUTOLOAD_ENABLE      Cache_Disable_L2_Cache()
#else
#define DATA_AUTOLOAD_ENABLE      cache_ll_is_cache_autoload_enabled(CACHE_TYPE_DATA)
#define INST_AUTOLOAD_ENABLE      cache_ll_is_cache_autoload_enabled(CACHE_TYPE_INSTRUCTION)
#endif

/**
 * Necessary hal contexts, could be maintained by upper layer in the future
 */
typedef struct {
    bool data_autoload_en;
    bool inst_autoload_en;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    // There's no register indicating if cache is enabled on these chips, use sw flag to save this state.
    volatile bool cache_enabled;
#endif
} cache_hal_context_t;

static cache_hal_context_t ctx;

void cache_hal_init(void)
{
    ctx.data_autoload_en = DATA_AUTOLOAD_ENABLE;
    ctx.inst_autoload_en = INST_AUTOLOAD_ENABLE;
#if SOC_CACHE_L2_SUPPORTED
    Cache_Enable_L2_Cache(ctx.inst_autoload_en);
#else
    cache_ll_enable_cache(CACHE_TYPE_ALL, ctx.inst_autoload_en, ctx.data_autoload_en);
#endif //SOC_CACHE_L2_SUPPORTED

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
#if SOC_CACHE_L2_SUPPORTED
    Cache_Disable_L2_Cache();
#else
    cache_ll_disable_cache(type);
#endif //SOC_CACHE_L2_SUPPORTED

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 0;
#endif
}

void cache_hal_enable(cache_type_t type)
{
#if SOC_CACHE_L2_SUPPORTED
    Cache_Enable_L2_Cache(ctx.inst_autoload_en);
#else
    cache_ll_enable_cache(type, ctx.inst_autoload_en, ctx.data_autoload_en);
#endif //SOC_CACHE_L2_SUPPORTED

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 1;
#endif
}

void cache_hal_suspend(cache_type_t type)
{
#if SOC_CACHE_L2_SUPPORTED
    Cache_Suspend_L2_Cache();
#else
    cache_ll_suspend_cache(type);
#endif //SOC_CACHE_L2_SUPPORTED

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 0;
#endif
}

void cache_hal_resume(cache_type_t type)
{
#if SOC_CACHE_L2_SUPPORTED
    Cache_Resume_L2_Cache(ctx.inst_autoload_en);
#else
    cache_ll_resume_cache(type, ctx.inst_autoload_en, ctx.data_autoload_en);
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.cache_enabled = 1;
#endif
}

bool cache_hal_is_cache_enabled(cache_type_t type)
{
    bool enabled;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    enabled = ctx.cache_enabled;
#else
    enabled = cache_ll_is_cache_enabled(type);
#endif //CACHE_LL_ENABLE_DISABLE_STATE_SW
    return enabled;
}

void cache_hal_invalidate_addr(uint32_t vaddr, uint32_t size)
{
    //Now only esp32 has 2 MMUs, this file doesn't build on esp32
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(0, vaddr, size, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION));
#if CONFIG_IDF_TARGET_ESP32P4
    Cache_Invalidate_Addr(CACHE_MAP_L1_DCACHE | CACHE_MAP_L2_CACHE, vaddr, size);
#else
    cache_ll_invalidate_addr(vaddr, size);
#endif
}

#if SOC_CACHE_WRITEBACK_SUPPORTED
void cache_hal_writeback_addr(uint32_t vaddr, uint32_t size)
{
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(0, vaddr, size, MMU_VADDR_DATA));
#if CONFIG_IDF_TARGET_ESP32P4
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, vaddr, size);
    Cache_WriteBack_Addr(CACHE_MAP_L2_CACHE, vaddr, size);
#else
    cache_ll_writeback_addr(vaddr, size);
#endif
}
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED


#if SOC_CACHE_FREEZE_SUPPORTED
void cache_hal_freeze(cache_type_t type)
{
#if SOC_CACHE_L2_SUPPORTED
    Cache_Freeze_L2_Cache_Enable(CACHE_FREEZE_ACK_BUSY);
#else
    cache_ll_freeze_cache(type);
#endif //SOC_CACHE_L2_SUPPORTED
}

void cache_hal_unfreeze(cache_type_t type)
{
#if SOC_CACHE_L2_SUPPORTED
    Cache_Freeze_L2_Cache_Disable();
#else
    cache_ll_unfreeze_cache(type);
#endif //SOC_CACHE_L2_SUPPORTED
}
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

uint32_t cache_hal_get_cache_line_size(cache_type_t type)
{
    uint32_t line_size = 0;
#if SOC_CACHE_L2_SUPPORTED
    line_size = Cache_Get_L2_Cache_Line_Size();
#else
    line_size = cache_ll_get_line_size(type);
#endif //SOC_CACHE_L2_SUPPORTED
    return line_size;
}
