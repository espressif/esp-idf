/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include "sdkconfig.h"
#include "esp_flash.h"
#include "esp_attr.h"

#include "esp_rom_sys.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/ets_sys.h"
#include "esp32c3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/ets_sys.h"
#include "esp32h2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/ets_sys.h"
#include "esp32c2/rom/cache.h"
#endif

#include "esp_attr.h"

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
typedef struct {
    uint32_t icache_autoload;
    uint32_t dcache_autoload;
} spi_noos_arg_t;

static DRAM_ATTR spi_noos_arg_t spi_arg = { 0 };
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C2
typedef struct {
    uint32_t icache_autoload;
} spi_noos_arg_t;

static DRAM_ATTR spi_noos_arg_t spi_arg = { 0 };
#endif

static IRAM_ATTR esp_err_t start(void *arg)
{
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    spi_noos_arg_t *spi_arg = arg;
    spi_arg->icache_autoload = Cache_Suspend_ICache();
    spi_arg->dcache_autoload = Cache_Suspend_DCache();
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C2
    spi_noos_arg_t *spi_arg = arg;
    spi_arg->icache_autoload = Cache_Suspend_ICache();
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
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    spi_noos_arg_t *spi_arg = arg;
    Cache_Invalidate_ICache_All();
    Cache_Resume_ICache(spi_arg->icache_autoload);
    Cache_Resume_DCache(spi_arg->dcache_autoload);
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C2
    spi_noos_arg_t *spi_arg = arg;
    Cache_Invalidate_ICache_All();
    Cache_Resume_ICache(spi_arg->icache_autoload);
#endif
    return ESP_OK;
}

static IRAM_ATTR esp_err_t delay_us(void *arg, uint32_t us)
{
    esp_rom_delay_us(us);
    return ESP_OK;
}

// Currently when the os is not up yet, the caller is supposed to call esp_flash APIs with proper
// buffers.
IRAM_ATTR void* get_temp_buffer_not_supported(void* arg, size_t reqest_size, size_t* out_size)
{
    return NULL;
}

const DRAM_ATTR esp_flash_os_functions_t esp_flash_noos_functions = {
    .start = start,
    .end = end,
    .delay_us = delay_us,
    .region_protected = NULL,
    /* the caller is supposed to call esp_flash_read/esp_flash_write APIs with buffers in DRAM */
    .get_temp_buffer = NULL,
    .release_temp_buffer = NULL,
    .yield = NULL,
};

esp_err_t IRAM_ATTR esp_flash_app_disable_os_functions(esp_flash_t* chip)
{
    chip->os_func = &esp_flash_noos_functions;

#if !CONFIG_IDF_TARGET_ESP32
    chip->os_func_data = &spi_arg;
#endif

    return ESP_OK;
}
