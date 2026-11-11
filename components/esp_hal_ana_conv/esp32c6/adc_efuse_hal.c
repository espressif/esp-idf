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

#define GET_SIGNED_VALUE(data, sign_bit) ((data & BIT(sign_bit)) ? -(int)(data & ~BIT(sign_bit)) : (int)data)

int adc_hal_efuse_get_version(void)
{
    uint32_t block_version = efuse_ll_get_blk_version_major() * 100 + efuse_ll_get_blk_version_minor();
    if (block_version == 1) {
        return ADC_HAL_EFUSE_CALIB_VER1;
    }
    if (block_version >= 2) {
        return ADC_HAL_EFUSE_CALIB_VER2;
    }
    return 0;
}

uint32_t adc_hal_efuse_get_init_code(int version, uint32_t adc_unit, int atten)
{
    HAL_ASSERT(version >= ADC_HAL_EFUSE_CALIB_VER_MIN && version <= ADC_HAL_EFUSE_CALIB_VER_MAX);
    HAL_ASSERT(atten >= 0 && atten < SOC_ADC_ATTEN_NUM);
    (void)adc_unit;
    return adc_ll_get_efuse_init_code(atten) + 1600;
}

esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv)
{
    static const uint32_t expected_mv[][SOC_ADC_ATTEN_NUM] = {
        {400, 550, 750, 1370},
        {750, 1000, 1500, 2800},
    };
    (void)adc_unit;
    if (version < ADC_HAL_EFUSE_CALIB_VER_MIN || version > ADC_HAL_EFUSE_CALIB_VER_MAX ||
            atten < 0 || atten >= SOC_ADC_ATTEN_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t offset = (version == ADC_HAL_EFUSE_CALIB_VER1) ? 1500 : ((atten == 2) ? 2900 : 2850);
    *out_digi = offset + GET_SIGNED_VALUE(adc_ll_get_efuse_cal_voltage(atten), 9);
    *out_vol_mv = expected_mv[version - ADC_HAL_EFUSE_CALIB_VER1][atten];
    return ESP_OK;
}

int adc_hal_efuse_get_chan_compens(int version, uint32_t adc_unit, uint32_t adc_channel, int atten)
{
    HAL_ASSERT(version >= ADC_HAL_EFUSE_CALIB_VER_MIN && version <= ADC_HAL_EFUSE_CALIB_VER_MAX);
    HAL_ASSERT(atten >= 0 && atten < SOC_ADC_ATTEN_NUM);
    HAL_ASSERT(adc_channel < ADC_LL_CHANNEL_NUM(adc_unit));
    return GET_SIGNED_VALUE(adc_ll_get_efuse_chan_compens(adc_channel), 3) * (4 - atten);
}
