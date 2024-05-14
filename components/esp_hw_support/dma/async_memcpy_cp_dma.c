/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/interrupts.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_memory_utils.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"
#include "hal/cp_dma_hal.h"
#include "hal/cp_dma_ll.h"
#include "hal/dma_types.h"

static const char *TAG = "async_mcp.cpdma";

/// @brief Transaction object for async memcpy
/// @note - the DMA descriptors to be 4-byte aligned
/// @note - The DMA descriptor link list is allocated dynamically from DMA-able memory
/// @note - Because of the eof_node, the transaction object should also be allocated from DMA-able memory
typedef struct async_memcpy_transaction_t {
    dma_descriptor_align4_t eof_node;      // this is the DMA node which act as the EOF descriptor (RX path only)
    dma_descriptor_align4_t *tx_desc_link; // descriptor link list, the length of the link is determined by the copy buffer size
    dma_descriptor_align4_t *rx_desc_link; // descriptor link list, the length of the link is determined by the copy buffer size
    intptr_t tx_start_desc_addr; // TX start descriptor address
    intptr_t rx_start_desc_addr; // RX start descriptor address
    async_memcpy_isr_cb_t cb;    // user callback
    void *cb_args;               // user callback args
    STAILQ_ENTRY(async_memcpy_transaction_t) idle_queue_entry;  // Entry for the idle queue
    STAILQ_ENTRY(async_memcpy_transaction_t) ready_queue_entry; // Entry for the ready queue
} async_memcpy_transaction_t;

/// @brief Context of async memcpy driver
/// @note - It saves two queues, one for idle transaction objects, one for ready transaction objects
/// @note - Transaction objects are allocated from DMA-able memory
/// @note - Number of transaction objects are determined by the backlog parameter
typedef struct {
    async_memcpy_context_t parent; // Parent IO interface
    size_t max_single_dma_buffer;  // max DMA buffer size by a single descriptor
    cp_dma_hal_context_t hal;      // CPDMA hal
    intr_handle_t intr;            // CPDMA interrupt handle
    portMUX_TYPE spin_lock;           // spin lock to avoid threads and isr from accessing the same resource simultaneously
    _Atomic async_memcpy_fsm_t fsm;   // driver state machine, changing state should be atomic
    async_memcpy_transaction_t *transaction_pool; // transaction object pool
    STAILQ_HEAD(, async_memcpy_transaction_t) idle_queue_head;  // Head of the idle queue
    STAILQ_HEAD(, async_memcpy_transaction_t) ready_queue_head; // Head of the ready queue
} async_memcpy_cpdma_context_t;

static void mcp_default_isr_handler(void *args);
static esp_err_t mcp_cpdma_del(async_memcpy_context_t *ctx);
static esp_err_t mcp_cpdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args);

static esp_err_t mcp_cpdma_destroy(async_memcpy_cpdma_context_t *mcp_dma)
{
    if (mcp_dma->transaction_pool) {
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
    // allocate memory of driver context from internal memory
    mcp_dma = heap_caps_calloc(1, sizeof(async_memcpy_cpdma_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_dma, ESP_ERR_NO_MEM, err, TAG, "no mem for driver context");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_TRANSACTION_QUEUE_LENGTH;
    // allocate memory for transaction pool, aligned to 4 because the trans->eof_node requires that alignment
    mcp_dma->transaction_pool = heap_caps_aligned_calloc(4, trans_queue_len, sizeof(async_memcpy_transaction_t),
                                                         MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
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
    size_t trans_align = config->dma_burst_size;
    mcp_dma->max_single_dma_buffer = trans_align ? ALIGN_DOWN(DMA_DESCRIPTOR_BUFFER_MAX_SIZE, trans_align) : DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
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

static void mount_tx_buffer_to_dma(dma_descriptor_align4_t *desc_array, int num_desc,
                                   uint8_t *buf, size_t buf_sz, size_t max_single_dma_buffer)
{
    uint32_t prepared_length = 0;
    size_t len = buf_sz;
    for (int i = 0; i < num_desc - 1; i++) {
        desc_array[i].buffer = &buf[prepared_length];
        desc_array[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc_array[i].dw0.suc_eof = 0;
        desc_array[i].dw0.size = max_single_dma_buffer;
        desc_array[i].dw0.length = max_single_dma_buffer;
        desc_array[i].next = &desc_array[i + 1];
        prepared_length += max_single_dma_buffer;
        len -= max_single_dma_buffer;
    }
    // take special care to the EOF descriptor
    desc_array[num_desc - 1].buffer = &buf[prepared_length];
    desc_array[num_desc - 1].next = NULL;
    desc_array[num_desc - 1].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    desc_array[num_desc - 1].dw0.suc_eof = 1;
    desc_array[num_desc - 1].dw0.size = len;
    desc_array[num_desc - 1].dw0.length = len;
}

static void mount_rx_buffer_to_dma(dma_descriptor_align4_t *desc_array, int num_desc, dma_descriptor_align4_t *eof_desc,
                                   uint8_t *buf, size_t buf_sz, size_t max_single_dma_buffer)
{
    uint32_t prepared_length = 0;
    size_t len = buf_sz;
    if (desc_array) {
        assert(num_desc > 0);
        for (int i = 0; i < num_desc; i++) {
            desc_array[i].buffer = &buf[prepared_length];
            desc_array[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
            desc_array[i].dw0.size = max_single_dma_buffer;
            desc_array[i].dw0.length = max_single_dma_buffer;
            desc_array[i].next = &desc_array[i + 1];
            prepared_length += max_single_dma_buffer;
            len -= max_single_dma_buffer;
        }
        desc_array[num_desc - 1].next = eof_desc;
    }
    eof_desc->buffer = &buf[prepared_length];
    eof_desc->next = NULL;
    eof_desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    eof_desc->dw0.size = len;
    eof_desc->dw0.length = len;
}

/// @brief help function to get one transaction from the ready queue
/// @note this function is allowed to be called in ISR
static async_memcpy_transaction_t *try_pop_trans_from_ready_queue(async_memcpy_cpdma_context_t *mcp_dma)
{
    async_memcpy_transaction_t *trans = NULL;
    portENTER_CRITICAL_SAFE(&mcp_dma->spin_lock);
    trans = STAILQ_FIRST(&mcp_dma->ready_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_dma->ready_queue_head, ready_queue_entry);
    }
    portEXIT_CRITICAL_SAFE(&mcp_dma->spin_lock);
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
            cp_dma_hal_set_desc_base_addr(&mcp_dma->hal, trans->tx_start_desc_addr, trans->rx_start_desc_addr);
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
    portENTER_CRITICAL_SAFE(&mcp_dma->spin_lock);
    trans = STAILQ_FIRST(&mcp_dma->idle_queue_head);
    if (trans) {
        STAILQ_REMOVE_HEAD(&mcp_dma->idle_queue_head, idle_queue_entry);
    }
    portEXIT_CRITICAL_SAFE(&mcp_dma->spin_lock);
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

    // calculate how many descriptors we want
    size_t max_single_dma_buffer = mcp_dma->max_single_dma_buffer;
    uint32_t num_desc_per_path = (n + max_single_dma_buffer - 1) / max_single_dma_buffer;
    // allocate DMA descriptors, descriptors need a strict alignment
    trans->tx_desc_link = heap_caps_aligned_calloc(4, num_desc_per_path, sizeof(dma_descriptor_align4_t),
                                                   MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(trans->tx_desc_link, ESP_ERR_NO_MEM, err, TAG, "no mem for DMA descriptors");
    // don't have to allocate the EOF descriptor, we will use trans->eof_node as the RX EOF descriptor
    if (num_desc_per_path > 1) {
        trans->rx_desc_link = heap_caps_aligned_calloc(4, num_desc_per_path - 1, sizeof(dma_descriptor_align4_t),
                                                       MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        ESP_GOTO_ON_FALSE(trans->rx_desc_link, ESP_ERR_NO_MEM, err, TAG, "no mem for DMA descriptors");
    } else {
        // small copy buffer, use the trans->eof_node is sufficient
        trans->rx_desc_link = NULL;
    }

    // (preload) mount src data to the TX descriptor
    mount_tx_buffer_to_dma(trans->tx_desc_link, num_desc_per_path, src, n, max_single_dma_buffer);
    // (preload) mount dst data to the RX descriptor
    mount_rx_buffer_to_dma(trans->rx_desc_link, num_desc_per_path - 1, &trans->eof_node, dst, n, max_single_dma_buffer);

    // save other transaction context
    trans->cb = cb_isr;
    trans->cb_args = cb_args;
    trans->tx_start_desc_addr = (intptr_t)trans->tx_desc_link;
    trans->rx_start_desc_addr = trans->rx_desc_link ? (intptr_t)trans->rx_desc_link : (intptr_t)&trans->eof_node;

    portENTER_CRITICAL(&mcp_dma->spin_lock);
    // insert the trans to ready queue
    STAILQ_INSERT_TAIL(&mcp_dma->ready_queue_head, trans, ready_queue_entry);
    portEXIT_CRITICAL(&mcp_dma->spin_lock);

    // check driver state, if there's no running transaction, start a new one
    try_start_pending_transaction(mcp_dma);

    return ESP_OK;

err:
    if (trans) {
        if (trans->tx_desc_link) {
            free(trans->tx_desc_link);
            trans->tx_desc_link = NULL;
        }
        if (trans->rx_desc_link) {
            free(trans->rx_desc_link);
            trans->rx_desc_link = NULL;
        }
        // return back the trans to idle queue
        portENTER_CRITICAL(&mcp_dma->spin_lock);
        STAILQ_INSERT_TAIL(&mcp_dma->idle_queue_head, trans, idle_queue_entry);
        portEXIT_CRITICAL(&mcp_dma->spin_lock);
    }
    return ret;
}

static void mcp_default_isr_handler(void *args)
{
    bool need_yield = false;
    async_memcpy_cpdma_context_t *mcp_dma = (async_memcpy_cpdma_context_t *)args;
    // get the interrupt status and clear it
    uint32_t status = cp_dma_hal_get_intr_status(&mcp_dma->hal);
    cp_dma_hal_clear_intr_status(&mcp_dma->hal, status);

    // End-Of-Frame on RX side
    if (status & CP_DMA_LL_EVENT_RX_EOF) {
        dma_descriptor_align4_t *eof_desc = (dma_descriptor_align4_t *)cp_dma_ll_get_rx_eof_descriptor_address(mcp_dma->hal.dev);
        // get the transaction object address by the EOF descriptor address
        async_memcpy_transaction_t *trans = __containerof(eof_desc, async_memcpy_transaction_t, eof_node);

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
            // recycle descriptor memory
            free(trans->tx_desc_link);
            free(trans->rx_desc_link);
            trans->tx_desc_link = NULL;
            trans->rx_desc_link = NULL;
            trans->cb = NULL;

            portENTER_CRITICAL_ISR(&mcp_dma->spin_lock);
            // insert the trans object to the idle queue
            STAILQ_INSERT_TAIL(&mcp_dma->idle_queue_head, trans, idle_queue_entry);
            portEXIT_CRITICAL_ISR(&mcp_dma->spin_lock);

            atomic_store(&mcp_dma->fsm, MCP_FSM_IDLE);
        }

        // try start the next pending transaction
        try_start_pending_transaction(mcp_dma);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
