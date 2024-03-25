/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_efuse_rtc_calib.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc_cali_interface.h"

/**
 * This file contains Line Fitting Calibration Scheme for ESP32C2.
 *
 * If ESP EFuse Line Fitting Calibration Scheme on future chips are similar to the scheme in this file, we can:
 *
 * 1. Rename this file to `adc_cali_line_fitting_v2.c`, as the Line Fitting Scheme on ESP32 and ESP32S2 are different to this.
 * 2. Move this file to common directory
 * 3. Still support `ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED`
 * 4. Add a new internal macro `ADC_CALI_SCHEME_LINE_FITTING_V2_SUPPORTED`
 * 5. Only build this file, when `ADC_CALI_SCHEME_LINE_FITTING_V2_SUPPORTED == true`
 */

// coeff_a is actually a float number
// it is scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;
const static char *TAG = "adc_cali";

typedef struct {
    adc_unit_t unit_id;
    adc_atten_t atten;
    uint32_t coeff_a;    ///< Gradient of ADC-Voltage characteristics
    uint32_t coeff_b;    ///< Offset of ADC-Voltage characteristics
} cali_chars_line_fitting_t;

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t cali_raw_to_voltage(void *arg, int raw, int *voltage);
static esp_err_t check_valid(const adc_cali_line_fitting_config_t *config);

/* ------------------------- Public API ------------------------------------- */
esp_err_t adc_cali_create_scheme_line_fitting(const adc_cali_line_fitting_config_t *config, adc_cali_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");
    ret = check_valid(config);
    if (ret != ESP_OK) {
        return ret;
    }

    //current version only accepts encoding version: ESP_EFUSE_ADC_CALIB_VER_MIN <= adc_encoding_version <= ESP_EFUSE_ADC_CALIB_VER_MAX.
    uint8_t adc_cali_version = esp_efuse_rtc_calib_get_ver();
    ESP_RETURN_ON_FALSE((adc_cali_version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
                        (adc_cali_version <= ESP_EFUSE_ADC_CALIB_VER_MAX), ESP_ERR_NOT_SUPPORTED, TAG, "Calibration required eFuse bits not burnt");

    adc_cali_scheme_t *scheme = (adc_cali_scheme_t *)heap_caps_calloc(1, sizeof(adc_cali_scheme_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_NO_MEM, TAG, "no mem for adc calibration scheme");

    cali_chars_line_fitting_t *chars = (cali_chars_line_fitting_t *)heap_caps_calloc(1, sizeof(cali_chars_line_fitting_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(chars, ESP_ERR_NO_MEM, err, TAG, "no memory for the calibration characteristics");

    scheme->raw_to_voltage = cali_raw_to_voltage;
    scheme->ctx = chars;

    chars->unit_id = config->unit_id;
    chars->atten = config->atten;

    uint32_t voltage_mv = 0;
    uint32_t digi_val = 0;
    ret = esp_efuse_rtc_calib_get_cal_voltage(adc_cali_version, chars->unit_id, chars->atten, &digi_val, &voltage_mv);
    assert(ret == ESP_OK);
    chars->coeff_a = coeff_a_scaling * voltage_mv / digi_val;
    chars->coeff_b = 0;
    ESP_LOGV(TAG, "Calib V1, Cal Voltage = %" PRId32 ", Digi out = %" PRId32 ", Coef_a = %" PRId32, voltage_mv, digi_val, chars->coeff_a);

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
    *voltage = raw * ctx->coeff_a / coeff_a_scaling + ctx->coeff_b;

    return ESP_OK;
}

static esp_err_t check_valid(const adc_cali_line_fitting_config_t *config)
{
    ESP_RETURN_ON_FALSE(config->unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC unit");
    ESP_RETURN_ON_FALSE((config->atten == ADC_ATTEN_DB_0 || config->atten == ADC_ATTEN_DB_12), ESP_ERR_NOT_SUPPORTED, TAG, "only ADC_ATTEN_DB_0 and ADC_ATTEN_DB_12 are supported");
    if (config->atten == ADC_ATTEN_DB_0) {
        ESP_LOGW(TAG, "Experimental: ADC Atten 0 calibration can now only used for inputs lower than 950mV. Calibration Scheme may get updated, DON'T USE FOR MASS PRODUCTION!");
    }

    bool available_oneshot_bitwidth = (config->bitwidth >= SOC_ADC_RTC_MIN_BITWIDTH && config->bitwidth <= SOC_ADC_RTC_MAX_BITWIDTH);
    bool available_dma_bitwidth = (config->bitwidth >= SOC_ADC_DIGI_MIN_BITWIDTH && config->bitwidth <= SOC_ADC_DIGI_MAX_BITWIDTH);
    bool default_bitwidth_mark = (config->bitwidth == ADC_BITWIDTH_DEFAULT);
    bool available_bitwidth = (available_oneshot_bitwidth || available_dma_bitwidth || default_bitwidth_mark);
    ESP_RETURN_ON_FALSE(available_bitwidth, ESP_ERR_INVALID_ARG, TAG, "invalid bitwidth");

    return ESP_OK;
}
