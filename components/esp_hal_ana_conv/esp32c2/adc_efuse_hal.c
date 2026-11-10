/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_bit_defs.h"
#include "hal/adc_efuse_hal.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#include "hal/assert.h"
#include "hal/efuse_ll.h"

int adc_hal_efuse_get_version(void)
{
    uint32_t cali_version = (efuse_ll_get_blk_version_major() == 0) ? ADC_HAL_EFUSE_CALIB_VER : 0;
    return cali_version;
}

uint32_t adc_hal_efuse_get_init_code(int version, uint32_t adc_unit, int atten)
{
    HAL_ASSERT(version == ADC_HAL_EFUSE_CALIB_VER1);
    HAL_ASSERT(atten <= ADC_ATTEN_DB_12);
    (void)adc_unit;

    if (atten == ADC_ATTEN_DB_2_5 || atten == ADC_ATTEN_DB_6) {
        return 0;
    }

    int32_t atten0 = adc_ll_get_efuse_init_code(ADC_ATTEN_DB_0);
    int32_t atten3 = adc_ll_get_efuse_init_code(ADC_ATTEN_DB_12);
    atten0 = (atten0 & BIT(7)) ? -(atten0 & 0x7f) : atten0;

    return (atten == ADC_ATTEN_DB_0) ? atten0 + 2160 : atten3 + atten0 + 2160;
}

esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv)
{
    (void)adc_unit;
    if (version != ADC_HAL_EFUSE_CALIB_VER1 ||
            atten < 0 || atten >= SOC_ADC_ATTEN_NUM ||
            atten == ADC_ATTEN_DB_2_5 || atten == ADC_ATTEN_DB_6) {
        return ESP_ERR_INVALID_ARG;
    }

    int32_t atten0 = adc_ll_get_efuse_cal_voltage(ADC_ATTEN_DB_0);
    int32_t atten3 = adc_ll_get_efuse_cal_voltage(ADC_ATTEN_DB_12);
    atten0 = (atten0 & BIT(7)) ? -(atten0 & 0x7f) : atten0;
    atten3 = (atten3 & BIT(5)) ? -(atten3 & 0x1f) : atten3;

    if (atten == ADC_ATTEN_DB_0) {
        *out_digi = atten0 + 1540;
        *out_vol_mv = 400;
    } else {
        *out_digi = atten0 + 1540 - atten3 - 123;
        *out_vol_mv = 1370;
    }
    return ESP_OK;
}
