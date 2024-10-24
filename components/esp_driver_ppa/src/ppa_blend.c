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

static const char *TAG = "ppa_blend";

bool ppa_blend_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 3 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_BLEND && trans_on_picked_desc->blend_desc && trans_on_picked_desc->ppa_engine);

    ppa_blend_oper_t *blend_trans_desc = (ppa_blend_oper_t *)trans_on_picked_desc->blend_desc;
    ppa_blend_engine_t *blend_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_blend_engine_t, base);
    ppa_platform_t *platform = blend_engine->base.platform;

    // Reset blending engine
    ppa_ll_blend_reset(platform->hal.dev);

    // Get the required 2D-DMA channel handles
    dma2d_channel_handle_t dma2d_tx_bg_chan = NULL;
    dma2d_channel_handle_t dma2d_tx_fg_chan = NULL;
    dma2d_channel_handle_t dma2d_rx_chan = NULL;
    for (uint32_t i = 0; i < num_chans; i++) {
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
            if (!dma2d_tx_bg_chan) {
                dma2d_tx_bg_chan = dma2d_chans[i].chan;
            } else {
                dma2d_tx_fg_chan = dma2d_chans[i].chan;
            }
        }
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
            dma2d_rx_chan = dma2d_chans[i].chan;
        }
    }
    assert(dma2d_tx_bg_chan && dma2d_tx_fg_chan && dma2d_rx_chan);

    color_space_pixel_format_t in_bg_pixel_format = {
        .color_type_id = blend_trans_desc->in_bg.blend_cm,
    };
    color_space_pixel_format_t in_fg_pixel_format = {
        .color_type_id = blend_trans_desc->in_fg.blend_cm,
    };
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = blend_trans_desc->out.blend_cm,
    };

    // Fill 2D-DMA descriptors
    blend_engine->dma_tx_bg_desc->vb_size = blend_trans_desc->in_bg.block_h;
    blend_engine->dma_tx_bg_desc->hb_length = blend_trans_desc->in_bg.block_w;
    blend_engine->dma_tx_bg_desc->err_eof = 0;
    blend_engine->dma_tx_bg_desc->dma2d_en = 1;
    blend_engine->dma_tx_bg_desc->suc_eof = 1;
    blend_engine->dma_tx_bg_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_tx_bg_desc->va_size = blend_trans_desc->in_bg.pic_h;
    blend_engine->dma_tx_bg_desc->ha_length = blend_trans_desc->in_bg.pic_w;
    blend_engine->dma_tx_bg_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_bg_pixel_format);
    blend_engine->dma_tx_bg_desc->y = blend_trans_desc->in_bg.block_offset_y;
    blend_engine->dma_tx_bg_desc->x = blend_trans_desc->in_bg.block_offset_x;
    blend_engine->dma_tx_bg_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_tx_bg_desc->buffer = (void *)blend_trans_desc->in_bg.buffer;
    blend_engine->dma_tx_bg_desc->next = NULL;

    blend_engine->dma_tx_fg_desc->vb_size = blend_trans_desc->in_fg.block_h;
    blend_engine->dma_tx_fg_desc->hb_length = blend_trans_desc->in_fg.block_w;
    blend_engine->dma_tx_fg_desc->err_eof = 0;
    blend_engine->dma_tx_fg_desc->dma2d_en = 1;
    blend_engine->dma_tx_fg_desc->suc_eof = 1;
    blend_engine->dma_tx_fg_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_tx_fg_desc->va_size = blend_trans_desc->in_fg.pic_h;
    blend_engine->dma_tx_fg_desc->ha_length = blend_trans_desc->in_fg.pic_w;
    blend_engine->dma_tx_fg_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_fg_pixel_format);
    blend_engine->dma_tx_fg_desc->y = blend_trans_desc->in_fg.block_offset_y;
    blend_engine->dma_tx_fg_desc->x = blend_trans_desc->in_fg.block_offset_x;
    blend_engine->dma_tx_fg_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_tx_fg_desc->buffer = (void *)blend_trans_desc->in_fg.buffer;
    blend_engine->dma_tx_fg_desc->next = NULL;

    blend_engine->dma_rx_desc->vb_size = blend_trans_desc->in_fg.block_h; // in_bg.block_h == in_fg.block_h
    blend_engine->dma_rx_desc->hb_length = blend_trans_desc->in_fg.block_w; // in_bg.block_w == in_fg.block_w
    blend_engine->dma_rx_desc->err_eof = 0;
    blend_engine->dma_rx_desc->dma2d_en = 1;
    blend_engine->dma_rx_desc->suc_eof = 1;
    blend_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_rx_desc->va_size = blend_trans_desc->out.pic_h;
    blend_engine->dma_rx_desc->ha_length = blend_trans_desc->out.pic_w;
    blend_engine->dma_rx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format);
    blend_engine->dma_rx_desc->y = blend_trans_desc->out.block_offset_y;
    blend_engine->dma_rx_desc->x = blend_trans_desc->out.block_offset_x;
    blend_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_rx_desc->buffer = (void *)blend_trans_desc->out.buffer;
    blend_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)blend_engine->dma_tx_bg_desc, platform->dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)blend_engine->dma_tx_fg_desc, platform->dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)blend_engine->dma_rx_desc, platform->dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // Configure 2D-DMA channels
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_PPA_BLEND,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_BG_TX,
    };
    dma2d_connect(dma2d_tx_bg_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_FG_TX;
    dma2d_connect(dma2d_tx_fg_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_RX;
    dma2d_connect(dma2d_rx_chan, &trig_periph);

    dma2d_transfer_ability_t dma_transfer_ability = {
        .data_burst_length = blend_trans_desc->data_burst_length,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
    dma2d_set_transfer_ability(dma2d_tx_bg_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_tx_fg_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    dma2d_set_desc_addr(dma2d_tx_bg_chan, (intptr_t)blend_engine->dma_tx_bg_desc);
    dma2d_set_desc_addr(dma2d_tx_fg_chan, (intptr_t)blend_engine->dma_tx_fg_desc);
    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)blend_engine->dma_rx_desc);
    dma2d_start(dma2d_tx_bg_chan);
    dma2d_start(dma2d_tx_fg_chan);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA Blending engine
    ppa_ll_blend_set_rx_bg_color_mode(platform->hal.dev, blend_trans_desc->in_bg.blend_cm);
    ppa_ll_blend_enable_rx_bg_byte_swap(platform->hal.dev, blend_trans_desc->bg_byte_swap);
    ppa_ll_blend_enable_rx_bg_rgb_swap(platform->hal.dev, blend_trans_desc->bg_rgb_swap);
    ppa_ll_blend_configure_rx_bg_alpha(platform->hal.dev, blend_trans_desc->bg_alpha_update_mode, blend_trans_desc->bg_alpha_value);

    ppa_ll_blend_set_rx_fg_color_mode(platform->hal.dev, blend_trans_desc->in_fg.blend_cm);
    if (COLOR_SPACE_TYPE((uint32_t)blend_trans_desc->in_fg.blend_cm) == COLOR_SPACE_ALPHA) {
        ppa_ll_blend_set_rx_fg_fix_rgb(platform->hal.dev, &blend_trans_desc->fg_fix_rgb_val);
    }
    ppa_ll_blend_enable_rx_fg_byte_swap(platform->hal.dev, blend_trans_desc->fg_byte_swap);
    ppa_ll_blend_enable_rx_fg_rgb_swap(platform->hal.dev, blend_trans_desc->fg_rgb_swap);
    ppa_ll_blend_configure_rx_fg_alpha(platform->hal.dev, blend_trans_desc->fg_alpha_update_mode, blend_trans_desc->fg_alpha_value);

    ppa_ll_blend_set_tx_color_mode(platform->hal.dev, blend_trans_desc->out.blend_cm);

    // Color keying
    color_pixel_rgb888_data_t rgb888_min = {.b = 0x00, .g = 0x00, .r = 0x00};
    color_pixel_rgb888_data_t rgb888_max = {.b = 0xFF, .g = 0xFF, .r = 0xFF};
    ppa_ll_blend_configure_rx_bg_ck_range(platform->hal.dev,
                                          blend_trans_desc->bg_ck_en ? &blend_trans_desc->bg_ck_rgb_low_thres : &rgb888_max,
                                          blend_trans_desc->bg_ck_en ? &blend_trans_desc->bg_ck_rgb_high_thres : &rgb888_min);
    ppa_ll_blend_configure_rx_fg_ck_range(platform->hal.dev,
                                          blend_trans_desc->fg_ck_en ? &blend_trans_desc->fg_ck_rgb_low_thres : &rgb888_max,
                                          blend_trans_desc->fg_ck_en ? &blend_trans_desc->fg_ck_rgb_high_thres : &rgb888_min);
    ppa_ll_blend_set_ck_default_rgb(platform->hal.dev, (blend_trans_desc->bg_ck_en && blend_trans_desc->fg_ck_en) ? &blend_trans_desc->ck_rgb_default_val : &rgb888_min);
    ppa_ll_blend_enable_ck_fg_bg_reverse(platform->hal.dev, blend_trans_desc->ck_reverse_bg2fg);

    ppa_ll_blend_start(platform->hal.dev, PPA_LL_BLEND_TRANS_MODE_BLEND);

    // No need to yield
    return false;
}

esp_err_t ppa_do_blend(ppa_client_handle_t ppa_client, const ppa_blend_oper_config_t *config)
{
    ESP_RETURN_ON_FALSE(ppa_client && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_client->oper_type == PPA_OPERATION_BLEND, ESP_ERR_INVALID_ARG, TAG, "client is not for blend operations");
    ESP_RETURN_ON_FALSE(config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // in_buffer could be anywhere (ram, flash, psram), out_buffer ptr cannot in flash region
    ESP_RETURN_ON_FALSE(esp_ptr_internal(config->out.buffer) || esp_ptr_external_ram(config->out.buffer), ESP_ERR_INVALID_ARG, TAG, "invalid out.buffer addr");
    uint32_t buf_alignment_size = (uint32_t)ppa_client->engine->platform->buf_alignment_size;
    ESP_RETURN_ON_FALSE(((uint32_t)config->out.buffer & (buf_alignment_size - 1)) == 0 && (config->out.buffer_size & (buf_alignment_size - 1)) == 0,
                        ESP_ERR_INVALID_ARG, TAG, "out.buffer addr or out.buffer_size not aligned to cache line size");
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = config->out.blend_cm,
    };
    uint32_t out_pixel_depth = color_hal_pixel_format_get_bit_depth(out_pixel_format); // bits
    uint32_t out_pic_len = config->out.pic_w * config->out.pic_h * out_pixel_depth / 8;
    ESP_RETURN_ON_FALSE(out_pic_len <= config->out.buffer_size, ESP_ERR_INVALID_ARG, TAG, "out.pic_w/h mismatch with out.buffer_size");
    ESP_RETURN_ON_FALSE(config->in_bg.block_w == config->in_fg.block_w && config->in_bg.block_h == config->in_fg.block_h,
                        ESP_ERR_INVALID_ARG, TAG, "in_bg.block_w/h must be equal to in_fg.block_w/h");
    if (config->bg_byte_swap) {
        PPA_CHECK_CM_SUPPORT_BYTE_SWAP("in_bg.blend", (uint32_t)config->in_bg.blend_cm);
    }
    if (config->bg_rgb_swap) {
        PPA_CHECK_CM_SUPPORT_RGB_SWAP("in_bg.blend", (uint32_t)config->in_bg.blend_cm);
    }
    if (config->fg_byte_swap) {
        PPA_CHECK_CM_SUPPORT_BYTE_SWAP("in_fg.blend", (uint32_t)config->in_fg.blend_cm);
    }
    if (config->fg_rgb_swap) {
        PPA_CHECK_CM_SUPPORT_RGB_SWAP("in_fg.blend", (uint32_t)config->in_fg.blend_cm);
    }
    uint32_t new_bg_alpha_value = 0;
    if (config->bg_alpha_update_mode == PPA_ALPHA_FIX_VALUE) {
        ESP_RETURN_ON_FALSE(config->bg_alpha_fix_val <= 0xFF, ESP_ERR_INVALID_ARG, TAG, "invalid bg_alpha_fix_val");
        new_bg_alpha_value = config->bg_alpha_fix_val;
    } else if (config->bg_alpha_update_mode == PPA_ALPHA_SCALE) {
        ESP_RETURN_ON_FALSE(config->bg_alpha_scale_ratio > 0 && config->bg_alpha_scale_ratio < 1, ESP_ERR_INVALID_ARG, TAG, "invalid bg_alpha_scale_ratio");
        new_bg_alpha_value = (uint32_t)(config->bg_alpha_scale_ratio * 256);
    }
    uint32_t new_fg_alpha_value = 0;
    if (config->fg_alpha_update_mode == PPA_ALPHA_FIX_VALUE) {
        ESP_RETURN_ON_FALSE(config->fg_alpha_fix_val <= 0xFF, ESP_ERR_INVALID_ARG, TAG, "invalid fg_alpha_fix_val");
        new_fg_alpha_value = config->fg_alpha_fix_val;
    } else if (config->fg_alpha_update_mode == PPA_ALPHA_SCALE) {
        ESP_RETURN_ON_FALSE(config->fg_alpha_scale_ratio > 0 && config->fg_alpha_scale_ratio < 1, ESP_ERR_INVALID_ARG, TAG, "invalid fg_alpha_scale_ratio");
        new_fg_alpha_value = (uint32_t)(config->fg_alpha_scale_ratio * 256);
    }
    // if (config->in_bg.blend_cm == PPA_BLEND_COLOR_MODE_L4) {
    //     ESP_RETURN_ON_FALSE(config->in_bg.block_w % 2 == 0 && config->in_bg.block_offset_x % 2 == 0,
    //                         ESP_ERR_INVALID_ARG, TAG, "in_bg.block_w and in_bg.block_offset_x must be even");
    // }
    if (config->in_fg.blend_cm == PPA_BLEND_COLOR_MODE_A4) { // || config->in_fg.blend_cm == PPA_BLEND_COLOR_MODE_L4
        ESP_RETURN_ON_FALSE(config->in_fg.block_w % 2 == 0 && config->in_fg.block_offset_x % 2 == 0,
                            ESP_ERR_INVALID_ARG, TAG, "in_fg.block_w and in_fg.block_offset_x must be even");
    }
    // To reduce complexity, color_mode, alpha_update_mode correctness are checked in their corresponding LL functions

    // Write back and invalidate necessary data (note that the window content is not continuous in the buffer)
    // Write back in_bg_buffer, in_fg_buffer extended windows (alignment not necessary on C2M direction)
    color_space_pixel_format_t in_bg_pixel_format = {
        .color_type_id = config->in_bg.blend_cm,
    };
    uint32_t in_bg_pixel_depth = color_hal_pixel_format_get_bit_depth(in_bg_pixel_format); // bits
    // Usually C2M can let the msync do alignment internally, however, it only do L1-cacheline-size alignment for L1->L2, and then L2-cacheline-size alignment for L2->mem
    // While M2C direction manual alignment is L2-cacheline-size alignment for mem->L2->L1
    // Mismatching writeback and invalidate data size could cause synchronization error if in_bg/fg_buffer and out_buffer are the same one
    uint32_t in_bg_ext_window = (uint32_t)config->in_bg.buffer + config->in_bg.block_offset_y * config->in_bg.pic_w * in_bg_pixel_depth / 8;
    uint32_t in_bg_ext_window_aligned = PPA_ALIGN_DOWN(in_bg_ext_window, buf_alignment_size);
    uint32_t in_bg_ext_window_len = config->in_bg.pic_w * config->in_bg.block_h * in_bg_pixel_depth / 8;
    esp_cache_msync((void *)in_bg_ext_window_aligned, PPA_ALIGN_UP(in_bg_ext_window_len + (in_bg_ext_window - in_bg_ext_window_aligned), buf_alignment_size), ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    color_space_pixel_format_t in_fg_pixel_format = {
        .color_type_id = config->in_fg.blend_cm,
    };
    uint32_t in_fg_pixel_depth = color_hal_pixel_format_get_bit_depth(in_fg_pixel_format); // bits
    uint32_t in_fg_ext_window = (uint32_t)config->in_fg.buffer + config->in_fg.block_offset_y * config->in_fg.pic_w * in_fg_pixel_depth / 8;
    // Same for fg_buffer msync, do manual alignment
    uint32_t in_fg_ext_window_aligned = PPA_ALIGN_DOWN(in_fg_ext_window, buf_alignment_size);
    uint32_t in_fg_ext_window_len = config->in_fg.pic_w * config->in_fg.block_h * in_fg_pixel_depth / 8;
    esp_cache_msync((void *)in_fg_ext_window_aligned, PPA_ALIGN_UP(in_fg_ext_window_len + (in_fg_ext_window - in_fg_ext_window_aligned), buf_alignment_size), ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    // Invalidate out_buffer extended window (alignment strict on M2C direction)
    uint32_t out_ext_window = (uint32_t)config->out.buffer + config->out.block_offset_y * config->out.pic_w * out_pixel_depth / 8;
    uint32_t out_ext_window_aligned = PPA_ALIGN_DOWN(out_ext_window, buf_alignment_size);
    uint32_t out_ext_window_len = config->out.pic_w * config->in_bg.block_h * out_pixel_depth / 8;
    esp_cache_msync((void *)out_ext_window_aligned, PPA_ALIGN_UP(out_ext_window_len + (out_ext_window - out_ext_window_aligned), buf_alignment_size), ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    esp_err_t ret = ESP_OK;
    ppa_trans_t *trans_elm = NULL;
    if (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0)) {
        dma2d_trans_config_t *dma_trans_desc = trans_elm->trans_desc;

        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = dma_trans_desc->user_config;

        ppa_blend_oper_t *blend_trans_desc = (ppa_blend_oper_t *)trans_on_picked_desc->blend_desc;
        memcpy(blend_trans_desc, config, sizeof(ppa_blend_oper_config_t));
        blend_trans_desc->bg_alpha_value = new_bg_alpha_value;
        blend_trans_desc->fg_alpha_value = new_fg_alpha_value;
        blend_trans_desc->data_burst_length = ppa_client->data_burst_length;

        trans_on_picked_desc->ppa_engine = ppa_client->engine;
        trans_on_picked_desc->trigger_periph = DMA2D_TRIG_PERIPH_PPA_BLEND;

        dma_trans_desc->tx_channel_num = 2;
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
