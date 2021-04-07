// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "hal/adc_ll.h"
#include "esp_efuse_rtc_calib.h"
#include "esp_adc_cal.h"


#define ADC_CALIB_CHECK(cond, err_msg, ret) do {\
             if (!(cond)) { \
                ESP_LOGE(LOG_TAG, err_msg); \
                return (ret); \
            } \
        } while(0)

const static char LOG_TAG[] = "adc_calib";


/* ------------------------ Characterization Constants ---------------------- */

// coeff_a and coeff_b are actually floats
// they are scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;
static const int coeff_b_scaling = 1024;
/* -------------------- Characterization Helper Data Types ------------------ */
typedef struct {
    uint32_t voltage;
    uint32_t digi;
} adc_calib_data_ver1;

typedef struct {
    char version_num;
    adc_unit_t adc_num;
    adc_atten_t atten_level;
    union {
        adc_calib_data_ver1 ver1;
    } efuse_data;
} adc_calib_parsed_info;

static esp_err_t prepare_calib_data_for(int version_num, adc_unit_t adc_num, adc_atten_t atten, adc_calib_parsed_info *parsed_data_storage)
{
    assert(version_num == 1);
    esp_err_t ret;

    parsed_data_storage->version_num = version_num;
    parsed_data_storage->adc_num = adc_num;
    parsed_data_storage->atten_level = atten;
    // V1 we don't have calibration data for ADC2, using the efuse data of ADC1
    uint32_t voltage, digi;
    ret = esp_efuse_rtc_calib_get_cal_voltage(version_num, atten, &digi, &voltage);
    if (ret != ESP_OK) {
        return ret;
    }
    parsed_data_storage->efuse_data.ver1.voltage = voltage;
    parsed_data_storage->efuse_data.ver1.digi = digi;
    return ret;
}

/* ----------------------- Characterization Functions ----------------------- */
/*
 * Estimate the (assumed) linear relationship btwn the measured raw value and the voltage
 * with the previously done measurement when the chip was manufactured.
 */
static void calculate_characterization_coefficients(const adc_calib_parsed_info *parsed_data, esp_adc_cal_characteristics_t *chars)
{
    ESP_LOGD(LOG_TAG, "Calib V1, Cal Voltage = %d, Digi out = %d\n", parsed_data->efuse_data.ver1.voltage, parsed_data->efuse_data.ver1.digi);

    chars->coeff_a = coeff_a_scaling * parsed_data->efuse_data.ver1.voltage / parsed_data->efuse_data.ver1.digi;
    chars->coeff_b = 0;
}

/* ------------------------- Public API ------------------------------------- */
esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t source)
{
    if (source != ESP_ADC_CAL_VAL_EFUSE_TP) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint8_t adc_encoding_version = esp_efuse_rtc_calib_get_ver();
    if (adc_encoding_version != 1) {
        // current version only accepts encoding ver 1.
        return ESP_ERR_INVALID_VERSION;
    }
    return ESP_OK;
}

esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
        adc_atten_t atten,
        adc_bits_width_t bit_width,
        uint32_t default_vref,
        esp_adc_cal_characteristics_t *chars)
{
    esp_err_t ret;
    adc_calib_parsed_info efuse_parsed_data = {0};
    // Check parameters
    ADC_CALIB_CHECK(adc_num == ADC_UNIT_1 || adc_num == ADC_UNIT_2, "Invalid unit num", ESP_ADC_CAL_VAL_NOT_SUPPORTED);
    ADC_CALIB_CHECK(chars != NULL, "Invalid characteristic", ESP_ADC_CAL_VAL_NOT_SUPPORTED);
    ADC_CALIB_CHECK(bit_width == ADC_WIDTH_BIT_12, "Invalid bit_width", ESP_ADC_CAL_VAL_NOT_SUPPORTED);
    ADC_CALIB_CHECK(atten < 4, "Invalid attenuation", ESP_ADC_CAL_VAL_NOT_SUPPORTED);

    int version_num = esp_efuse_rtc_calib_get_ver();
    ADC_CALIB_CHECK(version_num == 1, "No calibration efuse burnt", ESP_ADC_CAL_VAL_NOT_SUPPORTED);

    memset(chars, 0, sizeof(esp_adc_cal_characteristics_t));

    // make sure adc is calibrated.
    ret = prepare_calib_data_for(version_num, adc_num, atten, &efuse_parsed_data);
    if (ret != ESP_OK) {
        abort();
    }

    calculate_characterization_coefficients(&efuse_parsed_data, chars);
    ESP_LOGD(LOG_TAG, "adc%d (atten leven %d) calibration done: A:%d B:%d\n", adc_num, atten, chars->coeff_a, chars->coeff_b);

    // Initialize remaining fields
    chars->adc_num = adc_num;
    chars->atten = atten;
    chars->bit_width = bit_width;

    // in esp32c3 we only use the two point method to calibrate the adc.
    return ESP_ADC_CAL_VAL_EFUSE_TP;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    ADC_CALIB_CHECK(chars != NULL, "No characteristic input.", ESP_ERR_INVALID_ARG);

    return adc_reading * chars->coeff_a / coeff_a_scaling + chars->coeff_b / coeff_b_scaling;
}

esp_err_t esp_adc_cal_get_voltage(adc_channel_t channel,
                                  const esp_adc_cal_characteristics_t *chars,
                                  uint32_t *voltage)
{
    // Check parameters
    ADC_CALIB_CHECK(chars != NULL, "No characteristic input.", ESP_ERR_INVALID_ARG);
    ADC_CALIB_CHECK(voltage != NULL, "No output buffer.", ESP_ERR_INVALID_ARG);

    int adc_reading;
    if (chars->adc_num == ADC_UNIT_1) {
        //Check if channel is valid on ADC1
        ADC_CALIB_CHECK((adc1_channel_t)channel < ADC1_CHANNEL_MAX, "Invalid channel", ESP_ERR_INVALID_ARG);
        adc_reading = adc1_get_raw(channel);
    } else {
        //Check if channel is valid on ADC2
        ADC_CALIB_CHECK((adc2_channel_t)channel < ADC2_CHANNEL_MAX, "Invalid channel", ESP_ERR_INVALID_ARG);
        if (adc2_get_raw(channel, chars->bit_width, &adc_reading) != ESP_OK) {
            return ESP_ERR_TIMEOUT;     //Timed out waiting for ADC2
        }
    }
    *voltage = esp_adc_cal_raw_to_voltage((uint32_t)adc_reading, chars);
    return ESP_OK;
}
