/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "rom/cache.h"
#include "esp_private/esp_cache_esp32_private.h"

static cache_driver_t s_cache_drv = {
    Cache_Flush,
    NULL,
};

void cache_register_writeback(cache_driver_t *func)
{
    s_cache_drv.cache_writeback_psram = func->cache_writeback_psram;
}

void cache_sync(void)
{
    if (s_cache_drv.cache_writeback_psram) {
        s_cache_drv.cache_writeback_psram();
    }

    s_cache_drv.cache_flush(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_cache_drv.cache_flush(1);
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
}
