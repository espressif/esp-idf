/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_efuse_rtc_calib.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_ll.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc_cali_interface.h"

#define ADC_CODE_SLOPE_NUMERATOR   4000
#define ADC_CODE_SLOPE_DENOMINATOR 4393
#define ADC_CODE_OFFSET_MV         (-2000)

static const char *TAG = "adc_cali";

typedef struct {
    adc_unit_t unit_id;
    int32_t p_k_scaled;
    int32_t p_b_scaled;
    int32_t n_k_scaled;
    int32_t n_b_scaled;
} cali_chars_line_fitting_t;

static esp_err_t cali_chan_raw_to_voltage(void *arg, adc_channel_t channel, int raw, int *voltage);
static esp_err_t delete_cali_ctx(void *arg);
static esp_err_t check_valid(const adc_cali_line_fitting_config_t *config);

static int apply_efuse_calibration(uint32_t code, int32_t k_scaled, int32_t b_scaled)
{
    /* eFuse stores the forward model x = k * V + b. Recover V = (x - b) / k
     * without truncating the first-stage mV conversion before the divide. */
    int64_t voltage_times_den = (int64_t)code * ADC_CODE_SLOPE_NUMERATOR +
                                (int64_t)ADC_CODE_OFFSET_MV * ADC_CODE_SLOPE_DENOMINATOR;
    int64_t num = voltage_times_den * ESP_EFUSE_ADC_CALI_SCALE -
                  (int64_t)b_scaled * ADC_CODE_SLOPE_DENOMINATOR;
    int64_t den = (int64_t)k_scaled * ADC_CODE_SLOPE_DENOMINATOR;
    num += (num >= 0) ? (den / 2) : -(den / 2);
    return (int)(num / den);
}

static esp_err_t convert_to_voltage(const cali_chars_line_fitting_t *ctx, bool n_side,
                                    int raw, int *voltage)
{
    uint32_t code = adc_hal_sw_weight_apply(ctx->unit_id, (uint32_t)raw);
    if (n_side) {
        int64_t zero_code_scaled = -(int64_t)ADC_CODE_OFFSET_MV *
                                   ADC_CODE_SLOPE_DENOMINATOR +
                                   ADC_CODE_SLOPE_NUMERATOR - 1;
        uint32_t zero_code = (uint32_t)(zero_code_scaled / ADC_CODE_SLOPE_NUMERATOR);
        code = (code >= zero_code) ? zero_code : (2 * zero_code - code);
    }

    int32_t k_scaled = n_side ? ctx->n_k_scaled : ctx->p_k_scaled;
    int32_t b_scaled = n_side ? ctx->n_b_scaled : ctx->p_b_scaled;
    *voltage = apply_efuse_calibration(code, k_scaled, b_scaled);
    return ESP_OK;
}

esp_err_t adc_cali_create_scheme_line_fitting(const adc_cali_line_fitting_config_t *config,
                                              adc_cali_handle_t *ret_handle)
{
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");
    ESP_RETURN_ON_ERROR(check_valid(config), TAG, "invalid calibration configuration");

    cali_chars_line_fitting_t *chars = heap_caps_calloc(1, sizeof(*chars),
                                                        MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(chars, ESP_ERR_NO_MEM, TAG, "no memory for calibration characteristics");

    chars->unit_id = config->unit_id;

    int version = esp_efuse_rtc_calib_get_ver();
    esp_err_t ret = esp_efuse_rtc_calib_get_adc_cali_params(version, chars->unit_id, false,
                                                            &chars->p_k_scaled, &chars->p_b_scaled);
    if (ret == ESP_OK) {
        ret = esp_efuse_rtc_calib_get_adc_cali_params(version, chars->unit_id, true,
                                                      &chars->n_k_scaled, &chars->n_b_scaled);
    }
    if (ret != ESP_OK) {
        free(chars);
        return ret;
    }

    ret = adc_cali_new_scheme(NULL, cali_chan_raw_to_voltage,
                              delete_cali_ctx, chars, ret_handle);
    if (ret != ESP_OK) {
        free(chars);
    }
    return ret;
}

esp_err_t adc_cali_delete_scheme_line_fitting(adc_cali_handle_t handle)
{
    return adc_cali_delete_scheme(handle);
}

static esp_err_t cali_chan_raw_to_voltage(void *arg, adc_channel_t channel, int raw, int *voltage)
{
    cali_chars_line_fitting_t *ctx = arg;
    bool n_side = (((unsigned)channel) & 1U) == 0U;
    return convert_to_voltage(ctx, n_side, raw, voltage);
}

static esp_err_t delete_cali_ctx(void *arg)
{
    free(arg);
    return ESP_OK;
}

static esp_err_t check_valid(const adc_cali_line_fitting_config_t *config)
{
    ESP_RETURN_ON_FALSE((config->unit_id == ADC_UNIT_1) || (config->unit_id == ADC_UNIT_2),
                        ESP_ERR_INVALID_ARG, TAG, "invalid ADC unit");
    ESP_RETURN_ON_FALSE(config->atten == ADC_ATTEN_DB_0, ESP_ERR_NOT_SUPPORTED, TAG,
                        "only ADC_ATTEN_DB_0 is supported");
    bool valid_oneshot_width = (config->bitwidth >= ADC_LL_RTC_MIN_BITWIDTH) &&
                               (config->bitwidth <= ADC_LL_RTC_MAX_BITWIDTH);
    bool valid_dma_width = (config->bitwidth >= SOC_ADC_DIGI_MIN_BITWIDTH) &&
                           (config->bitwidth <= SOC_ADC_DIGI_MAX_BITWIDTH);
    ESP_RETURN_ON_FALSE(valid_oneshot_width || valid_dma_width ||
                        (config->bitwidth == ADC_BITWIDTH_DEFAULT),
                        ESP_ERR_INVALID_ARG, TAG, "invalid ADC bitwidth");
    return ESP_OK;
}
