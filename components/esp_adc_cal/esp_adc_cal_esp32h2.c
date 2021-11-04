/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/adc.h"
#include "hal/adc_ll.h"
#include "esp_efuse_rtc_calib.h"
#include "esp_adc_cal.h"

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
    ESP_RETURN_ON_FALSE(adc_num == ADC_UNIT_1 || adc_num == ADC_UNIT_2, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid unit num");
    ESP_RETURN_ON_FALSE(chars != NULL, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Ivalid characteristic");
    ESP_RETURN_ON_FALSE(bit_width == ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid bit_width");
    ESP_RETURN_ON_FALSE(atten < 4, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid attenuation");

    int version_num = esp_efuse_rtc_calib_get_ver();
    ESP_RETURN_ON_FALSE(version_num == 1, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "No calibration efuse burnt");

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

    // in esp32h2 we only use the two point method to calibrate the adc.
    return ESP_ADC_CAL_VAL_EFUSE_TP;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);
    return adc_reading * chars->coeff_a / coeff_a_scaling + chars->coeff_b / coeff_b_scaling;
}
