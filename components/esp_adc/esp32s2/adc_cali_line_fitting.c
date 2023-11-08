/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "assert.h"
#include "esp_types.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_table.h"
#include "hal/adc_types.h"
#include "soc/efuse_periph.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc_cali_interface.h"

const __attribute__((unused)) static char *TAG = "adc_cali";

/* ------------------------ Characterization Constants ---------------------- */
// coeff_a and coeff_b are actually floats
// they are scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;
static const int coeff_b_scaling = 1024;

/* -------------------- Characterization Helper Data Types ------------------ */
typedef struct {
    int adc_calib_high;
    int adc_calib_low;
} adc_calib_data_ver1_t;

typedef struct {
    int adc_calib_high;         // the reading of adc ...
    int adc_calib_high_voltage; // ... at this voltage (mV)
} adc_calib_data_ver2_t;

typedef struct {
    char version_num;
    adc_unit_t unit_id;
    adc_atten_t atten_level;
    union {
        adc_calib_data_ver1_t ver1;
        adc_calib_data_ver2_t ver2;
    } efuse_data;
} adc_calib_parsed_info_t;

/* ------------------------ Context Structure--------------------------- */
typedef struct {
    adc_unit_t unit_id;                     ///< ADC unit
    adc_atten_t atten;                      ///< ADC attenuation
    uint32_t coeff_a;                       ///< Gradient of ADC-Voltage curve
    uint32_t coeff_b;                       ///< Offset of ADC-Voltage curve
} cali_chars_line_fitting_t;

/* ----------------------- Characterization Functions ----------------------- */
static bool prepare_calib_data_for(adc_unit_t unit_id, adc_atten_t atten, adc_calib_parsed_info_t *parsed_data_storage);
/**
 *  (Used in V1 of calibration scheme)
 *  The Two Point calibration measures the reading at two specific input voltages, and calculates the (assumed linear) relation
 *  between input voltage and ADC response. (Response = A * Vinput + B)
 *  A and B are scaled ints.
 *  @param high The ADC response at the higher voltage of the corresponding attenuation (600mV, 800mV, 1000mV, 2000mV).
 *  @param low The ADC response at the lower voltage of the corresponding attenuation (all 250mV).
 *
 */
static void characterize_using_two_point(adc_unit_t unit_id,
                                         adc_atten_t atten,
                                         uint32_t high,
                                         uint32_t low,
                                         uint32_t *coeff_a,
                                         uint32_t *coeff_b);
/*
 * Estimate the (assumed) linear relationship btwn the measured raw value and the voltage
 * with the previously done measurement when the chip was manufactured.
 * */
static bool calculate_characterization_coefficients(const adc_calib_parsed_info_t *parsed_data, cali_chars_line_fitting_t *ctx);

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t cali_raw_to_voltage(void *arg, int raw, int *voltage);

/* ------------------------- Public API ------------------------------------- */
esp_err_t adc_cali_create_scheme_line_fitting(const adc_cali_line_fitting_config_t *config, adc_cali_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");
    ESP_RETURN_ON_FALSE(config->unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC unit");
    ESP_RETURN_ON_FALSE(config->atten < SOC_ADC_ATTEN_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC attenuation");
    //S2 Oneshot read only supports 13 bits, DMA read only supports 12 bits
    ESP_RETURN_ON_FALSE(((config->bitwidth == SOC_ADC_RTC_MAX_BITWIDTH || config->bitwidth == SOC_ADC_DIGI_MAX_BITWIDTH) || config->bitwidth == ADC_BITWIDTH_DEFAULT), ESP_ERR_INVALID_ARG, TAG, "invalid bitwidth");
    // current version only accepts encoding ver 1 and ver 2.
    uint8_t adc_encoding_version = esp_efuse_rtc_table_read_calib_version();
    ESP_RETURN_ON_FALSE(((adc_encoding_version == 1) || (adc_encoding_version == 2)), ESP_ERR_NOT_SUPPORTED, TAG, "Calibration required eFuse bits not burnt");

    adc_cali_scheme_t *scheme = (adc_cali_scheme_t *)heap_caps_calloc(1, sizeof(adc_cali_scheme_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_NO_MEM, TAG, "no mem for adc calibration scheme");

    cali_chars_line_fitting_t *chars = (cali_chars_line_fitting_t *)heap_caps_calloc(1, sizeof(cali_chars_line_fitting_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(chars, ESP_ERR_NO_MEM, err, TAG, "no memory for the calibration characteristics");

    scheme->raw_to_voltage = cali_raw_to_voltage;
    scheme->ctx = chars;

    adc_calib_parsed_info_t efuse_parsed_data = {0};
    bool success = prepare_calib_data_for(config->unit_id, config->atten, &efuse_parsed_data);
    assert(success);
    success = calculate_characterization_coefficients(&efuse_parsed_data, chars);
    assert(success);
    ESP_LOGD(TAG, "adc%d (atten leven %d) calibration done: A:%" PRId32" B:%" PRId32, config->unit_id, config->atten, chars->coeff_a, chars->coeff_b);
    chars->unit_id = config->unit_id;
    chars->atten = config->atten;

    *ret_handle = scheme;

    return ESP_OK;

err:
    if (scheme) {
        free(scheme);
    }
    return ret;
}

esp_err_t adc_cali_delete_scheme_line_fitting(adc_cali_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    free(handle->ctx);
    handle->ctx = NULL;

    free(handle);
    handle = NULL;

    return ESP_OK;
}

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t cali_raw_to_voltage(void *arg, int raw, int *voltage)
{
    //pointers are checked in the upper layer

    cali_chars_line_fitting_t *ctx = arg;
    *voltage = raw * ctx->coeff_a / coeff_a_scaling + ctx->coeff_b / coeff_b_scaling;

    return ESP_OK;
}

/* ----------------------- Characterization Functions ----------------------- */
static bool prepare_calib_data_for(adc_unit_t unit_id, adc_atten_t atten, adc_calib_parsed_info_t *parsed_data_storage)
{
    int version_num = esp_efuse_rtc_table_read_calib_version();
    int tag;
    parsed_data_storage->version_num = version_num;
    parsed_data_storage->unit_id = unit_id;
    parsed_data_storage->atten_level = atten;
    switch (version_num) {
    case 1:
        // note: use the unit_id as in hal, which start from 0.
        tag = esp_efuse_rtc_table_get_tag(version_num, unit_id, atten, RTCCALIB_V1_PARAM_VLOW);
        parsed_data_storage->efuse_data.ver1.adc_calib_low = esp_efuse_rtc_table_get_parsed_efuse_value(tag, false);
        tag = esp_efuse_rtc_table_get_tag(version_num, unit_id, atten, RTCCALIB_V1_PARAM_VHIGH);
        parsed_data_storage->efuse_data.ver1.adc_calib_high = esp_efuse_rtc_table_get_parsed_efuse_value(tag, false);
        break;
    case 2:
        tag = esp_efuse_rtc_table_get_tag(version_num, unit_id, atten, RTCCALIB_V2_PARAM_VHIGH);
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
        tag = esp_efuse_rtc_table_get_tag(version_num, unit_id, atten, RTCCALIB_V1_PARAM_VLOW);
        parsed_data_storage->efuse_data.ver1.adc_calib_high = esp_efuse_rtc_table_get_parsed_efuse_value(tag, true);
        tag = esp_efuse_rtc_table_get_tag(version_num, unit_id, atten, RTCCALIB_V1_PARAM_VHIGH);
        parsed_data_storage->efuse_data.ver1.adc_calib_low = esp_efuse_rtc_table_get_parsed_efuse_value(tag, true);
        break;
    }
    return true;
}

/**
 *  (Used in V1 of calibration scheme)
 *  The Two Point calibration measures the reading at two specific input voltages, and calculates the (assumed linear) relation
 *  between input voltage and ADC response. (Response = A * Vinput + B)
 *  A and B are scaled ints.
 *  @param high The ADC response at the higher voltage of the corresponding attenuation (600mV, 800mV, 1000mV, 2000mV).
 *  @param low The ADC response at the lower voltage of the corresponding attenuation (all 250mV).
 *
 */
static void characterize_using_two_point(adc_unit_t unit_id,
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
static bool calculate_characterization_coefficients(const adc_calib_parsed_info_t *parsed_data, cali_chars_line_fitting_t *ctx)
{
    switch (parsed_data->version_num) {
    case 1:
        ESP_LOGD(TAG, "Calib V1, low%dmV, high%dmV", parsed_data->efuse_data.ver1.adc_calib_low, parsed_data->efuse_data.ver1.adc_calib_high);

        characterize_using_two_point(parsed_data->unit_id, parsed_data->atten_level,
                                     parsed_data->efuse_data.ver1.adc_calib_high, parsed_data->efuse_data.ver1.adc_calib_low,
                                     &(ctx->coeff_a), &(ctx->coeff_b));
        break;
    case 2:
        ESP_LOGD(TAG, "Calib V2, volt%dmV", parsed_data->efuse_data.ver2.adc_calib_high);
        ctx->coeff_a = coeff_a_scaling * parsed_data->efuse_data.ver2.adc_calib_high_voltage /
                       parsed_data->efuse_data.ver2.adc_calib_high;
        ctx->coeff_b = 0;
        break;
    default:
        return false;
        break;
    }
    return true;
}
