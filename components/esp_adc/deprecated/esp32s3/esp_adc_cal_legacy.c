/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/adc_types.h"
#include "esp_efuse_rtc_calib.h"
#include "hal/adc_types.h"
#include "driver/adc_types_legacy.h"
#include "esp_adc_cal_types_legacy.h"
#include "../esp_adc_cal_internal_legacy.h"

const static char LOG_TAG[] = "ADC_CALI";

/* ------------------------ Characterization Constants ---------------------- */

//coeff_a is actually a float number
//it is scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 1000000;

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
 */
const static uint64_t adc1_error_coef_atten[4][5][2] = {
    {{27856531419538344, 1e16}, {50871540569528, 1e16}, {9798249589, 1e15}, {0, 0}, {0, 0}},                       //ADC1 atten0
    {{29831022915028695, 1e16}, {49393185868806, 1e16}, {101379430548, 1e16}, {0, 0}, {0, 0}},                     //ADC1 atten1
    {{23285545746296417, 1e16}, {147640181047414, 1e16}, {208385525314, 1e16}, {0, 0}, {0, 0}},                    //ADC1 atten2
    {{644403418269478, 1e15}, {644334888647536, 1e16}, {1297891447611, 1e16}, {70769718, 1e15}, {13515, 1e15}}     //ADC1 atten3
};
const static uint64_t adc2_error_coef_atten[4][5][2] = {
    {{25668651654328927, 1e16}, {1353548869615, 1e16}, {36615265189, 1e16}, {0, 0}, {0, 0}},                       //ADC2 atten0
    {{23690184690298404, 1e16}, {66319894226185, 1e16}, {118964995959, 1e16}, {0, 0}, {0, 0}},                     //ADC2 atten1
    {{9452499397020617, 1e16}, {200996773954387, 1e16}, {259011467956, 1e16}, {0, 0}, {0, 0}},                     //ADC2 atten2
    {{12247719764336924, 1e16}, {755717904943462, 1e16}, {1478791187119, 1e16}, {79672528, 1e15}, {15038, 1e15}}   //ADC2 atten3
};
/**
 * Term sign
 */
const static int32_t adc1_error_sign[4][5] = {
    {-1, -1, 1, 0,  0},  //ADC1 atten0
    {-1, -1, 1, 0,  0},  //ADC1 atten1
    {-1, -1, 1, 0,  0},  //ADC1 atten2
    {-1, -1, 1, -1, 1}   //ADC1 atten3
};
const static int32_t adc2_error_sign[4][5] = {
    {-1,  1, 1,  0, 0},  //ADC2 atten0
    {-1, -1, 1,  0, 0},  //ADC2 atten1
    {-1, -1, 1,  0, 0},  //ADC2 atten2
    { 1, -1, 1, -1, 1}   //ADC2 atten3
};

/* -------------------- Characterization Helper Data Types ------------------ */
typedef struct {
    uint32_t voltage;
    uint32_t digi;
} adc_calib_data_ver1_t;

typedef struct {
    char version_num;
    adc_unit_t adc_num;
    adc_atten_t atten_level;
    union {
        adc_calib_data_ver1_t ver1;
    } ref_data;
} adc_calib_info_t;

//To get the reference point (Dout, Vin)
static esp_err_t get_reference_point(int version_num, adc_unit_t adc_num, adc_atten_t atten, adc_calib_info_t *calib_info)
{
    assert(version_num == 1);
    esp_err_t ret;

    calib_info->version_num = version_num;
    calib_info->adc_num = adc_num;
    calib_info->atten_level = atten;

    uint32_t voltage = 0;
    uint32_t digi = 0;
    ret = esp_efuse_rtc_calib_get_cal_voltage(version_num, adc_num, atten, &digi, &voltage);
    assert(ret == ESP_OK);
    calib_info->ref_data.ver1.voltage = voltage;
    calib_info->ref_data.ver1.digi = digi;
    return ret;
}

esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t source)
{
    if (source != ESP_ADC_CAL_VAL_EFUSE_TP_FIT) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint8_t adc_encoding_version = esp_efuse_rtc_calib_get_ver();
    if (adc_encoding_version != 1) {
        // current version only accepts encoding ver 1.
        return ESP_ERR_INVALID_VERSION;
    }
    return ESP_OK;
}

/*
 * Get an expected linear relationship btwn Vin and Dout
 */
static void calculate_characterization_coefficients(const adc_calib_info_t *parsed_data, esp_adc_cal_characteristics_t *chars)
{
    chars->coeff_a = coeff_a_scaling * parsed_data->ref_data.ver1.voltage / parsed_data->ref_data.ver1.digi;
    chars->coeff_b = 0;
    ESP_LOGV(LOG_TAG, "Calib V1, Cal Voltage = %" PRId32 ", Digi out = %" PRId32 ", Coef_a = %" PRId32, parsed_data->ref_data.ver1.voltage, parsed_data->ref_data.ver1.digi, chars->coeff_a);
}

esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
                                             adc_atten_t atten,
                                             adc_bits_width_t bit_width,
                                             uint32_t default_vref,
                                             esp_adc_cal_characteristics_t *chars)
{
    (void) default_vref;

    // Check parameters
    ESP_RETURN_ON_FALSE(adc_num == ADC_UNIT_1 || adc_num == ADC_UNIT_2, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid unit num");
    ESP_RETURN_ON_FALSE(chars != NULL, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid characteristic");
    ESP_RETURN_ON_FALSE(atten < SOC_ADC_ATTEN_NUM, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "Invalid attenuation");

    int version_num = esp_efuse_rtc_calib_get_ver();
    ESP_RETURN_ON_FALSE(version_num == 1, ESP_ADC_CAL_VAL_NOT_SUPPORTED, LOG_TAG, "No calibration efuse burnt");

    memset(chars, 0, sizeof(esp_adc_cal_characteristics_t));

    adc_calib_info_t calib_info = {0};
    // make sure adc is calibrated.
    get_reference_point(version_num, adc_num, atten, &calib_info);
    calculate_characterization_coefficients(&calib_info, chars);

    // Initialize remaining fields
    chars->adc_num = adc_num;
    chars->atten = atten;
    chars->bit_width = bit_width;

    return ESP_ADC_CAL_VAL_EFUSE_TP_FIT;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);

    //ADC reading won't exceed 4096. Otherwise the raw reading result is wrong, the next calculation will overflow.
    assert(adc_reading < 4096);

    uint32_t voltage = 0;
    int32_t error = 0;
    uint64_t v_cali_1 = 0;

    //raw * gradient * 1000000
    v_cali_1 = (uint64_t)adc_reading * chars->coeff_a;
    //convert to real number
    v_cali_1 = v_cali_1 / coeff_a_scaling;
    ESP_LOGV(LOG_TAG, "v_cali_1 is %llu", v_cali_1);

    //Curve Fitting error correction
    esp_adc_error_calc_param_t param = {
        .v_cali_input = v_cali_1,
        .term_num = (chars->atten == 3) ? 5 : 3,
        .coeff = (chars->adc_num == ADC_UNIT_1) ? &adc1_error_coef_atten :&adc2_error_coef_atten,
        .sign = (chars->adc_num == ADC_UNIT_1) ? &adc1_error_sign :&adc2_error_sign,
    };
    error = esp_adc_cal_get_reading_error(&param, chars->atten);

    voltage = (int32_t)v_cali_1 - error;

    return voltage;
}
