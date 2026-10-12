/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_EFUSE_ADC_CALIB_VER              1
#define ESP_EFUSE_ADC_CALIB_VER_MIN          ESP_EFUSE_ADC_CALIB_VER
#define ESP_EFUSE_ADC_CALIB_VER_MAX          ESP_EFUSE_ADC_CALIB_VER
#define ESP_EFUSE_ADC_CALI_SCALE             10000

/**
 * @brief Get the ADC calibration eFuse version.
 *
 * @return Calibration version, or 0 when the eFuse block version is
 *         outside the supported range (major 0, minor >= 2).
 */
int esp_efuse_rtc_calib_get_ver(void);

/**
 * @brief Read the ADC eFuse linear calibration parameters.
 *
 * The returned values are fixed-point values with
 * ::ESP_EFUSE_ADC_CALI_SCALE as the denominator. eFuse encodes the forward
 * model ``x = k * V + b``; the driver recovers ``V = (x - b) / k``.
 * If the calibration eFuse version is not burnt, this function returns the
 * identity transform (k = 1, b = 0).
 *
 * @param[in] version        Calibration eFuse version returned by
 *                           :cpp:func:`esp_efuse_rtc_calib_get_ver`
 * @param[in] unit_id        ADC unit
 * @param[in] n_side         Select N-side single-ended coefficients when true
 * @param[out] k_scaled      Slope multiplied by 10000
 * @param[out] b_scaled      Offset in mV multiplied by 10000
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - other: eFuse read failure
 */
esp_err_t esp_efuse_rtc_calib_get_adc_cali_params(int version, adc_unit_t unit_id, bool n_side,
                                                   int32_t *k_scaled,
                                                   int32_t *b_scaled);

#ifdef __cplusplus
}
#endif
