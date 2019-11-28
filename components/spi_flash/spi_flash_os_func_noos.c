// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdarg.h>
#include "sdkconfig.h"
#include "esp_flash.h"
#include "esp_attr.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#include "esp32s2beta/rom/cache.h"
#endif

#include "esp_attr.h"

#if CONFIG_IDF_TARGET_ESP32S2BETA
typedef struct {
    uint32_t icache_autoload;
    uint32_t dcache_autoload;
} spi_noos_arg_t; 

static DRAM_ATTR spi_noos_arg_t spi_arg = { 0 };
#endif

static IRAM_ATTR esp_err_t start(void *arg)
{
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    spi_noos_arg_t *spi_arg = arg;
    spi_arg->icache_autoload = Cache_Suspend_ICache();
    spi_arg->dcache_autoload = Cache_Suspend_DCache();
#endif
    return ESP_OK;
}

static IRAM_ATTR esp_err_t end(void *arg)
{
#if CONFIG_IDF_TARGET_ESP32
    Cache_Flush(0);
    Cache_Flush(1);
    Cache_Read_Enable(0);
    Cache_Read_Enable(1);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    spi_noos_arg_t *spi_arg = arg;
    Cache_Invalidate_ICache_All();
    Cache_Resume_ICache(spi_arg->icache_autoload);
    Cache_Resume_DCache(spi_arg->dcache_autoload);
#endif
    return ESP_OK;
}

static IRAM_ATTR esp_err_t delay_ms(void *arg, unsigned ms)
{
    ets_delay_us(1000 * ms);
    return ESP_OK;
}

const DRAM_ATTR esp_flash_os_functions_t esp_flash_noos_functions = {
    .start = start,
    .end = end,
    .delay_ms = delay_ms,
    .region_protected = NULL,
};

esp_err_t IRAM_ATTR esp_flash_app_disable_os_functions(esp_flash_t* chip)
{
    chip->os_func = &esp_flash_noos_functions;

#if CONFIG_IDF_TARGET_ESP32S2BETA
    chip->os_func_data = &spi_arg;
#endif

    return ESP_OK;
}
