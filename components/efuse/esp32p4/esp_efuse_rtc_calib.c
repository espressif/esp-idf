/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_rtc_calib.h"

int esp_efuse_rtc_calib_get_ver(void)
{
    //TODO: IDF-7467, eFuses are not defined yet
    return 0;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    //TODO: IDF-7467, check below, eFuses are not defined yet
    (void) version;
    (void) adc_unit;
    (void) atten;
    return 0;
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, int atten, uint32_t* out_digi, uint32_t* out_vol_mv)
{
    //TODO: IDF-7467, check below, eFuses are not defined yet
    (void) version;
    (void) atten;
    (void) out_digi;
    (void) out_vol_mv;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    const esp_efuse_desc_t** cal_temp_efuse;
    cal_temp_efuse = ESP_EFUSE_TEMPERATURE_SENSOR;
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
