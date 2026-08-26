/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_async_color_convert.h"
#include "esp_async_color_convert_priv.h"

ESP_LOG_ATTR_TAG(TAG, "async_color_conv");

esp_err_t esp_async_color_convert_uninstall(async_color_convert_handle_t conv_hdl)
{
    ESP_RETURN_ON_FALSE(conv_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return conv_hdl->del(conv_hdl);
}

esp_err_t esp_async_color_convert(async_color_convert_handle_t conv_hdl,
                                  const async_color_convert_request_t *request,
                                  async_color_convert_isr_cb_t cb_isr,
                                  void *cb_args)
{
    ESP_RETURN_ON_FALSE(conv_hdl && request, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return conv_hdl->convert(conv_hdl, request, cb_isr, cb_args);
}

typedef struct {
    SemaphoreHandle_t done_sem;
    StaticSemaphore_t done_sem_buffer;
} color_convert_blocking_context_t;

static bool color_convert_blocking_cb(async_color_convert_handle_t conv_hdl,
                                      async_color_convert_event_data_t *edata,
                                      void *cb_args)
{
    BaseType_t high_task_woken = pdFALSE;
    color_convert_blocking_context_t *ctx = (color_convert_blocking_context_t *)cb_args;
    (void)conv_hdl;
    (void)edata;
    xSemaphoreGiveFromISR(ctx->done_sem, &high_task_woken);
    return (high_task_woken == pdTRUE);
}

esp_err_t esp_color_convert_blocking(async_color_convert_handle_t conv_hdl,
                                     const async_color_convert_request_t *request,
                                     int32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(conv_hdl && request, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(!xPortInIsrContext(), ESP_ERR_INVALID_STATE, TAG, "called from ISR context is not allowed");
    ESP_RETURN_ON_FALSE(timeout_ms == -1, ESP_ERR_INVALID_ARG, TAG, "only timeout -1 is supported");

    color_convert_blocking_context_t ctx = {};
    ctx.done_sem = xSemaphoreCreateBinaryStatic(&ctx.done_sem_buffer);

    ESP_RETURN_ON_ERROR(esp_async_color_convert(conv_hdl, request, color_convert_blocking_cb, &ctx), TAG, "fail to start async color conversion");
    // Wait for the conversion to complete
    xSemaphoreTake(ctx.done_sem, portMAX_DELAY);
    return ESP_OK;
}
