// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

