/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <esp_types.h>

#include "soc/soc_caps.h"
#include "driver/gpio.h"

#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
/**
  * @brief Emulate ESP32S2 behaviour to backup FUN_PU, FUN_PD information
  *
  * @note Need to be called before sleep.
  *
  * @return
  *      - ESP_OK Success
  */
esp_err_t gpio_sleep_pupd_config_apply(gpio_num_t gpio_num);

/**
  * @brief Emulate ESP32S2 behaviour to restore FUN_PU, FUN_PD information
  *
  * @note Need to be called after sleep.
  *
  * @return
  *      - ESP_OK Success
  */
esp_err_t gpio_sleep_pupd_config_unapply(gpio_num_t gpio_num);
#endif
