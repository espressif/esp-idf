/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_async_color_convert.h"
#include "esp_private/async_memcpy_dma2d.h"

ESP_LOG_ATTR_TAG(TAG, "async_memcpy_dma2d");

/*
 * DMA2D 2D window copy is a simplified same-format color-convert request.
 * Keep this file as a thin naming/API wrapper so LCD and other drivers can
 * express "2D memcpy" without calling color-convert APIs directly.
 */

esp_err_t esp_async_memcpy_install_dma2d(const async_memcpy_dma2d_config_t *config,
                                         async_memcpy_dma2d_handle_t *ret_hdl)
{
    ESP_RETURN_ON_FALSE(config && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    async_color_convert_config_t conv_config = {
        .backlog = config->backlog,
        .dma_burst_size = config->dma_burst_size,
        .intr_priority = config->intr_priority,
    };
    return esp_async_color_convert_install_dma2d(&conv_config, (async_color_convert_handle_t *)ret_hdl);
}

esp_err_t esp_async_memcpy_uninstall_dma2d(async_memcpy_dma2d_handle_t mcp)
{
    return esp_async_color_convert_uninstall((async_color_convert_handle_t)mcp);
}

esp_err_t esp_async_memcpy_dma2d(async_memcpy_dma2d_handle_t mcp,
                                 const async_memcpy_dma2d_trans_desc_t *trans,
                                 async_memcpy_dma2d_isr_cb_t cb_isr,
                                 void *cb_args)
{
    ESP_RETURN_ON_FALSE(mcp && trans, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    async_color_convert_request_t request = {
        .src_buffer = trans->src_buffer,
        .dst_buffer = trans->dst_buffer,
        .src_stride = trans->src_stride,
        .src_height = trans->src_height,
        .dst_stride = trans->dst_stride,
        .dst_height = trans->dst_height,
        .src_x = trans->src_x,
        .src_y = trans->src_y,
        .dst_x = trans->dst_x,
        .dst_y = trans->dst_y,
        .copy_width = trans->copy_width,
        .copy_height = trans->copy_height,
        // Same source/destination format disables CSC and performs 2D window copy only.
        .src_color_format = trans->pixel_format,
        .dst_color_format = trans->pixel_format,
    };

    // Callback prototypes are ABI-compatible: opaque handle pointer + empty event struct + user args.
    return esp_async_color_convert((async_color_convert_handle_t)mcp,
                                   &request,
                                   (async_color_convert_isr_cb_t)cb_isr,
                                   cb_args);
}
