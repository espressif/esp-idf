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
    //TODO: IDF-7482
    *tsens_cal = 0;
    return ESP_ERR_NOT_SUPPORTED;
}
