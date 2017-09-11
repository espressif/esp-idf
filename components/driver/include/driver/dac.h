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
#include "soc/dac_channel.h"

typedef enum {
    DAC_CHANNEL_1 = 1,  /*!< DAC channel 1 is GPIO25 */
    DAC_CHANNEL_2,      /*!< DAC channel 2 is GPIO26 */
    DAC_CHANNEL_MAX,
} dac_channel_t;

/** @cond */
/**
  * @brief  Set DAC output voltage.
  *
  * @note Function has been deprecated, please use dac_output_voltage instead.
  *       This name will be removed in a future release.
  *       The difference is that before calling dac_output_voltage, we need to initialize the dac pad by dac_output_enable
  *
  *
  * @param channel DAC channel
  * @param dac_value DAC output value
  *
  * @return
  *     - ESP_OK success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t dac_out_voltage(dac_channel_t channel, uint8_t dac_value) __attribute__ ((deprecated));
/** @endcond */

/**
 * @brief Set DAC output voltage.
 *
 * DAC output is 8-bit. Maximum (255) corresponds to VDD.
 *
 * @note Need to configure DAC pad before calling this function.
 *       DAC channel 1 is attached to GPIO25, DAC channel 2 is attached to GPIO26
 *
 * @param channel DAC channel
 * @param dac_value DAC output value
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value);

/**
 * @brief DAC pad output enable
 *
 * @param channel DAC channel
 * @note DAC channel 1 is attached to GPIO25, DAC channel 2 is attached to GPIO26
 *       I2S left channel will be mapped to DAC channel 2
 *       I2S right channel will be mapped to DAC channel 1
 */
esp_err_t dac_output_enable(dac_channel_t channel);

/**
 * @brief DAC pad output disable
 *
 * @param channel DAC channel
 * @note DAC channel 1 is attached to GPIO25, DAC channel 2 is attached to GPIO26
 */
esp_err_t dac_output_disable(dac_channel_t channel);

/**
 * @brief Enable DAC output data from I2S
 */
esp_err_t dac_i2s_enable();

/**
 * @brief Disable DAC output data from I2S
 */
esp_err_t dac_i2s_disable();
#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_DAC_H_*/

