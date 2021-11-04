/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "hal/adc_types.h"
#include "esp_adc_cal.h"

#define ADC_CAL_CHECK(cond, ret) ({                                         \
            if(!(cond)){                                                    \
                return ret;                                                 \
            }                                                               \
})

const __attribute__((unused)) static char *TAG = "ADC_CALI";

esp_err_t esp_adc_cal_get_voltage(adc_channel_t channel,
                                  const esp_adc_cal_characteristics_t *chars,
                                  uint32_t *voltage)
{
    // Check parameters
    ADC_CAL_CHECK(chars != NULL, ESP_ERR_INVALID_ARG);
    ADC_CAL_CHECK(voltage != NULL, ESP_ERR_INVALID_ARG);

    esp_err_t ret = ESP_OK;
    int adc_reading;
    if (chars->adc_num == ADC_UNIT_1) {
        ADC_CAL_CHECK(channel < SOC_ADC_CHANNEL_NUM(0), ESP_ERR_INVALID_ARG);
        adc_reading = adc1_get_raw(channel);
    } else {
        ADC_CAL_CHECK(channel < SOC_ADC_CHANNEL_NUM(1), ESP_ERR_INVALID_ARG);
        ret = adc2_get_raw(channel, chars->bit_width, &adc_reading);
    }

    if (ret == ESP_OK) {
        *voltage = esp_adc_cal_raw_to_voltage((uint32_t)adc_reading, chars);
    }
    return ret;
}
