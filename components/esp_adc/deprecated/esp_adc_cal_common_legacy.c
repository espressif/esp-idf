/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "inttypes.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "hal/adc_types.h"
#define CONFIG_ADC_SUPPRESS_DEPRECATE_WARN 1
#include "esp_adc_cal.h"
#include "esp_adc_cal_internal_legacy.h"
#include "driver/adc.h"

const __attribute__((unused)) static char *TAG = "ADC_CALI";

esp_err_t esp_adc_cal_get_voltage(adc_channel_t channel,
                                  const esp_adc_cal_characteristics_t *chars,
                                  uint32_t *voltage)
{
    // Check parameters
    ESP_RETURN_ON_FALSE(chars != NULL, ESP_ERR_INVALID_ARG, TAG, "No characteristic input");
    ESP_RETURN_ON_FALSE(voltage != NULL, ESP_ERR_INVALID_ARG, TAG, "No output buffer");

    esp_err_t ret = ESP_OK;
    int adc_reading;
    if (chars->adc_num == ADC_UNIT_1) {
        ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(0), ESP_ERR_INVALID_ARG, TAG, "Invalid channel");
        adc_reading = adc1_get_raw(channel);
    } else {
        ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(1), ESP_ERR_INVALID_ARG, TAG, "Invalid channel");
        ret = adc2_get_raw(channel, chars->bit_width, &adc_reading);
    }

    if (ret == ESP_OK) {
        *voltage = esp_adc_cal_raw_to_voltage((uint32_t)adc_reading, chars);
    }
    return ret;
}

#if ESP_ADC_CAL_CURVE_FITTING_SUPPORTED
/*------------------------------------------------------------------------------
 * Private API
 *----------------------------------------------------------------------------*/
int32_t esp_adc_cal_get_reading_error(const esp_adc_error_calc_param_t *param, uint8_t atten)
{
    if (param->v_cali_input == 0) {
        return 0;
    }

    uint64_t v_cali_1 = param->v_cali_input;
    uint8_t term_num = param->term_num;
    int32_t error = 0;
    uint64_t coeff = 0;
    uint64_t variable[term_num];
    uint64_t term[term_num];
    memset(variable, 0, term_num * sizeof(uint64_t));
    memset(term, 0, term_num * sizeof(uint64_t));

    /**
     * For atten0 ~ 2:
     * error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2);
     *
     * For atten3:
     * error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + (K4 * X^4);
     */
    variable[0] = 1;
    coeff = (*param->coeff)[atten][0][0];
    term[0] = variable[0] * coeff / (*param->coeff)[atten][0][1];
    error = (int32_t)term[0] * (*param->sign)[atten][0];

    for (int i = 1; i < term_num; i++) {
        variable[i] = variable[i - 1] * v_cali_1;
        coeff = (*param->coeff)[atten][i][0];
        term[i] = variable[i] * coeff;
        ESP_LOGV(TAG, "big coef is %llu, big term%d is %llu, coef_id is %d", coeff, i, term[i], i);

        term[i] = term[i] / (*param->coeff)[atten][i][1];
        error += (int32_t)term[i] * (*param->sign)[atten][i];
        ESP_LOGV(TAG, "term%d is %llu, error is %"PRId32, i, term[i], error);
    }

    return error;
}
#endif  //#if ESP_ADC_CAL_CURVE_FITTING_SUPPORTED

#endif  //#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
