/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_calib.h"
#include "hal/adc_types.h"

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t blk_ver_major = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_BLK_VERSION_MAJOR, &blk_ver_major, ESP_EFUSE_BLK_VERSION_MAJOR[0]->bit_count); // IDF-5366

    uint32_t cali_version = (blk_ver_major == 0) ? ESP_EFUSE_ADC_CALIB_VER : 0;
    if (!cali_version) {
        ESP_LOGW("eFuse", "calibration efuse version does not match, set default version to 0");
    }

    return cali_version;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    assert((version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (version <= ESP_EFUSE_ADC_CALIB_VER_MAX));
    assert(atten <= ADC_ATTEN_DB_12);
    (void) adc_unit;

    if (atten == ADC_ATTEN_DB_2_5 || atten == ADC_ATTEN_DB_6) {
        /**
         * - ESP32C2 only supports HW calibration on ADC_ATTEN_DB_0 and ADC_ATTEN_DB_12
         * - For other attenuation, we just return default value, which is 0.
         */
        return 0;
    }

    int32_t adc_icode_diff_atten0 = 0;
    int32_t adc_icode_diff_atten3 = 0;
    int efuse_icode_bits = 0;

    efuse_icode_bits = esp_efuse_get_field_size(ESP_EFUSE_ADC1_INIT_CODE_ATTEN0);
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_ADC1_INIT_CODE_ATTEN0, &adc_icode_diff_atten0, efuse_icode_bits));
    adc_icode_diff_atten0 = ((adc_icode_diff_atten0 & BIT(7)) != 0) ? -(adc_icode_diff_atten0 & 0x7f): adc_icode_diff_atten0;

    efuse_icode_bits = esp_efuse_get_field_size(ESP_EFUSE_ADC1_INIT_CODE_ATTEN3);
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_ADC1_INIT_CODE_ATTEN3, &adc_icode_diff_atten3, efuse_icode_bits));

    ESP_EARLY_LOGV("eFuse", "adc_icode_diff_atten0: 0d%"PRId32", adc_icode_diff_atten3: 0d%"PRId32, adc_icode_diff_atten0, adc_icode_diff_atten3);

    uint32_t init_code = 0;
    if (atten == ADC_ATTEN_DB_0) {
        init_code = adc_icode_diff_atten0 + 2160;
    } else {
        //ADC_ATTEN_DB_12
        init_code = adc_icode_diff_atten3 + adc_icode_diff_atten0 + 2160;
    }

    return init_code;
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, uint32_t adc_unit, int atten, uint32_t *out_digi, uint32_t *out_vol_mv)
{
    assert((version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (version <= ESP_EFUSE_ADC_CALIB_VER_MAX));
    assert(atten <= ADC_ATTEN_DB_12);
    (void) adc_unit;

    if (atten == ADC_ATTEN_DB_2_5 || atten == ADC_ATTEN_DB_6) {
        /**
         * - ESP32C2 only supports SW calibration on ADC_ATTEN_DB_0 and ADC_ATTEN_DB_12
         * - For other attenuation, we need to return an error, informing upper layer SW calibration driver
         *   to deal with the error.
         */
        return ESP_ERR_INVALID_ARG;
    }

    int32_t adc_vol_diff_atten0 = 0;
    int32_t adc_vol_diff_atten3 = 0;
    int efuse_vol_bits = 0;

    efuse_vol_bits = esp_efuse_get_field_size(ESP_EFUSE_ADC1_CAL_VOL_ATTEN0);
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_ADC1_CAL_VOL_ATTEN0, &adc_vol_diff_atten0, efuse_vol_bits));
    adc_vol_diff_atten0 = ((adc_vol_diff_atten0 & BIT(7)) != 0) ? -(adc_vol_diff_atten0 & 0x7f): adc_vol_diff_atten0;

    efuse_vol_bits = esp_efuse_get_field_size(ESP_EFUSE_ADC1_CAL_VOL_ATTEN3);
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_ADC1_CAL_VOL_ATTEN3, &adc_vol_diff_atten3, efuse_vol_bits));
    adc_vol_diff_atten3 = ((adc_vol_diff_atten3 & BIT(5)) != 0) ? -(adc_vol_diff_atten3 & 0x1f): adc_vol_diff_atten3;

    ESP_EARLY_LOGV("eFuse", "adc_vol_diff_atten0: 0d%"PRId32", adc_vol_diff_atten3: 0d%"PRId32, adc_vol_diff_atten0, adc_vol_diff_atten3);

    if (atten == ADC_ATTEN_DB_0) {
        *out_digi = adc_vol_diff_atten0 + 1540;
        *out_vol_mv = 400;
    } else {
        //ADC_ATTEN_DB_12
        *out_digi = adc_vol_diff_atten0 + 1540 - adc_vol_diff_atten3 - 123;
        *out_vol_mv = 1370;
    }

    return ESP_OK;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    const esp_efuse_desc_t** cal_temp_efuse;
    cal_temp_efuse = ESP_EFUSE_TEMP_CALIB;
    int cal_temp_size = esp_efuse_get_field_size(cal_temp_efuse);
    assert(cal_temp_size == 9);

    uint32_t cal_temp = 0;
    esp_err_t err = esp_efuse_read_field_blob(cal_temp_efuse, &cal_temp, cal_temp_size);
    if (err != ESP_OK) {
        *tsens_cal = 0.0;
        return err;
    }
    // BIT(8) stands for sign: 1: negative, 0: positive
    *tsens_cal = ((cal_temp & BIT(8)) != 0)? -(uint8_t)cal_temp: (uint8_t)cal_temp;
    return ESP_OK;
}
