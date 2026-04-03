/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_async_crc.h"
#include "esp_async_crc_priv.h"

ESP_LOG_ATTR_TAG(TAG, "async_crc");

#if SOC_HAS(AHB_GDMA)
esp_err_t esp_async_crc_install_gdma_ahb(const async_crc_config_t *config, async_crc_handle_t *crc_hdl)
{
    ESP_RETURN_ON_FALSE(config && crc_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return esp_async_crc_install_gdma_template(config, crc_hdl, gdma_new_ahb_channel, SOC_GDMA_BUS_AHB);
}
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
esp_err_t esp_async_crc_install_gdma_axi(const async_crc_config_t *config, async_crc_handle_t *crc_hdl)
{
    ESP_RETURN_ON_FALSE(config && crc_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return esp_async_crc_install_gdma_template(config, crc_hdl, gdma_new_axi_channel, SOC_GDMA_BUS_AXI);
}
#endif // SOC_HAS(AXI_GDMA)

esp_err_t esp_async_crc_uninstall(async_crc_handle_t crc_hdl)
{
    ESP_RETURN_ON_FALSE(crc_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return crc_hdl->del(crc_hdl);
}

esp_err_t esp_async_crc_calc(async_crc_handle_t crc_hdl, const void *data, size_t size,
                             const async_crc_params_t *params, async_crc_isr_cb_t cb_isr, void *cb_args)
{
    ESP_RETURN_ON_FALSE(crc_hdl && data && size && params, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return crc_hdl->calc(crc_hdl, data, size, params, cb_isr, cb_args);
}

typedef struct {
    uint32_t *result;
    SemaphoreHandle_t semaphore;
} crc_blocking_context_t;

static bool crc_blocking_callback(async_crc_handle_t crc_hdl, async_crc_event_data_t *event, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    crc_blocking_context_t *ctx = (crc_blocking_context_t *)user_data;

    *(ctx->result) = event->crc_result;

    // Give the semaphore to unblock the waiting task
    xSemaphoreGiveFromISR(ctx->semaphore, &task_woken);

    return (task_woken == pdTRUE); // Yield if a higher priority task was woken
}

esp_err_t esp_crc_calc_blocking(async_crc_handle_t crc_hdl, const void *data, size_t size,
                                const async_crc_params_t *params, int32_t timeout_ms, uint32_t *result)
{
    ESP_RETURN_ON_FALSE(crc_hdl && data && size && params && result, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(!xPortInIsrContext(), ESP_ERR_INVALID_STATE, TAG, "called from ISR context is not allowed");

    crc_blocking_context_t ctx = {
        .result = result,
        .semaphore = xSemaphoreCreateBinary()
    };
    ESP_RETURN_ON_FALSE(ctx.semaphore, ESP_ERR_NO_MEM, TAG, "create semaphore failed");

    esp_err_t ret = esp_async_crc_calc(crc_hdl, data, size, params, crc_blocking_callback, &ctx);
    if (ret != ESP_OK) {
        vSemaphoreDelete(ctx.semaphore);
        return ret;
    }

    // Wait for completion with timeout (<0 means wait forever)
    TickType_t ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    if (xSemaphoreTake(ctx.semaphore, ticks) != pdTRUE) {
        vSemaphoreDelete(ctx.semaphore);
        return ESP_ERR_TIMEOUT;
    }

    vSemaphoreDelete(ctx.semaphore);
    return ESP_OK;
}
