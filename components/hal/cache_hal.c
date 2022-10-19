/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32H4
#include "esp32h4/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/cache.h"
#endif

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
}
