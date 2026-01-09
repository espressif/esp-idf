/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_private/esp_psram_mspi.h"

__attribute__((unused)) ESP_LOG_ATTR_TAG_DRAM(TAG, "psram_mspi");

#if ESP_PSRAM_MSPI_MB_WORKAROUND
static void *s_psram_mb_dummy_cacheline;  //dummy cacheline for cache memory barrier
#endif

esp_err_t esp_psram_mspi_mb_init(void)
{
#if ESP_PSRAM_MSPI_MB_WORKAROUND
    s_psram_mb_dummy_cacheline = heap_caps_calloc(1, CONFIG_CACHE_L1_CACHE_LINE_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_CACHE_ALIGNED);
    if (!s_psram_mb_dummy_cacheline) {
        ESP_EARLY_LOGE(TAG, "Failed to allocate dummy cacheline for PSRAM memory barrier!");
    }
#endif

    return ESP_OK;
}

void IRAM_ATTR esp_psram_mspi_mb(void)
{
#if ESP_PSRAM_MSPI_MB_WORKAROUND
    if (s_psram_mb_dummy_cacheline) {
        uint32_t *p = (uint32_t *)s_psram_mb_dummy_cacheline;
        *p = (*p + 1) % UINT32_MAX;
        __attribute__((unused)) esp_err_t ret = ESP_FAIL;
        ret = esp_cache_msync(s_psram_mb_dummy_cacheline, sizeof(uint32_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED); //malloc is aligned, no need to writeback all
        assert(ret == ESP_OK);
        asm volatile("fence");
    }
#endif
}
