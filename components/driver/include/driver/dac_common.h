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

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "hal/dac_types.h"

/**
 * @brief Get the GPIO number of a specific DAC channel.
 *
 * @param channel Channel to get the gpio number
 * @param gpio_num output buffer to hold the gpio number
 * @return
 *   - ESP_OK if success
 */
esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num);

/**
 * @brief Set DAC output voltage.
 *        DAC output is 8-bit. Maximum (255) corresponds to VDD3P3_RTC.
 *
 * @note Need to configure DAC pad before calling this function.
 *       DAC channel 1 is attached to GPIO25, DAC channel 2 is attached to GPIO26
 * @param channel DAC channel
 * @param dac_value DAC output value
 *
 * @return
 *     - ESP_OK success
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
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_output_disable(dac_channel_t channel);

/**
 * @brief Enable cosine wave generator output.
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_cw_generator_enable(void);

/**
 * @brief Disable cosine wave generator output.
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_cw_generator_disable(void);

/**
 * @brief Config the cosine wave generator function in DAC module.
 *
 * @param cw Configuration.
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG The parameter is NULL.
 */
esp_err_t dac_cw_generator_config(dac_cw_config_t *cw);

#ifdef __cplusplus
}
#endif
