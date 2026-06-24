/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"

ESP_LOG_ATTR_TAG(TAG, "async_mcp");

esp_err_t esp_async_memcpy_uninstall(async_memcpy_handle_t asmcp)
{
    ESP_RETURN_ON_FALSE(asmcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return asmcp->del(asmcp);
}

esp_err_t esp_async_memcpy(async_memcpy_handle_t asmcp, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    ESP_RETURN_ON_FALSE(asmcp && dst && src && n, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return asmcp->memcpy(asmcp, dst, src, n, cb_isr, cb_args);
}

typedef struct {
    SemaphoreHandle_t semaphore;
    StaticSemaphore_t semaphore_buffer;
} memcpy_blocking_context_t;

static bool memcpy_blocking_callback(async_memcpy_handle_t mcp_hdl, async_memcpy_event_t *event, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    memcpy_blocking_context_t *ctx = (memcpy_blocking_context_t *)user_data;
    (void)mcp_hdl;
    (void)event;

    xSemaphoreGiveFromISR(ctx->semaphore, &task_woken);

    return task_woken == pdTRUE;
}

esp_err_t esp_memcpy_blocking(async_memcpy_handle_t asmcp, void *dst, void *src, size_t n, int32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(asmcp && dst && src && n, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(!xPortInIsrContext(), ESP_ERR_INVALID_STATE, TAG, "called from ISR context is not allowed");
    ESP_RETURN_ON_FALSE(timeout_ms == -1, ESP_ERR_INVALID_ARG, TAG, "only timeout_ms=-1 is supported");

    memcpy_blocking_context_t ctx = {};
    ctx.semaphore = xSemaphoreCreateBinaryStatic(&ctx.semaphore_buffer);

    ESP_RETURN_ON_ERROR(esp_async_memcpy(asmcp, dst, src, n, memcpy_blocking_callback, &ctx), TAG, "failed to start memory copy");

    xSemaphoreTake(ctx.semaphore, portMAX_DELAY);
    return ESP_OK;
}

#if SOC_ETM_SUPPORTED
esp_err_t esp_async_memcpy_new_etm_event(async_memcpy_handle_t asmcp, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event)
{
    ESP_RETURN_ON_FALSE(asmcp && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(asmcp->new_etm_event, ESP_ERR_NOT_SUPPORTED, TAG, "ETM is not supported");
    return asmcp->new_etm_event(asmcp, event_type, out_event);
}
#endif
