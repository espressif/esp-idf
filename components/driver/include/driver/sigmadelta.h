/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "hal/sigmadelta_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sigma-delta configure struct
 */
typedef struct {
    sigmadelta_channel_t channel; /*!< Sigma-delta channel number */
    int8_t sigmadelta_duty;       /*!< Sigma-delta duty, duty ranges from -128 to 127. */
    uint8_t sigmadelta_prescale;  /*!< Sigma-delta prescale, prescale ranges from 0 to 255. */
    gpio_num_t sigmadelta_gpio;   /*!< Sigma-delta output io number, refer to gpio.h for more details. */
} sigmadelta_config_t;

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

#ifdef __cplusplus
}
#endif
