/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "example_buffer.h"
#include "esp_log.h"
#include "esp_lcd_panel_ops.h"
#include "example_config.h"
#include "esp_check.h"

static const char *TAG = "isp_buffer";

esp_err_t example_isp_buffer_init(example_pingpong_buffer_ctx_t *ctx,
                                  void *fb0,
                                  void *fb1,
                                  esp_lcd_panel_handle_t panel,
                                  int h_res,
                                  int v_res)
{
    ESP_RETURN_ON_FALSE(ctx != NULL && fb0 != NULL && fb1 != NULL && panel != NULL, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");

    ctx->fb0 = fb0;
    ctx->fb1 = fb1;
    ctx->csi_buffer = fb0;  // CSI starts writing to fb0
    ctx->dsi_buffer = fb1;  // DSI starts displaying fb1
    ctx->panel = panel;
    ctx->h_res = h_res;
    ctx->v_res = v_res;

    return ESP_OK;
}

void example_isp_buffer_swap(example_pingpong_buffer_ctx_t *ctx)
{
    if (ctx == NULL) {
        return;
    }
    void *temp = ctx->csi_buffer;
    ctx->csi_buffer = ctx->dsi_buffer;
    ctx->dsi_buffer = temp;
}

void *example_isp_buffer_get_csi_buffer(example_pingpong_buffer_ctx_t *ctx)
{
    if (ctx == NULL) {
        return NULL;
    }
    return ctx->csi_buffer;
}

void *example_isp_buffer_get_dsi_buffer(example_pingpong_buffer_ctx_t *ctx)
{
    if (ctx == NULL) {
        return NULL;
    }
    return ctx->dsi_buffer;
}
