/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "driver/ppa.h"
#include "ppa_priv.h"
#include "esp_private/dma2d.h"
#include "hal/ppa_ll.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "soc/dma2d_channel.h"

static const char *TAG = "ppa_fill";

bool ppa_fill_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 1 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_BLEND && trans_on_picked_desc->fill_desc && trans_on_picked_desc->ppa_engine);

    ppa_fill_oper_t *fill_trans_desc = (ppa_fill_oper_t *)trans_on_picked_desc->fill_desc;
    ppa_blend_engine_t *blend_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_blend_engine_t, base);
    ppa_platform_t *platform = blend_engine->base.platform;

    // Reset blending engine
    ppa_ll_blend_reset(platform->hal.dev);

    // Get the required 2D-DMA channel handles
    assert(dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_RX);
    dma2d_channel_handle_t dma2d_rx_chan = dma2d_chans[0].chan;

    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = fill_trans_desc->out.fill_cm,
    };

    // Fill 2D-DMA descriptors
    blend_engine->dma_rx_desc->vb_size = fill_trans_desc->fill_block_h;
    blend_engine->dma_rx_desc->hb_length = fill_trans_desc->fill_block_w;
    blend_engine->dma_rx_desc->err_eof = 0;
    blend_engine->dma_rx_desc->dma2d_en = 1;
    blend_engine->dma_rx_desc->suc_eof = 1;
    blend_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_rx_desc->va_size = fill_trans_desc->out.pic_h;
    blend_engine->dma_rx_desc->ha_length = fill_trans_desc->out.pic_w;
    blend_engine->dma_rx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format);
    blend_engine->dma_rx_desc->y = fill_trans_desc->out.block_offset_y;
    blend_engine->dma_rx_desc->x = fill_trans_desc->out.block_offset_x;
    blend_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_rx_desc->buffer = (void *)fill_trans_desc->out.buffer;
    blend_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)blend_engine->dma_rx_desc, platform->dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // Configure 2D-DMA channels
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_PPA_BLEND,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_RX,
    };
    dma2d_connect(dma2d_rx_chan, &trig_periph);

    dma2d_transfer_ability_t dma_transfer_ability = {
        .data_burst_length = fill_trans_desc->data_burst_length,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)blend_engine->dma_rx_desc);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA Blending engine
    ppa_ll_blend_configure_filling_block(platform->hal.dev, &fill_trans_desc->fill_argb_color, fill_trans_desc->fill_block_w, fill_trans_desc->fill_block_h);
    ppa_ll_blend_set_tx_color_mode(platform->hal.dev, fill_trans_desc->out.fill_cm);

    ppa_ll_blend_start(platform->hal.dev, PPA_LL_BLEND_TRANS_MODE_FILL);

    // No need to yield
    return false;
}

esp_err_t ppa_do_fill(ppa_client_handle_t ppa_client, const ppa_fill_oper_config_t *config)
{
    ESP_RETURN_ON_FALSE(ppa_client && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_client->oper_type == PPA_OPERATION_FILL, ESP_ERR_INVALID_ARG, TAG, "client is not for fill operations");
    ESP_RETURN_ON_FALSE(config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // out_buffer ptr cannot in flash region
    ESP_RETURN_ON_FALSE(esp_ptr_internal(config->out.buffer) || esp_ptr_external_ram(config->out.buffer), ESP_ERR_INVALID_ARG, TAG, "invalid out.buffer addr");
    uint32_t buf_alignment_size = (uint32_t)ppa_client->engine->platform->buf_alignment_size;
    ESP_RETURN_ON_FALSE(((uint32_t)config->out.buffer & (buf_alignment_size - 1)) == 0 && (config->out.buffer_size & (buf_alignment_size - 1)) == 0,
                        ESP_ERR_INVALID_ARG, TAG, "out.buffer addr or out.buffer_size not aligned to cache line size");
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = config->out.fill_cm,
    };
    uint32_t out_pixel_depth = color_hal_pixel_format_get_bit_depth(out_pixel_format);
    uint32_t out_pic_len = config->out.pic_w * config->out.pic_h * out_pixel_depth / 8;
    ESP_RETURN_ON_FALSE(out_pic_len <= config->out.buffer_size, ESP_ERR_INVALID_ARG, TAG, "out.pic_w/h mismatch with out.buffer_size");
    // To reduce complexity, color_mode, fill_block_w/h correctness are checked in their corresponding LL functions

    // Write back and invalidate necessary data (note that the window content is not continuous in the buffer)
    // Write back and invalidate buffer extended window (alignment not necessary on C2M direction, but alignment strict on M2C direction)
    uint32_t out_ext_window = (uint32_t)config->out.buffer + config->out.block_offset_y * config->out.pic_w * out_pixel_depth / 8;
    uint32_t out_ext_window_aligned = PPA_ALIGN_DOWN(out_ext_window, buf_alignment_size);
    uint32_t out_ext_window_len = config->out.pic_w * config->fill_block_h * out_pixel_depth / 8;
    esp_cache_msync((void *)out_ext_window_aligned, PPA_ALIGN_UP(out_ext_window_len + (out_ext_window - out_ext_window_aligned), buf_alignment_size), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);

    esp_err_t ret = ESP_OK;
    ppa_trans_t *trans_elm = NULL;
    if (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0)) {
        dma2d_trans_config_t *dma_trans_desc = trans_elm->trans_desc;

        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = dma_trans_desc->user_config;

        ppa_fill_oper_t *fill_trans_desc = (ppa_fill_oper_t *)trans_on_picked_desc->fill_desc;
        memcpy(fill_trans_desc, config, sizeof(ppa_fill_oper_config_t));
        fill_trans_desc->data_burst_length = ppa_client->data_burst_length;

        trans_on_picked_desc->ppa_engine = ppa_client->engine;
        trans_on_picked_desc->trigger_periph = DMA2D_TRIG_PERIPH_PPA_BLEND;

        dma_trans_desc->tx_channel_num = 0;
        dma_trans_desc->rx_channel_num = 1;
        dma_trans_desc->channel_flags = 0;
        dma_trans_desc->specified_tx_channel_mask = 0;
        dma_trans_desc->specified_rx_channel_mask = 0;

        trans_elm->client = ppa_client;
        trans_elm->user_data = config->user_data;
        xSemaphoreTake(trans_elm->sem, 0); // Ensure no transaction semaphore before transaction starts

        ret = ppa_do_operation(ppa_client, ppa_client->engine, trans_elm, config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(ppa_client, trans_elm);
        }
    } else {
        ret = ESP_FAIL;
        ESP_LOGE(TAG, "exceed maximum pending transactions for the client, consider increase max_pending_trans_num");
    }
    return ret;
}
