/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
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
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_memory_utils.h"
#include "esp_cache.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/gdma_ll.h"

static const char *TAG = "async_mcp.gdma";

#define MCP_DMA_DESCRIPTOR_BUFFER_MAX_SIZE 4095

/// @brief Transaction object for async memcpy
typedef struct async_memcpy_transaction_t {
    gdma_link_list_handle_t tx_link_list;  // DMA link list for TX direction
    gdma_link_list_handle_t rx_link_list;  // DMA link list for RX direction
    dma_buffer_split_array_t rx_buf_array; // Split the destination buffer into cache aligned ones, save the splits in this array
    uint8_t* stash_buffer;                 // Stash buffer for cache aligned buffer
    async_memcpy_isr_cb_t cb; // user callback
    void *cb_args;            // user callback args
    STAILQ_ENTRY(async_memcpy_transaction_t) idle_queue_entry;  // Entry for the idle queue
    STAILQ_ENTRY(async_memcpy_transaction_t) ready_queue_entry; // Entry for the ready queue
} async_memcpy_transaction_t;

/// @brief Context of async memcpy driver
/// @note - It saves two queues, one for idle transaction objects, one for ready transaction objects
/// @note - Number of transaction objects are determined by the backlog parameter
typedef struct {
    async_memcpy_context_t parent; // Parent IO interface
    size_t rx_int_mem_alignment;   // Required DMA buffer alignment for internal RX memory
    size_t rx_ext_mem_alignment;   // Required DMA buffer alignment for external RX memory
    size_t tx_int_mem_alignment;   // Required DMA buffer alignment for internal TX memory
    size_t tx_ext_mem_alignment;   // Required DMA buffer alignment for external TX memory
    int gdma_bus_id;               // GDMA bus id (AHB, AXI, etc.)
    gdma_channel_handle_t tx_channel; // GDMA TX channel handle
    gdma_channel_handle_t rx_channel; // GDMA RX channel handle
    portMUX_TYPE spin_lock;           // spin lock to avoid threads and isr from accessing the same resource simultaneously
    _Atomic async_memcpy_fsm_t fsm;   // driver state machine, changing state should be atomic
    size_t num_trans_objs;            // number of transaction objects
    async_memcpy_transaction_t *transaction_pool;    // transaction object pool
    async_memcpy_transaction_t *current_transaction; // current transaction object
    STAILQ_HEAD(, async_memcpy_transaction_t) idle_queue_head;  // Head of the idle queue
    STAILQ_HEAD(, async_memcpy_transaction_t) ready_queue_head; // Head of the ready queue
} async_memcpy_gdma_context_t;

static bool mcp_gdma_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);
static esp_err_t mcp_gdma_del(async_memcpy_context_t *ctx);
static esp_err_t mcp_gdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args);
#if SOC_GDMA_SUPPORT_ETM
static esp_err_t mcp_new_etm_event(async_memcpy_context_t *ctx, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event);
#endif // SOC_GDMA_SUPPORT_ETM

static esp_err_t mcp_gdma_destroy(async_memcpy_gdma_context_t *mcp_gdma)
{
    // clean up transaction pool
    if (mcp_gdma->transaction_pool) {
        for (size_t i = 0; i < mcp_gdma->num_trans_objs; i++) {
            async_memcpy_transaction_t* trans = &mcp_gdma->transaction_pool[i];
            if (trans->tx_link_list) {
                gdma_del_link_list(trans->tx_link_list);
            }
            if (trans->rx_link_list) {
                gdma_del_link_list(trans->rx_link_list);
            }
            if (trans->stash_buffer) {
                free(trans->stash_buffer);
            }
        }
        free(mcp_gdma->transaction_pool);
    }
    // clean up GDMA channels
    if (mcp_gdma->tx_channel) {
        gdma_disconnect(mcp_gdma->tx_channel);
        gdma_del_channel(mcp_gdma->tx_channel);
    }
    if (mcp_gdma->rx_channel) {
        gdma_disconnect(mcp_gdma->rx_channel);
        gdma_del_channel(mcp_gdma->rx_channel);
    }
    free(mcp_gdma);
    return ESP_OK;
}

static esp_err_t esp_async_memcpy_install_gdma_template(const async_memcpy_config_t *config, async_memcpy_handle_t *mcp,
                                                        esp_err_t (*new_channel_func)(const gdma_channel_alloc_config_t *, gdma_channel_handle_t *),
                                                        int gdma_bus_id)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_gdma_context_t *mcp_gdma = NULL;
    ESP_RETURN_ON_FALSE(config && mcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // allocate memory of driver context from internal memory (because it contains atomic variable)
    mcp_gdma = heap_caps_calloc(1, sizeof(async_memcpy_gdma_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_gdma, ESP_ERR_NO_MEM, err, TAG, "no mem for driver context");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_TRANSACTION_QUEUE_LENGTH;
    // allocate memory for transaction pool from internal memory
    mcp_gdma->transaction_pool = heap_caps_calloc(trans_queue_len, sizeof(async_memcpy_transaction_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_gdma->transaction_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction pool");

    // create TX channel and RX channel, they should reside in the same DMA pair
    gdma_channel_alloc_config_t tx_alloc_config = {
        .flags.reserve_sibling = 1,
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_GOTO_ON_ERROR(new_channel_func(&tx_alloc_config, &mcp_gdma->tx_channel), err, TAG, "failed to alloc GDMA TX channel");
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = mcp_gdma->tx_channel,
    };
    ESP_GOTO_ON_ERROR(new_channel_func(&rx_alloc_config, &mcp_gdma->rx_channel), err, TAG, "failed to alloc GDMA RX channel");

    // get a free DMA trigger ID for memory copy
    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(mcp_gdma->tx_channel, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    ESP_GOTO_ON_ERROR(gdma_connect(mcp_gdma->rx_channel, m2m_trigger), err, TAG, "GDMA rx connect failed");
    ESP_GOTO_ON_ERROR(gdma_connect(mcp_gdma->tx_channel, m2m_trigger), err, TAG, "GDMA tx connect failed");

    gdma_strategy_config_t strategy_cfg = {
        .owner_check = true,
        .auto_update_desc = true,
        .eof_till_data_popped = true,
    };
    gdma_apply_strategy(mcp_gdma->tx_channel, &strategy_cfg);
    gdma_apply_strategy(mcp_gdma->rx_channel, &strategy_cfg);

#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
    if(config->weight){
        ESP_GOTO_ON_ERROR(gdma_set_weight(mcp_gdma->rx_channel, config->weight), err, TAG, "Set GDMA rx channel weight failed");
        ESP_GOTO_ON_ERROR(gdma_set_weight(mcp_gdma->tx_channel, config->weight), err, TAG, "Set GDMA tx channel weight failed");
    }
#endif
    gdma_transfer_config_t transfer_cfg = {
        .max_data_burst_size = config->dma_burst_size,
        .access_ext_mem = true, // allow to do memory copy from/to external memory
    };
    ESP_GOTO_ON_ERROR(gdma_config_transfer(mcp_gdma->tx_channel, &transfer_cfg), err, TAG, "config transfer for tx channel failed");
    ESP_GOTO_ON_ERROR(gdma_config_transfer(mcp_gdma->rx_channel, &transfer_cfg), err, TAG, "config transfer for rx channel failed");

    // get the buffer alignment required by the GDMA channel
    gdma_get_alignment_constraints(mcp_gdma->rx_channel, &mcp_gdma->rx_int_mem_alignment, &mcp_gdma->rx_ext_mem_alignment);
    gdma_get_alignment_constraints(mcp_gdma->tx_channel, &mcp_gdma->tx_int_mem_alignment, &mcp_gdma->tx_ext_mem_alignment);

    // register rx eof callback
    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = mcp_gdma_rx_eof_callback,
    };
    ESP_GOTO_ON_ERROR(gdma_register_rx_event_callbacks(mcp_gdma->rx_channel, &cbs, mcp_gdma), err, TAG, "failed to register RX EOF callback");

    // initialize transaction queue
    STAILQ_INIT(&mcp_gdma->idle_queue_head);
    STAILQ_INIT(&mcp_gdma->ready_queue_head);
    // pick transactions from the pool and insert to the idle queue
    for (int i = 0; i < trans_queue_len; i++) {
        STAILQ_INSERT_TAIL(&mcp_gdma->idle_queue_head, &mcp_gdma->transaction_pool[i], idle_queue_entry);
    }

    // initialize other members
    portMUX_INITIALIZE(&mcp_gdma->spin_lock);
    atomic_init(&mcp_gdma->fsm, MCP_FSM_IDLE);
    mcp_gdma->gdma_bus_id = gdma_bus_id;
    mcp_gdma->num_trans_objs = trans_queue_len;

    mcp_gdma->parent.del = mcp_gdma_del;
    mcp_gdma->parent.memcpy = mcp_gdma_memcpy;
#if SOC_GDMA_SUPPORT_ETM
    mcp_gdma->parent.new_etm_event = mcp_new_etm_event;
#endif
    // return base object
    *mcp = &mcp_gdma->parent;
    return ESP_OK;

err:
    if (mcp_gdma) {
        mcp_gdma_destroy(mcp_gdma);
    }
    return ret;
}

#if SOC_AHB_GDMA_SUPPORTED
esp_err_t esp_async_memcpy_install_gdma_ahb(const async_memcpy_config_t *config, async_memcpy_handle_t *mcp)
{
    return esp_async_memcpy_install_gdma_template(config, mcp, gdma_new_ahb_channel, SOC_GDMA_BUS_AHB);
}
#endif // SOC_AHB_GDMA_SUPPORTED

#if SOC_AXI_GDMA_SUPPORTED
esp_err_t esp_async_memcpy_install_gdma_axi(const async_memcpy_config_t *config, async_memcpy_handle_t *mcp)
{
    return esp_async_memcpy_install_gdma_template(config, mcp, gdma_new_axi_channel, SOC_GDMA_BUS_AXI);
}
#endif // SOC_AXI_GDMA_SUPPORTED

#if SOC_AHB_GDMA_SUPPORTED
/// default installation falls back to use the AHB GDMA
esp_err_t esp_async_memcpy_install(const async_memcpy_config_t *config, async_memcpy_handle_t *asmcp)
__attribute__((alias("esp_async_memcpy_install_gdma_ahb")));
#elif SOC_AXI_GDMA_SUPPORTED
/// default installation falls back to use the AXI GDMA
esp_err_t esp_async_memcpy_install(const async_memcpy_config_t *config, async_memcpy_handle_t *asmcp)
__attribute__((alias("esp_async_memcpy_install_gdma_axi")));
#endif

static esp_err_t mcp_gdma_del(async_memcpy_context_t *ctx)
{
    async_memcpy_gdma_context_t *mcp_gdma = __containerof(ctx, async_memcpy_gdma_context_t, parent);
    // check if there are pending transactions
    ESP_RETURN_ON_FALSE(STAILQ_EMPTY(&mcp_gdma->ready_queue_head), ESP_ERR_INVALID_STATE, TAG, "there are pending transactions");
    // check if the driver is in IDLE state
    ESP_RETURN_ON_FALSE(atomic_load(&mcp_gdma->fsm) == MCP_FSM_IDLE, ESP_ERR_INVALID_STATE, TAG, "driver is not in IDLE state");
    return mcp_gdma_destroy(mcp_gdma);
}

/// @brief help function to get one transaction from the ready queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_ready_queue(async_memcpy_gdma_context_t *mcp_gdma)
{
    async_memcpy_transaction_t *trans = NULL;
    portENTER_CRITICAL_SAFE(&mcp_gdma->spin_lock);
    trans = STAILQ_FIRST(&mcp_gdma->ready_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_gdma->ready_queue_head, ready_queue_entry);
    }
    portEXIT_CRITICAL_SAFE(&mcp_gdma->spin_lock);
    return trans;
}

/// @brief help function to start a pending transaction
/// @note this function is allowed to be called in ISR
static void try_start_pending_transaction(async_memcpy_gdma_context_t *mcp_gdma)
{
    async_memcpy_fsm_t expected_fsm = MCP_FSM_IDLE;
    async_memcpy_transaction_t *trans = NULL;
    if (atomic_compare_exchange_strong(&mcp_gdma->fsm, &expected_fsm, MCP_FSM_RUN_WAIT)) {
        trans = try_pop_trans_from_ready_queue(mcp_gdma);
        if (trans) {
            atomic_store(&mcp_gdma->fsm, MCP_FSM_RUN);
            mcp_gdma->current_transaction = trans;
            gdma_start(mcp_gdma->rx_channel, gdma_link_get_head_addr(trans->rx_link_list));
            gdma_start(mcp_gdma->tx_channel, gdma_link_get_head_addr(trans->tx_link_list));
        } else {
            atomic_store(&mcp_gdma->fsm, MCP_FSM_IDLE);
        }
    }
}

/// @brief help function to get one transaction from the idle queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_idle_queue(async_memcpy_gdma_context_t *mcp_gdma)
{
    async_memcpy_transaction_t *trans = NULL;
    portENTER_CRITICAL_SAFE(&mcp_gdma->spin_lock);
    trans = STAILQ_FIRST(&mcp_gdma->idle_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_gdma->idle_queue_head, idle_queue_entry);
    }
    portEXIT_CRITICAL_SAFE(&mcp_gdma->spin_lock);
    return trans;
}

/// @brief Check if the address and size can meet the requirement of the DMA engine
static bool check_buffer_alignment(async_memcpy_gdma_context_t *mcp_gdma, void *src, void *dst, size_t n)
{
    bool valid = true;

    if (esp_ptr_external_ram(dst)) {
        valid = valid && (((uint32_t)dst & (mcp_gdma->rx_ext_mem_alignment - 1)) == 0);
        valid = valid && ((n & (mcp_gdma->rx_ext_mem_alignment - 1)) == 0);
    } else {
        valid = valid && (((uint32_t)dst & (mcp_gdma->rx_int_mem_alignment - 1)) == 0);
        valid = valid && ((n & (mcp_gdma->rx_int_mem_alignment - 1)) == 0);
    }

    if (esp_ptr_external_ram(src)) {
        valid = valid && (((uint32_t)src & (mcp_gdma->tx_ext_mem_alignment - 1)) == 0);
        valid = valid && ((n & (mcp_gdma->tx_ext_mem_alignment - 1)) == 0);
    } else {
        valid = valid && (((uint32_t)src & (mcp_gdma->tx_int_mem_alignment - 1)) == 0);
        valid = valid && ((n & (mcp_gdma->tx_int_mem_alignment - 1)) == 0);
    }

    return valid;
}

static esp_err_t mcp_gdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_gdma_context_t *mcp_gdma = __containerof(ctx, async_memcpy_gdma_context_t, parent);
    size_t dma_link_item_alignment = 4;
    // buffer location check
#if SOC_AHB_GDMA_SUPPORTED
    if (mcp_gdma->gdma_bus_id == SOC_GDMA_BUS_AHB) {
#if !SOC_AHB_GDMA_SUPPORT_PSRAM
        ESP_RETURN_ON_FALSE(esp_ptr_internal(src) && esp_ptr_internal(dst), ESP_ERR_INVALID_ARG, TAG, "AHB GDMA can only access SRAM");
#endif // !SOC_AHB_GDMA_SUPPORT_PSRAM
        dma_link_item_alignment = GDMA_LL_AHB_DESC_ALIGNMENT;
    }
#endif // SOC_AHB_GDMA_SUPPORTED
#if SOC_AXI_GDMA_SUPPORTED
    if (mcp_gdma->gdma_bus_id == SOC_GDMA_BUS_AXI) {
#if !SOC_AXI_GDMA_SUPPORT_PSRAM
        ESP_RETURN_ON_FALSE(esp_ptr_internal(src) && esp_ptr_internal(dst), ESP_ERR_INVALID_ARG, TAG, "AXI GDMA can only access SRAM");
#endif // !SOC_AXI_GDMA_SUPPORT_PSRAM
        dma_link_item_alignment = GDMA_LL_AXI_DESC_ALIGNMENT;
    }
#endif // SOC_AXI_GDMA_SUPPORTED
    // alignment check
    ESP_RETURN_ON_FALSE(check_buffer_alignment(mcp_gdma, src, dst, n), ESP_ERR_INVALID_ARG, TAG, "address|size not aligned: %p -> %p, sz=%zu", src, dst, n);

    async_memcpy_transaction_t *trans = NULL;
    // pick one transaction node from idle queue
    trans = try_pop_trans_from_idle_queue(mcp_gdma);
    // check if we get the transaction object successfully
    ESP_RETURN_ON_FALSE(trans, ESP_ERR_INVALID_STATE, TAG, "no free node in the idle queue");

    // clean up the transaction configuration comes from the last one
    if (trans->tx_link_list) {
        gdma_del_link_list(trans->tx_link_list);
        trans->tx_link_list = NULL;
    }
    if (trans->rx_link_list) {
        gdma_del_link_list(trans->rx_link_list);
        trans->rx_link_list = NULL;
    }
    if (trans->stash_buffer) {
        free(trans->stash_buffer);
        trans->stash_buffer = NULL;
    }

    size_t buffer_alignment = 0;
    size_t num_dma_nodes = 0;

    // allocate gdma TX link
    buffer_alignment = esp_ptr_internal(src) ? mcp_gdma->tx_int_mem_alignment : mcp_gdma->tx_ext_mem_alignment;
    num_dma_nodes = esp_dma_calculate_node_count(n, buffer_alignment, MCP_DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
    gdma_link_list_config_t tx_link_cfg = {
        .buffer_alignment = buffer_alignment,
        .item_alignment = dma_link_item_alignment,
        .num_items = num_dma_nodes,
        .flags = {
            .check_owner = true,
            .items_in_ext_mem = false, // TODO: if the memcopy size is too large, we may need to allocate the link list items from external memory
        },
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&tx_link_cfg, &trans->tx_link_list), err, TAG, "failed to create TX link list");
    // mount the source buffer to the TX link list
    gdma_buffer_mount_config_t tx_buf_mount_config[1] = {
        [0] = {
            .buffer = src,
            .length = n,
            .flags = {
                .mark_eof = true,   // mark the last item as EOF, so the RX channel can also received an EOF list item
                .mark_final = true, // using singly list, so terminate the link here
            }
        }
    };
    gdma_link_mount_buffers(trans->tx_link_list, 0, tx_buf_mount_config, 1, NULL);

    // read the cache line size of internal and external memory, we use this information to check if a given memory is behind the cache
    // write back the source data if it's behind the cache
    size_t int_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t ext_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    bool need_write_back = false;
    if (esp_ptr_external_ram(src)) {
        need_write_back = ext_mem_cache_line_size > 0;
    } else if (esp_ptr_internal(src)) {
        need_write_back = int_mem_cache_line_size > 0;
    }
    if (need_write_back) {
        esp_cache_msync(src, n, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }

    // allocate gdma RX link
    buffer_alignment = esp_ptr_internal(dst) ? mcp_gdma->rx_int_mem_alignment : mcp_gdma->rx_ext_mem_alignment;
    num_dma_nodes = esp_dma_calculate_node_count(n, buffer_alignment, MCP_DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
    gdma_link_list_config_t rx_link_cfg = {
        .buffer_alignment = buffer_alignment,
        .item_alignment = dma_link_item_alignment,
        .num_items = num_dma_nodes + 3, // add 3 extra items for the cache aligned buffers
        .flags = {
            .check_owner = true,
            .items_in_ext_mem = false, // TODO: if the memcopy size is too large, we may need to allocate the link list items from external memory
        },
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&rx_link_cfg, &trans->rx_link_list), err, TAG, "failed to create RX link list");

    // if the destination buffer address is not cache line aligned, we need to split the buffer into cache line aligned ones
    ESP_GOTO_ON_ERROR(esp_dma_split_rx_buffer_to_cache_aligned(dst, n, &trans->rx_buf_array, &trans->stash_buffer),
                      err, TAG, "failed to split RX buffer into aligned ones");
    // mount the destination buffer to the RX link list
    gdma_buffer_mount_config_t rx_buf_mount_config[3] = {0};
    for (int i = 0; i < 3; i++) {
        rx_buf_mount_config[i].buffer = trans->rx_buf_array.aligned_buffer[i].aligned_buffer;
        rx_buf_mount_config[i].length = trans->rx_buf_array.aligned_buffer[i].length;
    }
    gdma_link_mount_buffers(trans->rx_link_list, 0, rx_buf_mount_config, 3, NULL);

    // save other transaction context
    trans->cb = cb_isr;
    trans->cb_args = cb_args;

    portENTER_CRITICAL(&mcp_gdma->spin_lock);
    // insert the trans to ready queue
    STAILQ_INSERT_TAIL(&mcp_gdma->ready_queue_head, trans, ready_queue_entry);
    portEXIT_CRITICAL(&mcp_gdma->spin_lock);

    // check driver state, if there's no running transaction, start a new one
    try_start_pending_transaction(mcp_gdma);

    return ESP_OK;

err:
    if (trans) {
        // return back the trans to idle queue
        portENTER_CRITICAL(&mcp_gdma->spin_lock);
        STAILQ_INSERT_TAIL(&mcp_gdma->idle_queue_head, trans, idle_queue_entry);
        portEXIT_CRITICAL(&mcp_gdma->spin_lock);
    }
    return ret;
}

static bool mcp_gdma_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    async_memcpy_gdma_context_t *mcp_gdma = (async_memcpy_gdma_context_t *)user_data;
    async_memcpy_transaction_t *trans = mcp_gdma->current_transaction;
    dma_buffer_split_array_t *rx_buf_array = &trans->rx_buf_array;

    // switch driver state from RUN to IDLE
    async_memcpy_fsm_t expected_fsm = MCP_FSM_RUN;
    if (atomic_compare_exchange_strong(&mcp_gdma->fsm, &expected_fsm, MCP_FSM_IDLE_WAIT)) {
        // merge the cache aligned buffers to the original buffer
        esp_dma_merge_aligned_rx_buffers(rx_buf_array);

        // invoked callback registered by user
        async_memcpy_isr_cb_t cb = trans->cb;
        if (cb) {
            async_memcpy_event_t e = {
                // No event data for now
            };
            need_yield = cb(&mcp_gdma->parent, &e, trans->cb_args);
        }
        trans->cb = NULL;

        portENTER_CRITICAL_ISR(&mcp_gdma->spin_lock);
        // insert the trans object to the idle queue
        STAILQ_INSERT_TAIL(&mcp_gdma->idle_queue_head, trans, idle_queue_entry);
        portEXIT_CRITICAL_ISR(&mcp_gdma->spin_lock);

        atomic_store(&mcp_gdma->fsm, MCP_FSM_IDLE);
    }

    // try start the next pending transaction
    try_start_pending_transaction(mcp_gdma);

    return need_yield;
}

#if SOC_GDMA_SUPPORT_ETM
static esp_err_t mcp_new_etm_event(async_memcpy_context_t *ctx, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event)
{
    async_memcpy_gdma_context_t *mcp_gdma = __containerof(ctx, async_memcpy_gdma_context_t, parent);
    if (event_type == ASYNC_MEMCPY_ETM_EVENT_COPY_DONE) {
        // use the RX EOF to indicate the async memcpy done event
        gdma_etm_event_config_t etm_event_conf = {
            .event_type = GDMA_ETM_EVENT_EOF,
        };
        return gdma_new_etm_event(mcp_gdma->rx_channel, &etm_event_conf, out_event);
    } else {
        return ESP_ERR_NOT_SUPPORTED;
    }
}
#endif // SOC_GDMA_SUPPORT_ETM
