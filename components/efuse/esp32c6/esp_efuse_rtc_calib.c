/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_calib.h"
#include "hal/efuse_hal.h"

/**
 * @brief Get the signed value by the raw data that read from eFuse
 * @param data  The raw data that read from eFuse
 * @param sign_bit  The index of the sign bit, start from 0
 */
#define RTC_CALIB_GET_SIGNED_VAL(data, sign_bit)    ((data & BIT##sign_bit) ? -(int)(data & ~BIT##sign_bit) : (int)data)

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t cali_version = 0;
    uint32_t blk_ver = efuse_hal_blk_version();
    if (blk_ver == 1) {
        cali_version = ESP_EFUSE_ADC_CALIB_VER1;
    } else if (blk_ver >= 2) {
        cali_version = ESP_EFUSE_ADC_CALIB_VER2;
    } else {
        ESP_LOGW("eFuse", "calibration efuse version does not match, set default version to 0");
    }

    return cali_version;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    /* Version validation should be guaranteed in the caller */
    assert(atten >=0 && atten < 4);
    (void) adc_unit;

    const esp_efuse_desc_t** init_code_efuse;
    if (atten == 0) {
        init_code_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0;
    } else if (atten == 1) {
        init_code_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN1;
    } else if (atten == 2) {
        init_code_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN2;
    } else {
        init_code_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN3;
    }

    int init_code_size = esp_efuse_get_field_size(init_code_efuse);
    assert(init_code_size == 10);

    uint32_t init_code = 0;
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(init_code_efuse, &init_code, init_code_size));
    return init_code + 1600;    // version 1 logic
}

int esp_efuse_rtc_calib_get_chan_compens(int version, uint32_t adc_unit, uint32_t adc_channel, int atten)
{
    /* Version validation should be guaranteed in the caller */
    assert(atten < 4);
    assert(adc_channel < SOC_ADC_CHANNEL_NUM(adc_unit));

    const esp_efuse_desc_t** chan_diff_efuse = NULL;
    switch (adc_channel) {
        case 0:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH0;
            break;
        case 1:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH1;
            break;
        case 2:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH2;
            break;
        case 3:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH3;
            break;
        case 4:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH4;
            break;
        case 5:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH5;
            break;
        default:
            chan_diff_efuse = ESP_EFUSE_ADC1_INIT_CODE_ATTEN0_CH6;
            break;
    }

    int chan_diff_size = esp_efuse_get_field_size(chan_diff_efuse);
    assert(chan_diff_size == 4);
    uint32_t chan_diff = 0;
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(chan_diff_efuse, &chan_diff, chan_diff_size));
    return RTC_CALIB_GET_SIGNED_VAL(chan_diff, 3) * (4 - atten);
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, uint32_t adc_unit, int atten, uint32_t* out_digi, uint32_t* out_vol_mv)
{
    (void) adc_unit;
    const esp_efuse_desc_t** cal_vol_efuse[4] = {
        ESP_EFUSE_ADC1_CAL_VOL_ATTEN0,
        ESP_EFUSE_ADC1_CAL_VOL_ATTEN1,
        ESP_EFUSE_ADC1_CAL_VOL_ATTEN2,
        ESP_EFUSE_ADC1_CAL_VOL_ATTEN3,
    };
    const uint32_t input_vout_mv[2][4] = {
        {400,  550,  750, 1370}, // Calibration V1 coefficients
        {750, 1000, 1500, 2800}, // Calibration V2 coefficients
    };

    if ((version < ESP_EFUSE_ADC_CALIB_VER_MIN) ||
        (version > ESP_EFUSE_ADC_CALIB_VER_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (atten >= 4 || atten < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    assert(cal_vol_efuse[atten][0]->bit_count == 10);

    uint32_t cal_vol = 0;
    esp_err_t ret = esp_efuse_read_field_blob(cal_vol_efuse[atten], &cal_vol, cal_vol_efuse[atten][0]->bit_count);
    if (ret != ESP_OK) {
        return ret;
    }
    uint32_t chk_offset = (version == ESP_EFUSE_ADC_CALIB_VER1) ? 1500 : (atten == 2) ? 2900 : 2850;
    *out_digi = chk_offset + RTC_CALIB_GET_SIGNED_VAL(cal_vol, 9);
    *out_vol_mv = input_vout_mv[VER2IDX(version)][atten];
    return ESP_OK;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    // Currently calibration is not supported on ESP32-C6, IDF-5236
    *tsens_cal = 0;
    return ESP_OK;
}
