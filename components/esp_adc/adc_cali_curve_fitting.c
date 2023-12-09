/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc_cali_interface.h"
#include "esp_private/adc_share_hw_ctrl.h"

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
#include "esp_efuse_rtc_calib.h"
#include "curve_fitting_coefficients.h"

const __attribute__((unused)) static char *TAG = "adc_cali";

// coeff_a is actually a float number
// it is scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;

/* -------------------- Characterization Helper Data Types ------------------ */
typedef struct {
    uint32_t voltage;
    uint32_t digi;
} adc_calib_data_ver1_t;

typedef struct {
    char version_num;
    adc_unit_t unit_id;
    adc_atten_t atten;
    union {
        adc_calib_data_ver1_t ver1;
    } ref_data;
} adc_calib_info_t;

/* ------------------------ Context Structure--------------------------- */
typedef struct {
    uint32_t coeff_a;    ///< Gradient of ADC-Voltage curve
    uint32_t coeff_b;    ///< Offset of ADC-Voltage curve
} cali_chars_first_step_t;

typedef struct {
    adc_unit_t unit_id;                            ///< ADC unit
    adc_channel_t chan;                            ///< ADC channel
    adc_atten_t atten;                             ///< ADC attenuation
    cali_chars_first_step_t chars_first_step;      ///< Calibration first step characteristics
    cali_chars_second_step_t chars_second_step;    ///< Calibration second step characteristics
} cali_chars_curve_fitting_t;

/* ----------------------- Characterization Functions ----------------------- */
static void get_first_step_reference_point(int version_num, adc_unit_t unit_id, adc_atten_t atten, adc_calib_info_t *calib_info);
static void calc_first_step_coefficients(const adc_calib_info_t *parsed_data, cali_chars_curve_fitting_t *chars);
static int32_t get_reading_error(uint64_t v_cali_1, const cali_chars_second_step_t *param);
static esp_err_t check_valid(const adc_cali_curve_fitting_config_t *config);

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t cali_raw_to_voltage(void *arg, int raw, int *voltage);

/* ------------------------- Public API ------------------------------------- */
esp_err_t adc_cali_create_scheme_curve_fitting(const adc_cali_curve_fitting_config_t *config, adc_cali_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");
    ret = check_valid(config);
    if (ret != ESP_OK) {
        return ret;
    }
    // current version only accepts encoding version: ESP_EFUSE_ADC_CALIB_VER_MIN <= adc_encoding_version <= ESP_EFUSE_ADC_CALIB_VER_MAX.
    uint32_t adc_encoding_version = esp_efuse_rtc_calib_get_ver();
    ESP_RETURN_ON_FALSE((adc_encoding_version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
                        (adc_encoding_version <= ESP_EFUSE_ADC_CALIB_VER_MAX), ESP_ERR_NOT_SUPPORTED, TAG, "Calibration required eFuse bits not burnt");

    adc_cali_scheme_t *scheme = (adc_cali_scheme_t *)heap_caps_calloc(1, sizeof(adc_cali_scheme_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_NO_MEM, TAG, "no mem for adc calibration scheme");

    cali_chars_curve_fitting_t *chars = (cali_chars_curve_fitting_t *)heap_caps_calloc(1, sizeof(cali_chars_curve_fitting_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(chars, ESP_ERR_NO_MEM, err, TAG, "no memory for the calibration characteristics");

    scheme->raw_to_voltage = cali_raw_to_voltage;
    scheme->ctx = chars;

    //Prepare calibration characteristics
    adc_calib_info_t calib_info = {0};
    //Set first step calibration context
    get_first_step_reference_point(adc_encoding_version, config->unit_id, config->atten, &calib_info);
    calc_first_step_coefficients(&calib_info, chars);
    //Set second step calibration context
    curve_fitting_get_second_step_coeff(config, &(chars->chars_second_step));
    chars->unit_id = config->unit_id;
    chars->chan = config->chan;
    chars->atten = config->atten;

    *ret_handle = scheme;

    return ESP_OK;

err:
    if (scheme) {
        free(scheme);
    }
    return ret;
}

esp_err_t adc_cali_delete_scheme_curve_fitting(adc_cali_handle_t handle)
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

    cali_chars_curve_fitting_t *ctx = arg;

#if SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED
    int chan_compensation = adc_get_hw_calibration_chan_compens(ctx->unit_id, ctx->chan, ctx->atten);
    raw -= chan_compensation;
    /* Limit the range */
    int max_val = (1L << SOC_ADC_RTC_MAX_BITWIDTH) - 1;
    raw = raw <= 0 ? 0 :
          raw > max_val ? max_val : raw;
#endif  // SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED

    uint64_t v_cali_1 = (uint64_t)raw * ctx->chars_first_step.coeff_a / coeff_a_scaling + ctx->chars_first_step.coeff_b;
    int32_t error = get_reading_error(v_cali_1, &(ctx->chars_second_step));

    *voltage = (int32_t)v_cali_1 - error;

    return ESP_OK;
}

/* ----------------------- Characterization Functions ----------------------- */
//To get the reference point (Dout, Vin)
static void get_first_step_reference_point(int version_num, adc_unit_t unit_id, adc_atten_t atten, adc_calib_info_t *calib_info)
{
    assert((version_num >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (version_num <= ESP_EFUSE_ADC_CALIB_VER_MAX));
    esp_err_t ret;

    calib_info->version_num = version_num;
    calib_info->unit_id = unit_id;
    calib_info->atten = atten;

    uint32_t voltage = 0;
    uint32_t digi = 0;
    ret = esp_efuse_rtc_calib_get_cal_voltage(version_num, unit_id, (int)atten, &digi, &voltage);
    assert(ret == ESP_OK);
    calib_info->ref_data.ver1.voltage = voltage;
    calib_info->ref_data.ver1.digi = digi;
}

/*
 * Estimate the (assumed) linear relationship btwn the measured raw value and the voltage
 * with the previously done measurement when the chip was manufactured.
 */
static void calc_first_step_coefficients(const adc_calib_info_t *parsed_data, cali_chars_curve_fitting_t *ctx)
{
    ctx->chars_first_step.coeff_a = coeff_a_scaling * parsed_data->ref_data.ver1.voltage / parsed_data->ref_data.ver1.digi;
    ctx->chars_first_step.coeff_b = 0;
    ESP_LOGV(TAG, "Calib V1, Cal Voltage = %" PRId32 ", Digi out = %" PRId32 ", Coef_a = %" PRId32, parsed_data->ref_data.ver1.voltage, parsed_data->ref_data.ver1.digi, ctx->chars_first_step.coeff_a);
}

static int32_t get_reading_error(uint64_t v_cali_1, const cali_chars_second_step_t *param)
{
    if (v_cali_1 == 0 || param->term_num == 0) {
        return 0;
    }

    uint8_t term_num = param->term_num;
    int32_t error = 0;
    uint64_t coeff = 0;
    uint64_t variable[term_num];
    uint64_t term[term_num];
    memset(variable, 0, term_num * sizeof(uint64_t));
    memset(term, 0, term_num * sizeof(uint64_t));

    /**
     *  The scheme formula is:
     *  error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + ... +  (Kn * X^n);
     */
    variable[0] = 1;
    coeff = (param->coeff)[0][0];
    term[0] = variable[0] * coeff / (param->coeff)[0][1];
    error = (int32_t)term[0] * (param->sign)[0];

    for (int i = 1; i < term_num; i++) {
        variable[i] = variable[i - 1] * v_cali_1;
        coeff = (param->coeff)[i][0];
        term[i] = variable[i] * coeff;
        ESP_LOGV(TAG, "big coef is %llu, big term%d is %llu, coef_id is %d", coeff, i, term[i], i);

        term[i] = term[i] / (param->coeff)[i][1];
        error += (int32_t)term[i] * (param->sign)[i];
        ESP_LOGV(TAG, "term%d is %llu, error is %"PRId32, i, term[i], error);
    }

    return error;
}

static esp_err_t check_valid(const adc_cali_curve_fitting_config_t *config)
{
    ESP_RETURN_ON_FALSE(config->unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC unit");
    ESP_RETURN_ON_FALSE(config->atten < SOC_ADC_ATTEN_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC attenuation");

    bool available_oneshot_bitwidth = (config->bitwidth >= SOC_ADC_RTC_MIN_BITWIDTH && config->bitwidth <= SOC_ADC_RTC_MAX_BITWIDTH);
    bool available_dma_bitwidth = (config->bitwidth >= SOC_ADC_DIGI_MIN_BITWIDTH && config->bitwidth <= SOC_ADC_DIGI_MAX_BITWIDTH);
    bool default_bitwidth_mark = (config->bitwidth == ADC_BITWIDTH_DEFAULT);
    bool available_bitwidth = (available_oneshot_bitwidth || available_dma_bitwidth || default_bitwidth_mark);
    ESP_RETURN_ON_FALSE(available_bitwidth, ESP_ERR_INVALID_ARG, TAG, "invalid bitwidth");

    return ESP_OK;
}

#endif  //#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
