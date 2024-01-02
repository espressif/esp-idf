/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_memory_utils.h"
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

static bool s_buf_in_region(const void *ptr, size_t size, esp_dma_buf_location_t location, uint32_t *in_out_flags)
{
    bool found = false;
    if (location == ESP_DMA_BUF_LOCATION_INTERNAL) {
        if (esp_ptr_dma_capable(ptr) && esp_ptr_dma_capable(ptr + size - 1)) {
            found = true;
        }
    } else if (location == ESP_DMA_BUF_LOCATION_PSRAM) {
#if SOC_PSRAM_DMA_CAPABLE
        if (esp_ptr_external_ram(ptr) && esp_ptr_external_ram(ptr + size - 1)) {
            *in_out_flags |= ESP_DMA_MALLOC_FLAG_PSRAM;
            found = true;
        }
#endif
    }
    return found;
}

bool esp_dma_is_buffer_aligned(const void *ptr, size_t size, esp_dma_buf_location_t location)
{
    assert(ptr);
    uint32_t flags = ESP_CACHE_MALLOC_FLAG_DMA;

    bool found = false;
    if (location == ESP_DMA_BUF_LOCATION_AUTO) {
        for (int i = ESP_DMA_BUF_LOCATION_INTERNAL; i < ESP_DMA_BUF_LOCATION_AUTO; i++) {
            if (s_buf_in_region(ptr, size, i, &flags)) {
                found = true;
                break;
            }
        }
    } else if (location == ESP_DMA_BUF_LOCATION_INTERNAL) {
        found = s_buf_in_region(ptr, size, ESP_DMA_BUF_LOCATION_INTERNAL, &flags);
    } else {
        found = s_buf_in_region(ptr, size, ESP_DMA_BUF_LOCATION_PSRAM, &flags);
    }
    if (!found) {
        return false;
    }

    bool is_aligned = false;
    size_t dma_alignment = 0;
    size_t cache_alignment = 0;
    size_t alignment = 0;
    esp_err_t ret = esp_cache_get_alignment(flags, &cache_alignment);
    assert(ret == ESP_OK);
    alignment = MAX(dma_alignment, cache_alignment);
    is_aligned = ((intptr_t)ptr % alignment == 0) && (size % alignment == 0);

    return is_aligned;
}
