/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "soc/dma2d_channel.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/dma2d_ll.h"
#include "esp_private/dma2d.h"
#include "esp_async_fbcpy.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

static const char *TAG = "async_fbcpy";

typedef struct esp_async_fbcpy_context_t {
    dma2d_pool_handle_t client;  // DMA2D client
    dma2d_descriptor_t* tx_desc; // DMA2D TX descriptor
    dma2d_descriptor_t* rx_desc; // DMA2D RX descriptor
    dma2d_trans_t* trans_desc;   // DMA2D transaction descriptor
    size_t dma_desc_size;        // DMA2D descriptor size
    esp_async_fbcpy_event_callback_t memcpy_done_cb; // memory copy done callback
    void *cb_args;                       // callback arguments
} esp_async_fbcpy_context_t;

static esp_err_t async_fbcpy_del_context(esp_async_fbcpy_context_t* ctx)
{
    if (ctx->tx_desc) {
        free(ctx->tx_desc);
    }
    if (ctx->rx_desc) {
        free(ctx->rx_desc);
    }
    if (ctx->trans_desc) {
        free(ctx->trans_desc);
    }
    if (ctx->client) {
        dma2d_release_pool(ctx->client);
    }
    free(ctx);
    return ESP_OK;
}

esp_err_t esp_async_fbcpy_install(const esp_async_fbcpy_config_t *config, esp_async_fbcpy_handle_t *mcp)
{
    esp_err_t ret = ESP_OK;
    esp_async_fbcpy_context_t *ctx = NULL;
    dma2d_trans_t* trans_desc = NULL;
    dma2d_descriptor_t* dma_tx_desc = NULL;
    dma2d_descriptor_t* dma_rx_desc = NULL;
    dma2d_pool_handle_t dma2d_client = NULL;

    ESP_RETURN_ON_FALSE(config && mcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // allocate context memory
    ctx = heap_caps_calloc(1, sizeof(esp_async_fbcpy_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(ctx, ESP_ERR_NO_MEM, err, TAG, "no mem for esp_async_fbcpy_context_t");
    // according to the dma2d design, the transaction descriptor is also saved by the user
    trans_desc = heap_caps_calloc(1, dma2d_get_trans_elm_size(), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(trans_desc, ESP_ERR_NO_MEM, err, TAG, "no mem for trans_desc");
    ctx->trans_desc = trans_desc;
    // allocate memory for DMA descriptor, the descriptor must be allocated from the internal memory, and alignment to the cache line size
    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t alignment = MAX(DMA2D_LL_DESC_ALIGNMENT, data_cache_line_size);
    size_t dma_desc_mem_size = ALIGN_UP(sizeof(dma2d_descriptor_align8_t), alignment);
    dma_tx_desc = heap_caps_aligned_calloc(alignment, 1, dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    dma_rx_desc = heap_caps_aligned_calloc(alignment, 1, dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(dma_tx_desc && dma_rx_desc, ESP_ERR_NO_MEM, err, TAG, "no memory for DMA2D descriptors");
    ctx->tx_desc = dma_tx_desc;
    ctx->rx_desc = dma_rx_desc;
    ctx->dma_desc_size = dma_desc_mem_size;

    // initialize DMA2D client
    dma2d_pool_config_t dma2d_client_config = {}; // all follow default configurations
    ESP_GOTO_ON_ERROR(dma2d_acquire_pool(&dma2d_client_config, &dma2d_client), err, TAG, "create DMA2D client failed");
    ctx->client = dma2d_client;

    *mcp = ctx;
    return ESP_OK;

err:
    if (ctx) {
        async_fbcpy_del_context(ctx);
    }
    return ret;
}

esp_err_t esp_async_fbcpy_uninstall(esp_async_fbcpy_handle_t mcp)
{
    ESP_RETURN_ON_FALSE(mcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return async_fbcpy_del_context(mcp);
}

static void async_memcpy_setup_dma2d_descriptor(esp_async_fbcpy_context_t* mcp_ctx, esp_async_fbcpy_trans_desc_t* transaction)
{
    dma2d_descriptor_t* tx_desc = mcp_ctx->tx_desc;
    dma2d_descriptor_t* rx_desc = mcp_ctx->rx_desc;
    size_t dma_desc_size = mcp_ctx->dma_desc_size;
    uint8_t dma2d_pbyte = dma2d_desc_pixel_format_to_pbyte_value(transaction->pixel_format_unique_id);

    tx_desc->buffer = (void*)transaction->src_buffer;
    tx_desc->next = NULL;
    tx_desc->dma2d_en = 1;
    tx_desc->suc_eof = 1;
    tx_desc->ha_length = transaction->src_buffer_size_x;
    tx_desc->va_size = transaction->src_buffer_size_y;
    tx_desc->hb_length = transaction->copy_size_x;
    tx_desc->vb_size = transaction->copy_size_y;
    tx_desc->x = transaction->src_offset_x;
    tx_desc->y = transaction->src_offset_y;
    tx_desc->pbyte = dma2d_pbyte;
    tx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    tx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;

    rx_desc->buffer = transaction->dst_buffer;
    rx_desc->next = NULL;
    rx_desc->dma2d_en = 1;
    rx_desc->suc_eof = 1;
    rx_desc->ha_length = transaction->dst_buffer_size_x;
    rx_desc->va_size = transaction->dst_buffer_size_y;
    rx_desc->hb_length = transaction->copy_size_x;
    rx_desc->vb_size = transaction->copy_size_y;
    rx_desc->x = transaction->dst_offset_x;
    rx_desc->y = transaction->dst_offset_y;
    rx_desc->pbyte = dma2d_pbyte;
    rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;

    esp_cache_msync(tx_desc, dma_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
    esp_cache_msync(rx_desc, dma_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
}

static bool dma2d_memcpy_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    esp_async_fbcpy_context_t* mcp = (esp_async_fbcpy_context_t*)user_data;

    if (mcp->memcpy_done_cb) {
        need_yield = mcp->memcpy_done_cb(mcp, NULL, mcp->cb_args);
    }

    return need_yield;
}

static bool dma2d_job_picked_cb(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_data)
{
    esp_async_fbcpy_context_t* mcp = (esp_async_fbcpy_context_t*)user_data;
    dma2d_channel_handle_t tx_chan = NULL;
    dma2d_channel_handle_t rx_chan = NULL;
    for (uint32_t i = 0; i < num_chans; i++) {
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
            tx_chan = dma2d_chans[i].chan;
        }
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
            rx_chan = dma2d_chans[i].chan;
        }
    }
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_M2M,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_M2M_TX,
    };
    dma2d_connect(tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_M2M_RX;
    dma2d_connect(rx_chan, &trig_periph);

    dma2d_rx_event_callbacks_t dma_cbs = {
        .on_recv_eof = dma2d_memcpy_done_cb,
    };
    dma2d_register_rx_event_callbacks(rx_chan, &dma_cbs, mcp);

    dma2d_set_desc_addr(tx_chan, (intptr_t)(mcp->tx_desc));
    dma2d_set_desc_addr(rx_chan, (intptr_t)(mcp->rx_desc));

    dma2d_start(tx_chan);
    dma2d_start(rx_chan);

    return false;
}

esp_err_t esp_async_fbcpy(esp_async_fbcpy_handle_t mcp, esp_async_fbcpy_trans_desc_t* transaction, esp_async_fbcpy_event_callback_t memcpy_done_cb, void *cb_args)
{
    ESP_RETURN_ON_FALSE(mcp && transaction, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcp->memcpy_done_cb = memcpy_done_cb;
    mcp->cb_args = cb_args;

    // mount the data to the DMA descriptor
    async_memcpy_setup_dma2d_descriptor(mcp, transaction);

    // submit the DMA2D request
    dma2d_trans_config_t dma2d_trans_conf = {
        .tx_channel_num = 1,
        .rx_channel_num = 1,
        .channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING,
        .on_job_picked = dma2d_job_picked_cb,
        .user_config = mcp,
    };
    ESP_RETURN_ON_ERROR(dma2d_enqueue(mcp->client, &dma2d_trans_conf, mcp->trans_desc), TAG, "DMA2D enqueue failed");
    return ESP_OK;
}
