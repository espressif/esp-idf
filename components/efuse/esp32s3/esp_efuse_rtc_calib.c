/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_calib.h"
#include "hal/adc_types.h"

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t blk_ver_major = 0;
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_BLK_VERSION_MAJOR, &blk_ver_major, ESP_EFUSE_BLK_VERSION_MAJOR[0]->bit_count)); // IDF-5366

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
    assert(atten < 4);
    assert(adc_unit <= ADC_UNIT_2);

    const esp_efuse_desc_t **desc[8] = {ESP_EFUSE_ADC1_INIT_CODE_ATTEN0, ESP_EFUSE_ADC1_INIT_CODE_ATTEN1, ESP_EFUSE_ADC1_INIT_CODE_ATTEN2, ESP_EFUSE_ADC1_INIT_CODE_ATTEN3,
                                        ESP_EFUSE_ADC2_INIT_CODE_ATTEN0, ESP_EFUSE_ADC2_INIT_CODE_ATTEN1, ESP_EFUSE_ADC2_INIT_CODE_ATTEN2, ESP_EFUSE_ADC2_INIT_CODE_ATTEN3};
    int efuse_icode_bits = 0;
    uint32_t adc_icode[4] = {};
    uint32_t adc_icode_diff[4] = {};
    uint8_t desc_index = (adc_unit == ADC_UNIT_1) ? 0 : 4;

    for (int diff_index = 0; diff_index < 4; diff_index++) {
        efuse_icode_bits = esp_efuse_get_field_size(desc[desc_index]);
        ESP_ERROR_CHECK(esp_efuse_read_field_blob(desc[desc_index], &adc_icode_diff[diff_index], efuse_icode_bits));
        desc_index++;
    }

    //Version 1 logic for calculating ADC ICode based on EFUSE burnt value
    if (adc_unit == ADC_UNIT_1) {
        adc_icode[0] = adc_icode_diff[0] + 1850;
        adc_icode[1] = adc_icode_diff[1] + adc_icode[0] + 90;
        adc_icode[2] = adc_icode_diff[2] + adc_icode[1];
        adc_icode[3] = adc_icode_diff[3] + adc_icode[2] + 70;
    } else {
        adc_icode[0] = adc_icode_diff[0] + 2020;
        adc_icode[1] = adc_icode_diff[1] + adc_icode[0];
        adc_icode[2] = adc_icode_diff[2] + adc_icode[1];
        adc_icode[3] = adc_icode_diff[3] + adc_icode[2];
    }

    return adc_icode[atten];
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, uint32_t adc_unit, int atten, uint32_t *out_digi, uint32_t *out_vol_mv)
{
    assert((version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (version <= ESP_EFUSE_ADC_CALIB_VER_MAX));
    assert(atten < 4);
    assert(adc_unit <= ADC_UNIT_2);

    int efuse_vol_bits = 0;
    uint32_t adc_vol_diff[8] = {};
    uint32_t adc1_vol[4] = {};
    uint32_t adc2_vol[4] = {};
    const esp_efuse_desc_t **desc[8] = {ESP_EFUSE_ADC1_CAL_VOL_ATTEN0, ESP_EFUSE_ADC1_CAL_VOL_ATTEN1, ESP_EFUSE_ADC1_CAL_VOL_ATTEN2, ESP_EFUSE_ADC1_CAL_VOL_ATTEN3,
                                        ESP_EFUSE_ADC2_CAL_VOL_ATTEN0, ESP_EFUSE_ADC2_CAL_VOL_ATTEN1, ESP_EFUSE_ADC2_CAL_VOL_ATTEN2, ESP_EFUSE_ADC2_CAL_VOL_ATTEN3};
    for (int i = 0; i < 8; i++) {
        efuse_vol_bits = esp_efuse_get_field_size(desc[i]);
        ESP_ERROR_CHECK(esp_efuse_read_field_blob(desc[i], &adc_vol_diff[i], efuse_vol_bits));
    }

    adc1_vol[3] = adc_vol_diff[3] + 900;
    adc1_vol[2] = adc_vol_diff[2] + adc1_vol[3] + 800;
    adc1_vol[1] = adc_vol_diff[1] + adc1_vol[2] + 700;
    adc1_vol[0] = adc_vol_diff[0] + adc1_vol[1] + 800;
    adc2_vol[3] = adc1_vol[3] - adc_vol_diff[7] + 15;
    adc2_vol[2] = adc1_vol[2] - adc_vol_diff[6] + 20;
    adc2_vol[1] = adc1_vol[1] - adc_vol_diff[5] + 10;
    adc2_vol[0] = adc1_vol[0] - adc_vol_diff[4] + 40;

    *out_digi = (adc_unit == ADC_UNIT_1) ? adc1_vol[atten] : adc2_vol[atten];
    *out_vol_mv = 850;

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
