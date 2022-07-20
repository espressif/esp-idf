/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Acquire ADC lock by unit
 *
 * The lock acquiring sequence will be: ADC1, ADC2, ...
 *
 * @note If any of the locks are taken, this API will wait until the lock is successfully acquired.
 *
 * @param[in] adc_unit ADC unit ID
 *
 * @return
 *        - ESP_OK: On success
 */
esp_err_t adc_lock_acquire(adc_unit_t adc_unit);

/**
 * @brief Release ADC lock by unit
 *
 * The lock releasing sequence will be: ..., ADC2, ADC1
 *
 * @param[in] adc_unit ADC unit ID
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_STATE: The lock(s) isn't acquired yet
 */
esp_err_t adc_lock_release(adc_unit_t adc_unit);

/**
 * @brief Try to acquire ADC lock by unit
 *
 * The lock acquiring sequence will be: ADC1, ADC2, ...
 *
 * @note If any of the locks are taken, this API will return immediately with an error `ESP_ERR_TIMEOUT`
 *
 * @param[in] adc_unit ADC unit ID
 *
 * @return
 *        - ESP_OK:          On success
 *        - ESP_ERR_TIMEOUT: Lock(s) is taken already
 */
esp_err_t adc_lock_try_acquire(adc_unit_t adc_unit);

#ifdef __cplusplus
}
#endif
