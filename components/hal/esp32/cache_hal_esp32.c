/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/cache_ll.h"
#include "hal/cache_hal.h"

static uint32_t s_cache_status[2];

/**
 * On ESP32, The cache_hal_suspend()/cache_hal_resume() are replacements
 * for Cache_Read_Disable()/Cache_Read_Enable() in ROM.
 * There's a bug that Cache_Read_Disable requires a call to Cache_Flush
 * before Cache_Read_Enable, even if cached data was not modified.
 */
void cache_hal_suspend(uint32_t cache_level, cache_type_t type)
{
    s_cache_status[0] = cache_ll_l1_get_enabled_bus(0);
    cache_ll_l1_disable_cache(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_cache_status[1] = cache_ll_l1_get_enabled_bus(1);
    cache_ll_l1_disable_cache(1);
#endif
}


void cache_hal_resume(uint32_t cache_level, cache_type_t type)
{
    cache_ll_l1_enable_cache(0);
    cache_ll_l1_enable_bus(0, s_cache_status[0]);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    cache_ll_l1_enable_cache(1);
    cache_ll_l1_enable_bus(1, s_cache_status[1]);
#endif
}


bool cache_hal_is_cache_enabled(uint32_t cache_level, cache_type_t type)
{
    bool result = cache_ll_l1_is_cache_enabled(0, CACHE_TYPE_ALL);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    result = result && cache_ll_l1_is_cache_enabled(1, CACHE_TYPE_ALL);
#endif
    return result;
}

bool cache_hal_vaddr_to_cache_level_id(uint32_t vaddr_start, uint32_t len, uint32_t *out_level, uint32_t *out_id)
{
    if (!out_level || !out_id) {
        return false;
    }
    return cache_ll_vaddr_to_cache_level_id(vaddr_start, len, out_level, out_id);
}

uint32_t cache_hal_get_cache_line_size(uint32_t cache_level, cache_type_t type)
{
    HAL_ASSERT(cache_level <= CACHE_LL_LEVEL_NUMS);

    uint32_t line_size = 0;

    if (cache_level == CACHE_LL_LEVEL_EXT_MEM) {
        line_size = 4;
    }

    return line_size;
}

bool cache_hal_invalidate_addr(uint32_t vaddr, uint32_t size)
{
    //esp32 doesn't support invalidate certain addr
    abort();
}
