/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_calib.h"

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t blk_ver_major = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_BLK_VERSION_MAJOR, &blk_ver_major, ESP_EFUSE_BLK_VERSION_MAJOR[0]->bit_count); // IDF-5366

    uint32_t cali_version = (blk_ver_major == 1) ? ESP_EFUSE_ADC_CALIB_VER : 0;
    if (!cali_version) {
        ESP_LOGW("eFuse", "calibration efuse version does not match, set default version to 0");
    }

    return cali_version;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    assert((version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (version <= ESP_EFUSE_ADC_CALIB_VER_MAX));
    (void) adc_unit;
    const esp_efuse_desc_t** init_code_efuse;
    assert(atten < 4);
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
    return init_code + 1000;    // version 1 logic
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, uint32_t adc_unit, int atten, uint32_t* out_digi, uint32_t* out_vol_mv)
{
    (void)adc_unit;    //On esp32c3,  V1 we don't have calibration data for ADC2, using the efuse data of ADC1
    const esp_efuse_desc_t** cal_vol_efuse;
    uint32_t calib_vol_expected_mv;
    if ((version < ESP_EFUSE_ADC_CALIB_VER_MIN) ||
        (version > ESP_EFUSE_ADC_CALIB_VER_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (atten >= 4) {
        return ESP_ERR_INVALID_ARG;
    }
    if (atten == 0) {
        cal_vol_efuse = ESP_EFUSE_ADC1_CAL_VOL_ATTEN0;
        calib_vol_expected_mv = 400;
    } else if (atten == 1) {
        cal_vol_efuse = ESP_EFUSE_ADC1_CAL_VOL_ATTEN1;
        calib_vol_expected_mv = 550;
    } else if (atten == 2) {
        cal_vol_efuse = ESP_EFUSE_ADC1_CAL_VOL_ATTEN2;
        calib_vol_expected_mv = 750;
    } else {
        cal_vol_efuse = ESP_EFUSE_ADC1_CAL_VOL_ATTEN3;
        calib_vol_expected_mv = 1370;
    }

    assert(cal_vol_efuse[0]->bit_count == 10);

    uint32_t cal_vol = 0;
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(cal_vol_efuse, &cal_vol, cal_vol_efuse[0]->bit_count));

    *out_digi = 2000 + ((cal_vol & BIT(9))? -(cal_vol & ~BIT9): cal_vol);
    *out_vol_mv = calib_vol_expected_mv;
    return ESP_OK;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    uint32_t version = esp_efuse_rtc_calib_get_ver();
    if (version != 1) {
        *tsens_cal = 0.0;
        return ESP_ERR_NOT_SUPPORTED;
    }
    const esp_efuse_desc_t** cal_temp_efuse;
    cal_temp_efuse = ESP_EFUSE_TEMP_CALIB;
    int cal_temp_size = esp_efuse_get_field_size(cal_temp_efuse);
    assert(cal_temp_size == 9);

    uint32_t cal_temp = 0;
    esp_err_t err = esp_efuse_read_field_blob(cal_temp_efuse, &cal_temp, cal_temp_size);
    assert(err == ESP_OK);
    (void)err;
    // BIT(8) stands for sign: 1: negtive, 0: positive
    *tsens_cal = ((cal_temp & BIT(8)) != 0)? -(uint8_t)cal_temp: (uint8_t)cal_temp;
    return ESP_OK;
}
