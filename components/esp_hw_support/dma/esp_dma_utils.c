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
#include "esp_private/gdma.h"
#include "soc/soc_caps.h"
#include "hal/hal_utils.h"

static const char *TAG = "dma_utils";
_Static_assert(ESP_DMA_MALLOC_FLAG_PSRAM == ESP_CACHE_MALLOC_FLAG_PSRAM);

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN_BY(num, align) ((num) & (~((align) - 1)))

esp_err_t esp_dma_capable_malloc(size_t size, const esp_dma_mem_info_t *dma_mem_info, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(dma_mem_info && out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    size_t alignment = 1;

    //dma align
    size_t dma_alignment = dma_mem_info->dma_alignment;

    //custom align
    size_t custom_alignment = dma_mem_info->custom_alignment;

    //cache align
    int cache_flags = 0;
    size_t cache_alignment = 1;
    if (dma_mem_info->heap_caps & MALLOC_CAP_SPIRAM) {
        cache_flags |= ESP_DMA_MALLOC_FLAG_PSRAM;
    }
    esp_err_t ret = esp_cache_get_alignment(cache_flags, &cache_alignment);
    assert(ret == ESP_OK);

    //lcm3
    alignment = _lcm_3(dma_alignment, cache_alignment, custom_alignment);
    ESP_LOGD(TAG, "alignment: 0x%x", alignment);

    //malloc
    size = ALIGN_UP_BY(size, alignment);
    int heap_caps = dma_mem_info->heap_caps;

    void *ptr = heap_caps_aligned_alloc(alignment, size, heap_caps);
    ESP_RETURN_ON_FALSE_ISR(ptr, ESP_ERR_NO_MEM, TAG, "no enough heap memory");

    *out_ptr = ptr;
    if (actual_size) {
        *actual_size = size;
    }

    return ESP_OK;
}

esp_err_t esp_dma_capable_calloc(size_t n, size_t size, const esp_dma_mem_info_t *dma_mem_info, void **out_ptr, size_t *actual_size)
{
    esp_err_t ret = ESP_FAIL;
    size_t size_bytes = 0;
    bool ovf = false;

    ovf = __builtin_mul_overflow(n, size, &size_bytes);
    ESP_RETURN_ON_FALSE_ISR(!ovf, ESP_ERR_INVALID_ARG, TAG, "wrong size, total size overflow");

    void *ptr = NULL;
    ret = esp_dma_capable_malloc(size_bytes, dma_mem_info, &ptr, actual_size);
    if (ret == ESP_OK) {
        memset(ptr, 0, size_bytes);
        *out_ptr = ptr;
    }

    return ret;
}

static bool s_buf_in_region(const void *ptr, size_t size, esp_dma_buf_location_t location)
{
    bool found = false;
    if (location == ESP_DMA_BUF_LOCATION_INTERNAL) {
        if (esp_ptr_dma_capable(ptr) && esp_ptr_dma_capable(ptr + size - 1)) {
            found = true;
        }
    } else if (location == ESP_DMA_BUF_LOCATION_PSRAM) {
#if SOC_PSRAM_DMA_CAPABLE
        if (esp_ptr_external_ram(ptr) && esp_ptr_external_ram(ptr + size - 1)) {
            found = true;
        }
#endif
    }
    return found;
}

static inline bool s_is_buf_aligned(intptr_t ptr, size_t alignment)
{
    return (ptr % alignment == 0);
}

bool esp_dma_is_buffer_alignment_satisfied(const void *ptr, size_t size, esp_dma_mem_info_t *dma_mem_info)
{
    assert(ptr);

    bool found = false;
    for (int i = ESP_DMA_BUF_LOCATION_INTERNAL; i < ESP_DMA_BUF_LOCATION_AUTO; i++) {
        if (s_buf_in_region(ptr, size, i)) {
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    size_t alignment = 1;

    //dma align
    size_t dma_alignment = dma_mem_info->dma_alignment;

    //custom align
    size_t custom_alignment = dma_mem_info->custom_alignment;

    //cache align
    int cache_flags = 0;
    size_t cache_alignment = 1;
    if (dma_mem_info->heap_caps & MALLOC_CAP_SPIRAM) {
        cache_flags |= ESP_DMA_MALLOC_FLAG_PSRAM;
    }
    esp_err_t ret = esp_cache_get_alignment(cache_flags, &cache_alignment);
    assert(ret == ESP_OK);

    //lcm3
    alignment = _lcm_3(dma_alignment, cache_alignment, custom_alignment);

    bool is_aligned = s_is_buf_aligned((intptr_t)ptr, alignment) && s_is_buf_aligned((intptr_t)size, alignment);
    return is_aligned;
}


//-----------------------Deprecated APIs-----------------------//
esp_err_t s_legacy_malloc(size_t size, uint32_t flags, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    int heap_caps = 0;
    if (flags & ESP_DMA_MALLOC_FLAG_PSRAM) {
        heap_caps |= MALLOC_CAP_SPIRAM;
    } else {
        heap_caps |= MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL;
    }

    esp_dma_mem_info_t dma_mem_info = {
        .heap_caps = heap_caps,
        .dma_alignment = 4, //legacy API behaviour is only check max dma buffer alignment
    };

    ESP_RETURN_ON_ERROR_ISR(esp_dma_capable_malloc(size, &dma_mem_info, out_ptr, actual_size), TAG, "failed to do malloc");

    return ESP_OK;
}

esp_err_t esp_dma_malloc(size_t size, uint32_t flags, void **out_ptr, size_t *actual_size)
{
    return s_legacy_malloc(size, flags, out_ptr, actual_size);
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
    ret = s_legacy_malloc(size_bytes, flags, &ptr, actual_size);
    if (ret == ESP_OK) {
        memset(ptr, 0, size_bytes);
        *out_ptr = ptr;
    }

    return ret;
}

static bool s_buf_in_region_legacy(const void *ptr, size_t size, esp_dma_buf_location_t location, int *heap_caps)
{
    bool found = false;
    if (location == ESP_DMA_BUF_LOCATION_INTERNAL) {
        if (esp_ptr_dma_capable(ptr) && esp_ptr_dma_capable(ptr + size - 1)) {
            *heap_caps = MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL;
            found = true;
        }
    } else if (location == ESP_DMA_BUF_LOCATION_PSRAM) {
#if SOC_PSRAM_DMA_CAPABLE
        if (esp_ptr_external_ram(ptr) && esp_ptr_external_ram(ptr + size - 1)) {
            *heap_caps = MALLOC_CAP_SPIRAM;
            found = true;
        }
#endif
    }
    return found;
}

bool esp_dma_is_buffer_aligned(const void *ptr, size_t size, esp_dma_buf_location_t location)
{
    assert(ptr);

    bool found = false;
    int heap_caps = 0;
    if (location == ESP_DMA_BUF_LOCATION_AUTO) {
        for (int i = ESP_DMA_BUF_LOCATION_INTERNAL; i < ESP_DMA_BUF_LOCATION_AUTO; i++) {
            if (s_buf_in_region_legacy(ptr, size, i, &heap_caps)) {
                found = true;
                break;
            }
        }
    } else if (location == ESP_DMA_BUF_LOCATION_INTERNAL) {
        found = s_buf_in_region_legacy(ptr, size, ESP_DMA_BUF_LOCATION_INTERNAL, &heap_caps);
    } else {
        found = s_buf_in_region_legacy(ptr, size, ESP_DMA_BUF_LOCATION_PSRAM, &heap_caps);
    }
    if (!found) {
        return false;
    }

    esp_dma_mem_info_t dma_mem_info = {
        .heap_caps = heap_caps,
        .dma_alignment = 4,  //legacy API behaviour is only check max dma buffer alignment
    };
    return esp_dma_is_buffer_alignment_satisfied(ptr, size, &dma_mem_info);
}

esp_err_t esp_dma_get_alignment(void *gdma_chan_handle, const dma_alignment_info_t *info, size_t *alignment)
{
    ESP_RETURN_ON_FALSE(info && alignment, ESP_ERR_INVALID_ARG, TAG, "null pointer");

#if SOC_GDMA_SUPPORTED
    if (gdma_chan_handle) {
        gdma_channel_handle_t dma_chan = (gdma_channel_handle_t)gdma_chan_handle;
        gdma_alignment_info_t gdma_info = {};
        memcpy(&gdma_info, info, sizeof(gdma_alignment_info_t));
        ESP_RETURN_ON_ERROR(gdma_get_alignment(dma_chan, &gdma_info, alignment), TAG, "failed to get gdma alignment");
    } else
#endif
    {
        //for esp32 and esp32s2
        *alignment = 4;
    }

    return ESP_OK;
}
