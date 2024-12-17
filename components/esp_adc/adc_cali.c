/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/cdefs.h>
#include "esp_types.h"
#include "sdkconfig.h"
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

    //Add your custom ADC calibration scheme here

    ESP_RETURN_ON_FALSE((*scheme_mask) != 0, ESP_ERR_NOT_SUPPORTED, TAG, "no supported calibration scheme yet");

    return ESP_OK;
}

esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle, int raw, int *voltage)
{
    ESP_RETURN_ON_FALSE(handle && voltage, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->ctx, ESP_ERR_INVALID_STATE, TAG, "no calibration scheme, create a scheme first");

    return handle->raw_to_voltage(handle->ctx, raw, voltage);
}
