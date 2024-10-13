/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_efuse_rtc_calib.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#include "driver/adc_types_legacy.h"
#include "esp_adc_cal_types_legacy.h"
#include "../esp_adc_cal_internal_legacy.h"

const static char LOG_TAG[] = "ADC_CALI";

/* ------------------------ Characterization Constants ---------------------- */

// coeff_a is actually a float number
// it is scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;

/**
 * @note Error Calculation
 * Coefficients for calculating the reading voltage error.
 * Four sets of coefficients for atten0 ~ atten3 respectively.
 *
 * For each item, first element is the Coefficient, second element is the Multiple. (Coefficient / Multiple) is the real coefficient.
 *
 * @note {0,0} stands for unused item
 * @note In case of the overflow, these coefficients are recorded as Absolute Value
 * @note For atten0 ~ 2, error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2); For atten3, error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + (K4 * X^4);
 * @note Above formula is rewritten from the original documentation, please note that the coefficients are re-ordered.
 * @note ADC1 and ADC2 use same coefficients
 */
const static uint64_t adc_error_coef_atten[4][5][2] = {
    {{225966470500043, 1e15}, {7265418501948, 1e16}, {109410402681, 1e16}, {0, 0}, {0, 0}},                         //atten0
    {{4229623392600516, 1e16}, {731527490903, 1e16}, {88166562521, 1e16}, {0, 0}, {0, 0}},                          //atten1
    {{1017859239236435, 1e15}, {97159265299153, 1e16}, {149794028038, 1e16}, {0, 0}, {0, 0}},                       //atten2
    {{14912262772850453, 1e16}, {228549975564099, 1e16}, {356391935717, 1e16}, {179964582, 1e16}, {42046, 1e16}}    //atten3
};
/**
 * Term sign
 * @note ADC1 and ADC2 use same coefficients
 */
const static int32_t adc_error_sign[4][5] = {
    {-1, -1, 1,  0,  0}, //atten0
    { 1, -1, 1,  0,  0}, //atten1
    {-1, -1, 1,  0,  0}, //atten2
    {-1, -1, 1, -1,  1}  //atten3
};

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
} adc_calib_parsed_info_t;

static esp_err_t prepare_calib_data_for(int version_num, adc_unit_t adc_num, adc_atten_t atten, adc_calib_parsed_info_t *parsed_data_storage)
{
    assert(version_num == 1);
    esp_err_t ret;

    parsed_data_storage->version_num = version_num;
    parsed_data_storage->adc_num = adc_num;
    parsed_data_storage->atten_level = atten;
    uint32_t voltage, digi;
    /**
     * V1 we don't have calibration data for ADC2, using the efuse data of ADC1.
     * Here passing the `adc_num` is just for compatibility
     */
    ret = esp_efuse_rtc_calib_get_cal_voltage(version_num, adc_num, atten, &digi, &voltage);
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
static void calculate_characterization_coefficients(const adc_calib_parsed_info_t *parsed_data, esp_adc_cal_characteristics_t *chars)
{
    ESP_LOGD(LOG_TAG, "Calib V1, Cal Voltage = %"PRId32", Digi out = %"PRId32, parsed_data->efuse_data.ver1.voltage, parsed_data->efuse_data.ver1.digi);

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
    adc_calib_parsed_info_t efuse_parsed_data = {0};
    // Check parameters
    ESP_RETURN_ON_FALSE(adc_num == ADC_UNIT_1 || adc_num == ADC_UNIT_2, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid unit num");
    ESP_RETURN_ON_FALSE(chars != NULL, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid characteristic");
    ESP_RETURN_ON_FALSE(bit_width == ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid bit_width");
    ESP_RETURN_ON_FALSE(atten < SOC_ADC_ATTEN_NUM, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid attenuation");

    int version_num = esp_efuse_rtc_calib_get_ver();
    ESP_RETURN_ON_FALSE(version_num == 1, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "No calibration efuse burnt");

    memset(chars, 0, sizeof(esp_adc_cal_characteristics_t));

    // make sure adc is calibrated.
    ret = prepare_calib_data_for(version_num, adc_num, atten, &efuse_parsed_data);
    if (ret != ESP_OK) {
        abort();
    }

    calculate_characterization_coefficients(&efuse_parsed_data, chars);
    ESP_LOGD(LOG_TAG, "adc%d (atten leven %d) calibration done: A:%"PRId32" B:%"PRId32, adc_num, atten, chars->coeff_a, chars->coeff_b);

    // Initialize remaining fields
    chars->adc_num = adc_num;
    chars->atten = atten;
    chars->bit_width = bit_width;

    // in esp32c3 we only use the two point method to calibrate the adc.
    return ESP_ADC_CAL_VAL_EFUSE_TP;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);

    int32_t error = 0;
    uint64_t v_cali_1 = (uint64_t)adc_reading * chars->coeff_a / coeff_a_scaling;
    esp_adc_error_calc_param_t param = {
        .v_cali_input = v_cali_1,
        .term_num = (chars->atten == 3) ? 5 : 3,
        .coeff = &adc_error_coef_atten,
        .sign = &adc_error_sign,
    };
    error = esp_adc_cal_get_reading_error(&param, chars->atten);

    return (int32_t)v_cali_1 - error;
}
