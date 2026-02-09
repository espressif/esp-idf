/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "rom/cache.h"
#include "soc/soc_caps.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"

static IRAM_ATTR esp_err_t start(void *arg, uint32_t flags)
{
#if SOC_BRANCH_PREDICTOR_SUPPORTED
    //branch predictor will start cache request as well
    esp_cpu_branch_prediction_disable();
#endif

#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);
#else
    cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif

    return ESP_OK;
}

static IRAM_ATTR esp_err_t end(void *arg)
{
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
    Cache_Read_Enable(1);
#else
    cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif

#if SOC_BRANCH_PREDICTOR_SUPPORTED
    esp_cpu_branch_prediction_enable();
#endif

    return ESP_OK;
}

static esp_err_t delay_us(void *arg, uint32_t us)
{
    esp_rom_delay_us(us);
    return ESP_OK;
}

// Currently when the os is not up yet, the caller is supposed to call esp_flash APIs with proper
// buffers.
void* get_temp_buffer_not_supported(void* arg, size_t reqest_size, size_t* out_size)
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

esp_err_t esp_flash_app_disable_os_functions(esp_flash_t* chip)
{
    chip->os_func = &esp_flash_noos_functions;

    return ESP_OK;
}
