/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/cdefs.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "soc/ext_mem_defs.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "esp_private/gdma_link.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"

static const char *TAG = "gdma";

#if SOC_NON_CACHEABLE_OFFSET
#define GDMA_CACHE_ADDR_TO_NON_CACHE_ADDR(addr) ((addr) + SOC_NON_CACHEABLE_OFFSET)
#else
#define GDMA_CACHE_ADDR_TO_NON_CACHE_ADDR(addr) (addr)
#endif

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
    size_t buffer_alignment; // Alignment of each buffer
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
    size_t buffer_alignment = config->buffer_alignment;
    if (buffer_alignment == 0) {
        buffer_alignment = 1;
    }
    ESP_RETURN_ON_FALSE((buffer_alignment & (buffer_alignment - 1)) == 0, ESP_ERR_INVALID_ARG, TAG, "invalid buffer alignment: %zu", buffer_alignment);

    // the link list container is allocated from internal memory
    list = heap_caps_calloc(1, sizeof(gdma_link_list_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(list, ESP_ERR_NO_MEM, err, TAG, "no mem for link list");

    uint32_t num_items = config->num_items;
    size_t item_alignment = config->item_alignment ? config->item_alignment : 4;
    // each list item should align to the specified alignment
    size_t item_size = ALIGN_UP(sizeof(gdma_link_list_item_t), item_alignment);

    uint32_t list_items_mem_caps = MALLOC_CAP_8BIT | MALLOC_CAP_DMA;
    if (config->flags.items_in_ext_mem) {
        list_items_mem_caps |= MALLOC_CAP_SPIRAM;
    } else {
        list_items_mem_caps |= MALLOC_CAP_INTERNAL;
    }
    items = heap_caps_aligned_calloc(item_alignment, num_items, item_size, list_items_mem_caps);
    ESP_GOTO_ON_FALSE(items, ESP_ERR_NO_MEM, err, TAG, "no mem for link list items");

    // do memory sync if the list items are in the cache
    uint32_t data_cache_line_size = 0;
    if (config->flags.items_in_ext_mem) {
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
    list->items_nc = GDMA_CACHE_ADDR_TO_NON_CACHE_ADDR(items);
    list->buffer_alignment = buffer_alignment;
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

esp_err_t gdma_link_mount_buffers(gdma_link_list_handle_t list, uint32_t start_item_index, const gdma_buffer_mount_config_t *buf_config_array, size_t num_buf, uint32_t *end_item_index)
{
    ESP_RETURN_ON_FALSE(list && buf_config_array && num_buf, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(start_item_index < list->num_items, ESP_ERR_INVALID_ARG, TAG, "invalid start item index");
    size_t buffer_alignment = list->buffer_alignment;
    size_t item_size = list->item_size;
    uint32_t list_item_capacity = list->num_items;
    size_t max_buffer_mount_length = ALIGN_DOWN(GDMA_MAX_BUFFER_SIZE_PER_LINK_ITEM, buffer_alignment);
    uint32_t begin_item_idx = start_item_index;
    gdma_link_list_item_t *lli_nc = NULL;

    uint32_t num_items_avail = 0;
    // if the link list is responsible for checking the ownership, we need to skip the items that are owned by the DMA
    if (list->flags.check_owner) {
        for (uint32_t i = 0; i < list_item_capacity; i++) {
            lli_nc = (gdma_link_list_item_t *)(list->items_nc + (i + start_item_index) % list_item_capacity * item_size);
            if (lli_nc->dw0.owner == GDMA_LLI_OWNER_CPU) {
                num_items_avail++;
            }
        }
    } else {
        num_items_avail = list_item_capacity;
    }

    // check alignment and length for each buffer
    for (size_t i = 0; i < num_buf; i++) {
        const gdma_buffer_mount_config_t *config = &buf_config_array[i];
        uint8_t *buf = (uint8_t *)config->buffer;
        size_t len = config->length;
        // check the buffer alignment
        ESP_RETURN_ON_FALSE(((uintptr_t)buf & (buffer_alignment - 1)) == 0, ESP_ERR_INVALID_ARG, TAG, "buffer not aligned to %zu", buffer_alignment);
        uint32_t num_items_need = (len + max_buffer_mount_length - 1) / max_buffer_mount_length;
        // check if there are enough link list items
        ESP_RETURN_ON_FALSE((begin_item_idx + num_items_need) <= (start_item_index + num_items_avail), ESP_ERR_INVALID_ARG, TAG, "no more space for buffer mounting");
        begin_item_idx += num_items_need;
    }

    // link_nodes[start_item_index-1] --> link_nodes[start_item_index]
    lli_nc = (gdma_link_list_item_t *)(list->items_nc + (start_item_index + list_item_capacity - 1) % list_item_capacity * item_size);
    lli_nc->next = (gdma_link_list_item_t *)(list->items + start_item_index * item_size);

    begin_item_idx = start_item_index;
    for (size_t i = 0; i < num_buf; i++) {
        const gdma_buffer_mount_config_t *config = &buf_config_array[i];
        uint8_t *buf = (uint8_t *)config->buffer;
        size_t len = config->length;
        // skip zero-length buffer
        if (len == 0 || buf == NULL) {
            continue;
        }
        uint32_t num_items_need = (len + max_buffer_mount_length - 1) / max_buffer_mount_length;
        // mount the buffer to the link list
        for (uint32_t i = 0; i < num_items_need; i++) {
            lli_nc = (gdma_link_list_item_t *)(list->items_nc + (i + begin_item_idx) % list_item_capacity * item_size);
            lli_nc->buffer = buf;
            lli_nc->dw0.length = len > max_buffer_mount_length ? max_buffer_mount_length : len;
            // in fact the DMA doesn't check the "size" field, but we still set it to "length" for consistency
            // it's the user's responsibility to make sure the buffer size is sufficient
            lli_nc->dw0.size = lli_nc->dw0.length;
            // mark the EOF node
            lli_nc->dw0.suc_eof = (config->flags.mark_eof == 1) && (i == num_items_need - 1);
            // mark the final node
            if ((config->flags.mark_final == 1) && (i == num_items_need - 1)) {
                lli_nc->next = NULL;
            } else {
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
    ESP_RETURN_ON_FALSE(list, 0, TAG, "invalid argument");
    return (uintptr_t)(list->items);
}

esp_err_t gdma_link_concat(gdma_link_list_handle_t first_link, int first_link_item_index, gdma_link_list_handle_t second_link, int second_link_item_index)
{
    ESP_RETURN_ON_FALSE(first_link && second_link, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_link_list_item_t *lli_nc = NULL;
    lli_nc = (gdma_link_list_item_t *)(first_link->items_nc + (first_link->num_items + first_link_item_index) % first_link->num_items * first_link->item_size);
    lli_nc->next = (gdma_link_list_item_t *)(second_link->items + (second_link->num_items + second_link_item_index) % second_link->num_items * second_link->item_size);
    return ESP_OK;
}

esp_err_t gdma_link_set_owner(gdma_link_list_handle_t list, int item_index, gdma_lli_owner_t owner)
{
    ESP_RETURN_ON_FALSE_ISR(list, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(item_index < list->num_items, ESP_ERR_INVALID_ARG, TAG, "invalid item index");
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + (list->num_items + item_index) % list->num_items * list->item_size);
    lli->dw0.owner = owner;
    return ESP_OK;
}

esp_err_t gdma_link_get_owner(gdma_link_list_handle_t list, int item_index, gdma_lli_owner_t *owner)
{
    ESP_RETURN_ON_FALSE_ISR(list && owner, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(item_index < list->num_items, ESP_ERR_INVALID_ARG, TAG, "invalid item index");
    gdma_link_list_item_t *lli = (gdma_link_list_item_t *)(list->items_nc + (list->num_items + item_index) % list->num_items * list->item_size);
    *owner = lli->dw0.owner;
    return ESP_OK;
}
