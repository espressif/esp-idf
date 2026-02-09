/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/cdefs.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_DW_GDMA_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "esp_memory_utils.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/dw_gdma.h"
#include "esp_private/critical_section.h"
#include "hal/dw_gdma_hal.h"
#include "hal/dw_gdma_ll.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"

ESP_LOG_ATTR_TAG(TAG, "dw-gdma");

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#define DW_GDMA_GET_NON_CACHE_ADDR(addr) ((addr) ? CACHE_LL_L2MEM_NON_CACHE_ADDR(addr) : 0)
#define DW_GDMA_GET_CACHE_ADDRESS(nc_addr) ((nc_addr) ? CACHE_LL_L2MEM_CACHE_ADDR(nc_addr) : 0)
#else
#define DW_GDMA_GET_NON_CACHE_ADDR(addr) (addr)
#define DW_GDMA_GET_CACHE_ADDRESS(nc_addr) (nc_addr)
#endif

#if CONFIG_DW_GDMA_OBJ_DRAM_SAFE
#define DW_GDMA_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DW_GDMA_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif

#if CONFIG_DW_GDMA_ISR_IRAM_SAFE
#define DW_GDMA_INTR_ALLOC_FLAGS  (ESP_INTR_FLAG_IRAM)
#else
#define DW_GDMA_INTR_ALLOC_FLAGS  0
#endif

#define DW_GDMA_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

typedef struct dw_gdma_group_t dw_gdma_group_t;
typedef struct dw_gdma_channel_t dw_gdma_channel_t;

typedef struct dw_gdma_link_list_t {
    uint32_t num_items;                 // number of items in the link list
    dw_gdma_link_list_item_t *items;    // pointer to the link list items
    dw_gdma_link_list_item_t *items_nc; // pointer to the link list items, non-cached
} dw_gdma_link_list_t;

typedef struct {
    _lock_t mutex;                              // platform level mutex lock
    dw_gdma_group_t *groups[DW_GDMA_LL_GROUPS]; // array of DMA group instances
    int group_ref_counts[DW_GDMA_LL_GROUPS];    // reference count used to protect group install/uninstall
} dw_gdma_platform_t;

struct dw_gdma_group_t {
    int group_id;              // Group ID, index from 0
    dw_gdma_hal_context_t hal; // HAL instance is at group level
    int intr_priority;         // all channels in the same group should share the same interrupt priority
    portMUX_TYPE spinlock;     // group level spinlock, protect group level stuffs, e.g. hal object, pair handle slots and reference count of each pair
    dw_gdma_channel_t *channels[DW_GDMA_LL_CHANNELS_PER_GROUP]; // handles of DMA channels
};

struct dw_gdma_channel_t {
    int chan_id;            // channel ID, index from 0
    intr_handle_t intr;     // per-channel interrupt handle
    portMUX_TYPE spinlock;  // channel level spinlock
    dw_gdma_group_t *group; // pointer to the group which the channel belongs to
    void *user_data;        // user registered DMA event data
    dw_gdma_event_callbacks_t cbs; // Event callbacks
    dw_gdma_block_transfer_type_t src_transfer_type; // transfer type for source
    dw_gdma_block_transfer_type_t dst_transfer_type; // transfer type for destination
};

// dw_gdma driver platform
static dw_gdma_platform_t s_platform;

static dw_gdma_group_t *dw_gdma_acquire_group_handle(int group_id)
{
    bool new_group = false;
    dw_gdma_group_t *group = NULL;

    // prevent install dw_gdma group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        // The group is handle is not created yet
        group = heap_caps_calloc(1, sizeof(dw_gdma_group_t), DW_GDMA_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group;
            // enable APB to access DMA registers
            PERIPH_RCC_ATOMIC() {
                dw_gdma_ll_enable_bus_clock(group_id, true);
                dw_gdma_ll_reset_register(group_id);
            }
            // initialize the HAL context
            dw_gdma_hal_config_t hal_config = {};
            dw_gdma_hal_init(&group->hal, &hal_config);
        }
    } else {
        // the group is installed, we just retrieve it and increase the reference count
        group = s_platform.groups[group_id];
    }
    if (group) {
        // someone acquired the group handle means we have a new object that refer to this group
        s_platform.group_ref_counts[group_id]++;
    }
    _lock_release(&s_platform.mutex);

    if (new_group) {
        portMUX_INITIALIZE(&group->spinlock);
        group->group_id = group_id;
        group->intr_priority = -1; // interrupt priority not assigned yet
        ESP_LOGD(TAG, "new group (%d) at %p", group_id, group);
    }

    return group;
}

static void dw_gdma_release_group_handle(dw_gdma_group_t *group)
{
    int group_id = group->group_id;
    bool del_group = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        del_group = true;
        // the group now is not used by any channel, unregister it from the platform
        s_platform.groups[group_id] = NULL;
        // deinitialize the HAL context
        dw_gdma_hal_deinit(&group->hal);
        PERIPH_RCC_ATOMIC() {
            dw_gdma_ll_enable_bus_clock(group_id, false);
        }
    }
    _lock_release(&s_platform.mutex);

    if (del_group) {
        free(group);
        ESP_LOGD(TAG, "delete group (%d)", group_id);
    }
}

static esp_err_t channel_register_to_group(dw_gdma_channel_t *chan)
{
    dw_gdma_group_t *group = NULL;
    int chan_id = -1;
    for (int i = 0; i < DW_GDMA_LL_GROUPS; i++) {
        group = dw_gdma_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group(%d)", i);
        // loop to search free channel in the group
        esp_os_enter_critical(&group->spinlock);
        for (int j = 0; j < DW_GDMA_LL_CHANNELS_PER_GROUP; j++) {
            if (group->channels[j] == NULL) {
                group->channels[j] = chan;
                chan_id = j;
                break;
            }
        }
        esp_os_exit_critical(&group->spinlock);
        if (chan_id < 0) {
            dw_gdma_release_group_handle(group);
        } else {
            chan->group = group;
            chan->chan_id = chan_id;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(chan_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free channels");
    return ESP_OK;
}

static void channel_unregister_from_group(dw_gdma_channel_t *chan)
{
    dw_gdma_group_t *group = chan->group;
    int chan_id = chan->chan_id;
    esp_os_enter_critical(&group->spinlock);
    group->channels[chan_id] = NULL;
    esp_os_exit_critical(&group->spinlock);
    // channel has a reference on group, release it now
    dw_gdma_release_group_handle(group);
}

static esp_err_t channel_destroy(dw_gdma_channel_t *chan)
{
    if (chan->group) {
        channel_unregister_from_group(chan);
    }
    if (chan->intr) {
        esp_intr_free(chan->intr);
    }
    free(chan);
    return ESP_OK;
}

esp_err_t dw_gdma_new_channel(const dw_gdma_channel_alloc_config_t *config, dw_gdma_channel_handle_t *ret_chan)
{
#if CONFIG_DW_GDMA_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    dw_gdma_channel_t *chan = NULL;
    ESP_RETURN_ON_FALSE(config && ret_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->src.num_outstanding_requests >= 1 && config->src.num_outstanding_requests <= DW_GDMA_LL_MAX_OUTSTANDING_REQUESTS,
                        ESP_ERR_INVALID_ARG, TAG, "invalid num_outstanding_requests");
    ESP_RETURN_ON_FALSE(config->dst.num_outstanding_requests >= 1 && config->dst.num_outstanding_requests <= DW_GDMA_LL_MAX_OUTSTANDING_REQUESTS,
                        ESP_ERR_INVALID_ARG, TAG, "invalid num_outstanding_request");
    ESP_RETURN_ON_FALSE(config->chan_priority >= 0 && config->chan_priority < DW_GDMA_LL_CHANNELS_PER_GROUP,
                        ESP_ERR_INVALID_ARG, TAG, "invalid channel priority");
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & DW_GDMA_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG,
                            TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    chan = heap_caps_calloc(1, sizeof(dw_gdma_channel_t), DW_GDMA_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_NO_MEM, TAG, "no mem for channel");
    // register channel to the group
    ESP_GOTO_ON_ERROR(channel_register_to_group(chan), err, TAG, "register to group failed");
    dw_gdma_group_t *group = chan->group;
    dw_gdma_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;

    // all channels in the same group should use the same interrupt priority
    bool intr_priority_conflict = false;
    esp_os_enter_critical(&group->spinlock);
    if (group->intr_priority == -1) {
        group->intr_priority = config->intr_priority;
    } else if (config->intr_priority != 0) {
        intr_priority_conflict = (group->intr_priority != config->intr_priority);
    }
    esp_os_exit_critical(&group->spinlock);
    ESP_GOTO_ON_FALSE(!intr_priority_conflict, ESP_ERR_INVALID_STATE, err, TAG, "intr_priority conflict, already is %d but attempt to %d", group->intr_priority, config->intr_priority);

    // basic initialization
    portMUX_INITIALIZE(&chan->spinlock);
    chan->src_transfer_type = config->src.block_transfer_type;
    chan->dst_transfer_type = config->dst.block_transfer_type;
    // set transfer flow type
    dw_gdma_ll_channel_set_trans_flow(hal->dev, chan_id, config->src.role, config->dst.role, config->flow_controller);
    // set the transfer type for source and destination
    dw_gdma_ll_channel_set_src_multi_block_type(hal->dev, chan_id, config->src.block_transfer_type);
    dw_gdma_ll_channel_set_dst_multi_block_type(hal->dev, chan_id, config->dst.block_transfer_type);
    // set handshake interface
    dw_gdma_ll_channel_set_src_handshake_interface(hal->dev, chan_id, config->src.handshake_type);
    dw_gdma_ll_channel_set_dst_handshake_interface(hal->dev, chan_id, config->dst.handshake_type);
    // set handshake peripheral
    if (config->src.role != DW_GDMA_ROLE_MEM) {
        dw_gdma_ll_channel_set_src_handshake_periph(hal->dev, chan_id, config->src.role);
    }
    if (config->dst.role != DW_GDMA_ROLE_MEM) {
        dw_gdma_ll_channel_set_dst_handshake_periph(hal->dev, chan_id, config->dst.role);
    }
    // set channel priority
    dw_gdma_ll_channel_set_priority(hal->dev, chan_id, config->chan_priority);
    // set the outstanding request number
    dw_gdma_ll_channel_set_src_outstanding_limit(hal->dev, chan_id, config->src.num_outstanding_requests);
    dw_gdma_ll_channel_set_dst_outstanding_limit(hal->dev, chan_id, config->dst.num_outstanding_requests);
    // set the status fetch address
    dw_gdma_ll_channel_set_src_periph_status_addr(hal->dev, chan_id, config->src.status_fetch_addr);
    dw_gdma_ll_channel_set_dst_periph_status_addr(hal->dev, chan_id, config->dst.status_fetch_addr);

    // enable all channel events (notes, they can't trigger an interrupt until `dw_gdma_ll_channel_enable_intr_propagation` is called)
    dw_gdma_ll_channel_enable_intr_generation(hal->dev, chan_id, UINT32_MAX, true);

    ESP_LOGD(TAG, "new channel (%d,%d) at %p", group_id, chan_id, chan);
    *ret_chan = chan;
    return ESP_OK;
err:
    if (chan) {
        channel_destroy(chan);
    }
    return ret;
}

esp_err_t dw_gdma_del_channel(dw_gdma_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_group_t *group = chan->group;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;
    ESP_LOGD(TAG, "del channel (%d,%d)", group_id, chan_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(channel_destroy(chan), TAG, "destroy channel failed");
    return ESP_OK;
}

esp_err_t dw_gdma_channel_enable_ctrl(dw_gdma_channel_handle_t chan, bool en_or_dis)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;
    // the atomic is ensured by the hardware, so no lock is needed here
    dw_gdma_ll_channel_enable(hal->dev, chan_id, en_or_dis);
    return ESP_OK;
}

esp_err_t dw_gdma_channel_suspend_ctrl(dw_gdma_channel_handle_t chan, bool enter_or_exit)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;
    // the atomic is ensured by the hardware, so no lock is needed here
    dw_gdma_ll_channel_suspend(hal->dev, chan_id, enter_or_exit);
    return ESP_OK;
}

esp_err_t dw_gdma_channel_abort(dw_gdma_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;
    // the atomic is ensured by the hardware, so no lock is needed here
    dw_gdma_ll_channel_abort(hal->dev, chan_id);
    return ESP_OK;
}

esp_err_t dw_gdma_channel_lock(dw_gdma_channel_handle_t chan, dw_gdma_lock_level_t level)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;

    // the lock control bit is located in a cfg register, with other configuration bits
    esp_os_enter_critical(&chan->spinlock);
    dw_gdma_ll_channel_lock(hal->dev, chan_id, level);
    esp_os_exit_critical(&chan->spinlock);
    return ESP_OK;
}

esp_err_t dw_gdma_channel_unlock(dw_gdma_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;

    // the lock control bit is located in a cfg register, with other configuration bits
    esp_os_enter_critical(&chan->spinlock);
    dw_gdma_ll_channel_unlock(hal->dev, chan_id);
    esp_os_exit_critical(&chan->spinlock);
    return ESP_OK;
}

esp_err_t dw_gdma_channel_continue(dw_gdma_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;
    // the atomic is ensured by the hardware, so no lock is needed here
    dw_gdma_ll_channel_resume_multi_block_transfer(hal->dev, chan_id);
    return ESP_OK;
}

esp_err_t dw_gdma_new_link_list(const dw_gdma_link_list_config_t *config, dw_gdma_link_list_handle_t *ret_list)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_list, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_link_list_item_t *items = NULL;
    dw_gdma_link_list_t *list = NULL;
    uint32_t num_items = config->num_items;
    list = heap_caps_calloc(1, sizeof(dw_gdma_link_list_t), DW_GDMA_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(list, ESP_ERR_NO_MEM, err, TAG, "no mem for link list");
    // allocate memory for link list items, from internal memory
    // the link list items has its own alignment requirement, the heap allocator can help handle the cache alignment as well
    items = heap_caps_aligned_calloc(DW_GDMA_LL_LINK_LIST_ALIGNMENT, num_items, sizeof(dw_gdma_link_list_item_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(items, ESP_ERR_NO_MEM, err, TAG, "no mem for link list items");
    // do memory sync when the link list items are cached
    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    if (data_cache_line_size) {
        // write back and then invalidate the cache, because later we will read/write the link list items by non-cacheable address
        ESP_GOTO_ON_ERROR(esp_cache_msync(items, num_items * sizeof(dw_gdma_link_list_item_t),
                                          ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE | ESP_CACHE_MSYNC_FLAG_UNALIGNED),
                          err, TAG, "cache sync failed");
    }

    list->num_items = num_items;
    list->items = items;
    // want to use non-cached address to operate the link list items
    list->items_nc = (dw_gdma_link_list_item_t *)DW_GDMA_GET_NON_CACHE_ADDR(items);

    // set up the link list
    for (size_t i = 0; i < num_items; i++) {
        dw_gdma_ll_lli_set_next_item_addr(list->items_nc + i, (uint32_t)(list->items + i + 1));
        // set master port for the link list
        dw_gdma_ll_lli_set_link_list_master_port(list->items_nc + i, DW_GDMA_LL_MASTER_PORT_MEMORY);
    }
    switch (config->link_type) {
    case DW_GDMA_LINKED_LIST_TYPE_CIRCULAR:
        dw_gdma_ll_lli_set_next_item_addr(list->items_nc + num_items - 1, (uint32_t)(list->items));
        break;
    case DW_GDMA_LINKED_LIST_TYPE_SINGLY:
        dw_gdma_ll_lli_set_next_item_addr(list->items_nc + num_items - 1, 0);
        break;
    }

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

esp_err_t dw_gdma_del_link_list(dw_gdma_link_list_handle_t list)
{
    ESP_RETURN_ON_FALSE(list, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_LOGD(TAG, "del link list at %p", list);
    free(list->items);
    free(list);
    return ESP_OK;
}

esp_err_t dw_gdma_channel_use_link_list(dw_gdma_channel_handle_t chan, dw_gdma_link_list_handle_t list)
{
    ESP_RETURN_ON_FALSE(chan && list, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->src_transfer_type == DW_GDMA_BLOCK_TRANSFER_LIST ||
                        chan->dst_transfer_type == DW_GDMA_BLOCK_TRANSFER_LIST,
                        ESP_ERR_INVALID_STATE, TAG, "invalid transfer type");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;

    // set master port for the link list
    dw_gdma_ll_channel_set_link_list_master_port(hal->dev, chan_id, DW_GDMA_LL_MASTER_PORT_MEMORY);
    // set the link list head address
    dw_gdma_ll_channel_set_link_list_head_addr(hal->dev, chan_id, (uint32_t)(list->items));
    return ESP_OK;
}

dw_gdma_lli_handle_t dw_gdma_link_list_get_item(dw_gdma_link_list_handle_t list, int item_index)
{
    ESP_RETURN_ON_FALSE_ISR(list, NULL, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(item_index < list->num_items, NULL, TAG, "invalid item index");
    // Note: the returned address is non-cached
    dw_gdma_link_list_item_t *lli = list->items_nc + item_index;
    return lli;
}

esp_err_t dw_gdma_lli_set_next(dw_gdma_lli_handle_t lli, dw_gdma_lli_handle_t next)
{
    ESP_RETURN_ON_FALSE(lli && next, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // the next field must use a cached address, so convert it to a cached address
    dw_gdma_ll_lli_set_next_item_addr(lli, DW_GDMA_GET_CACHE_ADDRESS(next));

    return ESP_OK;
}

esp_err_t dw_gdma_channel_config_transfer(dw_gdma_channel_handle_t chan, const dw_gdma_block_transfer_config_t *config)
{
    ESP_RETURN_ON_FALSE(chan && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->src_transfer_type != DW_GDMA_BLOCK_TRANSFER_LIST &&
                        chan->dst_transfer_type != DW_GDMA_BLOCK_TRANSFER_LIST,
                        ESP_ERR_INVALID_STATE, TAG, "invalid transfer type");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;

    // set memory address
    dw_gdma_ll_channel_set_src_addr(hal->dev, chan_id, config->src.addr);
    dw_gdma_ll_channel_set_dst_addr(hal->dev, chan_id, config->dst.addr);
    // transfer size
    dw_gdma_ll_channel_set_trans_block_size(hal->dev, chan_id, config->size);
    // [Ctrl0] register
    // set master port for the source and destination target
    dw_gdma_ll_channel_set_src_master_port(hal->dev, chan_id, config->src.addr);
    dw_gdma_ll_channel_set_dst_master_port(hal->dev, chan_id, config->dst.addr);
    // transfer width
    dw_gdma_ll_channel_set_src_trans_width(hal->dev, chan_id, config->src.width);
    dw_gdma_ll_channel_set_dst_trans_width(hal->dev, chan_id, config->dst.width);
    // set burst items
    dw_gdma_ll_channel_set_src_burst_items(hal->dev, chan_id, config->src.burst_items);
    dw_gdma_ll_channel_set_dst_burst_items(hal->dev, chan_id, config->dst.burst_items);
    // set burst mode
    dw_gdma_ll_channel_set_src_burst_mode(hal->dev, chan_id, config->src.burst_mode);
    dw_gdma_ll_channel_set_dst_burst_mode(hal->dev, chan_id, config->dst.burst_mode);
    // [Ctrl1] register
    // set burst length
    dw_gdma_ll_channel_set_src_burst_len(hal->dev, chan_id, config->src.burst_len);
    dw_gdma_ll_channel_set_dst_burst_len(hal->dev, chan_id, config->dst.burst_len);
    // whether to enable the peripheral status write back
    dw_gdma_ll_channel_enable_src_periph_status_write_back(hal->dev, chan_id, config->src.flags.en_status_write_back);
    dw_gdma_ll_channel_enable_dst_periph_status_write_back(hal->dev, chan_id, config->dst.flags.en_status_write_back);

    return ESP_OK;
}

esp_err_t dw_gdma_channel_set_block_markers(dw_gdma_channel_handle_t chan, dw_gdma_block_markers_t markers)
{
    ESP_RETURN_ON_FALSE_ISR(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(chan->src_transfer_type != DW_GDMA_BLOCK_TRANSFER_LIST &&
                            chan->dst_transfer_type != DW_GDMA_BLOCK_TRANSFER_LIST,
                            ESP_ERR_INVALID_STATE, TAG, "invalid transfer type");
    dw_gdma_hal_context_t *hal = &chan->group->hal;
    int chan_id = chan->chan_id;

    // [Ctrl1] register
    // set the block markers
    dw_gdma_ll_channel_set_block_markers(hal->dev, chan_id, markers.en_trans_done_intr, markers.is_last, markers.is_valid);

    return ESP_OK;
}

esp_err_t dw_gdma_lli_config_transfer(dw_gdma_lli_handle_t lli, const dw_gdma_block_transfer_config_t *config)
{
    ESP_RETURN_ON_FALSE(lli && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // set memory address
    dw_gdma_ll_lli_set_src_addr(lli, config->src.addr);
    dw_gdma_ll_lli_set_dst_addr(lli, config->dst.addr);
    // transfer size
    dw_gdma_ll_lli_set_trans_block_size(lli, config->size);
    // [Ctrl0] register
    // set master port for the source and destination target
    dw_gdma_ll_lli_set_src_master_port(lli, config->src.addr);
    dw_gdma_ll_lli_set_dst_master_port(lli, config->dst.addr);
    // transfer width
    dw_gdma_ll_lli_set_src_trans_width(lli, config->src.width);
    dw_gdma_ll_lli_set_dst_trans_width(lli, config->dst.width);
    // set burst items
    dw_gdma_ll_lli_set_src_burst_items(lli, config->src.burst_items);
    dw_gdma_ll_lli_set_dst_burst_items(lli, config->dst.burst_items);
    // set burst mode
    dw_gdma_ll_lli_set_src_burst_mode(lli, config->src.burst_mode);
    dw_gdma_ll_lli_set_dst_burst_mode(lli, config->dst.burst_mode);
    // [Ctrl1] register
    // set burst length
    dw_gdma_ll_lli_set_src_burst_len(lli, config->src.burst_len);
    dw_gdma_ll_lli_set_dst_burst_len(lli, config->dst.burst_len);
    // whether to enable the peripheral status write back
    dw_gdma_ll_lli_enable_src_periph_status_write_back(lli, config->src.flags.en_status_write_back);
    dw_gdma_ll_lli_enable_dst_periph_status_write_back(lli, config->dst.flags.en_status_write_back);

    return ESP_OK;
}

esp_err_t dw_gdma_lli_set_block_markers(dw_gdma_lli_handle_t lli, dw_gdma_block_markers_t markers)
{
    ESP_RETURN_ON_FALSE_ISR(lli, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // [Ctrl1] register
    // set the block markers
    dw_gdma_ll_lli_set_block_markers(lli, markers.en_trans_done_intr, markers.is_last, markers.is_valid);

    return ESP_OK;
}

void dw_gdma_channel_default_isr(void *args)
{
    dw_gdma_channel_t *chan = (dw_gdma_channel_t *)args;
    dw_gdma_group_t *group = chan->group;
    dw_gdma_hal_context_t *hal = &group->hal;
    int chan_id = chan->chan_id;
    bool need_yield = false;
    // clear pending interrupt event
    uint32_t intr_status = dw_gdma_ll_channel_get_intr_status(hal->dev, chan_id);
    dw_gdma_ll_channel_clear_intr(hal->dev, chan_id, intr_status);

    // call user callbacks
    if (intr_status & DW_GDMA_LL_CHANNEL_EVENT_SHADOWREG_OR_LLI_INVALID_ERR) {
        if (chan->cbs.on_invalid_block) {
            intptr_t invalid_lli_addr = dw_gdma_ll_channel_get_current_link_list_item_addr(hal->dev, chan_id);
            dw_gdma_break_event_data_t edata = {
                .invalid_lli = (dw_gdma_lli_handle_t)DW_GDMA_GET_NON_CACHE_ADDR(invalid_lli_addr),
            };
            if (chan->cbs.on_invalid_block(chan, &edata, chan->user_data)) {
                need_yield = true;
            }
        }
    }

    if (intr_status & DW_GDMA_LL_CHANNEL_EVENT_BLOCK_TFR_DONE) {
        if (chan->cbs.on_block_trans_done) {
            dw_gdma_trans_done_event_data_t edata = {};
            if (chan->cbs.on_block_trans_done(chan, &edata, chan->user_data)) {
                need_yield = true;
            }
        }
    }

    if (intr_status & DW_GDMA_LL_CHANNEL_EVENT_DMA_TFR_DONE) {
        if (chan->cbs.on_full_trans_done) {
            dw_gdma_trans_done_event_data_t edata = {};
            if (chan->cbs.on_full_trans_done(chan, &edata, chan->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t dw_gdma_install_channel_interrupt(dw_gdma_channel_t *chan)
{
    esp_err_t ret = ESP_OK;
    dw_gdma_group_t *group = chan->group;
    dw_gdma_hal_context_t *hal = &group->hal;
    int chan_id = chan->chan_id;
    // clear pending events
    dw_gdma_ll_channel_enable_intr_propagation(hal->dev, chan_id, UINT32_MAX, false);
    dw_gdma_ll_channel_clear_intr(hal->dev, chan_id, UINT32_MAX);

    // pre-alloc a interrupt handle, with handler disabled
    // DW_GDMA multiple channels share the same interrupt source, so we use a shared interrupt handle
    intr_handle_t intr = NULL;
    int isr_flags = DW_GDMA_INTR_ALLOC_FLAGS | ESP_INTR_FLAG_SHARED;
    if (group->intr_priority) {
        isr_flags |= 1 << (group->intr_priority);
    } else {
        isr_flags |= DW_GDMA_ALLOW_INTR_PRIORITY_MASK;
    }
    ret = esp_intr_alloc_intrstatus(ETS_DW_GDMA_INTR_SOURCE, isr_flags,
                                    (uint32_t)dw_gdma_ll_get_intr_status_reg(hal->dev), DW_GDMA_LL_CHANNEL_EVENT_MASK(chan_id),
                                    dw_gdma_channel_default_isr, chan, &intr);
    ESP_RETURN_ON_ERROR(ret, TAG, "alloc interrupt failed");

    ESP_LOGD(TAG, "install interrupt service for channel (%d,%d)", group->group_id, chan_id);
    chan->intr = intr;
    return ESP_OK;
}

esp_err_t dw_gdma_channel_register_event_callbacks(dw_gdma_channel_handle_t chan, dw_gdma_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(chan && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dw_gdma_group_t *group = chan->group;
    dw_gdma_hal_context_t *hal = &group->hal;
    int chan_id = chan->chan_id;

#if CONFIG_DW_GDMA_ISR_IRAM_SAFE
    if (cbs->on_block_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_block_trans_done), ESP_ERR_INVALID_ARG,
                            TAG, "on_block_trans_done not in IRAM");
    }
    if (cbs->on_full_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_full_trans_done), ESP_ERR_INVALID_ARG,
                            TAG, "on_full_trans_done not in IRAM");
    }
    if (cbs->on_invalid_block) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_invalid_block), ESP_ERR_INVALID_ARG,
                            TAG, "on_invalid_block not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG,
                            TAG, "user context not in internal RAM");
    }
#endif // CONFIG_DW_GDMA_ISR_IRAM_SAFE

    // lazy install interrupt service
    if (!chan->intr) {
        ESP_RETURN_ON_ERROR(dw_gdma_install_channel_interrupt(chan), TAG, "install interrupt service failed");
    }

    // enable the event to be able to trigger an interrupt
    dw_gdma_ll_channel_enable_intr_propagation(hal->dev, chan_id, DW_GDMA_LL_CHANNEL_EVENT_BLOCK_TFR_DONE, cbs->on_block_trans_done != NULL);
    dw_gdma_ll_channel_enable_intr_propagation(hal->dev, chan_id, DW_GDMA_LL_CHANNEL_EVENT_DMA_TFR_DONE, cbs->on_full_trans_done != NULL);
    dw_gdma_ll_channel_enable_intr_propagation(hal->dev, chan_id, DW_GDMA_LL_CHANNEL_EVENT_SHADOWREG_OR_LLI_INVALID_ERR, cbs->on_invalid_block != NULL);

    chan->user_data = user_data;
    memcpy(&chan->cbs, cbs, sizeof(dw_gdma_event_callbacks_t));

    return ESP_OK;
}

esp_err_t dw_gdma_channel_get_id(dw_gdma_channel_handle_t chan, int *channel_id)
{
    ESP_RETURN_ON_FALSE(chan && channel_id, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *channel_id = chan->chan_id;
    return ESP_OK;
}
