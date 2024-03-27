/*
 * SPDX-FileCopyrightText: 2016-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include "esp_types.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/adc_ll.h"
#include "adc_continuous_internal.h"
#include "esp_adc/adc_filter.h"

static const char *TAG = "adc_filter";
static portMUX_TYPE s_filter_spinlock = portMUX_INITIALIZER_UNLOCKED;

#if SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED
static atomic_bool s_adc_filter_claimed[SOC_ADC_DIGI_IIR_FILTER_NUM] = {ATOMIC_VAR_INIT(false),
#if (SOC_ADC_DIGI_IIR_FILTER_NUM >= 2)
                                                                        ATOMIC_VAR_INIT(false)
#endif
                                                                       };

static esp_err_t s_adc_filter_claim(adc_continuous_handle_t handle, adc_iir_filter_t *filter_ctx, adc_unit_t unit)
{
    assert(handle && filter_ctx);
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    bool false_var = false;
    if (atomic_compare_exchange_strong(&s_adc_filter_claimed[unit], &false_var, true)) {
        ret = ESP_OK;
        filter_ctx->filter_id = unit;
        handle->iir_filter[unit] = filter_ctx;
    }

    return ret;
}

static esp_err_t s_adc_filter_free(adc_iir_filter_t *filter_ctx)
{
    assert(filter_ctx);
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    bool true_var = true;
    if (atomic_compare_exchange_strong(&s_adc_filter_claimed[filter_ctx->cfg.unit], &true_var, false)) {
        ret = ESP_OK;
        filter_ctx->continuous_ctx->iir_filter[filter_ctx->filter_id] = NULL;
    }

    return ret;
}

#else
static esp_err_t s_adc_filter_claim(adc_continuous_handle_t handle, adc_iir_filter_t *filter_ctx, adc_unit_t unit)
{
    (void)unit;
    assert(handle && filter_ctx);

    portENTER_CRITICAL(&s_filter_spinlock);
    for (int i = 0; i < SOC_ADC_DIGI_IIR_FILTER_NUM; i++) {
        bool found = !handle->iir_filter[i];
        if (found) {
            handle->iir_filter[i] = filter_ctx;
            filter_ctx->filter_id = i;
            portEXIT_CRITICAL(&s_filter_spinlock);
            return ESP_OK;
        }
    }
    portEXIT_CRITICAL(&s_filter_spinlock);

    return ESP_ERR_NOT_FOUND;
}

static esp_err_t s_adc_filter_free(adc_iir_filter_t *filter_ctx)
{
    assert(filter_ctx);
    portENTER_CRITICAL(&s_filter_spinlock);
    filter_ctx->continuous_ctx->iir_filter[filter_ctx->filter_id] = NULL;
    portEXIT_CRITICAL(&s_filter_spinlock);

    return ESP_OK;
}
#endif

esp_err_t adc_new_continuous_iir_filter(adc_continuous_handle_t handle, const adc_continuous_iir_filter_config_t *config, adc_iir_filter_handle_t *ret_hdl)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(handle && config && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "ADC continuous driver should be in init state");
    ESP_RETURN_ON_FALSE(config->unit < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid unit");

    adc_iir_filter_t *filter_ctx = heap_caps_calloc(1, sizeof(adc_iir_filter_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(filter_ctx, ESP_ERR_NO_MEM, TAG, "no mem");

    //claim a filter
#if SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED
    ESP_GOTO_ON_ERROR(s_adc_filter_claim(handle, filter_ctx, config->unit), err, TAG, "filter already in use");
#else
    ESP_GOTO_ON_ERROR(s_adc_filter_claim(handle, filter_ctx, config->unit), err, TAG, "no free filter");
#endif

    filter_ctx->cfg.unit = config->unit;
    filter_ctx->cfg.channel = config->channel;
    filter_ctx->cfg.coeff = config->coeff;
    filter_ctx->continuous_ctx = handle;

    *ret_hdl = filter_ctx;

    return ESP_OK;

err:
    free(filter_ctx);

    return ret;
}

esp_err_t adc_continuous_iir_filter_enable(adc_iir_filter_handle_t filter_hdl)
{
    ESP_RETURN_ON_FALSE(filter_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(filter_hdl->continuous_ctx->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "ADC continuous driver should be in init state");

    portENTER_CRITICAL(&s_filter_spinlock);
    adc_ll_digi_filter_reset(filter_hdl->filter_id, filter_hdl->cfg.unit);
    adc_ll_digi_filter_set_factor(filter_hdl->filter_id, filter_hdl->cfg.unit, filter_hdl->cfg.channel, filter_hdl->cfg.coeff);
    adc_ll_digi_filter_enable(filter_hdl->filter_id, filter_hdl->cfg.unit, true);
    portEXIT_CRITICAL(&s_filter_spinlock);

    return ESP_OK;
}

esp_err_t adc_continuous_iir_filter_disable(adc_iir_filter_handle_t filter_hdl)
{
    ESP_RETURN_ON_FALSE(filter_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(filter_hdl->continuous_ctx->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "ADC continuous driver should be in init state");

    portENTER_CRITICAL(&s_filter_spinlock);
    adc_ll_digi_filter_enable(filter_hdl->filter_id, filter_hdl->cfg.unit, false);
    portEXIT_CRITICAL(&s_filter_spinlock);

    return ESP_OK;
}

esp_err_t adc_del_continuous_iir_filter(adc_iir_filter_handle_t filter_hdl)
{
    ESP_RETURN_ON_FALSE(filter_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(filter_hdl->continuous_ctx->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "ADC continuous driver should be in init state");

    ESP_RETURN_ON_ERROR(s_adc_filter_free(filter_hdl), TAG, "filter isn't in use");

    free(filter_hdl);

    return ESP_OK;
}
