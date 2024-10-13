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
#include "esp_private/esp_dma_utils.h"
#include "esp_private/esp_cache_private.h"
#include "soc/soc_caps.h"
#include "hal/hal_utils.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"

static const char *TAG = "dma_utils";

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN_BY(num, align) ((num) & (~((align) - 1)))

esp_err_t esp_dma_split_buffer_to_aligned(void *input_buffer, size_t input_buffer_len, void *stash_buffer, size_t stash_buffer_len, size_t split_alignment, dma_buffer_split_array_t *align_array)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(align_array && input_buffer && input_buffer_len && stash_buffer && split_alignment && !(split_alignment & (split_alignment - 1)
                        && (stash_buffer_len >= 2 * split_alignment)), ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(!((uintptr_t)stash_buffer % split_alignment), ESP_ERR_INVALID_ARG, TAG, "extra buffer is not aligned");

    // calculate head_overflow_len
    size_t head_overflow_len = (uintptr_t)input_buffer % split_alignment;
    head_overflow_len = head_overflow_len ? split_alignment - head_overflow_len : 0;
    ESP_LOGD(TAG, "head_addr:%p split_alignment:%zu head_overflow_len:%zu", input_buffer, split_alignment, head_overflow_len);
    // calculate tail_overflow_len
    size_t tail_overflow_len = ((uintptr_t)input_buffer + input_buffer_len) % split_alignment;
    ESP_LOGD(TAG, "tail_addr:%p split_alignment:%zu tail_overflow_len:%zu", input_buffer + input_buffer_len - tail_overflow_len, split_alignment, tail_overflow_len);

    uint32_t extra_buf_count = 0;
    input_buffer = (uint8_t*)input_buffer;
    stash_buffer = (uint8_t*)stash_buffer;
    align_array->buf.head.recovery_address = input_buffer;
    align_array->buf.head.aligned_buffer = stash_buffer + split_alignment * extra_buf_count++;
    align_array->buf.head.length = head_overflow_len;
    align_array->buf.body.recovery_address = input_buffer + head_overflow_len;
    align_array->buf.body.aligned_buffer = input_buffer + head_overflow_len;
    align_array->buf.body.length = input_buffer_len - head_overflow_len - tail_overflow_len;
    align_array->buf.tail.recovery_address = input_buffer + input_buffer_len - tail_overflow_len;
    align_array->buf.tail.aligned_buffer = stash_buffer + split_alignment * extra_buf_count++;
    align_array->buf.tail.length = tail_overflow_len;

    // special handling when input_buffer length is no more than buffer alignment
    if(head_overflow_len >= input_buffer_len || tail_overflow_len >= input_buffer_len)
    {
        align_array->buf.head.length  = input_buffer_len ;
        align_array->buf.body.length  = 0 ;
        align_array->buf.tail.length  = 0 ;
    }

    for(int i = 0; i < 3; i++) {
        if(!align_array->aligned_buffer[i].length) {
            align_array->aligned_buffer[i].aligned_buffer = NULL;
            align_array->aligned_buffer[i].recovery_address = NULL;
        }
    }

    return ret;
}

esp_err_t esp_dma_merge_aligned_buffers(dma_buffer_split_array_t *align_array)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(align_array, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // only need to copy the head and tail buffer
    if(align_array->buf.head.length) {
        memcpy(align_array->buf.head.recovery_address, align_array->buf.head.aligned_buffer, align_array->buf.head.length);
    }
    if(align_array->buf.tail.length) {
        memcpy(align_array->buf.tail.recovery_address, align_array->buf.tail.aligned_buffer, align_array->buf.tail.length);
    }

    return ret;
}

esp_err_t esp_dma_capable_malloc(size_t size, const esp_dma_mem_info_t *dma_mem_info, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(dma_mem_info && out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    size_t alignment_bytes = 0;

    //dma align
    size_t dma_alignment_bytes = dma_mem_info->dma_alignment_bytes;

    //cache align
    int cache_flags = 0;
    size_t cache_alignment_bytes = 0;

    int heap_caps = dma_mem_info->extra_heap_caps | MALLOC_CAP_DMA;
    if (dma_mem_info->extra_heap_caps & MALLOC_CAP_SPIRAM) {
        cache_flags |= MALLOC_CAP_SPIRAM;
        heap_caps = dma_mem_info->extra_heap_caps | MALLOC_CAP_SPIRAM;
        /**
         * This is a workaround because we don't have `MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM`
         * match when using heap_cap related allocations.
         */
        heap_caps &= ~MALLOC_CAP_DMA;
    }

    // Return value unused if asserts are disabled
    esp_err_t __attribute((unused)) ret = esp_cache_get_alignment(cache_flags, &cache_alignment_bytes);
    assert(ret == ESP_OK);

    //Get the least common multiple of two alignment
    alignment_bytes = hal_utils_calc_lcm(dma_alignment_bytes, cache_alignment_bytes);

    //malloc
    size = ALIGN_UP_BY(size, alignment_bytes);
    void *ptr = heap_caps_aligned_alloc(alignment_bytes, size, heap_caps);
    ESP_RETURN_ON_FALSE_ISR(ptr, ESP_ERR_NO_MEM, TAG, "Not enough heap memory");

    *out_ptr = ptr;
    if (actual_size) {
        *actual_size = size;
    }

    return ESP_OK;
}

esp_err_t esp_dma_capable_calloc(size_t calloc_num, size_t size, const esp_dma_mem_info_t *dma_mem_info, void **out_ptr, size_t *actual_size)
{
    esp_err_t ret = ESP_FAIL;
    size_t size_bytes = 0;
    bool ovf = false;

    ovf = __builtin_mul_overflow(calloc_num, size, &size_bytes);
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

bool esp_dma_is_buffer_alignment_satisfied(const void *ptr, size_t size, esp_dma_mem_info_t dma_mem_info)
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

    size_t alignment_bytes = 0;

    //dma align
    size_t dma_alignment_bytes = dma_mem_info.dma_alignment_bytes;

    //cache align
    int cache_flags = 0;
    size_t cache_alignment_bytes = 0;
    if (esp_ptr_external_ram(ptr)) {
        cache_flags |= MALLOC_CAP_SPIRAM;
    }
    // Return value unused if asserts are disabled
    esp_err_t __attribute__((unused)) ret = esp_cache_get_alignment(cache_flags, &cache_alignment_bytes);
    assert(ret == ESP_OK);

    //Get the least common multiple of two alignment
    alignment_bytes = hal_utils_calc_lcm(dma_alignment_bytes, cache_alignment_bytes);

    bool is_aligned = s_is_buf_aligned((intptr_t)ptr, alignment_bytes) && s_is_buf_aligned((intptr_t)size, alignment_bytes);
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
        .extra_heap_caps = heap_caps,
        .dma_alignment_bytes = 4, //legacy API behaviour is only check max dma buffer alignment
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
        .extra_heap_caps = heap_caps,
        .dma_alignment_bytes = 4,  //legacy API behaviour is only check max dma buffer alignment
    };
    return esp_dma_is_buffer_alignment_satisfied(ptr, size, dma_mem_info);
}
