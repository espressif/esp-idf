/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_memory_utils.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"
#include "hal/dma_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "rom/cache.h"

static const char *TAG = "async_mcp.gdma";

#define MCP_NEEDS_INVALIDATE_DST_CACHE  CONFIG_IDF_TARGET_ESP32P4
#define MCP_NEEDS_WRITE_BACK_SRC_CACHE  CONFIG_IDF_TARGET_ESP32P4
#define MCP_NEEDS_WRITE_BACK_DESC_CACHE CONFIG_IDF_TARGET_ESP32P4

#if SOC_AXI_GDMA_SUPPORTED
#define MCP_DMA_DESC_ALIGN 64
typedef dma_descriptor_align8_t mcp_dma_descriptor_t;
#elif SOC_AHB_GDMA_SUPPORTED
#define MCP_DMA_DESC_ALIGN 32
typedef dma_descriptor_align4_t mcp_dma_descriptor_t;
#else
#error "Unsupported GDMA type"
#endif

/// @brief Transaction object for async memcpy
/// @note - GDMA requires the DMA descriptors to be 4 or 8 bytes aligned
/// @note - The DMA descriptor link list is allocated dynamically from DMA-able memory
/// @note - Because of the eof_node, the transaction object should also be allocated from DMA-able memory
typedef struct async_memcpy_transaction_t {
    mcp_dma_descriptor_t eof_node;      // this is the DMA node which act as the EOF descriptor (RX path only)
    mcp_dma_descriptor_t *tx_desc_link; // descriptor link list, the length of the link is determined by the copy buffer size
    mcp_dma_descriptor_t *rx_desc_link; // descriptor link list, the length of the link is determined by the copy buffer size
    intptr_t tx_start_desc_addr; // TX start descriptor address
    intptr_t rx_start_desc_addr; // RX start descriptor address
    intptr_t memcpy_dst_addr;    // memcpy destination address
    size_t memcpy_size;          // memcpy size
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
    size_t sram_trans_align;       // DMA transfer alignment (both in size and address) for SRAM memory
    size_t psram_trans_align;      // DMA transfer alignment (both in size and address) for PSRAM memory
    size_t max_single_dma_buffer;  // max DMA buffer size by a single descriptor
    int gdma_bus_id;               // GDMA bus id (AHB, AXI, etc.)
    gdma_channel_handle_t tx_channel; // GDMA TX channel handle
    gdma_channel_handle_t rx_channel; // GDMA RX channel handle
    portMUX_TYPE spin_lock;           // spin lock to avoid threads and isr from accessing the same resource simultaneously
    _Atomic async_memcpy_fsm_t fsm;   // driver state machine, changing state should be atomic
    async_memcpy_transaction_t *transaction_pool; // transaction object pool
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
    if (mcp_gdma->transaction_pool) {
        free(mcp_gdma->transaction_pool);
    }
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
        esp_err_t (*new_channel)(const gdma_channel_alloc_config_t *, gdma_channel_handle_t *), int gdma_bus_id)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_gdma_context_t *mcp_gdma = NULL;
    ESP_RETURN_ON_FALSE(config && mcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // allocate memory of driver context from internal memory
    mcp_gdma = heap_caps_calloc(1, sizeof(async_memcpy_gdma_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(mcp_gdma, ESP_ERR_NO_MEM, err, TAG, "no mem for driver context");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_TRANSACTION_QUEUE_LENGTH;
    // allocate memory for transaction pool
    mcp_gdma->transaction_pool = heap_caps_aligned_calloc(MCP_DMA_DESC_ALIGN, trans_queue_len, sizeof(async_memcpy_transaction_t),
                                 MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(mcp_gdma->transaction_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction pool");

    // create TX channel and RX channel, they should reside in the same DMA pair
    gdma_channel_alloc_config_t tx_alloc_config = {
        .flags.reserve_sibling = 1,
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_GOTO_ON_ERROR(new_channel(&tx_alloc_config, &mcp_gdma->tx_channel), err, TAG, "failed to create GDMA TX channel");
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = mcp_gdma->tx_channel,
    };
    ESP_GOTO_ON_ERROR(new_channel(&rx_alloc_config, &mcp_gdma->rx_channel), err, TAG, "failed to create GDMA RX channel");

    // initialize GDMA channels
    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    // get a free DMA trigger ID for memory copy
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(mcp_gdma->tx_channel, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    gdma_connect(mcp_gdma->rx_channel, m2m_trigger);
    gdma_connect(mcp_gdma->tx_channel, m2m_trigger);

    gdma_transfer_ability_t transfer_ability = {
        .sram_trans_align = config->sram_trans_align,
        .psram_trans_align = config->psram_trans_align,
    };
    ESP_GOTO_ON_ERROR(gdma_set_transfer_ability(mcp_gdma->tx_channel, &transfer_ability), err, TAG, "set tx trans ability failed");
    ESP_GOTO_ON_ERROR(gdma_set_transfer_ability(mcp_gdma->rx_channel, &transfer_ability), err, TAG, "set rx trans ability failed");

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

    uint32_t psram_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    uint32_t sram_cache_line_size = 0;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    sram_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
#endif

    // if the psram_trans_align is configured to zero, we should fall back to use the data cache line size
    size_t psram_trans_align = MAX(psram_cache_line_size, config->psram_trans_align);
    size_t sram_trans_align = MAX(sram_cache_line_size, config->sram_trans_align);
    size_t trans_align = MAX(sram_trans_align, psram_trans_align);
    mcp_gdma->max_single_dma_buffer = ALIGN_DOWN(DMA_DESCRIPTOR_BUFFER_MAX_SIZE, trans_align);
    mcp_gdma->psram_trans_align = psram_trans_align;
    mcp_gdma->sram_trans_align = sram_trans_align;
    mcp_gdma->parent.del = mcp_gdma_del;
    mcp_gdma->parent.memcpy = mcp_gdma_memcpy;
#if SOC_GDMA_SUPPORT_ETM
    mcp_gdma->parent.new_etm_event = mcp_new_etm_event;
#endif
    // return driver object
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

/// default installation falls back to use the AHB GDMA
esp_err_t esp_async_memcpy_install(const async_memcpy_config_t *config, async_memcpy_handle_t *asmcp)
__attribute__((alias("esp_async_memcpy_install_gdma_ahb")));
#endif // SOC_AHB_GDMA_SUPPORTED

#if SOC_AXI_GDMA_SUPPORTED
esp_err_t esp_async_memcpy_install_gdma_axi(const async_memcpy_config_t *config, async_memcpy_handle_t *mcp)
{
    return esp_async_memcpy_install_gdma_template(config, mcp, gdma_new_axi_channel, SOC_GDMA_BUS_AXI);
}
#endif // SOC_AXI_GDMA_SUPPORTED

static esp_err_t mcp_gdma_del(async_memcpy_context_t *ctx)
{
    async_memcpy_gdma_context_t *mcp_gdma = __containerof(ctx, async_memcpy_gdma_context_t, parent);
    // check if there are pending transactions
    ESP_RETURN_ON_FALSE(STAILQ_EMPTY(&mcp_gdma->ready_queue_head), ESP_ERR_INVALID_STATE, TAG, "there are pending transactions");
    // check if the driver is in IDLE state
    ESP_RETURN_ON_FALSE(atomic_load(&mcp_gdma->fsm) == MCP_FSM_IDLE, ESP_ERR_INVALID_STATE, TAG, "driver is not in IDLE state");
    return mcp_gdma_destroy(mcp_gdma);
}

static void mount_tx_buffer_to_dma(mcp_dma_descriptor_t *desc_array, int num_desc,
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

#if MCP_NEEDS_WRITE_BACK_DESC_CACHE
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)desc_array, sizeof(mcp_dma_descriptor_t) * num_desc);
#endif
}

static void mount_rx_buffer_to_dma(mcp_dma_descriptor_t *desc_array, int num_desc, mcp_dma_descriptor_t *eof_desc,
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

#if MCP_NEEDS_WRITE_BACK_DESC_CACHE
    if (desc_array) {
        Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)desc_array, sizeof(mcp_dma_descriptor_t) * num_desc);
    }
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)eof_desc, sizeof(mcp_dma_descriptor_t));
#endif
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
            gdma_start(mcp_gdma->rx_channel, trans->rx_start_desc_addr);
            gdma_start(mcp_gdma->tx_channel, trans->tx_start_desc_addr);
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

static bool check_buffer_aligned(async_memcpy_gdma_context_t *mcp_gdma, void *src, void *dst, size_t n)
{
    bool valid = true;
    uint32_t align_mask = 0;

    if (esp_ptr_external_ram(dst)) {
        if (mcp_gdma->psram_trans_align) {
            align_mask = mcp_gdma->psram_trans_align - 1;
        }
    } else {
        if (mcp_gdma->sram_trans_align) {
            align_mask = mcp_gdma->sram_trans_align - 1;
        }
    }

    // destination address must be cache line aligned
    valid = valid && (((uint32_t)dst & align_mask) == 0);
    valid = valid && ((n & align_mask) == 0);
    return valid;
}

static esp_err_t mcp_gdma_memcpy(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_gdma_context_t *mcp_gdma = __containerof(ctx, async_memcpy_gdma_context_t, parent);
    // buffer location check
#if SOC_AHB_GDMA_SUPPORTED && !SOC_AHB_GDMA_SUPPORT_PSRAM
    if (mcp_gdma->gdma_bus_id == SOC_GDMA_BUS_AHB) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(src) && esp_ptr_internal(dst), ESP_ERR_INVALID_ARG, TAG, "AHB GDMA can only access SRAM");
    }
#endif // SOC_AHB_GDMA_SUPPORTED && !SOC_AHB_GDMA_SUPPORT_PSRAM
#if SOC_AXI_GDMA_SUPPORTED && !SOC_AXI_GDMA_SUPPORT_PSRAM
    if (mcp_gdma->gdma_bus_id == SOC_GDMA_BUS_AXI) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(src) && esp_ptr_internal(dst), ESP_ERR_INVALID_ARG, TAG, "AXI_DMA can only access SRAM");
    }
#endif // SOC_AXI_GDMA_SUPPORTED && !SOC_AXI_GDMA_SUPPORT_PSRAM
    // alignment check
    ESP_RETURN_ON_FALSE(check_buffer_aligned(mcp_gdma, src, dst, n), ESP_ERR_INVALID_ARG, TAG, "buffer not aligned: %p -> %p, sz=%zu", src, dst, n);

    async_memcpy_transaction_t *trans = NULL;
    // pick one transaction node from idle queue
    trans = try_pop_trans_from_idle_queue(mcp_gdma);
    // check if we get the transaction object successfully
    ESP_RETURN_ON_FALSE(trans, ESP_ERR_INVALID_STATE, TAG, "no free node in the idle queue");

    // calculate how many descriptors we want
    size_t max_single_dma_buffer = mcp_gdma->max_single_dma_buffer;
    uint32_t num_desc_per_path = (n + max_single_dma_buffer - 1) / max_single_dma_buffer;
    // allocate DMA descriptors, descriptors need a strict alignment
    trans->tx_desc_link = heap_caps_aligned_calloc(MCP_DMA_DESC_ALIGN, num_desc_per_path, sizeof(mcp_dma_descriptor_t),
                          MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(trans->tx_desc_link, ESP_ERR_NO_MEM, err, TAG, "no mem for DMA descriptors");
    // don't have to allocate the EOF descriptor, we will use trans->eof_node as the RX EOF descriptor
    if (num_desc_per_path > 1) {
        trans->rx_desc_link = heap_caps_aligned_calloc(MCP_DMA_DESC_ALIGN, num_desc_per_path - 1, sizeof(mcp_dma_descriptor_t),
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

    // if the data is in the cache, write back, then DMA can see the latest data
#if MCP_NEEDS_WRITE_BACK_SRC_CACHE
    int write_back_map = CACHE_MAP_L1_DCACHE;
    if (esp_ptr_external_ram(src)) {
        write_back_map |= CACHE_MAP_L2_CACHE;
    }
    Cache_WriteBack_Addr(write_back_map, (uint32_t)src, n);
#endif

    // save other transaction context
    trans->cb = cb_isr;
    trans->cb_args = cb_args;
    trans->memcpy_size = n;
    trans->memcpy_dst_addr = (intptr_t)dst;
    trans->tx_start_desc_addr = (intptr_t)trans->tx_desc_link;
    trans->rx_start_desc_addr = trans->rx_desc_link ? (intptr_t)trans->rx_desc_link : (intptr_t)&trans->eof_node;

    portENTER_CRITICAL(&mcp_gdma->spin_lock);
    // insert the trans to ready queue
    STAILQ_INSERT_TAIL(&mcp_gdma->ready_queue_head, trans, ready_queue_entry);
    portEXIT_CRITICAL(&mcp_gdma->spin_lock);

    // check driver state, if there's no running transaction, start a new one
    try_start_pending_transaction(mcp_gdma);

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
    mcp_dma_descriptor_t *eof_desc = (mcp_dma_descriptor_t *)event_data->rx_eof_desc_addr;
    // get the transaction object address by the EOF descriptor address
    async_memcpy_transaction_t *trans = __containerof(eof_desc, async_memcpy_transaction_t, eof_node);

    // switch driver state from RUN to IDLE
    async_memcpy_fsm_t expected_fsm = MCP_FSM_RUN;
    if (atomic_compare_exchange_strong(&mcp_gdma->fsm, &expected_fsm, MCP_FSM_IDLE_WAIT)) {
        // if the data is in the cache, invalidate, then CPU can see the latest data
#if MCP_NEEDS_INVALIDATE_DST_CACHE
        int invalidate_map = CACHE_MAP_L1_DCACHE;
        if (esp_ptr_external_ram((const void *)trans->memcpy_dst_addr)) {
            invalidate_map |= CACHE_MAP_L2_CACHE;
        }
        Cache_Invalidate_Addr(invalidate_map, (uint32_t)trans->memcpy_dst_addr, trans->memcpy_size);
#endif

        // invoked callback registered by user
        async_memcpy_isr_cb_t cb = trans->cb;
        if (cb) {
            async_memcpy_event_t e = {
                // No event data for now
            };
            need_yield = cb(&mcp_gdma->parent, &e, trans->cb_args);
        }
        // recycle descriptor memory
        if (trans->tx_desc_link) {
            free(trans->tx_desc_link);
            trans->tx_desc_link = NULL;
        }
        if (trans->rx_desc_link) {
            free(trans->rx_desc_link);
            trans->rx_desc_link = NULL;
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
