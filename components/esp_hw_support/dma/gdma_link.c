/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/cdefs.h>
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "esp_private/gdma_link.h"
#include "hal/cache_hal.h"
#include "hal/efuse_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"

ESP_LOG_ATTR_TAG(TAG, "gdma-link");

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

// GDMA link list item definition
// TODO: this type will eventually become target specific, we need to move it to the LL layer or soc layer
typedef struct gdma_link_list_item_t gdma_link_list_item_t;
struct gdma_link_list_item_t {
    struct {
        uint32_t size : 12;         /*!< Buffer size */
        uint32_t length : 12;       /*!< Number of valid bytes in the buffer */
        uint32_t reversed24 : 4;    /*!< Reserved */
        uint32_t err_eof : 1;       /*!< Whether the received buffer contains error, the error was reported by the peripheral */
        uint32_t reserved29 : 1;    /*!< Reserved */
        uint32_t suc_eof : 1;       /*!< Whether the list item should notify the peripheral an "EOF" event */
        uint32_t owner : 1;         /*!< Who is allowed to access the buffer */
    } dw0;                          /*!< list item Word 0 */
    void *buffer;                   /*!< Pointer to the buffer */
    gdma_link_list_item_t *next;    /*!< Pointer to the next list item (set to NULL if the list item is the last one of the link) */
};

///< Maximum size of the buffer that can be carried by a DMA link list item
#define GDMA_MAX_BUFFER_SIZE_PER_LINK_ITEM 4095

typedef struct gdma_link_list_t {
    uint32_t num_items; // number of items in the link list
    size_t item_size;   // size of each item
    uint8_t *items;     // pointer to the link list items
    uint8_t *items_nc;  // pointer to the link list items, non-cached
    struct {
        uint32_t check_owner: 1; // Whether the link list is responsible for checking the ownership when mount data buffers
    } flags;
} gdma_link_list_t;

esp_err_t gdma_new_link_list(const gdma_link_list_config_t *config, gdma_link_list_handle_t *ret_list)
{
    esp_err_t ret = ESP_OK;
    uint8_t *items = NULL;
    gdma_link_list_t *list = NULL;
    ESP_RETURN_ON_FALSE(config && ret_list, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->num_items, ESP_ERR_INVALID_ARG, TAG, "invalid number of items");

    // the link list container is allocated from internal memory
    list = heap_caps_calloc(1, sizeof(gdma_link_list_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(list, ESP_ERR_NO_MEM, err, TAG, "no mem for link list");

    uint32_t num_items = config->num_items;
    size_t item_alignment = config->item_alignment ? config->item_alignment : 4;
    // each list item should align to the specified alignment
    size_t item_size = ALIGN_UP(sizeof(gdma_link_list_item_t), item_alignment);
    // guard against overflow when calculating total bytes for descriptors
    ESP_GOTO_ON_FALSE(num_items <= SIZE_MAX / item_size, ESP_ERR_INVALID_SIZE, err, TAG, "list too big");

    bool items_in_ext_mem = config->flags.items_in_ext_mem;
    uint32_t list_items_mem_caps = MALLOC_CAP_8BIT | MALLOC_CAP_DMA;
    if (items_in_ext_mem) {
        if (efuse_hal_flash_encryption_enabled()) {
            items_in_ext_mem = false;
            list_items_mem_caps |= MALLOC_CAP_INTERNAL;
            ESP_LOGW(TAG, "DMA linked list items cannot be placed in PSRAM when external memory encryption is enabled, using internal memory instead");
        } else {
            list_items_mem_caps |= MALLOC_CAP_SPIRAM;
        }
    } else {
        list_items_mem_caps |= MALLOC_CAP_INTERNAL;
    }
    items = heap_caps_aligned_calloc(item_alignment, num_items, item_size, list_items_mem_caps);
    ESP_GOTO_ON_FALSE(items, ESP_ERR_NO_MEM, err, TAG, "no mem for link list items");

    // do memory sync if the list items are in the cache
    uint32_t data_cache_line_size = 0;
    if (items_in_ext_mem) {
        data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    } else {
        data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    }
    if (data_cache_line_size) {
        // write back and then invalidate the cache, because later we will read/write the link list items by non-cached address
        ESP_GOTO_ON_ERROR(esp_cache_msync(items, ALIGN_UP(num_items * item_size, data_cache_line_size),
                                          ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE),
                          err, TAG, "cache sync failed");
    }

    list->num_items = num_items;
    list->item_size = item_size;
    list->items = items;

    // calculate the non-cached address
    if (items_in_ext_mem) {
#if SOC_NON_CACHEABLE_OFFSET_PSRAM
        list->items_nc = items + SOC_NON_CACHEABLE_OFFSET_PSRAM;
#else
        ESP_GOTO_ON_ERROR(ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported to put link list items in external memory");
#endif
    } else {
#if SOC_NON_CACHEABLE_OFFSET_SRAM
        list->items_nc = items + SOC_NON_CACHEABLE_OFFSET_SRAM;
#else
        list->items_nc = items;
#endif
    }

    list->flags.check_owner = config->flags.check_owner;

    ESP_LOGD(TAG, "new link list @%p, items @%p", list, items);
    *ret_list = list;
    return ESP_OK;

err:
    if (list) {
        free(list);
    }
    if (items) {
        free(items);
    }
    return ret;
}

esp_err_t gdma_del_link_list(gdma_link_list_handle_t list)
{
    ESP_RETURN_ON_FALSE(list, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_LOGD(TAG, "del link list at %p", list);
    free(list->items);
    free(list);
    return ESP_OK;
}

esp_err_t gdma_link_mount_buffers(gdma_link_list_handle_t list, int start_item_index, const gdma_buffer_mount_config_t *buf_config_array, size_t num_buf, int *end_item_index)
{
    if (!list || !buf_config_array || !num_buf) {
        return ESP_ERR_INVALID_ARG;
    }
    size_t item_size = list->item_size;
    uint32_t list_item_capacity = list->num_items;
    // ensure the start_item_index is between 0 and `list_item_capacity - 1`
    start_item_index = (start_item_index % list_item_capacity + list_item_capacity) % list_item_capacity;
    gdma_link_list_item_t *lli_nc = NULL;

    uint32_t num_items_avail = 0;
    // if the link list is responsible for checking the ownership, we need to skip the items that are owned by the DMA
    if (list->flags.check_owner) {
        for (int i = 0; i < list_item_capacity; i++) {
            lli_nc = (gdma_link_list_item_t *)(list->items_nc + (i + start_item_index) % list_item_capacity * item_size);
            if (lli_nc->dw0.owner == GDMA_LLI_OWNER_CPU) {
                num_items_avail++;
            } else {
                // if the DMA descriptor "write back" feature is not enabled, descriptor is always owned by DMA after being used
                break;
            }
        }
    } else {
        num_items_avail = list_item_capacity;
    }

    // check alignment and length for each buffer
    uint32_t remaining = num_items_avail;
    for (size_t bi = 0; bi < num_buf; bi++) {
        const gdma_buffer_mount_config_t *config = &buf_config_array[bi];
        uint8_t *buf = (uint8_t *)config->buffer;
        size_t len = config->length;
        // zero-length/NULL buffers don't consume a slot in pre-check
        if (len == 0 || buf == NULL) {
            continue;
        }
        size_t buffer_alignment = config->buffer_alignment;
        if (buffer_alignment == 0) {
            buffer_alignment = 1;
        }
        // alignment must be a power of 2
        ESP_RETURN_ON_FALSE_ISR((buffer_alignment & (buffer_alignment - 1)) == 0, ESP_ERR_INVALID_ARG, TAG, "align err idx=%"PRIu32" align=%"PRIu32, bi, buffer_alignment);
        size_t max_buffer_mount_length = ALIGN_DOWN(GDMA_MAX_BUFFER_SIZE_PER_LINK_ITEM, buffer_alignment);
        if (!config->flags.bypass_buffer_align_check) {
            ESP_RETURN_ON_FALSE_ISR(((uintptr_t)buf & (buffer_alignment - 1)) == 0, ESP_ERR_INVALID_ARG, TAG, "buf misalign idx=%"PRIu32" align=%"PRIu32, bi, buffer_alignment);
            if (efuse_hal_flash_encryption_enabled()) {
                // buffer size must be aligned to the encryption alignment which should be provided by the upper buffer_alignment
                ESP_RETURN_ON_FALSE_ISR((len & (buffer_alignment - 1)) == 0, ESP_ERR_INVALID_ARG, TAG, "buf len misalign idx=%"PRIu32" len=%"PRIu32" align=%"PRIu32"", bi, len, buffer_alignment);
            }
        }
        size_t num_items_need = (len + max_buffer_mount_length - 1) / max_buffer_mount_length;
        ESP_RETURN_ON_FALSE_ISR(num_items_need <= remaining, ESP_ERR_INVALID_ARG, TAG,
                                "lli full start=%d need=%"PRIu32" avail=%"PRIu32, start_item_index, num_items_need, remaining);
        remaining -= num_items_need;
    }

    // link_nodes[start_item_index-1] --> link_nodes[start_item_index]
    lli_nc = (gdma_link_list_item_t *)(list->items_nc + (start_item_index + list_item_capacity - 1) % list_item_capacity * item_size);
    lli_nc->next = (gdma_link_list_item_t *)(list->items + start_item_index * item_size);

    int begin_item_idx = start_item_index;
    for (size_t bi = 0; bi < num_buf; bi++) {
        const gdma_buffer_mount_config_t *config = &buf_config_array[bi];
        uint8_t *buf = (uint8_t *)config->buffer;
        size_t len = config->length;
        size_t buffer_alignment = config->buffer_alignment;
        if (buffer_alignment == 0) {
            buffer_alignment = 1;
        }
        size_t max_buffer_mount_length = ALIGN_DOWN(GDMA_MAX_BUFFER_SIZE_PER_LINK_ITEM, buffer_alignment);
        // skip zero-length buffer but scrub any stale descriptor to keep ring clean; no slot consumption
        if (len == 0 || buf == NULL) {
            lli_nc = (gdma_link_list_item_t *)(list->items_nc + begin_item_idx % list_item_capacity * item_size);
            // reset the descriptor, especially the owner and next fields
            memset(lli_nc, 0, item_size);
            continue;
        }
        size_t num_items_need = (len + max_buffer_mount_length - 1) / max_buffer_mount_length;
        // mount the buffer to the link list
        for (size_t i = 0; i < num_items_need; i++) {
            lli_nc = (gdma_link_list_item_t *)(list->items_nc + (i + begin_item_idx) % list_item_capacity * item_size);
            lli_nc->buffer = buf;
            lli_nc->dw0.length = len > max_buffer_mount_length ? max_buffer_mount_length : len;
            // in fact the DMA doesn't check the "size" field, but we still set it to "length" for consistency
            // it's the user's responsibility to make sure the buffer size is sufficient
            lli_nc->dw0.size = lli_nc->dw0.length;
            // mark the EOF node
            lli_nc->dw0.suc_eof = (config->flags.mark_eof == 1) && (i == num_items_need - 1);
            if (i == num_items_need - 1) {
                // mark the final node
                switch (config->flags.mark_final) {
                case GDMA_FINAL_LINK_TO_NULL:
                    lli_nc->next = NULL;
                    break;
                case GDMA_FINAL_LINK_TO_HEAD:
                    lli_nc->next = (gdma_link_list_item_t *)(list->items);
                    break;
                case GDMA_FINAL_LINK_TO_START:
                    lli_nc->next = (gdma_link_list_item_t *)(list->items + start_item_index * item_size);
                    break;
                default:
                    // DMA expects cached addresses in `next`
                    lli_nc->next = (gdma_link_list_item_t *)(list->items + (i + begin_item_idx + 1) % list_item_capacity * item_size);
                    break;
                }
            } else {
                // DMA expects cached addresses in `next`
                lli_nc->next = (gdma_link_list_item_t *)(list->items + (i + begin_item_idx + 1) % list_item_capacity * item_size);
            }
            lli_nc->dw0.owner = GDMA_LLI_OWNER_DMA;
            buf += max_buffer_mount_length;
            len -= max_buffer_mount_length;
        }
        begin_item_idx += num_items_need;
    }

    // return the index of the last modified list item
    if (end_item_index) {
        *end_item_index = (begin_item_idx - 1 + list_item_capacity) % list_item_capacity;
    }
    return ESP_OK;
}

uintptr_t gdma_link_get_head_addr(gdma_link_list_handle_t list)
{
    if (!list) {
        return 0;
    }
    return (uintptr_t)(list->items);
}

esp_err_t gdma_link_concat(gdma_link_list_handle_t first_link, int first_link_item_index, gdma_link_list_handle_t second_link, int second_link_item_index)
{
    if (!first_link) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_link_list_item_t *lli_nc = NULL;
    // ensure the first_link_item_index is between 0 and `num_items - 1`
    int num_items = first_link->num_items;
    first_link_item_index = (first_link_item_index % num_items + num_items) % num_items;
    lli_nc = (gdma_link_list_item_t *)(first_link->items_nc + first_link_item_index * first_link->item_size);
    if (second_link == NULL) {
        lli_nc->next = NULL;
    } else {
        // ensure the second_link_item_index is between 0 and `num_items - 1`
        num_items = second_link->num_items;
        second_link_item_index = (second_link_item_index % num_items + num_items) % num_items;
        // concatenate the two link lists
        lli_nc->next = (gdma_link_list_item_t *)(second_link->items + second_link_item_index * second_link->item_size);
    }
    return ESP_OK;
}

esp_err_t gdma_link_set_owner(gdma_link_list_handle_t list, int item_index, gdma_lli_owner_t owner)
{
    if (!list) {
        return ESP_ERR_INVALID_ARG;
    }
    int num_items = list->num_items;
    // ensure the item_index is between 0 and `num_items - 1`
    item_index = (item_index % num_items + num_items) % num_items;
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + item_index * list->item_size);
    lli->dw0.owner = owner;
    return ESP_OK;
}

esp_err_t gdma_link_get_owner(gdma_link_list_handle_t list, int item_index, gdma_lli_owner_t *owner)
{
    if (!list || !owner) {
        return ESP_ERR_INVALID_ARG;
    }
    int num_items = list->num_items;
    // ensure the item_index is between 0 and `num_items - 1`
    item_index = (item_index % num_items + num_items) % num_items;
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + item_index * list->item_size);
    *owner = lli->dw0.owner;
    return ESP_OK;
}

size_t gdma_link_count_buffer_size_till_eof(gdma_link_list_handle_t list, int start_item_index)
{
    if (!list) {
        return 0;
    }
    int num_items = list->num_items;
    // ensure the start_item_index is between 0 and `num_items - 1`
    start_item_index = (start_item_index % num_items + num_items) % num_items;
    size_t buf_size = 0;
    gdma_link_list_item_t *lli_nc = NULL;
    for (int i = 0; i < num_items; i++) {
        lli_nc = (gdma_link_list_item_t *)(list->items_nc + (start_item_index + i) % num_items * list->item_size);
        buf_size += lli_nc->dw0.length;
        // break if the current item is the last one or the EOF item
        if (lli_nc->dw0.suc_eof || lli_nc->next == NULL) {
            break;
        }
    }
    return buf_size;
}

void* gdma_link_get_buffer(gdma_link_list_handle_t list, int item_index)
{
    if (!list) {
        return NULL;
    }
    int num_items = list->num_items;
    // ensure the item_index is between 0 and `num_items - 1`
    item_index = (item_index % num_items + num_items) % num_items;
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + item_index * list->item_size);
    return lli->buffer;
}

size_t gdma_link_get_length(gdma_link_list_handle_t list, int item_index)
{
    if (!list) {
        return 0;
    }
    int num_items = list->num_items;
    // ensure the item_index is between 0 and `num_items - 1`
    item_index = (item_index % num_items + num_items) % num_items;
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + item_index * list->item_size);
    return lli->dw0.length;
}

esp_err_t gdma_link_set_length(gdma_link_list_handle_t list, int item_index, size_t length)
{
    if (!list) {
        return ESP_ERR_INVALID_ARG;
    }
    int num_items = list->num_items;
    // ensure the item_index is between 0 and `num_items - 1`
    item_index = (item_index % num_items + num_items) % num_items;
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + item_index * list->item_size);
    lli->dw0.length = length;
    return ESP_OK;
}

bool gdma_link_check_end(gdma_link_list_handle_t list, int item_index)
{
    if (!list) {
        return false;
    }
    int num_items = list->num_items;
    // ensure the item_index is between 0 and `num_items - 1`
    item_index = (item_index % num_items + num_items) % num_items;
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + item_index * list->item_size);
    return lli->next == NULL;
}
