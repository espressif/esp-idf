/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/adc_efuse_hal.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#include "hal/efuse_ll.h"

#define ADC_K_BIAS 8500

static int32_t decode_sign_magnitude(uint32_t value, uint32_t data_bits)
{
    uint32_t magnitude = value & ((1U << data_bits) - 1U);
    return (value & (1U << data_bits)) ? -(int32_t)magnitude : (int32_t)magnitude;
}

int adc_hal_efuse_get_version(void)
{
    if (efuse_ll_get_blk_version_major() == 0 &&
            efuse_ll_get_blk_version_minor() >= 2) {
        return ADC_HAL_EFUSE_CALIB_VER;
    }
    return 0;
}

esp_err_t adc_hal_efuse_get_line_fitting_params(int version, uint32_t adc_unit, bool n_side,
                                                int32_t *k_scaled, int32_t *b_scaled)
{
    if (adc_unit > ADC_UNIT_2 || !k_scaled || !b_scaled) {
        return ESP_ERR_INVALID_ARG;
    }
    if (version == 0) {
        *k_scaled = ADC_HAL_EFUSE_CALI_SCALE;
        *b_scaled = 0;
        return ESP_OK;
    }
    if (version != ADC_HAL_EFUSE_CALIB_VER) {
        return ESP_ERR_INVALID_ARG;
    }

    *k_scaled = (int32_t)adc_ll_get_efuse_line_fitting_k(adc_unit, n_side) + ADC_K_BIAS;
    *b_scaled = decode_sign_magnitude(
                    adc_ll_get_efuse_line_fitting_b(adc_unit, n_side), 9);
    return ESP_OK;
}
