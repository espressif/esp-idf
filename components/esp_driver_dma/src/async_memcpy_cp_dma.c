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
#include "soc/interrupts.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_memory_utils.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/critical_section.h"
#include "hal/cp_dma_hal.h"
#include "hal/cp_dma_ll.h"

ESP_LOG_ATTR_TAG(TAG, "async_mcp.cpdma");

#define MCP_DMA_DESCRIPTOR_BUFFER_MAX_SIZE 4095

/// @brief Transaction object for async memcpy
typedef struct async_memcpy_transaction_t {
    gdma_link_list_handle_t tx_link_list; // DMA link list for TX direction
    gdma_link_list_handle_t rx_link_list; // DMA link list for RX direction
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
    cp_dma_hal_context_t hal;      // CPDMA hal
    intr_handle_t intr;            // CPDMA interrupt handle
    portMUX_TYPE spin_lock;        // spin lock to avoid threads and isr from accessing the same resource simultaneously
    _Atomic async_memcpy_fsm_t fsm;// driver state machine, changing state should be atomic
    size_t num_trans_objs;         // number of transaction objects
    async_memcpy_transaction_t *transaction_pool;    // transaction object pool
    async_memcpy_transaction_t *current_transaction; // current transaction object
    STAILQ_HEAD(, async_memcpy_transaction_t) idle_queue_head;  // Head of the idle queue
    STAILQ_HEAD(, async_memcpy_transaction_t) ready_queue_head; // Head of the ready queue
} async_memcpy_cpdma_context_t;

static void mcp_default_isr_handler(void *args);
static esp_err_t mcp_cpdma_del(async_memcpy_context_t *ctx);
static esp_err_t mcp_cpdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args);

static esp_err_t mcp_cpdma_destroy(async_memcpy_cpdma_context_t *mcp_dma)
{
    if (mcp_dma->transaction_pool) {
        for (size_t i = 0; i < mcp_dma->num_trans_objs; i++) {
            async_memcpy_transaction_t* trans = &mcp_dma->transaction_pool[i];
            if (trans->tx_link_list) {
                gdma_del_link_list(trans->tx_link_list);
            }
            if (trans->rx_link_list) {
                gdma_del_link_list(trans->rx_link_list);
            }
        }
        free(mcp_dma->transaction_pool);
    }
    if (mcp_dma->intr) {
        esp_intr_free(mcp_dma->intr);
    }
    if (mcp_dma->hal.dev) { // this is for check if the hal is initialized
        cp_dma_hal_stop(&mcp_dma->hal);
        cp_dma_hal_deinit(&mcp_dma->hal);
    }
    free(mcp_dma);
    return ESP_OK;
}

esp_err_t esp_async_memcpy_install_cpdma(const async_memcpy_config_t *config, async_memcpy_handle_t *mcp)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_cpdma_context_t *mcp_dma = NULL;
    ESP_RETURN_ON_FALSE(config && mcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // allocate memory of driver context from internal memory (because it contains atomic variable)
    mcp_dma = heap_caps_calloc(1, sizeof(async_memcpy_cpdma_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_dma, ESP_ERR_NO_MEM, err, TAG, "no mem for driver context");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_TRANSACTION_QUEUE_LENGTH;
    // allocate memory for transaction pool from internal memory
    mcp_dma->transaction_pool = heap_caps_calloc(trans_queue_len, sizeof(async_memcpy_transaction_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_dma->transaction_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction pool");

    // Init hal context
    cp_dma_hal_config_t hal_config = {};
    cp_dma_hal_init(&mcp_dma->hal, &hal_config);

    ESP_GOTO_ON_ERROR(esp_intr_alloc(ETS_DMA_COPY_INTR_SOURCE, 0, mcp_default_isr_handler, mcp_dma, &mcp_dma->intr),
                      err, TAG, "install isr failed");

    // initialize transaction queue
    STAILQ_INIT(&mcp_dma->idle_queue_head);
    STAILQ_INIT(&mcp_dma->ready_queue_head);
    // pick transactions from the pool and insert to the idle queue
    for (int i = 0; i < trans_queue_len; i++) {
        STAILQ_INSERT_TAIL(&mcp_dma->idle_queue_head, &mcp_dma->transaction_pool[i], idle_queue_entry);
    }

    // initialize other members
    portMUX_INITIALIZE(&mcp_dma->spin_lock);
    atomic_init(&mcp_dma->fsm, MCP_FSM_IDLE);
    mcp_dma->num_trans_objs = trans_queue_len;

    mcp_dma->parent.del = mcp_cpdma_del;
    mcp_dma->parent.memcpy = mcp_cpdma_memcpy;
    // return driver object
    *mcp = &mcp_dma->parent;
    return ESP_OK;

err:
    if (mcp_dma) {
        mcp_cpdma_destroy(mcp_dma);
    }
    return ret;
}

esp_err_t esp_async_memcpy_install(const async_memcpy_config_t *config, async_memcpy_handle_t *asmcp)
__attribute__((alias("esp_async_memcpy_install_cpdma")));

static esp_err_t mcp_cpdma_del(async_memcpy_context_t *ctx)
{
    async_memcpy_cpdma_context_t *mcp_dma = __containerof(ctx, async_memcpy_cpdma_context_t, parent);
    // check if there are pending transactions
    ESP_RETURN_ON_FALSE(STAILQ_EMPTY(&mcp_dma->ready_queue_head), ESP_ERR_INVALID_STATE, TAG, "there are pending transactions");
    // check if the driver is in IDLE state
    ESP_RETURN_ON_FALSE(atomic_load(&mcp_dma->fsm) == MCP_FSM_IDLE, ESP_ERR_INVALID_STATE, TAG, "driver is not in IDLE state");
    return mcp_cpdma_destroy(mcp_dma);
}

/// @brief help function to get one transaction from the ready queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_ready_queue(async_memcpy_cpdma_context_t *mcp_dma)
{
    async_memcpy_transaction_t *trans = NULL;
    esp_os_enter_critical_safe(&mcp_dma->spin_lock);
    trans = STAILQ_FIRST(&mcp_dma->ready_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_dma->ready_queue_head, ready_queue_entry);
    }
    esp_os_exit_critical_safe(&mcp_dma->spin_lock);
    return trans;
}

/// @brief help function to start a pending transaction
/// @note this function is allowed to be called in ISR
static void try_start_pending_transaction(async_memcpy_cpdma_context_t *mcp_dma)
{
    async_memcpy_fsm_t expected_fsm = MCP_FSM_IDLE;
    async_memcpy_transaction_t *trans = NULL;
    if (atomic_compare_exchange_strong(&mcp_dma->fsm, &expected_fsm, MCP_FSM_RUN_WAIT)) {
        trans = try_pop_trans_from_ready_queue(mcp_dma);
        if (trans) {
            atomic_store(&mcp_dma->fsm, MCP_FSM_RUN);
            mcp_dma->current_transaction = trans;
            cp_dma_hal_set_desc_base_addr(&mcp_dma->hal,
                                          gdma_link_get_head_addr(trans->tx_link_list),
                                          gdma_link_get_head_addr(trans->rx_link_list));
            cp_dma_hal_start(&mcp_dma->hal); // enable DMA and interrupt
        } else {
            atomic_store(&mcp_dma->fsm, MCP_FSM_IDLE);
        }
    }
}

/// @brief help function to get one transaction from the idle queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_idle_queue(async_memcpy_cpdma_context_t *mcp_dma)
{
    async_memcpy_transaction_t *trans = NULL;
    esp_os_enter_critical_safe(&mcp_dma->spin_lock);
    trans = STAILQ_FIRST(&mcp_dma->idle_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_dma->idle_queue_head, idle_queue_entry);
    }
    esp_os_exit_critical_safe(&mcp_dma->spin_lock);
    return trans;
}

static esp_err_t mcp_cpdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_cpdma_context_t *mcp_dma = __containerof(ctx, async_memcpy_cpdma_context_t, parent);
    ESP_RETURN_ON_FALSE(esp_ptr_internal(src) && esp_ptr_internal(dst), ESP_ERR_INVALID_ARG, TAG, "CP_DMA can only access SRAM");
    async_memcpy_transaction_t *trans = NULL;
    // pick one transaction node from idle queue
    trans = try_pop_trans_from_idle_queue(mcp_dma);
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

    size_t num_dma_nodes = esp_dma_calculate_node_count(n, 1, MCP_DMA_DESCRIPTOR_BUFFER_MAX_SIZE);

    // allocate gdma TX link
    gdma_link_list_config_t tx_link_cfg = {
        .item_alignment = 4,   // CP_DMA requires 4 bytes alignment for each descriptor
        .num_items = num_dma_nodes,
        .flags = {
            .check_owner = true,
            .items_in_ext_mem = false,
        },
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&tx_link_cfg, &trans->tx_link_list), err, TAG, "failed to create TX link list");
    // mount the source buffer to the TX link list
    gdma_buffer_mount_config_t tx_buf_mount_config[1] = {
        [0] = {
            .buffer = src,
            .buffer_alignment = 1, // CP_DMA doesn't have alignment requirement for internal memory
            .length = n,
            .flags = {
                .mark_eof = true,   // mark the last item as EOF, so the RX channel can also received an EOF list item
                .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
            }
        }
    };
    gdma_link_mount_buffers(trans->tx_link_list, 0, tx_buf_mount_config, 1, NULL);

    // allocate gdma RX link
    gdma_link_list_config_t rx_link_cfg = {
        .item_alignment = 4,   // CP_DMA requires 4 bytes alignment for each descriptor
        .num_items = num_dma_nodes,
        .flags = {
            .check_owner = true,
            .items_in_ext_mem = false,
        },
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&rx_link_cfg, &trans->rx_link_list), err, TAG, "failed to create RX link list");
    // mount the destination buffer to the RX link list
    gdma_buffer_mount_config_t rx_buf_mount_config[1] = {
        [0] = {
            .buffer = dst,
            .buffer_alignment = 1, // CP_DMA doesn't have alignment requirement for internal memory
            .length = n,
            .flags = {
                .mark_eof = false,  // EOF is set by TX side
                .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
            }
        }
    };
    gdma_link_mount_buffers(trans->rx_link_list, 0, rx_buf_mount_config, 1, NULL);

    // save other transaction context
    trans->cb = cb_isr;
    trans->cb_args = cb_args;

    esp_os_enter_critical(&mcp_dma->spin_lock);
    // insert the trans to ready queue
    STAILQ_INSERT_TAIL(&mcp_dma->ready_queue_head, trans, ready_queue_entry);
    esp_os_exit_critical(&mcp_dma->spin_lock);

    // check driver state, if there's no running transaction, start a new one
    try_start_pending_transaction(mcp_dma);

    return ESP_OK;

err:
    if (trans) {
        // return back the trans to idle queue
        esp_os_enter_critical(&mcp_dma->spin_lock);
        STAILQ_INSERT_TAIL(&mcp_dma->idle_queue_head, trans, idle_queue_entry);
        esp_os_exit_critical(&mcp_dma->spin_lock);
    }
    return ret;
}

static void mcp_default_isr_handler(void *args)
{
    bool need_yield = false;
    async_memcpy_cpdma_context_t *mcp_dma = (async_memcpy_cpdma_context_t *)args;
    // get the interrupt status and clear it
    uint32_t status = cp_dma_ll_get_intr_status(mcp_dma->hal.dev);
    cp_dma_ll_clear_intr_status(mcp_dma->hal.dev, status);

    // End-Of-Frame on RX side
    if (status & CP_DMA_LL_EVENT_RX_EOF) {
        async_memcpy_transaction_t *trans = mcp_dma->current_transaction;

        // switch driver state from RUN to IDLE
        async_memcpy_fsm_t expected_fsm = MCP_FSM_RUN;
        if (atomic_compare_exchange_strong(&mcp_dma->fsm, &expected_fsm, MCP_FSM_IDLE_WAIT)) {
            // invoked callback registered by user
            async_memcpy_isr_cb_t cb = trans->cb;
            if (cb) {
                async_memcpy_event_t e = {
                    // No event data for now
                };
                need_yield = cb(&mcp_dma->parent, &e, trans->cb_args);
            }
            trans->cb = NULL;

            esp_os_enter_critical_isr(&mcp_dma->spin_lock);
            // insert the trans object to the idle queue
            STAILQ_INSERT_TAIL(&mcp_dma->idle_queue_head, trans, idle_queue_entry);
            esp_os_exit_critical_isr(&mcp_dma->spin_lock);

            atomic_store(&mcp_dma->fsm, MCP_FSM_IDLE);
        }

        // try start the next pending transaction
        try_start_pending_transaction(mcp_dma);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
