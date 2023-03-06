/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t result = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_BLK_VERSION_MAJOR, &result, ESP_EFUSE_BLK_VERSION_MAJOR[0]->bit_count); // IDF-5366
    return result;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    // Currently calibration is not supported on ESP32-C6, IDF-5236
    (void) version;
    (void) adc_unit;
    (void) atten;
    return 0;
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, int atten, uint32_t* out_digi, uint32_t* out_vol_mv)
{
    // Currently calibration is not supported on ESP32-C6, IDF-5236
    (void) version;
    (void) atten;
    (void) out_digi;
    (void) out_vol_mv;
    return ESP_OK;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    // Currently calibration is not supported on ESP32-C6, IDF-5236
    *tsens_cal = 0;
    return ESP_OK;
}
