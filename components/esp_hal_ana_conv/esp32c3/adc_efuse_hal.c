/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_bit_defs.h"
#include "hal/adc_efuse_hal.h"
#include "hal/adc_ll.h"
#include "hal/assert.h"
#include "hal/efuse_ll.h"

int adc_hal_efuse_get_version(void)
{
    uint32_t cali_version = (efuse_ll_get_blk_version_major() == 1) ? ADC_HAL_EFUSE_CALIB_VER : 0;
    return cali_version;
}

uint32_t adc_hal_efuse_get_init_code(int version, uint32_t adc_unit, int atten)
{
    HAL_ASSERT(version == ADC_HAL_EFUSE_CALIB_VER1);
    HAL_ASSERT(atten >= 0 && atten < SOC_ADC_ATTEN_NUM);
    (void)adc_unit;
    return adc_ll_get_efuse_init_code(atten) + 1000;
}

esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv)
{
    (void)adc_unit;
    if (version != ADC_HAL_EFUSE_CALIB_VER1 ||
            atten < 0 || atten >= SOC_ADC_ATTEN_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    static const uint32_t expected_mv[SOC_ADC_ATTEN_NUM] = {400, 550, 750, 1370};
    uint32_t cal_vol = adc_ll_get_efuse_cal_voltage(atten);
    *out_digi = 2000 + ((cal_vol & BIT(9)) ? -(cal_vol & ~BIT(9)) : cal_vol);
    *out_vol_mv = expected_mv[atten];
    return ESP_OK;
}
