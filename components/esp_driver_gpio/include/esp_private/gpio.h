/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <esp_types.h>

#include "soc/soc_caps.h"
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
 * @brief Enable output for an IO
 *
 * @param gpio_num GPIO number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_output_enable(gpio_num_t gpio_num);

/**
 * @brief Disable output for an IO
 *
 * @param gpio_num GPIO number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_output_disable(gpio_num_t gpio_num);

/**
 * @brief Enable open-drain for an IO
 *
 * @param gpio_num GPIO number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_od_disable(gpio_num_t gpio_num);

/**
 * @brief Disable open-drain for an IO
 *
 * @param gpio_num GPIO number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t gpio_od_enable(gpio_num_t gpio_num);

#ifdef __cplusplus
}
#endif
