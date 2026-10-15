/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_calib.h"
#include "esp_log.h"
#include "hal/efuse_hal.h"

#define ADC_K_DATA_BITS 11
#define ADC_B_DATA_BITS 9
#define ADC_K_BIAS      8500

static esp_err_t read_field(const esp_efuse_desc_t **field, uint32_t *value)
{
    return esp_efuse_read_field_blob(field, value, esp_efuse_get_field_size(field));
}

static int32_t decode_sign_magnitude(uint32_t value, uint8_t data_bits)
{
    uint32_t magnitude = value & ((1U << data_bits) - 1);
    return (value & (1U << data_bits)) ? -(int32_t)magnitude : (int32_t)magnitude;
}

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t blk_ver = efuse_hal_blk_version();
    if (blk_ver >= 2 && blk_ver < 100) {
        return ESP_EFUSE_ADC_CALIB_VER;
    }
    ESP_LOGW("eFuse", "ADC calibration eFuse version does not match, use default calibration");
    return 0;
}

esp_err_t esp_efuse_rtc_calib_get_adc_cali_params(int version, adc_unit_t unit_id, bool n_side,
                                                   int32_t *k_scaled,
                                                   int32_t *b_scaled)
{
    ESP_RETURN_ON_FALSE((unit_id == ADC_UNIT_1) || (unit_id == ADC_UNIT_2),
                        ESP_ERR_INVALID_ARG, "eFuse", "invalid ADC unit");
    ESP_RETURN_ON_FALSE(k_scaled && b_scaled, ESP_ERR_INVALID_ARG, "eFuse", "null output pointer");

    if (version == 0) {
        *k_scaled = ESP_EFUSE_ADC_CALI_SCALE;
        *b_scaled = 0;
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(version == ESP_EFUSE_ADC_CALIB_VER, ESP_ERR_INVALID_ARG,
                        "eFuse", "unsupported ADC calibration eFuse version");

    const esp_efuse_desc_t **k;
    const esp_efuse_desc_t **b;
    if (unit_id == ADC_UNIT_1) {
        k = n_side ? ESP_EFUSE_ADC1_N_K : ESP_EFUSE_ADC1_P_K;
        b = n_side ? ESP_EFUSE_ADC1_N_B : ESP_EFUSE_ADC1_P_B;
    } else {
        k = n_side ? ESP_EFUSE_ADC2_N_K : ESP_EFUSE_ADC2_P_K;
        b = n_side ? ESP_EFUSE_ADC2_N_B : ESP_EFUSE_ADC2_P_B;
    }

    uint32_t k_raw;
    uint32_t b_raw;
    ESP_RETURN_ON_ERROR(read_field(k, &k_raw), "eFuse", "failed to read ADC calibration slope");
    ESP_RETURN_ON_ERROR(read_field(b, &b_raw), "eFuse", "failed to read ADC calibration offset");

    *k_scaled = (int32_t)(k_raw & ((1U << ADC_K_DATA_BITS) - 1)) + ADC_K_BIAS;
    *b_scaled = decode_sign_magnitude(b_raw, ADC_B_DATA_BITS);
    return ESP_OK;
}
