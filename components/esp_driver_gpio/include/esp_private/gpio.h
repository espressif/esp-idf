/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_types.h"
#include "soc/soc_caps.h"
#include "soc/io_mux_reg.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
/**
  * @brief Emulate ESP32S2 behaviour to backup FUN_PU, FUN_PD information
  *
  * @note Need to be called before sleep.
  *
  * @return
  *      - ESP_OK Success
  *      - ESP_ERR_INVALID_ARG GPIO number error
  */
esp_err_t gpio_sleep_pupd_config_apply(gpio_num_t gpio_num);

/**
  * @brief Emulate ESP32S2 behaviour to restore FUN_PU, FUN_PD information
  *
  * @note Need to be called after sleep.
  *
  * @return
  *      - ESP_OK Success
  *      - ESP_ERR_INVALID_ARG GPIO number error
  */
esp_err_t gpio_sleep_pupd_config_unapply(gpio_num_t gpio_num);
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
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
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

#ifdef __cplusplus
}
#endif
