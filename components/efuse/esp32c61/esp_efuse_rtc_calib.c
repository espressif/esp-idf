/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    // TODO: [ESP32C61] IDF-9303
    abort();

    return cali_version;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    // TODO: [ESP32C61] IDF-9303
    abort();
    return 0;
}

int esp_efuse_rtc_calib_get_chan_compens(int version, uint32_t adc_unit, uint32_t adc_channel, int atten)
{
    // TODO: [ESP32C61] IDF-9303
    abort();
    return 0;
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, uint32_t adc_unit, int atten, uint32_t* out_digi, uint32_t* out_vol_mv)
{
    // TODO: [ESP32C61] IDF-9303
    abort();
    return ESP_OK;
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
