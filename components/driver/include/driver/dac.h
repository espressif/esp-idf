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

#ifndef _DRIVER_DAC_H_
#define _DRIVER_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"

typedef enum {
    DAC_CHANNEL_1 = 1,  /*!< DAC channel 1 is GPIO25 */
    DAC_CHANNEL_2,      /*!< DAC channel 2 is GPIO26 */
    DAC_CHANNEL_MAX,
} dac_channel_t;

/**
 * @brief Set DAC output voltage.
 *
 * DAC output is 8-bit. Maximum (255) corresponds to VDD.
 *
 * @note When this function is called, function for the DAC
 * channel's GPIO pin is reconfigured for RTC DAC function.
 *
 * @param channel DAC channel
 * @param dac_value DAC output value
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t dac_out_voltage(dac_channel_t channel, uint8_t dac_value);

#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_DAC_H_*/

