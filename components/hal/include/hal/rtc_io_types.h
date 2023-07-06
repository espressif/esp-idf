/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** RTCIO output/input mode type. */
typedef enum {
    RTC_GPIO_MODE_INPUT_ONLY ,  /*!< Pad input */
    RTC_GPIO_MODE_OUTPUT_ONLY,  /*!< Pad output */
    RTC_GPIO_MODE_INPUT_OUTPUT, /*!< Pad input + output */
    RTC_GPIO_MODE_DISABLED,     /*!< Pad (output + input) disable */
    RTC_GPIO_MODE_OUTPUT_OD,    /*!< Pad open-drain output */
    RTC_GPIO_MODE_INPUT_OUTPUT_OD, /*!< Pad input + open-drain output */
} rtc_gpio_mode_t;

#ifdef __cplusplus
}
#endif
