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
 * @brief For I2S dma to claim the usage of ADC1.
 *
 * Other tasks will be forbidden to use ADC1 between ``adc1_dma_mode_acquire`` and ``adc1_i2s_release``.
 * The I2S module may have to wait for a short time for the current conversion (if exist) to finish.
 *
 * @return
 *      - ESP_OK success
 *      - ESP_ERR_TIMEOUT reserved for future use. Currently the function will wait until success.
 */
esp_err_t adc1_dma_mode_acquire(void);

/**
 * @brief For ADC1 to claim the usage of ADC1.
 *
 * Other tasks will be forbidden to use ADC1 between ``adc1_rtc_mode_acquire`` and ``adc1_i2s_release``.
 * The ADC1 may have to wait for some time for the I2S read operation to finish.
 *
 * @return
 *      - ESP_OK success
 *      - ESP_ERR_TIMEOUT reserved for future use. Currently the function will wait until success.
 */
esp_err_t adc1_rtc_mode_acquire(void);

/**
 * @brief to let other tasks use the ADC1 when I2S is not work.
 *
 * Other tasks will be forbidden to use ADC1 between ``adc1_adc/i2s_mode_acquire`` and ``adc1_i2s_release``.
 * Call this function to release the occupation of ADC1
 *
 * @return always return ESP_OK.
 */
esp_err_t adc1_lock_release(void);

#ifdef __cplusplus
}
#endif
