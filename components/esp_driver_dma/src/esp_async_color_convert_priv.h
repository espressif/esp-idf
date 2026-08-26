/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_private/dma2d.h"
#include "esp_async_color_convert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_COLOR_CONVERT_BACKLOG 8

typedef struct async_color_convert_context_t async_color_convert_context_t;

struct async_color_convert_context_t {
    esp_err_t (*convert)(async_color_convert_context_t *ctx,
                         const async_color_convert_request_t *request,
                         async_color_convert_isr_cb_t cb_isr,
                         void *cb_args);
    esp_err_t (*del)(async_color_convert_context_t *ctx);
};

#ifdef __cplusplus
}
#endif
