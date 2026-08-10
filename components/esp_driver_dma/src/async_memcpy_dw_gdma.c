/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <sys/queue.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_private/dw_gdma.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/critical_section.h"
#include "esp_memory_utils.h"
#include "esp_cache.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

ESP_LOG_ATTR_TAG(TAG, "async_mcp.dw_gdma");

/// @brief Maximum number of data items that one DW_GDMA block (link list item) can transfer.
/// @note The DW_GDMA block_ts register field is 22 bits wide and stores (size - 1).
#define MCP_DW_GDMA_MAX_BLOCK_ITEMS  (1 << 22)

/// @brief Maximum body transfer width (in bits), capped by the AXI data width.
#define MCP_DW_GDMA_MAX_BODY_WIDTH_BITS  64

/// @brief Transaction object for async memcpy
typedef struct async_memcpy_transaction_t {
    dw_gdma_link_list_handle_t link_list;   // DW_GDMA link list for this transaction (body only)
    async_memcpy_isr_cb_t cb;               // user callback
    void *cb_args;                          // user callback args
    async_memcpy_split_t split;             // cache aligned split, consumed by the deferred cache ops and CPU copy
    STAILQ_ENTRY(async_memcpy_transaction_t) idle_queue_entry;  // Entry for the idle queue
    STAILQ_ENTRY(async_memcpy_transaction_t) ready_queue_entry; // Entry for the ready queue
} async_memcpy_transaction_t;

/// @brief Context of async memcpy driver
/// @note - It saves two queues, one for idle transaction objects, one for ready transaction objects
/// @note - Number of transaction objects are determined by the backlog parameter
typedef struct {
    async_memcpy_context_t parent;          // Parent IO interface
    size_t dma_burst_size;                  // DMA burst size (in bytes), configured by the user
    dw_gdma_channel_handle_t channel;       // DW_GDMA channel handle
    portMUX_TYPE spin_lock;                 // spin lock to avoid threads and isr from accessing the same resource simultaneously
    _Atomic async_memcpy_fsm_t fsm;         // driver state machine, changing state should be atomic
    size_t num_trans_objs;                  // number of transaction objects
    async_memcpy_transaction_t *transaction_pool;     // transaction object pool
    async_memcpy_transaction_t *current_transaction;  // current transaction object
    STAILQ_HEAD(, async_memcpy_transaction_t) idle_queue_head;  // Head of the idle queue
    STAILQ_HEAD(, async_memcpy_transaction_t) ready_queue_head; // Head of the ready queue
} async_memcpy_dw_gdma_context_t;

static bool mcp_dw_gdma_full_trans_done_callback(dw_gdma_channel_handle_t dma_chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data);
static bool mcp_dw_gdma_invalid_block_callback(dw_gdma_channel_handle_t dma_chan, const dw_gdma_break_event_data_t *event_data, void *user_data);
static esp_err_t mcp_dw_gdma_del(async_memcpy_context_t *ctx);
static esp_err_t mcp_dw_gdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args);

static esp_err_t mcp_dw_gdma_destroy(async_memcpy_dw_gdma_context_t *mcp_dw_gdma)
{
    // clean up transaction pool
    if (mcp_dw_gdma->transaction_pool) {
        for (size_t i = 0; i < mcp_dw_gdma->num_trans_objs; i++) {
            async_memcpy_transaction_t *trans = &mcp_dw_gdma->transaction_pool[i];
            if (trans->link_list) {
                dw_gdma_del_link_list(trans->link_list);
            }
        }
        free(mcp_dw_gdma->transaction_pool);
    }
    // clean up DW_GDMA channel
    if (mcp_dw_gdma->channel) {
        dw_gdma_del_channel(mcp_dw_gdma->channel);
    }
    free(mcp_dw_gdma);
    return ESP_OK;
}

/// @brief Pick the largest supported transfer width (in bits) that the given address is aligned to.
/// @note The width is also capped by the cache line size (the destination body is cache aligned) and the AXI data width.
/// @note The width must also divide the body length, so that the (cache line aligned) body can be evenly
///       split into transfer width sized data items without a remainder.
static dw_gdma_transfer_width_t mcp_dw_gdma_get_body_width(size_t cache_line_size, uintptr_t body_addr, size_t body_len)
{
    // the body width is capped by the cache line size (the destination body is cache aligned) and the AXI data width
    size_t max_width_bits = cache_line_size ? MIN(cache_line_size * 8, MCP_DW_GDMA_MAX_BODY_WIDTH_BITS) : MCP_DW_GDMA_MAX_BODY_WIDTH_BITS;
    size_t width_bits = 8;
    for (size_t cand = max_width_bits; cand >= 8; cand >>= 1) {
        if (((body_addr & ((cand >> 3) - 1)) == 0) && (body_len % (cand >> 3) == 0)) {
            width_bits = cand;
            break;
        }
    }
    switch (width_bits) {
    case 64:
        return DW_GDMA_TRANS_WIDTH_64;
    case 32:
        return DW_GDMA_TRANS_WIDTH_32;
    case 16:
        return DW_GDMA_TRANS_WIDTH_16;
    default:
        return DW_GDMA_TRANS_WIDTH_8;
    }
}

/// @brief Map a DMA burst size (in bytes) to a supported DW_GDMA burst size enum value.
static dw_gdma_burst_size_t mcp_dw_gdma_get_burst_size(size_t burst_size_bytes, size_t width_bytes)
{
    static const dw_gdma_burst_size_t supported[] = {
        DW_GDMA_BURST_SIZE_1, DW_GDMA_BURST_SIZE_4, DW_GDMA_BURST_SIZE_8, DW_GDMA_BURST_SIZE_16,
        DW_GDMA_BURST_SIZE_32, DW_GDMA_BURST_SIZE_64, DW_GDMA_BURST_SIZE_128, DW_GDMA_BURST_SIZE_256,
        DW_GDMA_BURST_SIZE_512, DW_GDMA_BURST_SIZE_1024,
    };
    static const size_t supported_count[] = {1, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    size_t items = (burst_size_bytes && width_bytes) ? burst_size_bytes / width_bytes : 0;
    dw_gdma_burst_size_t pick = DW_GDMA_BURST_SIZE_4;
    for (int i = sizeof(supported) / sizeof(supported[0]) - 1; i >= 0; i--) {
        if (supported_count[i] <= items) {
            pick = supported[i];
            break;
        }
    }
    return pick;
}

esp_err_t esp_async_memcpy_install_dw_gdma(const async_memcpy_config_t *config, async_memcpy_handle_t *mcp)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_dw_gdma_context_t *mcp_dw_gdma = NULL;
    ESP_RETURN_ON_FALSE(config && mcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // allocate memory of driver context from internal memory (because it contains atomic variable)
    mcp_dw_gdma = heap_caps_calloc(1, sizeof(async_memcpy_dw_gdma_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_dw_gdma, ESP_ERR_NO_MEM, err, TAG, "no mem for driver context");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_TRANSACTION_QUEUE_LENGTH;
    // allocate memory for transaction pool from internal memory
    mcp_dw_gdma->transaction_pool = heap_caps_calloc(trans_queue_len, sizeof(async_memcpy_transaction_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_dw_gdma->transaction_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction pool");

    // create a single M2M DW_GDMA channel
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_LIST,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        // Fix the channel priority to a constant. The `async_memcpy_config_t.weight` field is intentionally
        // ignored here: DW_GDMA has no weight/bandwidth control capability, and `chan_priority`(arbitration
        // priority) is a different concept from the GDMA weight, so it can't be mapped 1:1.
        .chan_priority = 1,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_channel(&alloc_config, &mcp_dw_gdma->channel), err, TAG, "failed to alloc DW_GDMA channel");

    // register event callbacks
    dw_gdma_event_callbacks_t cbs = {
        .on_full_trans_done = mcp_dw_gdma_full_trans_done_callback,
        .on_invalid_block = mcp_dw_gdma_invalid_block_callback,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_channel_register_event_callbacks(mcp_dw_gdma->channel, &cbs, mcp_dw_gdma),
                      err, TAG, "failed to register event callbacks");

    // initialize transaction queue
    STAILQ_INIT(&mcp_dw_gdma->idle_queue_head);
    STAILQ_INIT(&mcp_dw_gdma->ready_queue_head);
    // pick transactions from the pool and insert to the idle queue
    for (int i = 0; i < trans_queue_len; i++) {
        STAILQ_INSERT_TAIL(&mcp_dw_gdma->idle_queue_head, &mcp_dw_gdma->transaction_pool[i], idle_queue_entry);
    }

    // initialize other members
    portMUX_INITIALIZE(&mcp_dw_gdma->spin_lock);
    atomic_init(&mcp_dw_gdma->fsm, MCP_FSM_IDLE);
    mcp_dw_gdma->num_trans_objs = trans_queue_len;
    mcp_dw_gdma->dma_burst_size = config->dma_burst_size;

    mcp_dw_gdma->parent.del = mcp_dw_gdma_del;
    mcp_dw_gdma->parent.memcpy = mcp_dw_gdma_memcpy;
    // ETM is not supported by the DW_GDMA backend, leave new_etm_event as NULL
    // return base object
    *mcp = &mcp_dw_gdma->parent;
    return ESP_OK;

err:
    if (mcp_dw_gdma) {
        mcp_dw_gdma_destroy(mcp_dw_gdma);
    }
    return ret;
}

static esp_err_t mcp_dw_gdma_del(async_memcpy_context_t *ctx)
{
    async_memcpy_dw_gdma_context_t *mcp_dw_gdma = __containerof(ctx, async_memcpy_dw_gdma_context_t, parent);
    // check if there are pending transactions
    ESP_RETURN_ON_FALSE(STAILQ_EMPTY(&mcp_dw_gdma->ready_queue_head), ESP_ERR_INVALID_STATE, TAG, "there are pending transactions");
    // check if the driver is in IDLE state
    ESP_RETURN_ON_FALSE(atomic_load(&mcp_dw_gdma->fsm) == MCP_FSM_IDLE, ESP_ERR_INVALID_STATE, TAG, "driver is not in IDLE state");
    return mcp_dw_gdma_destroy(mcp_dw_gdma);
}

/// @brief help function to get one transaction from the ready queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_ready_queue(async_memcpy_dw_gdma_context_t *mcp_dw_gdma)
{
    async_memcpy_transaction_t *trans = NULL;
    esp_os_enter_critical_safe(&mcp_dw_gdma->spin_lock);
    trans = STAILQ_FIRST(&mcp_dw_gdma->ready_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_dw_gdma->ready_queue_head, ready_queue_entry);
    }
    esp_os_exit_critical_safe(&mcp_dw_gdma->spin_lock);
    return trans;
}

/// @brief help function to start a pending transaction
/// @note this function is allowed to be called in ISR
static void try_start_pending_transaction(async_memcpy_dw_gdma_context_t *mcp_dw_gdma)
{
    async_memcpy_fsm_t expected_fsm = MCP_FSM_IDLE;
    async_memcpy_transaction_t *trans = NULL;
    if (atomic_compare_exchange_strong(&mcp_dw_gdma->fsm, &expected_fsm, MCP_FSM_WAIT)) {
        trans = try_pop_trans_from_ready_queue(mcp_dw_gdma);
        if (trans) {
            atomic_store(&mcp_dw_gdma->fsm, MCP_FSM_RUN);
            mcp_dw_gdma->current_transaction = trans;
            // Deferred during submit to avoid racing with the previous (still running) DMA transfer that
            // may write to the same/overlapping destination. Now the transfer is actually starting, so the
            // CPU copies and cache maintenance are serialized with the DMA engine.
            async_memcpy_do_cache_ops_and_cpu_copy(&trans->split);
            // apply the link list and kick off the DMA engine
            dw_gdma_channel_use_link_list(mcp_dw_gdma->channel, trans->link_list);
            dw_gdma_channel_enable_ctrl(mcp_dw_gdma->channel, true);
        } else {
            atomic_store(&mcp_dw_gdma->fsm, MCP_FSM_IDLE);
        }
    }
}

/// @brief help function to get one transaction from the idle queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_idle_queue(async_memcpy_dw_gdma_context_t *mcp_dw_gdma)
{
    async_memcpy_transaction_t *trans = NULL;
    esp_os_enter_critical_safe(&mcp_dw_gdma->spin_lock);
    trans = STAILQ_FIRST(&mcp_dw_gdma->idle_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_dw_gdma->idle_queue_head, idle_queue_entry);
    }
    esp_os_exit_critical_safe(&mcp_dw_gdma->spin_lock);
    return trans;
}

static esp_err_t mcp_dw_gdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_dw_gdma_context_t *mcp_dw_gdma = __containerof(ctx, async_memcpy_dw_gdma_context_t, parent);

    async_memcpy_transaction_t *trans = NULL;
    // pick one transaction node from idle queue
    trans = try_pop_trans_from_idle_queue(mcp_dw_gdma);
    // check if we get the transaction object successfully
    ESP_RETURN_ON_FALSE(trans, ESP_ERR_INVALID_STATE, TAG, "no free node in the idle queue");

    // clean up the transaction configuration comes from the last one
    if (trans->link_list) {
        dw_gdma_del_link_list(trans->link_list);
        trans->link_list = NULL;
    }

    // Split the destination buffer into a cache aligned body and (optional) head/tail.
    // The DMA engine only handles the cache aligned body; the unaligned head and tail (if any) are
    // copied by the CPU later, right before the DMA transfer starts (see try_start_pending_transaction).
    // The head/tail and the body never share a cache line, so the CPU and DMA can work concurrently
    // without cache coherency conflicts.
    async_memcpy_split_t split;
    ESP_GOTO_ON_ERROR(async_memcpy_split_cache_aligned(dst, src, n, &split), err, TAG, "failed to split buffer");

    size_t body_len = split.body_len;
    void *body_src = split.body_src;
    void *body_dst = split.body_dst;
    size_t cache_line_size = split.cache_line_size;

    // pick the body transfer width based on the source body address alignment
    // note: the destination body is already cache aligned (so it's aligned to any width <= cache line),
    // but the source body address may not be, which limits the usable transfer width.
    dw_gdma_transfer_width_t body_width = mcp_dw_gdma_get_body_width(cache_line_size, (uintptr_t)body_src, body_len);
    size_t body_width_bytes = 1;
    switch (body_width) {
    case DW_GDMA_TRANS_WIDTH_64:
        body_width_bytes = 8;
        break;
    case DW_GDMA_TRANS_WIDTH_32:
        body_width_bytes = 4;
        break;
    case DW_GDMA_TRANS_WIDTH_16:
        body_width_bytes = 2;
        break;
    default:
        body_width_bytes = 1;
        body_width = DW_GDMA_TRANS_WIDTH_8;
        break;
    }

    // the body might need to be split into multiple link list items if it exceeds the maximum block size
    size_t max_body_bytes_per_item = MCP_DW_GDMA_MAX_BLOCK_ITEMS * body_width_bytes;
    size_t body_alignment = cache_line_size ? cache_line_size : body_width_bytes;
    size_t body_num_items = esp_dma_calculate_node_count(body_len, body_alignment, max_body_bytes_per_item);
    // align down the max body bytes per item to the body alignment, so each chunk stays aligned
    if (cache_line_size) {
        max_body_bytes_per_item &= ~(cache_line_size - 1);
    }

    dw_gdma_link_list_config_t link_list_cfg = {
        .num_items = body_num_items,
        .link_type = DW_GDMA_LINKED_LIST_TYPE_SINGLY,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_link_list(&link_list_cfg, &trans->link_list), err, TAG, "failed to create link list");

    // the burst configuration is derived from the user configured dma_burst_size
    dw_gdma_burst_size_t body_burst_size = mcp_dw_gdma_get_burst_size(mcp_dw_gdma->dma_burst_size, body_width_bytes);

    // configure each body link list item
    uint8_t *cur_src = body_src;
    uint8_t *cur_dst = body_dst;
    size_t remaining = body_len;
    for (size_t i = 0; i < body_num_items; i++) {
        size_t chunk = MIN(remaining, max_body_bytes_per_item);
        dw_gdma_block_transfer_config_t transfer_config = {
            .src = {
                .addr = (uint32_t)cur_src,
                .addr_inc_mode = DW_GDMA_ADDR_INC_MODE_INCREMENT,
                .width = body_width,
                .burst_size = body_burst_size,
                .axi_burst_len = 0,
            },
            .dst = {
                .addr = (uint32_t)cur_dst,
                .addr_inc_mode = DW_GDMA_ADDR_INC_MODE_INCREMENT,
                .width = body_width,
                .burst_size = body_burst_size,
                .axi_burst_len = 0,
            },
            .size = chunk / body_width_bytes,
        };
        dw_gdma_lli_handle_t lli = dw_gdma_link_list_get_item(trans->link_list, i);
        ESP_GOTO_ON_FALSE(lli, ESP_ERR_INVALID_STATE, err, TAG, "failed to get link list item");
        ESP_GOTO_ON_ERROR(dw_gdma_lli_config_transfer(lli, &transfer_config), err, TAG, "failed to config link list item");
        cur_src += chunk;
        cur_dst += chunk;
        remaining -= chunk;
    }

    // set block markers: all items are valid, the last item marks the end of the transfer.
    // the singly link list already chains the items in order (and terminates at the last item),
    // so we only need to mark the last item with is_last and enable the transfer done interrupt on it.
    for (size_t i = 0; i < body_num_items; i++) {
        dw_gdma_lli_handle_t lli = dw_gdma_link_list_get_item(trans->link_list, i);
        dw_gdma_block_markers_t markers = {
            .is_valid = true,
            .is_last = (i == body_num_items - 1),
            .en_trans_done_intr = (i == body_num_items - 1),
        };
        ESP_GOTO_ON_ERROR(dw_gdma_lli_set_block_markers(lli, markers), err, TAG, "failed to set block markers");
    }

    // save other transaction context
    trans->cb = cb_isr;
    trans->cb_args = cb_args;
    // save the split info (includes the original buffers and cache aligned body) for the deferred
    // cache ops and CPU copy (performed in try_start_pending_transaction when the transfer actually starts)
    trans->split = split;

    esp_os_enter_critical(&mcp_dw_gdma->spin_lock);
    // insert the trans to ready queue
    STAILQ_INSERT_TAIL(&mcp_dw_gdma->ready_queue_head, trans, ready_queue_entry);
    esp_os_exit_critical(&mcp_dw_gdma->spin_lock);

    // check driver state, if there's no running transaction, start a new one
    try_start_pending_transaction(mcp_dw_gdma);

    return ESP_OK;

err:
    if (trans) {
        // return back the trans to idle queue
        esp_os_enter_critical(&mcp_dw_gdma->spin_lock);
        STAILQ_INSERT_TAIL(&mcp_dw_gdma->idle_queue_head, trans, idle_queue_entry);
        esp_os_exit_critical(&mcp_dw_gdma->spin_lock);
    }
    return ret;
}

static bool mcp_dw_gdma_full_trans_done_callback(dw_gdma_channel_handle_t dma_chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    async_memcpy_dw_gdma_context_t *mcp_dw_gdma = (async_memcpy_dw_gdma_context_t *)user_data;
    async_memcpy_transaction_t *trans = mcp_dw_gdma->current_transaction;

    // switch driver state from RUN to IDLE
    async_memcpy_fsm_t expected_fsm = MCP_FSM_RUN;
    if (atomic_compare_exchange_strong(&mcp_dw_gdma->fsm, &expected_fsm, MCP_FSM_WAIT)) {
        // the head/tail were already copied by the CPU before the transfer started, and the body was written by the
        // DMA to memory (the body cache lines were invalidated before the transfer), so nothing to merge here.

        // invoked callback registered by user
        async_memcpy_isr_cb_t cb = trans->cb;
        if (cb) {
            async_memcpy_event_t e = {
                // No event data for now
            };
            need_yield = cb(&mcp_dw_gdma->parent, &e, trans->cb_args);
        }
        trans->cb = NULL;

        esp_os_enter_critical_isr(&mcp_dw_gdma->spin_lock);
        // insert the trans object to the idle queue
        STAILQ_INSERT_TAIL(&mcp_dw_gdma->idle_queue_head, trans, idle_queue_entry);
        mcp_dw_gdma->current_transaction = NULL;
        esp_os_exit_critical_isr(&mcp_dw_gdma->spin_lock);

        atomic_store(&mcp_dw_gdma->fsm, MCP_FSM_IDLE);
    }

    // try start the next pending transaction
    try_start_pending_transaction(mcp_dw_gdma);

    return need_yield;
}

static bool mcp_dw_gdma_invalid_block_callback(dw_gdma_channel_handle_t dma_chan, const dw_gdma_break_event_data_t *event_data, void *user_data)
{
    // A valid transfer should never hit an invalid block. Terminate instead of leaving callers blocked.
    ESP_EARLY_LOGE(TAG, "invalid block detected, terminating");
    abort();
}
