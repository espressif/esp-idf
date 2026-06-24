/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include <sys/queue.h>
#include <inttypes.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_async_color_convert_priv.h"
#include "soc/dma2d_channel.h"
#include "hal/dma2d_types.h"
#include "hal/dma2d_ll.h"
#include "hal/color_hal.h"

ESP_LOG_ATTR_TAG(TAG, "async_color_dma2d");

typedef struct async_color_convert_dma2d_context_t async_color_convert_dma2d_context_t;
typedef struct async_color_convert_transaction async_color_convert_transaction_t;

struct async_color_convert_transaction {
    // Keep descriptors in dedicated cache-line-sized allocations so cache sync only
    // touches descriptor state, not the surrounding transaction metadata.
    dma2d_descriptor_t *tx_desc;               // TX descriptor used by DMA2D source channel
    dma2d_descriptor_t *rx_desc;               // RX descriptor used by DMA2D destination channel
    dma2d_trans_t *dma2d_trans_placeholder;   // Opaque DMA2D transaction object storage
    dma2d_trans_config_t dma2d_trans_config;  // Per-request DMA2D transaction configuration

    async_color_convert_request_t request;    // Cached user request used to build DMA2D transaction
    dma2d_csc_config_t tx_csc;                // Cached DMA2D CSC configuration resolved in task context
    async_color_convert_isr_cb_t cb_isr;      // User ISR callback for this request
    void *cb_args;                            // User callback argument
    async_color_convert_dma2d_context_t *ctx; // Back pointer to parent context

    STAILQ_ENTRY(async_color_convert_transaction) queue_entry; // Node in idle transaction queue
};

struct async_color_convert_dma2d_context_t {
    async_color_convert_context_t parent;       // Base interface used by common API wrappers
    dma2d_pool_handle_t pool;                   // DMA2D pool handle for enqueue/dequeue scheduling
    size_t dma_burst_size;                      // DMA burst size applied on job picked
    size_t desc_alloc_size;                     // Cache-line-sized DMA2D descriptor allocation size
    portMUX_TYPE spinlock;                      // Protects idle_queue
    uint32_t num_trans_objs;                    // Total number of transaction objects in trans_pool
    _Atomic uint32_t idle_num;                  // Number of currently available transaction objects
    _Atomic bool deleting;                      // Whether uninstall is in progress
    async_color_convert_transaction_t *trans_pool; // Pre-allocated transaction object pool
    STAILQ_HEAD(, async_color_convert_transaction) idle_queue; // Queue of available transaction objects
};

static esp_err_t async_color_convert_dma2d_del(async_color_convert_context_t *ctx);
static esp_err_t async_color_convert_dma2d_convert(async_color_convert_context_t *ctx,
                                                   const async_color_convert_request_t *request,
                                                   async_color_convert_isr_cb_t cb_isr,
                                                   void *cb_args);

static bool try_convert_request_to_dma2d_csc(const async_color_convert_request_t *request,
                                             dma2d_csc_config_t *out_tx_csc)
{
    esp_color_fourcc_t src_fourcc = request->src_color_format;
    esp_color_fourcc_t dst_fourcc = request->dst_color_format;

    if (src_fourcc == dst_fourcc) {
        out_tx_csc->tx_csc_option = DMA2D_CSC_TX_NONE;
        out_tx_csc->pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        out_tx_csc->post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        return true;
    }

    if (src_fourcc == ESP_COLOR_FOURCC_RGB16 && dst_fourcc == ESP_COLOR_FOURCC_BGR24) {
        out_tx_csc->tx_csc_option = DMA2D_CSC_TX_RGB565_TO_RGB888;
        out_tx_csc->pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        out_tx_csc->post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        return true;
    }

    if (src_fourcc == ESP_COLOR_FOURCC_BGR24 && dst_fourcc == ESP_COLOR_FOURCC_RGB16) {
        out_tx_csc->tx_csc_option = DMA2D_CSC_TX_RGB888_TO_RGB565;
        out_tx_csc->pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        out_tx_csc->post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        return true;
    }

    if (src_fourcc == ESP_COLOR_FOURCC_BGR24 && dst_fourcc == ESP_COLOR_FOURCC_UYVY) {
        if (request->color_conv_std == COLOR_CONV_STD_RGB_YUV_BT601) {
            out_tx_csc->tx_csc_option = DMA2D_CSC_TX_RGB888_TO_YUV422_601;
        } else if (request->color_conv_std == COLOR_CONV_STD_RGB_YUV_BT709) {
            out_tx_csc->tx_csc_option = DMA2D_CSC_TX_RGB888_TO_YUV422_709;
        } else {
            return false;
        }
        out_tx_csc->pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        out_tx_csc->post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        return true;
    }

    if (src_fourcc == ESP_COLOR_FOURCC_UYVY && dst_fourcc == ESP_COLOR_FOURCC_BGR24) {
        if (request->color_conv_std == COLOR_CONV_STD_RGB_YUV_BT601) {
            out_tx_csc->tx_csc_option = DMA2D_CSC_TX_YUV422_TO_RGB888_601;
        } else if (request->color_conv_std == COLOR_CONV_STD_RGB_YUV_BT709) {
            out_tx_csc->tx_csc_option = DMA2D_CSC_TX_YUV422_TO_RGB888_709;
        } else {
            return false;
        }
        out_tx_csc->pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        out_tx_csc->post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
        return true;
    }

    return false;
}

static inline bool needs_tx_csc(const dma2d_csc_config_t *tx_csc)
{
    return tx_csc->tx_csc_option != DMA2D_CSC_TX_NONE;
}

static esp_err_t sync_if_cacheable(void *addr, size_t size, int flags)
{
    return esp_cache_get_line_size_by_addr(addr) > 0 ? esp_cache_msync(addr, size, flags) : ESP_OK;
}

static size_t get_picture_size_bytes(uint32_t stride, uint32_t height, uint32_t bit_depth)
{
    return (((size_t)stride * height * bit_depth) + 7) / 8;
}

static esp_err_t validate_request(const async_color_convert_request_t *request)
{
    ESP_RETURN_ON_FALSE(request->src_color_format != 0 && request->dst_color_format != 0,
                        ESP_ERR_INVALID_ARG, TAG, "invalid color format");
    ESP_RETURN_ON_FALSE(request->src_buffer && request->dst_buffer, ESP_ERR_INVALID_ARG, TAG, "invalid buffer");
    ESP_RETURN_ON_FALSE(request->copy_width > 0 && request->copy_height > 0, ESP_ERR_INVALID_ARG, TAG, "invalid copy window");

    uint64_t src_x_end = (uint64_t)request->src_x + request->copy_width;
    uint64_t src_y_end = (uint64_t)request->src_y + request->copy_height;
    uint64_t dst_x_end = (uint64_t)request->dst_x + request->copy_width;
    uint64_t dst_y_end = (uint64_t)request->dst_y + request->copy_height;

    ESP_RETURN_ON_FALSE(src_x_end <= request->src_stride, ESP_ERR_INVALID_ARG, TAG, "source window out of width");
    ESP_RETURN_ON_FALSE(src_y_end <= request->src_height, ESP_ERR_INVALID_ARG, TAG, "source window out of height");
    ESP_RETURN_ON_FALSE(dst_x_end <= request->dst_stride, ESP_ERR_INVALID_ARG, TAG, "destination window out of width");
    ESP_RETURN_ON_FALSE(dst_y_end <= request->dst_height, ESP_ERR_INVALID_ARG, TAG, "destination window out of height");

    ESP_RETURN_ON_FALSE(request->src_stride <= DMA2D_LL_DESC_2D_FIELD_MAX &&
                        request->src_height <= DMA2D_LL_DESC_2D_FIELD_MAX &&
                        request->dst_stride <= DMA2D_LL_DESC_2D_FIELD_MAX &&
                        request->dst_height <= DMA2D_LL_DESC_2D_FIELD_MAX,
                        ESP_ERR_INVALID_ARG, TAG, "dimension exceeds DMA2D descriptor field limit");

    return ESP_OK;
}

static async_color_convert_transaction_t *try_acquire_trans(async_color_convert_dma2d_context_t *ctx)
{
    async_color_convert_transaction_t *trans = NULL;
    portENTER_CRITICAL(&ctx->spinlock);
    if (!atomic_load(&ctx->deleting)) {
        trans = STAILQ_FIRST(&ctx->idle_queue);
        if (trans) {
            STAILQ_REMOVE_HEAD(&ctx->idle_queue, queue_entry);
            atomic_fetch_sub(&ctx->idle_num, 1);
        }
    }
    portEXIT_CRITICAL(&ctx->spinlock);
    return trans;
}

static void recycle_trans(async_color_convert_dma2d_context_t *ctx,
                          async_color_convert_transaction_t *trans)
{
    portENTER_CRITICAL_SAFE(&ctx->spinlock);
    STAILQ_INSERT_TAIL(&ctx->idle_queue, trans, queue_entry);
    atomic_fetch_add(&ctx->idle_num, 1);
    portEXIT_CRITICAL_SAFE(&ctx->spinlock);
}

static bool async_color_convert_done_cb(dma2d_channel_handle_t dma2d_chan,
                                        dma2d_event_data_t *event_data,
                                        void *user_data)
{
    bool need_yield = false;
    async_color_convert_transaction_t *trans = (async_color_convert_transaction_t *)user_data;
    async_color_convert_dma2d_context_t *ctx = trans->ctx;
    (void)dma2d_chan;
    (void)event_data;

    if (trans->cb_isr) {
        async_color_convert_event_data_t edata = {};
        need_yield = trans->cb_isr(&ctx->parent, &edata, trans->cb_args);
    }
    trans->cb_isr = NULL;
    trans->cb_args = NULL;

    recycle_trans(ctx, trans);
    return need_yield;
}

static bool async_color_convert_on_job_picked(uint32_t channel_num,
                                              const dma2d_trans_channel_info_t *dma2d_chans,
                                              void *user_config)
{
    async_color_convert_transaction_t *trans = (async_color_convert_transaction_t *)user_config;

    dma2d_channel_handle_t tx_chan = NULL;
    dma2d_channel_handle_t rx_chan = NULL;

    for (uint32_t i = 0; i < channel_num; i++) {
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
            tx_chan = dma2d_chans[i].chan;
        } else {
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

    async_color_convert_dma2d_context_t *ctx = trans->ctx;
    dma2d_transfer_ability_t transfer_ability = {
        .desc_burst_en = true,
        .data_burst_length = ctx->dma_burst_size,
        .access_ext_mem = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
    dma2d_set_transfer_ability(tx_chan, &transfer_ability);
    dma2d_set_transfer_ability(rx_chan, &transfer_ability);

    if (needs_tx_csc(&trans->tx_csc)) {
        dma2d_configure_color_space_conversion(tx_chan, &trans->tx_csc);
    }

    dma2d_rx_event_callbacks_t cbs = {
        .on_recv_eof = async_color_convert_done_cb,
    };
    dma2d_register_rx_event_callbacks(rx_chan, &cbs, trans);

    dma2d_set_desc_addr(rx_chan, (intptr_t)trans->rx_desc);
    dma2d_set_desc_addr(tx_chan, (intptr_t)trans->tx_desc);

    dma2d_start(rx_chan);
    dma2d_start(tx_chan);
    return false;
}

static void setup_desc(dma2d_descriptor_t *desc,
                       void *buffer,
                       uint32_t pic_w,
                       uint32_t pic_h,
                       uint32_t win_w,
                       uint32_t win_h,
                       uint32_t x,
                       uint32_t y,
                       uint32_t pbyte)
{
    memset(desc, 0, sizeof(*desc));
    desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    desc->suc_eof = 1;
    desc->dma2d_en = 1;
    desc->ha_length = pic_w;
    desc->va_size = pic_h;
    desc->hb_length = win_w;
    desc->vb_size = win_h;
    desc->x = x;
    desc->y = y;
    desc->pbyte = pbyte;
    desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    desc->buffer = buffer;
    desc->next = NULL;
}

static esp_err_t async_color_convert_dma2d_convert(async_color_convert_context_t *ctx,
                                                   const async_color_convert_request_t *request,
                                                   async_color_convert_isr_cb_t cb_isr,
                                                   void *cb_args)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(ctx && request, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_ERROR(validate_request(request), TAG, "invalid request");

    async_color_convert_dma2d_context_t *color_ctx = __containerof(ctx, async_color_convert_dma2d_context_t, parent);
    async_color_convert_transaction_t *trans = try_acquire_trans(color_ctx);
    ESP_RETURN_ON_FALSE(trans, ESP_ERR_INVALID_STATE, TAG, "no free transaction in pool");

    trans->request = *request; // copy request to transaction object
    trans->cb_isr = cb_isr;
    trans->cb_args = cb_args;

    esp_color_fourcc_t src_fourcc = request->src_color_format;
    esp_color_fourcc_t dst_fourcc = request->dst_color_format;
    trans->tx_csc = (dma2d_csc_config_t) {};
    ESP_GOTO_ON_FALSE(try_convert_request_to_dma2d_csc(request, &trans->tx_csc),
                      ESP_ERR_INVALID_ARG, recycle_and_out, TAG, "unsupported color conversion mode");

    if (needs_tx_csc(&trans->tx_csc)) {
        trans->dma2d_trans_config.channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING | DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC;
    } else {
        trans->dma2d_trans_config.channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING;
    }

    setup_desc(trans->tx_desc,
               (void *)request->src_buffer,
               request->src_stride,
               request->src_height,
               request->copy_width,
               request->copy_height,
               request->src_x,
               request->src_y,
               dma2d_desc_pixel_format_to_pbyte_value(src_fourcc));

    setup_desc(trans->rx_desc,
               request->dst_buffer,
               request->dst_stride,
               request->dst_height,
               request->copy_width,
               request->copy_height,
               request->dst_x,
               request->dst_y,
               dma2d_desc_pixel_format_to_pbyte_value(dst_fourcc));

    uint32_t src_bpp = color_hal_pixel_format_fourcc_get_bit_depth(src_fourcc);
    uint32_t dst_bpp = color_hal_pixel_format_fourcc_get_bit_depth(dst_fourcc);

    size_t src_total_size = get_picture_size_bytes(request->src_stride, request->src_height, src_bpp);
    size_t dst_total_size = get_picture_size_bytes(request->dst_stride, request->dst_height, dst_bpp);

    ESP_GOTO_ON_ERROR(sync_if_cacheable((void *)request->src_buffer, src_total_size,
                                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED),
                      recycle_and_out, TAG, "source cache sync failed");

    ESP_GOTO_ON_ERROR(sync_if_cacheable(request->dst_buffer, dst_total_size,
                                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE),
                      recycle_and_out, TAG, "destination cache sync failed");

    ESP_GOTO_ON_ERROR(sync_if_cacheable(trans->tx_desc, color_ctx->desc_alloc_size,
                                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE),
                      recycle_and_out, TAG, "tx descriptor cache sync failed");

    ESP_GOTO_ON_ERROR(sync_if_cacheable(trans->rx_desc, color_ctx->desc_alloc_size,
                                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE),
                      recycle_and_out, TAG, "rx descriptor cache sync failed");

    ESP_GOTO_ON_ERROR(dma2d_enqueue(color_ctx->pool,
                                    &trans->dma2d_trans_config,
                                    trans->dma2d_trans_placeholder),
                      recycle_and_out, TAG, "enqueue dma2d transaction failed");

    return ESP_OK;

recycle_and_out:
    recycle_trans(color_ctx, trans);
    return ret;
}

static esp_err_t async_color_convert_dma2d_destroy(async_color_convert_dma2d_context_t *ctx)
{
    if (ctx->pool) {
        dma2d_release_pool(ctx->pool);
    }
    if (ctx->trans_pool) {
        for (uint32_t i = 0; i < ctx->num_trans_objs; i++) {
            free(ctx->trans_pool[i].tx_desc);
            free(ctx->trans_pool[i].rx_desc);
            free(ctx->trans_pool[i].dma2d_trans_placeholder);
        }
        free(ctx->trans_pool);
    }
    free(ctx);
    return ESP_OK;
}

static esp_err_t async_color_convert_dma2d_del(async_color_convert_context_t *ctx)
{
    async_color_convert_dma2d_context_t *color_ctx = __containerof(ctx, async_color_convert_dma2d_context_t, parent);
    bool can_destroy = false;
    portENTER_CRITICAL(&color_ctx->spinlock);
    atomic_store(&color_ctx->deleting, true);
    can_destroy = (atomic_load(&color_ctx->idle_num) == color_ctx->num_trans_objs);
    if (!can_destroy) {
        atomic_store(&color_ctx->deleting, false);
    }
    portEXIT_CRITICAL(&color_ctx->spinlock);
    ESP_RETURN_ON_FALSE(can_destroy,
                        ESP_ERR_INVALID_STATE, TAG, "pending transactions exist");
    return async_color_convert_dma2d_destroy(color_ctx);
}

esp_err_t esp_async_color_convert_install_dma2d(const async_color_convert_config_t *config,
                                                async_color_convert_handle_t *ret_hdl)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t trans_queue_len = config->backlog ? config->backlog : DEFAULT_COLOR_CONVERT_BACKLOG;

    async_color_convert_dma2d_context_t *ctx = heap_caps_calloc(1, sizeof(async_color_convert_dma2d_context_t),
                                                                MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(ctx, ESP_ERR_NO_MEM, TAG, "no mem for color convert context");

    ctx->trans_pool = heap_caps_calloc(trans_queue_len, sizeof(async_color_convert_transaction_t),
                                       MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(ctx->trans_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction pool");

    esp_cache_get_alignment(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA, &ctx->desc_alloc_size);
    if (ctx->desc_alloc_size < sizeof(dma2d_descriptor_t)) {
        ctx->desc_alloc_size = sizeof(dma2d_descriptor_t);
    }

    ctx->num_trans_objs = trans_queue_len;
    ctx->dma_burst_size = config->dma_burst_size ? config->dma_burst_size : 32;
    portMUX_INITIALIZE(&ctx->spinlock);
    STAILQ_INIT(&ctx->idle_queue);
    atomic_init(&ctx->idle_num, trans_queue_len);
    atomic_init(&ctx->deleting, false);

    dma2d_pool_config_t pool_cfg = {
        .pool_id = 0,
        .intr_priority = config->intr_priority,
    };
    ESP_GOTO_ON_ERROR(dma2d_acquire_pool(&pool_cfg, &ctx->pool), err, TAG, "acquire dma2d pool failed");

    for (uint32_t i = 0; i < trans_queue_len; i++) {
        async_color_convert_transaction_t *trans = &ctx->trans_pool[i];
        trans->ctx = ctx;
        // one DMA descriptor is enough for one color-conversion job since
        // the driver will configure the descriptors in single-block mode and won't split the block into multiple tiles
        trans->tx_desc = heap_caps_aligned_calloc(DMA2D_LL_DESC_ALIGNMENT, 1, ctx->desc_alloc_size,
                                                  MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(trans->tx_desc, ESP_ERR_NO_MEM, err, TAG, "no memory for tx descriptor");
        trans->rx_desc = heap_caps_aligned_calloc(DMA2D_LL_DESC_ALIGNMENT, 1, ctx->desc_alloc_size,
                                                  MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(trans->rx_desc, ESP_ERR_NO_MEM, err, TAG, "no memory for rx descriptor");
        trans->dma2d_trans_placeholder = heap_caps_calloc(1, dma2d_get_trans_elm_size(), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(trans->dma2d_trans_placeholder, ESP_ERR_NO_MEM, err, TAG, "no memory for dma2d transaction placeholder");
        // dma2d_enqueue requires a long-lived transaction config, so we prepare it here with the common part configured.
        // The per-request specific part will be filled in on job picked.
        trans->dma2d_trans_config = (dma2d_trans_config_t) {
            .tx_channel_num = 1,
            .rx_channel_num = 1,
            .channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING,
            .specified_tx_channel_mask = 0,
            .specified_rx_channel_mask = 0,
            .on_job_picked = async_color_convert_on_job_picked,
            .user_config = trans,
        };
        STAILQ_INSERT_TAIL(&ctx->idle_queue, trans, queue_entry);
    }

    ctx->parent.convert = async_color_convert_dma2d_convert;
    ctx->parent.del = async_color_convert_dma2d_del;

    *ret_hdl = &ctx->parent;
    return ESP_OK;

err:
    async_color_convert_dma2d_destroy(ctx);
    return ret;
}
