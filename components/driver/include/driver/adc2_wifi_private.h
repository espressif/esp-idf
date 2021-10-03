/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/**
 * @brief For WIFI module to claim the usage of ADC2.
 *
 * Other tasks will be forbidden to use ADC2 between ``adc2_wifi_acquire`` and ``adc2_wifi_release``.
 * The WIFI module may have to wait for a short time for the current conversion (if exist) to finish.
 *
 * @return
 *      - ESP_OK success
 *      - ESP_ERR_TIMEOUT reserved for future use. Currently the function will wait until success.
 */
esp_err_t adc2_wifi_acquire(void);


/**
 * @brief For WIFI module to let other tasks use the ADC2 when WIFI is not work.
 *
 * Other tasks will be forbidden to use ADC2 between ``adc2_wifi_acquire`` and ``adc2_wifi_release``.
 * Call this function to release the occupation of ADC2 by WIFI.
 *
 * @return always return ESP_OK.
 */
esp_err_t adc2_wifi_release(void);

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
/**
 * @brief This API help ADC2 calibration constructor be linked.
 *
 * @note  This is a private function, Don't call `adc2_cal_include` in user code.
 */
void adc2_cal_include(void);
#else
/**
 * @brief There's no calibration involved on this chip.
 *
 * @note  This is a private function, Don't call `adc2_cal_include` in user code.
 */
#define adc2_cal_include()
#endif //CONFIG_IDF_TARGET_*

#ifdef __cplusplus
}
#endif
