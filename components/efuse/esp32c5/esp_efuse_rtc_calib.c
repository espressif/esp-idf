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
    // TODO: [ESP32C5] IDF-8702
    abort();

    return cali_version;
}

uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    // TODO: [ESP32C5] IDF-8702
    abort();
    return 0;
}

int esp_efuse_rtc_calib_get_chan_compens(int version, uint32_t adc_unit, uint32_t adc_channel, int atten)
{
    // TODO: [ESP32C5] IDF-8702
    abort();
    return 0;
}

esp_err_t esp_efuse_rtc_calib_get_cal_voltage(int version, uint32_t adc_unit, int atten, uint32_t* out_digi, uint32_t* out_vol_mv)
{
    // TODO: [ESP32C5] IDF-8702
    abort();
    return ESP_OK;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    // Currently calibration is not supported on ESP32-C5, IDF-5236
    // Allow no calibration
    *tsens_cal = 0;
    return ESP_OK;
}
