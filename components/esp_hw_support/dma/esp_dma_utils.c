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

ESP_LOG_ATTR_TAG(TAG, "dma_utils");

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
