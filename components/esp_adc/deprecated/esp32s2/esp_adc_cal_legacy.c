/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_types.h"
#include "soc/efuse_periph.h"
#include "esp_err.h"
#include "esp_check.h"
#include "assert.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_table.h"
#include "hal/adc_hal.h"
#include "hal/adc_types.h"
#include "driver/adc_types_legacy.h"
#include "esp_adc_cal_types_legacy.h"

const static char LOG_TAG[] = "adc_calib";

/* ------------------------ Characterization Constants ---------------------- */

// coeff_a and coeff_b are actually floats
// they are scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;
static const int coeff_b_scaling = 1024;
/* -------------------- Characterization Helper Data Types ------------------ */
typedef struct {
    int adc_calib_high;
    int adc_calib_low;
} adc_calib_data_ver1;

typedef struct {
    int adc_calib_high;         // the reading of adc ...
    int adc_calib_high_voltage; // ... at this voltage (mV)
} adc_calib_data_ver2;

typedef struct {
    char version_num;
    adc_unit_t adc_num;
    adc_atten_t atten_level;
    union {
        adc_calib_data_ver1 ver1;
        adc_calib_data_ver2 ver2;
    } efuse_data;
} adc_calib_parsed_info;

static bool prepare_calib_data_for(adc_unit_t adc_num, adc_atten_t atten, adc_calib_parsed_info *parsed_data_storage)
{
    int version_num = esp_efuse_rtc_table_read_calib_version();
    int tag;
    parsed_data_storage->version_num = version_num;
    parsed_data_storage->adc_num = adc_num;
    parsed_data_storage->atten_level = atten;
    switch (version_num) {
    case 1:
        // note: use the adc_num as in hal, which start from 0.
        tag = esp_efuse_rtc_table_get_tag(version_num, adc_num, atten, RTCCALIB_V1_PARAM_VLOW);
        parsed_data_storage->efuse_data.ver1.adc_calib_low = esp_efuse_rtc_table_get_parsed_efuse_value(tag, false);
        tag = esp_efuse_rtc_table_get_tag(version_num, adc_num, atten, RTCCALIB_V1_PARAM_VHIGH);
        parsed_data_storage->efuse_data.ver1.adc_calib_high = esp_efuse_rtc_table_get_parsed_efuse_value(tag, false);
        break;
    case 2:
        tag = esp_efuse_rtc_table_get_tag(version_num, adc_num, atten, RTCCALIB_V2_PARAM_VHIGH);
        parsed_data_storage->efuse_data.ver2.adc_calib_high = esp_efuse_rtc_table_get_parsed_efuse_value(tag, false);
        switch (parsed_data_storage->atten_level) {
        case ADC_ATTEN_DB_0:
            parsed_data_storage->efuse_data.ver2.adc_calib_high_voltage = 600;
            break;
        case ADC_ATTEN_DB_2_5:
            parsed_data_storage->efuse_data.ver2.adc_calib_high_voltage = 800;
            break;
        case ADC_ATTEN_DB_6:
            parsed_data_storage->efuse_data.ver2.adc_calib_high_voltage = 1000;
            break;
        case ADC_ATTEN_DB_12:
            parsed_data_storage->efuse_data.ver2.adc_calib_high_voltage = 2000;
            break;
        default:
            break;
        }
        break;
    default:
        // fall back to case 1 with zeros as params.
        parsed_data_storage->version_num = 1;
        tag = esp_efuse_rtc_table_get_tag(version_num, adc_num, atten, RTCCALIB_V1_PARAM_VLOW);
        parsed_data_storage->efuse_data.ver1.adc_calib_high = esp_efuse_rtc_table_get_parsed_efuse_value(tag, true);
        tag = esp_efuse_rtc_table_get_tag(version_num, adc_num, atten, RTCCALIB_V1_PARAM_VHIGH);
        parsed_data_storage->efuse_data.ver1.adc_calib_low = esp_efuse_rtc_table_get_parsed_efuse_value(tag, true);
        break;
    }
    return true;
}

/* ----------------------- Characterization Functions ----------------------- */
/**
 *  (Used in V1 of calibration scheme)
 *  The Two Point calibration measures the reading at two specific input voltages, and calculates the (assumed linear) relation
 *  between input voltage and ADC response. (Response = A * Vinput + B)
 *  A and B are scaled ints.
 *  @param high The ADC response at the higher voltage of the corresponding attenuation (600mV, 800mV, 1000mV, 2000mV).
 *  @param low The ADC response at the lower voltage of the corresponding attenuation (all 250mV).
 *
 */
static void characterize_using_two_point(adc_unit_t adc_num,
                                         adc_atten_t atten,
                                         uint32_t high,
                                         uint32_t low,
                                         uint32_t *coeff_a,
                                         uint32_t *coeff_b)
{
    // once we have recovered the reference high(Dhigh) and low(Dlow) readings, we can calculate a and b from
    // the measured high and low readings
    static const uint32_t v_high[] = {600, 800, 1000, 2000};
    static const uint32_t v_low = 250;
    *coeff_a = coeff_a_scaling * (v_high[atten] - v_low) / (high - low);
    *coeff_b = coeff_b_scaling * (v_low * high - v_high[atten] * low) / (high - low);
}

/*
 * Estimate the (assumed) linear relationship btwn the measured raw value and the voltage
 * with the previously done measurement when the chip was manufactured.
 * */
static bool calculate_characterization_coefficients(const adc_calib_parsed_info *parsed_data, esp_adc_cal_characteristics_t *chars)
{
    switch (parsed_data->version_num) {
    case 1:
        ESP_LOGD(LOG_TAG, "Calib V1, low%dmV, high%dmV", parsed_data->efuse_data.ver1.adc_calib_low, parsed_data->efuse_data.ver1.adc_calib_high);

        characterize_using_two_point(parsed_data->adc_num, parsed_data->atten_level,
                                     parsed_data->efuse_data.ver1.adc_calib_high, parsed_data->efuse_data.ver1.adc_calib_low,
                                     &(chars->coeff_a), &(chars->coeff_b));
        break;
    case 2:
        ESP_LOGD(LOG_TAG, "Calib V2, volt%dmV", parsed_data->efuse_data.ver2.adc_calib_high);
        chars->coeff_a = coeff_a_scaling * parsed_data->efuse_data.ver2.adc_calib_high_voltage /
                         parsed_data->efuse_data.ver2.adc_calib_high;
        chars->coeff_b = 0;
        break;
    default:
        return false;
        break;
    }
    return true;
}

/* ------------------------- Public API ------------------------------------- */
esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t source)
{
    if (source != ESP_ADC_CAL_VAL_EFUSE_TP) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint8_t adc_encoding_version = esp_efuse_rtc_table_read_calib_version();
    if (adc_encoding_version != 1 && adc_encoding_version != 2) {
        // current version only accepts encoding ver 1 and ver 2.
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
    bool res __attribute__((unused));
    adc_calib_parsed_info efuse_parsed_data = {0};
    // Check parameters
    assert((adc_num == ADC_UNIT_1) || (adc_num == ADC_UNIT_2));
    assert(chars != NULL);
    assert(bit_width == ADC_WIDTH_BIT_13);

    // make sure adc is calibrated.
    res = prepare_calib_data_for(adc_num, atten, &efuse_parsed_data);
    assert(res);
    res = calculate_characterization_coefficients(&efuse_parsed_data, chars);
    assert(res);
    ESP_LOGD(LOG_TAG, "adc%d (atten leven %d) calibration done: A:%" PRId32 " B:%" PRId32, adc_num, atten, chars->coeff_a, chars->coeff_b);

    // Initialize remaining fields
    chars->adc_num = adc_num;
    chars->atten = atten;
    chars->bit_width = bit_width;

    // these values are not used as the corresponding calibration themes are deprecated.
    chars->vref = 0;
    chars->low_curve = NULL;
    chars->high_curve = NULL;

    // in esp32s2 we only use the two point method to calibrate the adc.
    return ESP_ADC_CAL_VAL_EFUSE_TP;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);
    return adc_reading * chars->coeff_a / coeff_a_scaling + chars->coeff_b / coeff_b_scaling;
}
