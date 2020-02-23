// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_types.h>
#include "soc/sigmadelta_periph.h"
#include "soc/sigmadelta_caps.h"
#include "driver/gpio.h"
#include "hal/sigmadelta_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure Sigma-delta channel
 *
 * @param  config Pointer of Sigma-delta channel configuration struct
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE sigmadelta driver already initialized
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t sigmadelta_config(const sigmadelta_config_t *config);

/**
 * @brief Set Sigma-delta channel duty.
 *
 *        This function is used to set Sigma-delta channel duty,
 *        If you add a capacitor between the output pin and ground,
 *        the average output voltage will be Vdc = VDDIO / 256 * duty + VDDIO/2,
 *        where VDDIO is the power supply voltage.
 *
 * @param channel Sigma-delta channel number
 * @param duty Sigma-delta duty of one channel, the value ranges from -128 to 127, recommended range is -90 ~ 90.
 *             The waveform is more like a random one in this range.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE sigmadelta driver has not been initialized
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t sigmadelta_set_duty(sigmadelta_channel_t channel, int8_t duty);

/**
 * @brief Set Sigma-delta channel's clock pre-scale value.
 *        The source clock is APP_CLK, 80MHz. The clock frequency of the sigma-delta channel is APP_CLK / pre_scale
 *
 * @param channel Sigma-delta channel number
 * @param prescale The divider of source clock, ranges from 0 to 255
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE sigmadelta driver has not been initialized
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t sigmadelta_set_prescale(sigmadelta_channel_t channel, uint8_t prescale);

/**
 * @brief Set Sigma-delta signal output pin
 *
 * @param channel Sigma-delta channel number
 * @param gpio_num GPIO number of output pin.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE sigmadelta driver has not been initialized
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t sigmadelta_set_pin(sigmadelta_channel_t channel, gpio_num_t gpio_num);

#ifdef _cplusplus
}
#endif
