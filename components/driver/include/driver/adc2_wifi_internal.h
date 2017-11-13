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

#ifndef _DRIVER_ADC2_INTERNAL_H_
#define _DRIVER_ADC2_INTERNAL_H_

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
esp_err_t adc2_wifi_acquire();


/**
 * @brief For WIFI module to let other tasks use the ADC2 when WIFI is not work.
 *
 * Other tasks will be forbidden to use ADC2 between ``adc2_wifi_acquire`` and ``adc2_wifi_release``.
 * Call this function to release the occupation of ADC2 by WIFI.
 *
 * @return always return ESP_OK.
 */
esp_err_t adc2_wifi_release();

#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_ADC2_INTERNAL_H_*/

