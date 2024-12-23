/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/cache_ll.h"
#include "hal/cache_hal.h"

static uint32_t s_cache_status[2];

void cache_hal_init(void)
{
    //for compatibility
}

void cache_hal_suspend(cache_type_t type)
{
    s_cache_status[0] = cache_ll_l1_get_enabled_bus(0);
    cache_ll_l1_disable_cache(0);
#if !CONFIG_FREERTOS_UNICORE
    s_cache_status[1] = cache_ll_l1_get_enabled_bus(1);
    cache_ll_l1_disable_cache(1);
#endif
}


void cache_hal_resume(cache_type_t type)
{
    cache_ll_l1_enable_cache(0);
    cache_ll_l1_enable_bus(0, s_cache_status[0]);
#if !CONFIG_FREERTOS_UNICORE
    cache_ll_l1_enable_cache(1);
    cache_ll_l1_enable_bus(1, s_cache_status[1]);
#endif
}


bool cache_hal_is_cache_enabled(cache_type_t type)
{
    bool result = cache_ll_l1_is_cache_enabled(0, CACHE_TYPE_ALL);
#if !CONFIG_FREERTOS_UNICORE
    result = result && cache_ll_l1_is_cache_enabled(1, CACHE_TYPE_ALL);
#endif
    return result;
}
