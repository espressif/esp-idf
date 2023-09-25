/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_dma_utils.h"
#include "esp_private/esp_cache_private.h"
#include "soc/soc_caps.h"

static const char *TAG = "dma_utils";
_Static_assert(ESP_DMA_MALLOC_FLAG_PSRAM == ESP_CACHE_MALLOC_FLAG_PSRAM);

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))


esp_err_t esp_dma_malloc(size_t size, uint32_t flags, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    esp_err_t ret = ESP_OK;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    ret = esp_cache_aligned_malloc(size, flags | ESP_CACHE_MALLOC_FLAG_DMA, out_ptr, actual_size);
#else
    if (flags & ESP_DMA_MALLOC_FLAG_PSRAM) {
        ret = esp_cache_aligned_malloc(size, flags | ESP_CACHE_MALLOC_FLAG_DMA, out_ptr, actual_size);
    } else {
        size = ALIGN_UP_BY(size, 4);
        void *ptr = heap_caps_aligned_alloc(4, size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        ESP_RETURN_ON_FALSE_ISR(ptr, ESP_ERR_NO_MEM, TAG, "no enough heap memory");
        *out_ptr = ptr;
        if (actual_size) {
            *actual_size = size;
        }
    }
#endif

    return ret;
}


esp_err_t esp_dma_calloc(size_t n, size_t size, uint32_t flags, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    esp_err_t ret = ESP_FAIL;
    size_t size_bytes = 0;
    bool ovf = false;

    ovf = __builtin_mul_overflow(n, size, &size_bytes);
    ESP_RETURN_ON_FALSE_ISR(!ovf, ESP_ERR_INVALID_ARG, TAG, "wrong size, total size overflow");

    void *ptr = NULL;
    ret = esp_dma_malloc(size_bytes, flags, &ptr, actual_size);
    if (ret == ESP_OK) {
        memset(ptr, 0, size_bytes);
        *out_ptr = ptr;
    }

    return ret;
}
