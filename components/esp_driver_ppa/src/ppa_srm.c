/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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

static const char *TAG = "ppa_srm";

bool ppa_srm_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 2 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_SRM && trans_on_picked_desc->srm_desc && trans_on_picked_desc->ppa_engine);

    ppa_srm_oper_t *srm_trans_desc = (ppa_srm_oper_t *)trans_on_picked_desc->srm_desc;
    ppa_srm_engine_t *srm_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_srm_engine_t, base);
    ppa_platform_t *platform = srm_engine->base.platform;

    // Reset SRM engine
    ppa_ll_srm_reset(platform->hal.dev);

    // Get the required 2D-DMA channel handles
    dma2d_channel_handle_t dma2d_tx_chan = NULL;
    dma2d_channel_handle_t dma2d_rx_chan = NULL;
    for (uint32_t i = 0; i < num_chans; i++) {
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
            dma2d_tx_chan = dma2d_chans[i].chan;
        }
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
            dma2d_rx_chan = dma2d_chans[i].chan;
        }
    }
    assert(dma2d_tx_chan && dma2d_rx_chan);

    color_space_pixel_format_t in_pixel_format = {
        .color_type_id = srm_trans_desc->in.srm_cm,
    };

    // Fill 2D-DMA descriptors
    srm_engine->dma_tx_desc->vb_size = srm_trans_desc->in.block_h;
    srm_engine->dma_tx_desc->hb_length = srm_trans_desc->in.block_w;
    srm_engine->dma_tx_desc->err_eof = 0;
    srm_engine->dma_tx_desc->dma2d_en = 1;
    srm_engine->dma_tx_desc->suc_eof = 1;
    srm_engine->dma_tx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    srm_engine->dma_tx_desc->va_size = srm_trans_desc->in.pic_h;
    srm_engine->dma_tx_desc->ha_length = srm_trans_desc->in.pic_w;
    srm_engine->dma_tx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_pixel_format);
    srm_engine->dma_tx_desc->y = srm_trans_desc->in.block_offset_y;
    srm_engine->dma_tx_desc->x = srm_trans_desc->in.block_offset_x;
    srm_engine->dma_tx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    srm_engine->dma_tx_desc->buffer = (void *)srm_trans_desc->in.buffer;
    srm_engine->dma_tx_desc->next = NULL;

    // vb_size, hb_length can be any value (auto writeback)
    // However, if vb_size/hb_length is 0, it triggers 2D-DMA DESC_ERROR interrupt, and dma2d driver will automatically ends the transaction
    // Moreover, for YUV420, hb/vb have to be even
    // Therefore, we set them to 2
    srm_engine->dma_rx_desc->vb_size = 2;
    srm_engine->dma_rx_desc->hb_length = 2;
    srm_engine->dma_rx_desc->err_eof = 0;
    srm_engine->dma_rx_desc->dma2d_en = 1;
    srm_engine->dma_rx_desc->suc_eof = 1;
    srm_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    srm_engine->dma_rx_desc->va_size = srm_trans_desc->out.pic_h;
    srm_engine->dma_rx_desc->ha_length = srm_trans_desc->out.pic_w;
    // pbyte can be any value
    srm_engine->dma_rx_desc->y = srm_trans_desc->out.block_offset_y;
    srm_engine->dma_rx_desc->x = srm_trans_desc->out.block_offset_x;
    srm_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    srm_engine->dma_rx_desc->buffer = (void *)srm_trans_desc->out.buffer;
    srm_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)srm_engine->dma_tx_desc, platform->dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)srm_engine->dma_rx_desc, platform->dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // Configure 2D-DMA channels
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_PPA_SRM,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_SRM_TX,
    };
    dma2d_connect(dma2d_tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_SRM_RX;
    dma2d_connect(dma2d_rx_chan, &trig_periph);

    dma2d_transfer_ability_t dma_transfer_ability = {
        .data_burst_length = srm_trans_desc->data_burst_length,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
    dma2d_set_transfer_ability(dma2d_tx_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    // Configure the block size to be received by the SRM engine, which is passed from the 2D-DMA TX channel (i.e. 2D-DMA dscr-port mode)
    dma2d_dscr_port_mode_config_t dma_dscr_port_mode_config = {
        .block_h = (srm_trans_desc->in.srm_cm == PPA_SRM_COLOR_MODE_YUV420) ? PPA_LL_SRM_YUV420_BLOCK_SIZE : PPA_LL_SRM_DEFAULT_BLOCK_SIZE,
        .block_v = (srm_trans_desc->in.srm_cm == PPA_SRM_COLOR_MODE_YUV420) ? PPA_LL_SRM_YUV420_BLOCK_SIZE : PPA_LL_SRM_DEFAULT_BLOCK_SIZE,
    };
    dma2d_configure_dscr_port_mode(dma2d_tx_chan, &dma_dscr_port_mode_config);

    // YUV444 is not supported by PPA module, need to utilize 2D-DMA color space conversion feature to do a conversion
    ppa_srm_color_mode_t ppa_in_color_mode = srm_trans_desc->in.srm_cm;
    if (ppa_in_color_mode == PPA_SRM_COLOR_MODE_YUV444) {
        ppa_in_color_mode = PPA_SRM_COLOR_MODE_RGB888;
        dma2d_csc_config_t dma_tx_csc = {0};
        if (srm_trans_desc->in.yuv_std == PPA_COLOR_CONV_STD_RGB_YUV_BT601) {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV444_TO_RGB888_601;
        } else {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV444_TO_RGB888_709;
        }
        dma2d_configure_color_space_conversion(dma2d_tx_chan, &dma_tx_csc);
    }

    ppa_srm_color_mode_t ppa_out_color_mode = srm_trans_desc->out.srm_cm;
    if (ppa_out_color_mode == PPA_SRM_COLOR_MODE_YUV444) {
        ppa_out_color_mode = PPA_SRM_COLOR_MODE_YUV420;
        dma2d_csc_config_t dma_rx_csc = {
            .rx_csc_option = DMA2D_CSC_RX_YUV420_TO_YUV444,
        };
        dma2d_configure_color_space_conversion(dma2d_rx_chan, &dma_rx_csc);
    }

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    dma2d_set_desc_addr(dma2d_tx_chan, (intptr_t)srm_engine->dma_tx_desc);
    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)srm_engine->dma_rx_desc);
    dma2d_start(dma2d_tx_chan);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA SRM engine
    ppa_ll_srm_set_rx_color_mode(platform->hal.dev, ppa_in_color_mode);
    if (COLOR_SPACE_TYPE((uint32_t)ppa_in_color_mode) == COLOR_SPACE_YUV) {
        ppa_ll_srm_set_rx_yuv_range(platform->hal.dev, srm_trans_desc->in.yuv_range);
        ppa_ll_srm_set_rx_yuv2rgb_std(platform->hal.dev, srm_trans_desc->in.yuv_std);
    }
    ppa_ll_srm_enable_rx_byte_swap(platform->hal.dev, srm_trans_desc->byte_swap);
    ppa_ll_srm_enable_rx_rgb_swap(platform->hal.dev, srm_trans_desc->rgb_swap);
    ppa_ll_srm_configure_rx_alpha(platform->hal.dev, srm_trans_desc->alpha_update_mode, srm_trans_desc->alpha_value);

    ppa_ll_srm_set_tx_color_mode(platform->hal.dev, ppa_out_color_mode);
    if (COLOR_SPACE_TYPE((uint32_t)ppa_out_color_mode) == COLOR_SPACE_YUV) {
        ppa_ll_srm_set_tx_yuv_range(platform->hal.dev, srm_trans_desc->out.yuv_range);
        ppa_ll_srm_set_tx_rgb2yuv_std(platform->hal.dev, srm_trans_desc->out.yuv_std);
    }

    ppa_ll_srm_set_rotation_angle(platform->hal.dev, srm_trans_desc->rotation_angle);
    ppa_ll_srm_set_scaling_x(platform->hal.dev, srm_trans_desc->scale_x_int, srm_trans_desc->scale_x_frag);
    ppa_ll_srm_set_scaling_y(platform->hal.dev, srm_trans_desc->scale_y_int, srm_trans_desc->scale_y_frag);
    ppa_ll_srm_enable_mirror_x(platform->hal.dev, srm_trans_desc->mirror_x);
    ppa_ll_srm_enable_mirror_y(platform->hal.dev, srm_trans_desc->mirror_y);

    ppa_ll_srm_start(platform->hal.dev);

    // No need to yield
    return false;
}

esp_err_t ppa_do_scale_rotate_mirror(ppa_client_handle_t ppa_client, const ppa_srm_oper_config_t *config)
{
    ESP_RETURN_ON_FALSE(ppa_client && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_client->oper_type == PPA_OPERATION_SRM, ESP_ERR_INVALID_ARG, TAG, "client is not for SRM operations");
    ESP_RETURN_ON_FALSE(config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // in_buffer could be anywhere (ram, flash, psram), out_buffer ptr cannot in flash region
    ESP_RETURN_ON_FALSE(esp_ptr_internal(config->out.buffer) || esp_ptr_external_ram(config->out.buffer), ESP_ERR_INVALID_ARG, TAG, "invalid out.buffer addr");
    uint32_t buf_alignment_size = (uint32_t)ppa_client->engine->platform->buf_alignment_size;
    ESP_RETURN_ON_FALSE(((uint32_t)config->out.buffer & (buf_alignment_size - 1)) == 0 && (config->out.buffer_size & (buf_alignment_size - 1)) == 0,
                        ESP_ERR_INVALID_ARG, TAG, "out.buffer addr or out.buffer_size not aligned to cache line size");
    // For YUV420 input/output: in desc, ha/hb/va/vb/x/y must be even number
    if (config->in.srm_cm == PPA_SRM_COLOR_MODE_YUV420) {
        ESP_RETURN_ON_FALSE(config->in.pic_h % 2 == 0 && config->in.pic_w % 2 == 0 &&
                            config->in.block_h % 2 == 0 && config->in.block_w % 2 == 0 &&
                            config->in.block_offset_x % 2 == 0 && config->in.block_offset_y % 2 == 0,
                            ESP_ERR_INVALID_ARG, TAG, "YUV420 input does not support odd h/w/offset_x/offset_y");
    }
    // TODO: P4 ECO2 support YUV422
    // else if (config->in.srm_cm == PPA_SRM_COLOR_MODE_YUV422) {
    //     ESP_RETURN_ON_FALSE(config->in.pic_w % 2 == 0 && config->in.block_w % 2 == 0 && config->in.block_offset_x % 2 == 0,
    //                         ESP_ERR_INVALID_ARG, TAG, "YUV422 input does not support odd w/offset_x");
    // }
    if (config->out.srm_cm == PPA_SRM_COLOR_MODE_YUV420) {
        ESP_RETURN_ON_FALSE(config->out.pic_h % 2 == 0 && config->out.pic_w % 2 == 0 &&
                            config->out.block_offset_x % 2 == 0 && config->out.block_offset_y % 2 == 0,
                            ESP_ERR_INVALID_ARG, TAG, "YUV420 output does not support odd h/w/offset_x/offset_y");
    }
    ESP_RETURN_ON_FALSE(config->in.block_w <= (config->in.pic_w - config->in.block_offset_x) &&
                        config->in.block_h <= (config->in.pic_h - config->in.block_offset_y),
                        ESP_ERR_INVALID_ARG, TAG, "in.block_w/h + in.block_offset_x/y does not fit in the in pic");
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = config->out.srm_cm,
    };
    uint32_t out_pixel_depth = color_hal_pixel_format_get_bit_depth(out_pixel_format); // bits
    uint32_t out_pic_len = config->out.pic_w * config->out.pic_h * out_pixel_depth / 8;
    ESP_RETURN_ON_FALSE(out_pic_len <= config->out.buffer_size, ESP_ERR_INVALID_ARG, TAG, "out.pic_w/h mismatch with out.buffer_size");
    ESP_RETURN_ON_FALSE(config->scale_x < PPA_LL_SRM_SCALING_INT_MAX && config->scale_x >= (1.0 / PPA_LL_SRM_SCALING_FRAG_MAX) &&
                        config->scale_y < PPA_LL_SRM_SCALING_INT_MAX && config->scale_y >= (1.0 / PPA_LL_SRM_SCALING_FRAG_MAX),
                        ESP_ERR_INVALID_ARG, TAG, "invalid scale");
    uint32_t new_block_w = 0;
    uint32_t new_block_h = 0;
    if (config->rotation_angle == PPA_SRM_ROTATION_ANGLE_0 || config->rotation_angle == PPA_SRM_ROTATION_ANGLE_180) {
        new_block_w = (uint32_t)(config->scale_x * config->in.block_w);
        new_block_h = (uint32_t)(config->scale_y * config->in.block_h);
    } else {
        new_block_w = (uint32_t)(config->scale_y * config->in.block_h);
        new_block_h = (uint32_t)(config->scale_x * config->in.block_w);
    }
    ESP_RETURN_ON_FALSE(new_block_w <= (config->out.pic_w - config->out.block_offset_x) &&
                        new_block_h <= (config->out.pic_h - config->out.block_offset_y),
                        ESP_ERR_INVALID_ARG, TAG, "scale does not fit in the out pic");
    if (config->byte_swap) {
        PPA_CHECK_CM_SUPPORT_BYTE_SWAP("in.srm", (uint32_t)config->in.srm_cm);
    }
    if (config->rgb_swap) {
        PPA_CHECK_CM_SUPPORT_RGB_SWAP("in.srm", (uint32_t)config->in.srm_cm);
    }
    uint32_t new_alpha_value = 0;
    if (config->alpha_update_mode == PPA_ALPHA_FIX_VALUE) {
        ESP_RETURN_ON_FALSE(config->alpha_fix_val <= 0xFF, ESP_ERR_INVALID_ARG, TAG, "invalid alpha_fix_val");
        new_alpha_value = config->alpha_fix_val;
    } else if (config->alpha_update_mode == PPA_ALPHA_SCALE) {
        ESP_RETURN_ON_FALSE(config->alpha_scale_ratio > 0 && config->alpha_scale_ratio < 1, ESP_ERR_INVALID_ARG, TAG, "invalid alpha_scale_ratio");
        new_alpha_value = (uint32_t)(config->alpha_scale_ratio * 256);
    }
    // To reduce complexity, rotation_angle, color_mode, alpha_update_mode correctness are checked in their corresponding LL functions

    // Write back and invalidate necessary data (note that the window content is not continuous in the buffer)
    // Write back in_buffer extended window (alignment not necessary on C2M direction)
    color_space_pixel_format_t in_pixel_format = {
        .color_type_id = config->in.srm_cm,
    };
    uint32_t in_pixel_depth = color_hal_pixel_format_get_bit_depth(in_pixel_format); // bits
    uint32_t in_ext_window = (uint32_t)config->in.buffer + config->in.block_offset_y * config->in.pic_w * in_pixel_depth / 8;
    uint32_t in_ext_window_len = config->in.pic_w * config->in.block_h * in_pixel_depth / 8;
    esp_cache_msync((void *)in_ext_window, in_ext_window_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    // Invalidate out_buffer extended window (alignment strict on M2C direction)
    uint32_t out_ext_window = (uint32_t)config->out.buffer + config->out.block_offset_y * config->out.pic_w * out_pixel_depth / 8;
    uint32_t out_ext_window_aligned = PPA_ALIGN_DOWN(out_ext_window, buf_alignment_size);
    uint32_t out_ext_window_len = config->out.pic_w * new_block_h * out_pixel_depth / 8; // actual ext_window_len must be less than or equal to this, since actual block_h <= new_block_h (may round down)
    assert(out_ext_window + out_ext_window_len <= (uint32_t)config->out.buffer + config->out.buffer_size);
    esp_cache_msync((void *)out_ext_window_aligned, PPA_ALIGN_UP(out_ext_window_len + (out_ext_window - out_ext_window_aligned), buf_alignment_size), ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    esp_err_t ret = ESP_OK;
    ppa_trans_t *trans_elm = NULL;
    if (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0) == pdTRUE) {
        assert(trans_elm);
        dma2d_trans_config_t *dma_trans_desc = trans_elm->trans_desc;

        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = dma_trans_desc->user_config;

        ppa_srm_oper_t *srm_trans_desc = (ppa_srm_oper_t *)trans_on_picked_desc->srm_desc;
        memcpy(srm_trans_desc, config, sizeof(ppa_srm_oper_config_t));
        srm_trans_desc->scale_x_int = (uint32_t)srm_trans_desc->scale_x;
        srm_trans_desc->scale_x_frag = (uint32_t)(srm_trans_desc->scale_x * PPA_LL_SRM_SCALING_FRAG_MAX) & (PPA_LL_SRM_SCALING_FRAG_MAX - 1);
        srm_trans_desc->scale_y_int = (uint32_t)srm_trans_desc->scale_y;
        srm_trans_desc->scale_y_frag = (uint32_t)(srm_trans_desc->scale_y * PPA_LL_SRM_SCALING_FRAG_MAX) & (PPA_LL_SRM_SCALING_FRAG_MAX - 1);
        // SRM processes in blocks. Block x/(y) cannot be scaled to odd number when YUV422/YUV420 is the output color mode
        // When block size is 16x16, odd number is possible, so needs to make them even
        // When block size is 32x32, calculated frag values are always even
        if (config->out.srm_cm == PPA_SRM_COLOR_MODE_YUV420) {
            srm_trans_desc->scale_x_frag = srm_trans_desc->scale_x_frag & ~1;
            srm_trans_desc->scale_y_frag = srm_trans_desc->scale_y_frag & ~1;
        }
        srm_trans_desc->alpha_value = new_alpha_value;
        srm_trans_desc->data_burst_length = ppa_client->data_burst_length;

        trans_on_picked_desc->ppa_engine = ppa_client->engine;
        trans_on_picked_desc->trigger_periph = DMA2D_TRIG_PERIPH_PPA_SRM;

        dma_trans_desc->tx_channel_num = 1;
        dma_trans_desc->rx_channel_num = 1;
        dma_trans_desc->channel_flags = 0;
        if (config->in.srm_cm == PPA_SRM_COLOR_MODE_YUV444) {
            dma_trans_desc->channel_flags |= DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC;
        }
        if (config->out.srm_cm == PPA_SRM_COLOR_MODE_YUV444) {
            dma_trans_desc->channel_flags |= DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC;
        }
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
