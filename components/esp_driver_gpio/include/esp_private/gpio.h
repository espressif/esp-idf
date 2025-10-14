/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "soc/io_mux_reg.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief Configure a pin to perform GPIO function or an IOMUX function
  *
  * @param gpio_num GPIO number.
  * @param func Function to assign to the pin. see "io_mux_reg.h"
  *
  * @return
  *      - ESP_OK Success
  *      - ESP_ERR_INVALID_ARG GPIO number error
  */
esp_err_t gpio_func_sel(gpio_num_t gpio_num, uint32_t func);

/**
 * @brief Configure the pin to be used for analog purpose (such as ADC, touch, etc.)
 *
 * @param gpio_num GPIO number
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_config_as_analog(gpio_num_t gpio_num);

/**
  * @brief Set pad input to a peripheral signal through the IOMUX.
  *
  * @param gpio_num GPIO number of the pad.
  * @param func The index number of the IOMUX function to be selected for the pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  * @param signal_idx Peripheral signal index to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG GPIO number error
  */
esp_err_t gpio_iomux_input(gpio_num_t gpio_num, int func, uint32_t signal_idx);

/**
  * @brief Set peripheral output to an GPIO pad through the IOMUX.
  *
  * @param gpio_num GPIO number of the pad.
  * @param func The index number of the IOMUX function to be selected for the pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG GPIO number error
  */
esp_err_t gpio_iomux_output(gpio_num_t gpio_num, int func);

/**
 * @brief Set pad input to a peripheral signal through the GPIO matrix.
 *
 * @note A GPIO can combine with multiple input signals.
 *
 * @param gpio_num GPIO number, especially, `GPIO_MATRIX_CONST_ZERO_INPUT` means connect logic 0 to signal;
 *                                          `GPIO_MATRIX_CONST_ONE_INPUT` means connect logic 1 to signal.
 * @param signal_idx Peripheral signal index (tagged as input attribute).
 *                   One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
 * @param in_inv Whether the GPIO input to be inverted or not.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_matrix_input(gpio_num_t gpio_num, uint32_t signal_idx, bool in_inv);

/**
 * @brief Set peripheral output to an GPIO pad through the GPIO matrix.
 *
 * @note An output signal can be combined with multiple GPIOs.
 *
 * @param gpio_num GPIO number
 * @param signal_idx Peripheral signal index (tagged as output attribute).
 *                   One of the ``*_OUT_IDX`` signals in ``soc/gpio_sig_map.h``.
 *                   Particularly, `SIG_GPIO_OUT_IDX` means disconnect GPIO with any peripheral.
 * @param out_inv Whether to signal to be inverted or not.
 * @param oen_inv Whether the output enable control is inverted or not.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_matrix_output(gpio_num_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv);

#ifdef __cplusplus
}
#endif
