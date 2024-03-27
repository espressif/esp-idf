/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LP_GPIO_MATRIX_SUPPORTED
/**
 * @brief Connect a RTC(LP) GPIO input with a peripheral signal, which tagged as input attribute
 *
 * @note There's no limitation on the number of signals that a RTC(LP) GPIO can connect with
 *
 * @param gpio_num GPIO number, especially, `LP_GPIO_MATRIX_CONST_ZERO_INPUT` means connect logic 0 to signal
 *                                          `LP_GPIO_MATRIX_CONST_ONE_INPUT` means connect logic 1 to signal
 * @param signal_idx LP peripheral signal index (tagged as input attribute)
 * @param inv Whether the RTC(LP) GPIO input to be inverted or not
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t lp_gpio_connect_in_signal(gpio_num_t gpio_num, uint32_t signal_idx, bool inv);

/**
 * @brief Connect a peripheral signal which tagged as output attribute with a RTC(LP) GPIO
 *
 * @note There's no limitation on the number of RTC(LP) GPIOs that a signal can connect with
 *
 * @param gpio_num GPIO number
 * @param signal_idx LP peripheral signal index (tagged as input attribute), especially, `SIG_LP_GPIO_OUT_IDX` means disconnect RTC(LP) GPIO and other peripherals. Only the RTC GPIO driver can control the output level
 * @param out_inv Whether to signal to be inverted or not
 * @param oen_inv Whether the output enable control is inverted or not
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t lp_gpio_connect_out_signal(gpio_num_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv);
#endif // SOC_LP_GPIO_MATRIX_SUPPORTED

#ifdef __cplusplus
}
#endif
