/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_cali.h"
#include "adc_cali_interface.h"
#include "adc_cali_schemes.h"

const __attribute__((unused)) static char *TAG = "adc_cali";

esp_err_t adc_cali_check_scheme(adc_cali_scheme_ver_t *scheme_mask)
{
    ESP_RETURN_ON_FALSE(scheme_mask, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    *scheme_mask = 0;
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    *scheme_mask |= ADC_CALI_SCHEME_VER_LINE_FITTING;
#elif ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    *scheme_mask |= ADC_CALI_SCHEME_VER_CURVE_FITTING;
#endif

    ESP_RETURN_ON_FALSE((*scheme_mask) != 0, ESP_ERR_NOT_SUPPORTED, TAG, "no supported calibration scheme yet");

    return ESP_OK;
}

esp_err_t adc_cali_new_scheme(esp_err_t (*raw_to_voltage)(void *arg, int raw, int *voltage),
                              esp_err_t (*chan_raw_to_voltage)(void *arg, adc_channel_t channel, int raw, int *voltage),
                              esp_err_t (*del)(void *arg),
                              void *ctx,
                              adc_cali_scheme_t **ret_handle)
{
    ESP_RETURN_ON_FALSE((raw_to_voltage || chan_raw_to_voltage) && del && ret_handle,
                        ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    adc_cali_scheme_t *scheme = heap_caps_calloc(1, sizeof(adc_cali_scheme_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_NO_MEM, TAG, "no mem for adc calibration scheme");

    scheme->raw_to_voltage = raw_to_voltage;
    scheme->chan_raw_to_voltage = chan_raw_to_voltage;
    scheme->del = del;
    scheme->ctx = ctx;
    *ret_handle = scheme;

    return ESP_OK;
}

esp_err_t adc_cali_delete_scheme(adc_cali_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle && handle->del, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    esp_err_t ret = handle->del(handle->ctx);
    if (ret != ESP_OK) {
        return ret;
    }
    handle->ctx = NULL;
    free(handle);

    return ESP_OK;
}

esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle, int raw, int *voltage)
{
    ESP_RETURN_ON_FALSE(handle && voltage, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->ctx, ESP_ERR_INVALID_STATE, TAG, "no calibration scheme, create a scheme first");
    ESP_RETURN_ON_FALSE(handle->raw_to_voltage, ESP_ERR_NOT_SUPPORTED, TAG,
                        "this scheme requires adc_cali_chan_raw_to_voltage");

    return handle->raw_to_voltage(handle->ctx, raw, voltage);
}

esp_err_t adc_cali_chan_raw_to_voltage(adc_cali_handle_t handle, adc_channel_t channel, int raw, int *voltage)
{
    ESP_RETURN_ON_FALSE(handle && voltage, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->ctx, ESP_ERR_INVALID_STATE, TAG, "no calibration scheme, create a scheme first");

    if (handle->chan_raw_to_voltage) {
        return handle->chan_raw_to_voltage(handle->ctx, channel, raw, voltage);
    }
    ESP_RETURN_ON_FALSE(handle->raw_to_voltage, ESP_ERR_NOT_SUPPORTED, TAG, "no conversion callback");
    return handle->raw_to_voltage(handle->ctx, raw, voltage);
}
