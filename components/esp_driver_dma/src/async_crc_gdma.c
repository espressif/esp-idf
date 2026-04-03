/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdatomic.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_cache.h"
#include "esp_async_crc_priv.h"
#include "hal/gdma_ll.h"

ESP_LOG_ATTR_TAG(TAG, "async_crc_gdma");

#define CRC_DMA_DESCRIPTOR_BUFFER_MAX_SIZE 4095

__attribute__((always_inline))
static inline uint32_t bit_reverse32(uint32_t val)
{
    val = ((val >> 1) & 0x55555555) | ((val << 1) & 0xAAAAAAAA);
    val = ((val >> 2) & 0x33333333) | ((val << 2) & 0xCCCCCCCC);
    val = ((val >> 4) & 0x0F0F0F0F) | ((val << 4) & 0xF0F0F0F0);
    val = ((val >> 8) & 0x00FF00FF) | ((val << 8) & 0xFF00FF00);
    val = (val >> 16) | (val << 16);
    return val;
}

/// @brief Transaction object for async CRC
typedef struct async_crc_transaction {
    gdma_link_list_handle_t link_list; // DMA link list for user buffer
    const void *data;                  // User data buffer pointer
    size_t size;                       // Size of data buffer
    async_crc_params_t params;         // CRC parameters (polynomial, init value, etc.)
    async_crc_isr_cb_t cb_isr;         // User callback
    void *cb_args;                     // User callback arguments
    STAILQ_ENTRY(async_crc_transaction) queue_entry; // Entry for idle/ready queue
} async_crc_transaction_t;

/// @brief Context of async CRC driver using GDMA
typedef struct {
    async_crc_context_t parent;        // Parent IO interface
    gdma_channel_handle_t tx_channel;  // GDMA TX channel handle
    portMUX_TYPE spin_lock;            // Spinlock for synchronization
    _Atomic async_crc_fsm_t fsm;       // driver state machine, changing state should be atomic
    size_t tx_int_mem_alignment;       // Required DMA buffer alignment for internal TX memory
    size_t tx_ext_mem_alignment;       // Required DMA buffer alignment for external TX memory
    uint32_t gdma_bus_id;              // GDMA bus id (AHB, AXI, etc.)
    uint32_t num_trans_objs;           // number of transaction objects
    async_crc_transaction_t *transaction_pool;    // transaction object pool
    async_crc_transaction_t *current_transaction; // Track active transaction to avoid search
    STAILQ_HEAD(, async_crc_transaction) idle_queue_head;  // Head of the idle queue
    STAILQ_HEAD(, async_crc_transaction) ready_queue_head; // Head of the ready queue
} async_crc_gdma_context_t;

// Forward declarations
static esp_err_t async_crc_gdma_del(async_crc_context_t *ctx);
static esp_err_t async_crc_gdma_calc(async_crc_context_t *ctx, const void *data, size_t size,
                                     const async_crc_params_t *params, async_crc_isr_cb_t cb_isr, void *cb_args);
static bool async_crc_gdma_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);
static esp_err_t async_crc_prepare_transaction(async_crc_gdma_context_t *crc_gdma, async_crc_transaction_t *trans);

// Queue helper functions
static async_crc_transaction_t *try_pop_trans_from_idle_queue(async_crc_gdma_context_t *crc_gdma);
static async_crc_transaction_t *try_pop_trans_from_ready_queue(async_crc_gdma_context_t *crc_gdma);
static void try_start_pending_transaction(async_crc_gdma_context_t *crc_gdma);

static esp_err_t async_crc_gdma_destroy_context(async_crc_gdma_context_t *crc_gdma)
{
    // clean up transaction pool
    if (crc_gdma->transaction_pool) {
        for (uint32_t i = 0; i < crc_gdma->num_trans_objs; i++) {
            async_crc_transaction_t* trans = &crc_gdma->transaction_pool[i];
            if (trans->link_list) {
                gdma_del_link_list(trans->link_list);
            }
        }
        free(crc_gdma->transaction_pool);
    }
    // Delete GDMA channel
    if (crc_gdma->tx_channel) {
        gdma_disconnect(crc_gdma->tx_channel);
        gdma_del_channel(crc_gdma->tx_channel);
    }
    free(crc_gdma);
    return ESP_OK;
}

esp_err_t esp_async_crc_install_gdma_template(const async_crc_config_t *config, async_crc_handle_t *crc_hdl,
                                              gdma_new_channel_t new_channel_func, uint32_t gdma_bus_id)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && crc_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_TRANSACTION_QUEUE_LENGTH;

    // allocate memory of driver context from internal memory (because it contains atomic variable)
    async_crc_gdma_context_t *crc_gdma = heap_caps_calloc(1, sizeof(async_crc_gdma_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(crc_gdma, ESP_ERR_NO_MEM, TAG, "no mem for crc context");

    // init basic members of crc_gdma context
    portMUX_INITIALIZE(&crc_gdma->spin_lock);
    atomic_init(&crc_gdma->fsm, CRC_FSM_IDLE);
    crc_gdma->gdma_bus_id = gdma_bus_id;
    crc_gdma->num_trans_objs = trans_queue_len;

    // allocate memory for transaction pool from internal memory
    crc_gdma->transaction_pool = heap_caps_calloc(trans_queue_len, sizeof(async_crc_transaction_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(crc_gdma->transaction_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction pool");

    // Create TX channel for CRC calculation with optimized allocation strategy
    gdma_channel_alloc_config_t dma_chan_alloc_cfg = {0};
    ESP_GOTO_ON_ERROR(new_channel_func(&dma_chan_alloc_cfg, &crc_gdma->tx_channel, NULL),
                      err, TAG, "alloc DMA channel failed");
    gdma_reset(crc_gdma->tx_channel);

    // get a free DMA trigger ID for CRC calculation, and connect it to the allocated channel
    gdma_trigger_t m2m_trigger = {
        .bus_id = gdma_bus_id,
    };
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(crc_gdma->tx_channel, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    ESP_GOTO_ON_ERROR(gdma_connect(crc_gdma->tx_channel, m2m_trigger), err, TAG, "connect DMA channel failed");

    gdma_strategy_config_t strategy_cfg = {
        .owner_check = true,
        .auto_update_desc = true,
        .eof_till_data_popped = false,
    };
    gdma_apply_strategy(crc_gdma->tx_channel, &strategy_cfg);

    // Configure DMA transfer
    gdma_transfer_config_t transfer_cfg = {
        .max_data_burst_size = config->dma_burst_size,
        .access_ext_mem = true, // allow to copy data from external memory
    };
    ESP_GOTO_ON_ERROR(gdma_config_transfer(crc_gdma->tx_channel, &transfer_cfg), err, TAG, "config DMA transfer failed");

    // Get buffer alignment required by GDMA channel
    gdma_get_alignment_constraints(crc_gdma->tx_channel, &crc_gdma->tx_int_mem_alignment, &crc_gdma->tx_ext_mem_alignment);

    // Register EOF callback for completion detection
    gdma_tx_event_callbacks_t cbs = {
        .on_trans_eof = async_crc_gdma_eof_callback,
    };
    ESP_GOTO_ON_ERROR(gdma_register_tx_event_callbacks(crc_gdma->tx_channel, &cbs, crc_gdma), err, TAG, "register TX EOF callback failed");

    STAILQ_INIT(&crc_gdma->idle_queue_head);
    STAILQ_INIT(&crc_gdma->ready_queue_head);
    // Pick transactions from pool and insert to idle queue
    for (int i = 0; i < trans_queue_len; i++) {
        STAILQ_INSERT_TAIL(&crc_gdma->idle_queue_head, &crc_gdma->transaction_pool[i], queue_entry);
    }

    // Set function pointers
    crc_gdma->parent.del = async_crc_gdma_del;
    crc_gdma->parent.calc = async_crc_gdma_calc;

    // Return base object
    *crc_hdl = &crc_gdma->parent;
    return ESP_OK;

err:
    if (crc_gdma) {
        async_crc_gdma_destroy_context(crc_gdma);
    }
    return ret;
}

/// @brief Check if the ready queue is empty
/// @note this function is allowed to be called in ISR
static bool is_ready_queue_empty(async_crc_gdma_context_t *crc_gdma)
{
    bool empty;
    portENTER_CRITICAL_SAFE(&crc_gdma->spin_lock);
    empty = STAILQ_EMPTY(&crc_gdma->ready_queue_head);
    portEXIT_CRITICAL_SAFE(&crc_gdma->spin_lock);
    return empty;
}

/// @brief help function to get one transaction from the ready queue
/// @note this function is allowed to be called in ISR
static async_crc_transaction_t *try_pop_trans_from_ready_queue(async_crc_gdma_context_t *crc_gdma)
{
    async_crc_transaction_t *trans = NULL;
    portENTER_CRITICAL_SAFE(&crc_gdma->spin_lock);
    trans = STAILQ_FIRST(&crc_gdma->ready_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&crc_gdma->ready_queue_head, queue_entry);
    }
    portEXIT_CRITICAL_SAFE(&crc_gdma->spin_lock);
    return trans;
}

/// @brief help function to get one transaction from the idle queue
/// @note this function is allowed to be called in ISR
static async_crc_transaction_t *try_pop_trans_from_idle_queue(async_crc_gdma_context_t *crc_gdma)
{
    async_crc_transaction_t *trans = NULL;
    portENTER_CRITICAL_SAFE(&crc_gdma->spin_lock);
    trans = STAILQ_FIRST(&crc_gdma->idle_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&crc_gdma->idle_queue_head, queue_entry);
    }
    portEXIT_CRITICAL_SAFE(&crc_gdma->spin_lock);
    return trans;
}

/// @brief help function to start a pending transaction
/// @note this function is allowed to be called in ISR
static void try_start_pending_transaction(async_crc_gdma_context_t *crc_gdma)
{
    async_crc_fsm_t expected_fsm = CRC_FSM_IDLE;
    async_crc_transaction_t *trans = NULL;

    // Retry loop to handle race condition:
    // If we acquire FSM but find queue empty, another task might have just inserted
    // a transaction. We re-check the queue before giving up.
    while (atomic_compare_exchange_strong(&crc_gdma->fsm, &expected_fsm, CRC_FSM_WAIT)) {
        trans = try_pop_trans_from_ready_queue(crc_gdma);
        if (trans) {
            // Configure CRC calculator with transaction parameters
            // This must be done just before starting DMA to ensure the hardware
            // is not busy with a previous calculation using different params
            gdma_crc_calculator_config_t crc_cfg = {
                .crc_bit_width = trans->params.width,
                .init_value = trans->params.init_value,
                .poly_hex = trans->params.polynomial,
                .reverse_data_mask = trans->params.reverse_input,
            };
            // crc config validation is done in the async_crc_prepare_transaction
            // so no need to check the return value here
            gdma_config_crc_calculator(crc_gdma->tx_channel, &crc_cfg);
            gdma_reset(crc_gdma->tx_channel);

            atomic_store(&crc_gdma->fsm, CRC_FSM_RUN);
            crc_gdma->current_transaction = trans;
            // Start DMA operation for CRC calculation
            gdma_start(crc_gdma->tx_channel, gdma_link_get_head_addr(trans->link_list));
            return;
        }

        // Queue was empty - go back to IDLE
        atomic_store(&crc_gdma->fsm, CRC_FSM_IDLE);

        // Re-check if queue is still empty before giving up
        // If not empty, a concurrent insert happened - retry to pick it up
        if (is_ready_queue_empty(crc_gdma)) {
            // Queue is truly empty, safe to exit
            return;
        }

        // Queue has items now, retry the loop
        expected_fsm = CRC_FSM_IDLE;
    }
}

/// @brief prepare the transaction by mounting buffer and configuring DMA
/// @note This function mounts the user buffer to a DMA link list
static esp_err_t async_crc_prepare_transaction(async_crc_gdma_context_t *crc_gdma, async_crc_transaction_t *trans)
{
    // Validate CRC bit width against GDMA bus type
    // in fact this validation is also done in gdma_config_crc_calculator,
    // but we do it here to catch the error earlier
    uint32_t max_crc_bit_width = (crc_gdma->gdma_bus_id == SOC_GDMA_BUS_AXI) ? GDMA_LL_AXI_MAX_CRC_BIT_WIDTH : GDMA_LL_AHB_MAX_CRC_BIT_WIDTH;
    ESP_RETURN_ON_FALSE(trans->params.width <= max_crc_bit_width, ESP_ERR_INVALID_ARG, TAG, "invalid crc bit width %"PRIu32, trans->params.width);

    // Get buffer alignment based on memory type
    size_t buffer_alignment = esp_ptr_internal(trans->data) ? crc_gdma->tx_int_mem_alignment : crc_gdma->tx_ext_mem_alignment;

    // Verify user buffer satisfies DMA alignment requirements
    ESP_RETURN_ON_FALSE(((uintptr_t)trans->data % buffer_alignment) == 0, ESP_ERR_INVALID_ARG, TAG,
                        "Data buffer not aligned to %zu bytes", buffer_alignment);

    // Calculate number of DMA nodes needed
    size_t num_dma_nodes = esp_dma_calculate_node_count(trans->size, buffer_alignment, CRC_DMA_DESCRIPTOR_BUFFER_MAX_SIZE);

    // Get descriptor alignment based on GDMA bus type
    size_t item_alignment = (crc_gdma->gdma_bus_id == SOC_GDMA_BUS_AXI) ? GDMA_LL_AXI_DESC_ALIGNMENT : GDMA_LL_AHB_DESC_ALIGNMENT;

    // Create DMA link list for the buffer
    gdma_link_list_config_t link_cfg = {
        .item_alignment = item_alignment,
        .num_items = num_dma_nodes,
        .flags = {
            .check_owner = true,
            .items_in_ext_mem = false,
        },
    };
    ESP_RETURN_ON_ERROR(gdma_new_link_list(&link_cfg, &trans->link_list), TAG, "failed to create DMA link list");

    // Mount the user buffer to the DMA link list
    gdma_buffer_mount_config_t buf_mount_config[1] = {
        [0] = {
            .buffer = (void *)trans->data, // DMA only reads from this buffer, so it's safe to cast away const
            .buffer_alignment = buffer_alignment,
            .length = trans->size,
            .flags = {
                .mark_eof = true,
                .mark_final = GDMA_FINAL_LINK_TO_NULL,
            }
        }
    };
    ESP_RETURN_ON_ERROR(gdma_link_mount_buffers(trans->link_list, 0, buf_mount_config, 1, NULL), TAG, "failed to mount buffer to DMA link list");

    // write back the source data if it's behind the cache
    size_t cache_line_size = esp_cache_get_line_size_by_addr(trans->data);
    if (cache_line_size > 0) {
        esp_cache_msync((void*)trans->data, trans->size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }

    return ESP_OK;
}

/// @brief EOF callback for GDMA CRC completion
/// @note This is called in ISR context when DMA transfer completes
static bool async_crc_gdma_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    async_crc_gdma_context_t *crc_gdma = (async_crc_gdma_context_t *)user_data;

    // Handle normal EOF completion
    if (event_data->flags.normal_eof) {
        // Get current transaction that just completed
        // Capture to local var before CAS in case user clears crc_gdma->current_transaction
        async_crc_transaction_t *current_trans = crc_gdma->current_transaction;

        // Get CRC result from DMA peripheral
        uint32_t crc_result;
        gdma_crc_get_result(crc_gdma->tx_channel, &crc_result);

        // Apply output bit reversal if specified
        if (current_trans->params.reverse_output) {
            crc_result = bit_reverse32(crc_result);
            // Mask to appropriate width
            if (current_trans->params.width == 8) {
                crc_result = crc_result >> 24; // For 8-bit CRC, the result is in the highest byte after bit reversal
            } else if (current_trans->params.width == 16) {
                crc_result = crc_result >> 16; // For 16-bit CRC, the result is in the highest 2 bytes after bit reversal
            }
        }

        // Apply final XOR value if specified
        if (current_trans->params.final_xor_value != 0) {
            crc_result ^= current_trans->params.final_xor_value;
        }

        // Switch driver state from RUN to IDLE using FSM transitions
        async_crc_fsm_t expected_fsm = CRC_FSM_RUN;
        if (atomic_compare_exchange_strong(&crc_gdma->fsm, &expected_fsm, CRC_FSM_WAIT)) {
            // Call user callback with successful completion data
            if (current_trans->cb_isr) {
                async_crc_event_data_t event = {.crc_result = crc_result};
                need_yield = current_trans->cb_isr(&crc_gdma->parent, &event, current_trans->cb_args);
            }
            current_trans->cb_isr = NULL;

            portENTER_CRITICAL_ISR(&crc_gdma->spin_lock);
            // Return transaction to idle queue for reuse
            STAILQ_INSERT_TAIL(&crc_gdma->idle_queue_head, current_trans, queue_entry);
            crc_gdma->current_transaction = NULL; // Clear current transaction
            portEXIT_CRITICAL_ISR(&crc_gdma->spin_lock);

            atomic_store(&crc_gdma->fsm, CRC_FSM_IDLE);
        }
    }

    // Try start the next pending transaction
    try_start_pending_transaction(crc_gdma);

    return need_yield;
}

static esp_err_t async_crc_gdma_del(async_crc_context_t *ctx)
{
    async_crc_gdma_context_t *crc_gdma = __containerof(ctx, async_crc_gdma_context_t, parent);

    // Check if driver is busy or has pending requests
    // Uninstall can only proceed when driver is IDLE and no pending transactions
    ESP_RETURN_ON_FALSE(atomic_load(&crc_gdma->fsm) == CRC_FSM_IDLE, ESP_ERR_INVALID_STATE, TAG, "Cannot uninstall: CRC engine is busy");

    // Check if there are pending transactions in the ready queue
    ESP_RETURN_ON_FALSE(is_ready_queue_empty(crc_gdma), ESP_ERR_INVALID_STATE, TAG, "Cannot uninstall: pending CRC requests in queue");

    return async_crc_gdma_destroy_context(crc_gdma);
}

static esp_err_t async_crc_gdma_calc(async_crc_context_t *ctx, const void *data, size_t size,
                                     const async_crc_params_t *params, async_crc_isr_cb_t cb_isr, void *cb_args)
{
    ESP_RETURN_ON_FALSE(ctx && data && size && params, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    async_crc_gdma_context_t *crc_gdma = __containerof(ctx, async_crc_gdma_context_t, parent);

    // pick one transaction node from idle queue
    async_crc_transaction_t *trans = try_pop_trans_from_idle_queue(crc_gdma);
    ESP_RETURN_ON_FALSE(trans, ESP_ERR_INVALID_STATE, TAG, "no free node in the idle queue");

    // clean up the transaction configuration comes from the last one
    if (trans->link_list) {
        gdma_del_link_list(trans->link_list);
        trans->link_list = NULL;
    }

    // Store transaction data
    trans->data = data;
    trans->size = size;
    trans->params = *params;
    trans->cb_isr = cb_isr;
    trans->cb_args = cb_args;

    // Prepare the DMA transfer (create link list and mount buffer)
    esp_err_t ret = async_crc_prepare_transaction(crc_gdma, trans);
    if (ret != ESP_OK) {
        goto err;
    }

    // insert the trans to ready queue
    portENTER_CRITICAL(&crc_gdma->spin_lock);
    STAILQ_INSERT_TAIL(&crc_gdma->ready_queue_head, trans, queue_entry);
    portEXIT_CRITICAL(&crc_gdma->spin_lock);

    // check driver state, if there's no running transaction, start a new one
    try_start_pending_transaction(crc_gdma);

    return ESP_OK;

err:
    // return back the trans to idle queue
    // Note: link_list is not freed here. It will be lazily freed on next reuse
    // or during driver uninstall
    if (trans) {
        portENTER_CRITICAL(&crc_gdma->spin_lock);
        STAILQ_INSERT_TAIL(&crc_gdma->idle_queue_head, trans, queue_entry);
        portEXIT_CRITICAL(&crc_gdma->spin_lock);
    }
    return ret;
}
