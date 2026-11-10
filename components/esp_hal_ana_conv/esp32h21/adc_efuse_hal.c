/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "hal/adc_efuse_hal.h"

int adc_hal_efuse_get_version(void)
{
    abort();
    return 0;
}

uint32_t adc_hal_efuse_get_init_code(int version, uint32_t adc_unit, int atten)
{
    abort();
    return 0;
}

int adc_hal_efuse_get_chan_compens(int version, uint32_t adc_unit, uint32_t adc_channel, int atten)
{
    abort();
    return ESP_OK;
}

esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv)
{
    return ESP_OK;
}
