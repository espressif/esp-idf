/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include <stdint.h>
#include <stdbool.h>
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
 * Necessary hal contexts, could be maintained by upper layer in the future
 */
typedef struct {
    bool i_autoload_en;
    bool d_autoload_en;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    // There's no register indicating if cache is enabled on these chips, use sw flag to save this state.
    bool i_cache_enabled;
    bool d_cache_enabled;
#endif
} cache_hal_state_t;


typedef struct {
    cache_hal_state_t l1;
    cache_hal_state_t l2;
} cache_hal_context_t;

static cache_hal_context_t ctx;


void s_cache_hal_init_ctx(void)
{
    ctx.l1.d_autoload_en = cache_ll_is_cache_autoload_enabled(1, CACHE_TYPE_DATA, CACHE_LL_ID_ALL);
    ctx.l1.i_autoload_en = cache_ll_is_cache_autoload_enabled(1, CACHE_TYPE_INSTRUCTION, CACHE_LL_ID_ALL);
    ctx.l2.d_autoload_en = cache_ll_is_cache_autoload_enabled(2, CACHE_TYPE_DATA, CACHE_LL_ID_ALL);
    ctx.l2.i_autoload_en = cache_ll_is_cache_autoload_enabled(2, CACHE_TYPE_INSTRUCTION, CACHE_LL_ID_ALL);
}

void cache_hal_init(void)
{
    s_cache_hal_init_ctx();

    if (CACHE_LL_LEVEL_EXT_MEM == 1) {
        cache_ll_enable_cache(1, CACHE_TYPE_ALL, CACHE_LL_ID_ALL, ctx.l1.i_autoload_en, ctx.l1.d_autoload_en);
    } else if (CACHE_LL_LEVEL_EXT_MEM == 2) {
        cache_ll_enable_cache(2, CACHE_TYPE_ALL, CACHE_LL_ID_ALL, ctx.l2.i_autoload_en, ctx.l2.d_autoload_en);
    }

    cache_ll_l1_enable_bus(0, CACHE_LL_DEFAULT_DBUS_MASK);
    cache_ll_l1_enable_bus(0, CACHE_LL_DEFAULT_IBUS_MASK);
#if !CONFIG_FREERTOS_UNICORE
    cache_ll_l1_enable_bus(1, CACHE_LL_DEFAULT_DBUS_MASK);
    cache_ll_l1_enable_bus(1, CACHE_LL_DEFAULT_IBUS_MASK);
#endif

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    ctx.l1.i_cache_enabled = 1;
    ctx.l1.d_cache_enabled = 1;
    ctx.l2.i_cache_enabled = 1;
    ctx.l2.d_cache_enabled = 1;
#endif
}

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
void s_update_cache_state(uint32_t cache_level, cache_type_t type, bool en)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    switch (cache_level) {
    case 1:
        if (type == CACHE_TYPE_INSTRUCTION) {
            ctx.l1.i_cache_enabled = en;
            break;
        } else if (type == CACHE_TYPE_DATA) {
            ctx.l1.d_cache_enabled = en;
            break;
        } else if (type == CACHE_TYPE_ALL) {
            ctx.l1.i_cache_enabled = en;
            ctx.l1.d_cache_enabled = en;
            break;
        } else {
            HAL_ASSERT(false);
            break;
        }
    case 2:
        if (type == CACHE_TYPE_INSTRUCTION) {
            ctx.l2.i_cache_enabled = en;
            break;
        } else if (type == CACHE_TYPE_DATA) {
            ctx.l2.d_cache_enabled = en;
            break;
        } else if (type == CACHE_TYPE_ALL) {
            ctx.l2.i_cache_enabled = en;
            ctx.l2.d_cache_enabled = en;
            break;
        } else {
            HAL_ASSERT(false);
            break;
        }
    default:
        HAL_ASSERT(false);
        break;
    }
}

bool s_get_cache_state(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));
    bool enabled = false;

    switch (cache_level) {
    case 1:
        if (type == CACHE_TYPE_INSTRUCTION) {
            enabled = ctx.l1.i_cache_enabled;
            break;
        } else if (type == CACHE_TYPE_DATA) {
            enabled = ctx.l1.d_cache_enabled;
            break;
        } else if (type == CACHE_TYPE_ALL) {
            enabled = ctx.l1.i_cache_enabled;
            enabled &= ctx.l1.d_cache_enabled;
            break;
        } else {
            HAL_ASSERT(false);
            break;
        }
    case 2:
        if (type == CACHE_TYPE_INSTRUCTION) {
            enabled = ctx.l2.i_cache_enabled;
            break;
        } else if (type == CACHE_TYPE_DATA) {
            enabled = ctx.l2.d_cache_enabled;
            break;
        } else if (type == CACHE_TYPE_ALL) {
            enabled = ctx.l2.i_cache_enabled;
            enabled &= ctx.l2.d_cache_enabled;
            break;
        } else {
            HAL_ASSERT(false);
            break;
        }
    default:
        HAL_ASSERT(false);
        break;
    }

    return enabled;
}
#endif  //#if CACHE_LL_ENABLE_DISABLE_STATE_SW

void cache_hal_disable(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    cache_ll_disable_cache(cache_level, type, CACHE_LL_ID_ALL);

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    s_update_cache_state(cache_level, type, false);
#endif
}

void cache_hal_enable(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    if (cache_level == 1) {
        cache_ll_enable_cache(1, type, CACHE_LL_ID_ALL, ctx.l1.i_autoload_en, ctx.l1.d_autoload_en);
    } else if (cache_level == 2) {
        cache_ll_enable_cache(2, type, CACHE_LL_ID_ALL, ctx.l2.i_autoload_en, ctx.l2.d_autoload_en);
    }

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    s_update_cache_state(cache_level, type, true);
#endif
}

void cache_hal_suspend(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    cache_ll_suspend_cache(cache_level, type, CACHE_LL_ID_ALL);

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    s_update_cache_state(cache_level, type, false);
#endif
}

void cache_hal_resume(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    if (cache_level == 1) {
        cache_ll_resume_cache(1, type, CACHE_LL_ID_ALL, ctx.l1.i_autoload_en, ctx.l1.d_autoload_en);
    } else if (cache_level == 2) {
        cache_ll_resume_cache(2, type, CACHE_LL_ID_ALL, ctx.l2.i_autoload_en, ctx.l2.d_autoload_en);
    }

#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    s_update_cache_state(cache_level, type, true);
#endif
}

bool cache_hal_is_cache_enabled(uint32_t cache_level, cache_type_t type)
{
    bool enabled = false;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    enabled = s_get_cache_state(cache_level, type);
#else
    enabled = cache_ll_is_cache_enabled(type);
#endif //CACHE_LL_ENABLE_DISABLE_STATE_SW
    return enabled;
}

bool cache_hal_vaddr_to_cache_level_id(uint32_t vaddr_start, uint32_t len, uint32_t *out_level, uint32_t *out_id)
{
    if (!out_level || !out_id) {
        return false;
    }
    return cache_ll_vaddr_to_cache_level_id(vaddr_start, len, out_level, out_id);
}

bool cache_hal_invalidate_addr(uint32_t vaddr, uint32_t size)
{
    bool valid = false;
    uint32_t cache_level = 0;
    uint32_t cache_id = 0;

    valid = cache_hal_vaddr_to_cache_level_id(vaddr, size, &cache_level, &cache_id);
    if (valid) {
        cache_ll_invalidate_addr(cache_level, CACHE_TYPE_ALL, cache_id, vaddr, size);
    }

    return valid;
}

#if SOC_CACHE_WRITEBACK_SUPPORTED
bool cache_hal_writeback_addr(uint32_t vaddr, uint32_t size)
{
    bool valid = false;
    uint32_t cache_level = 0;
    uint32_t cache_id = 0;

    valid = cache_hal_vaddr_to_cache_level_id(vaddr, size, &cache_level, &cache_id);
    if (valid) {
        cache_ll_writeback_addr(cache_level, CACHE_TYPE_DATA, cache_id, vaddr, size);
    }

    return valid;
}
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED

#if SOC_CACHE_FREEZE_SUPPORTED
void cache_hal_freeze(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    cache_ll_freeze_cache(cache_level, type, CACHE_LL_ID_ALL);
}

void cache_hal_unfreeze(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));

    cache_ll_unfreeze_cache(cache_level, type, CACHE_LL_ID_ALL);
}
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

uint32_t cache_hal_get_cache_line_size(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level && (cache_level <= CACHE_LL_LEVEL_NUMS));
    uint32_t line_size = 0;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    line_size = cache_ll_get_line_size(cache_level, type, CACHE_LL_ID_ALL);
#else
    if (cache_level == CACHE_LL_LEVEL_EXT_MEM) {
        line_size = cache_ll_get_line_size(cache_level, type, CACHE_LL_ID_ALL);
    }
#endif

    return line_size;
}
