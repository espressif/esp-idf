/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/adc_efuse_hal.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
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
    HAL_ASSERT(adc_unit <= ADC_UNIT_2);

    uint32_t diff[SOC_ADC_ATTEN_NUM];
    uint32_t code[SOC_ADC_ATTEN_NUM];
    for (int i = 0; i < SOC_ADC_ATTEN_NUM; i++) {
        diff[i] = adc_ll_get_efuse_init_code(adc_unit, i);
    }

    if (adc_unit == ADC_UNIT_1) {
        code[0] = diff[0] + 1850;
        code[1] = diff[1] + code[0] + 90;
        code[2] = diff[2] + code[1];
        code[3] = diff[3] + code[2] + 70;
    } else {
        code[0] = diff[0] + 2020;
        code[1] = diff[1] + code[0];
        code[2] = diff[2] + code[1];
        code[3] = diff[3] + code[2];
    }
    return code[atten];
}

esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv)
{
    if (version != ADC_HAL_EFUSE_CALIB_VER1 ||
            atten < 0 || atten >= SOC_ADC_ATTEN_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    HAL_ASSERT(adc_unit <= ADC_UNIT_2);

    uint32_t diff[2][SOC_ADC_ATTEN_NUM];
    uint32_t adc1_vol[SOC_ADC_ATTEN_NUM];
    uint32_t adc2_vol[SOC_ADC_ATTEN_NUM];
    for (int unit = 0; unit < 2; unit++) {
        for (int i = 0; i < SOC_ADC_ATTEN_NUM; i++) {
            diff[unit][i] = adc_ll_get_efuse_cal_voltage(unit, i);
        }
    }

    adc1_vol[3] = diff[0][3] + 900;
    adc1_vol[2] = diff[0][2] + adc1_vol[3] + 800;
    adc1_vol[1] = diff[0][1] + adc1_vol[2] + 700;
    adc1_vol[0] = diff[0][0] + adc1_vol[1] + 800;
    adc2_vol[3] = adc1_vol[3] - diff[1][3] + 15;
    adc2_vol[2] = adc1_vol[2] - diff[1][2] + 20;
    adc2_vol[1] = adc1_vol[1] - diff[1][1] + 10;
    adc2_vol[0] = adc1_vol[0] - diff[1][0] + 40;

    *out_digi = (adc_unit == ADC_UNIT_1) ? adc1_vol[atten] : adc2_vol[atten];
    *out_vol_mv = 850;
    return ESP_OK;
}
