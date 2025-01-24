/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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

esp_err_t esp_dma_split_rx_buffer_to_cache_aligned(void *rx_buffer, size_t buffer_len, dma_buffer_split_array_t *align_buf_array, uint8_t** ret_stash_buffer)
{
    esp_err_t ret = ESP_OK;
    uint8_t* stash_buffer = NULL;
    ESP_RETURN_ON_FALSE(rx_buffer && buffer_len && align_buf_array, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // read the cache line size of internal and external memory, we also use this information to check if a given memory is behind the cache
    size_t int_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t ext_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);

    size_t split_line_size = 0;
    if (esp_ptr_external_ram(rx_buffer)) {
        split_line_size = ext_mem_cache_line_size;
    } else if (esp_ptr_internal(rx_buffer)) {
        split_line_size = int_mem_cache_line_size;
    }
    ESP_LOGV(TAG, "split_line_size:%zu", split_line_size);

    // allocate the stash buffer from internal RAM
    // Note, the split_line_size can be 0, in this case, the stash_buffer is also NULL, which is fine
    stash_buffer = heap_caps_calloc(2, split_line_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(!(split_line_size && !stash_buffer), ESP_ERR_NO_MEM, TAG, "no mem for stash buffer");

    // clear align_array to avoid garbage data
    memset(align_buf_array, 0, sizeof(dma_buffer_split_array_t));
    bool need_cache_sync[3] = {false};

    // if split_line_size is non-zero, split the buffer into head, body and tail
    if (split_line_size > 0) {
        // calculate head_overflow_len
        size_t head_overflow_len = (uintptr_t)rx_buffer % split_line_size;
        head_overflow_len = head_overflow_len ? split_line_size - head_overflow_len : 0;
        ESP_LOGV(TAG, "head_addr:%p head_overflow_len:%zu", rx_buffer, head_overflow_len);
        // calculate tail_overflow_len
        size_t tail_overflow_len = ((uintptr_t)rx_buffer + buffer_len) % split_line_size;
        ESP_LOGV(TAG, "tail_addr:%p tail_overflow_len:%zu", rx_buffer + buffer_len - tail_overflow_len, tail_overflow_len);

        uint8_t extra_buf_count = 0;
        uint8_t* input_buffer = (uint8_t*)rx_buffer;
        align_buf_array->buf.head.recovery_address = input_buffer;
        align_buf_array->buf.head.aligned_buffer = stash_buffer + split_line_size * extra_buf_count++;
        align_buf_array->buf.head.length = head_overflow_len;
        need_cache_sync[0] = int_mem_cache_line_size > 0;
        align_buf_array->buf.body.recovery_address = input_buffer + head_overflow_len;
        align_buf_array->buf.body.aligned_buffer = input_buffer + head_overflow_len;
        align_buf_array->buf.body.length = buffer_len - head_overflow_len - tail_overflow_len;
        need_cache_sync[1] = true;
        align_buf_array->buf.tail.recovery_address = input_buffer + buffer_len - tail_overflow_len;
        align_buf_array->buf.tail.aligned_buffer = stash_buffer + split_line_size * extra_buf_count++;
        align_buf_array->buf.tail.length = tail_overflow_len;
        need_cache_sync[2] = int_mem_cache_line_size > 0;

        // special handling when input_buffer length is no more than buffer alignment
        if (head_overflow_len >= buffer_len || tail_overflow_len >= buffer_len) {
            align_buf_array->buf.head.length  = buffer_len ;
            align_buf_array->buf.body.length  = 0 ;
            align_buf_array->buf.tail.length  = 0 ;
        }
    } else {
        align_buf_array->buf.body.aligned_buffer = rx_buffer;
        align_buf_array->buf.body.recovery_address = rx_buffer;
        align_buf_array->buf.body.length = buffer_len;
        need_cache_sync[1] = false;
    }

    for (int i = 0; i < 3; i++) {
        if (align_buf_array->aligned_buffer[i].length == 0) {
            align_buf_array->aligned_buffer[i].aligned_buffer = NULL;
            align_buf_array->aligned_buffer[i].recovery_address = NULL;
            need_cache_sync[i] = false;
        }
    }

    // invalidate the aligned buffer if necessary
    for (int i = 0; i < 3; i++) {
        if (need_cache_sync[i]) {
            size_t sync_size = align_buf_array->aligned_buffer[i].length;
            if (sync_size < split_line_size) {
                // If the buffer is smaller than the cache line size, we need to sync the whole buffer
                sync_size = split_line_size;
            }
            esp_err_t res = esp_cache_msync(align_buf_array->aligned_buffer[i].aligned_buffer, sync_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
            ESP_GOTO_ON_ERROR(res, err, TAG, "failed to do cache sync");
        }
    }

    *ret_stash_buffer = stash_buffer;
    return ESP_OK;
err:
    if (stash_buffer) {
        free(stash_buffer);
    }
    return ret;
}

esp_err_t esp_dma_merge_aligned_rx_buffers(dma_buffer_split_array_t *align_array)
{
    ESP_RETURN_ON_FALSE_ISR(align_array, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // only need to copy the head and tail buffer
    if (align_array->buf.head.length) {
        memcpy(align_array->buf.head.recovery_address, align_array->buf.head.aligned_buffer, align_array->buf.head.length);
    }
    if (align_array->buf.tail.length) {
        memcpy(align_array->buf.tail.recovery_address, align_array->buf.tail.aligned_buffer, align_array->buf.tail.length);
    }
    return ESP_OK;
}

size_t esp_dma_calculate_node_count(size_t buffer_size, size_t buffer_alignment, size_t max_buffer_size_per_node)
{
    // buffer_alignment should be power of 2
    ESP_RETURN_ON_FALSE(buffer_alignment && ((buffer_alignment & (buffer_alignment - 1)) == 0), 0, TAG, "invalid buffer alignment");
    // align down the max_buffer_size_per_node
    max_buffer_size_per_node = max_buffer_size_per_node & ~(buffer_alignment - 1);
    // calculate the number of nodes
    return (buffer_size + max_buffer_size_per_node - 1) / max_buffer_size_per_node;
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
