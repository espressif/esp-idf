/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_HAL_EFUSE_CALIB_VER1     1
#define ADC_HAL_EFUSE_CALIB_VER2     2

/* Compatibility alias for targets whose only encoding is V1. */
#define ADC_HAL_EFUSE_CALIB_VER      ADC_HAL_EFUSE_CALIB_VER1
#define ADC_HAL_EFUSE_CALIB_VER_MIN  ADC_HAL_EFUSE_CALIB_VER1
#define ADC_HAL_EFUSE_CALIB_VER_MAX  ADC_HAL_EFUSE_CALIB_VER2
#define ADC_HAL_EFUSE_VERSION_TO_INDEX(version)  ((version) - ADC_HAL_EFUSE_CALIB_VER_MIN)
#define ADC_HAL_EFUSE_CALI_SCALE     10000
#define ADC_HAL_EFUSE_INIT_CODE_INVALID UINT32_MAX

/**
 * @brief Get the ADC calibration encoding version stored in eFuse.
 *
 * @return Supported encoding version, or 0 when calibration data is unavailable.
 */
int adc_hal_efuse_get_version(void);

/**
 * @brief Get the hardware ADC initialization code
 *
 * @return Initialization code, or ADC_HAL_EFUSE_INIT_CODE_INVALID when the
 *         selected encoding version has no hardware initialization code.
 */
uint32_t adc_hal_efuse_get_init_code(int version, uint32_t adc_unit, int atten);

/**
 * @brief Get an ADC calibration reference point.
 */
esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv);

/**
 * @brief Get the low ADC calibration reference point used by S2 encoding V1.
 */
esp_err_t adc_hal_efuse_get_cal_voltage_low(int version, uint32_t adc_unit, int atten,
                                            uint32_t *out_digi, uint32_t *out_vol_mv);

/**
 * @brief Get the S31 line-fitting slope and offset in fixed-point form.
 */
esp_err_t adc_hal_efuse_get_line_fitting_params(int version, uint32_t adc_unit, bool n_side,
                                                int32_t *k_scaled, int32_t *b_scaled);

/**
 * @brief Get the per-channel hardware calibration compensation.
 */
int adc_hal_efuse_get_chan_compens(int version, uint32_t adc_unit, uint32_t adc_channel, int atten);

#ifdef __cplusplus
}
#endif
