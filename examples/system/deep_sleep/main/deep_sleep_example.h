/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#if CONFIG_EXAMPLE_GPIO_WAKEUP
void example_deep_sleep_register_gpio_wakeup(void);
#endif

#if CONFIG_EXAMPLE_EXT0_WAKEUP
void example_deep_sleep_register_ext0_wakeup(void);
#endif

#if CONFIG_EXAMPLE_EXT1_WAKEUP
void example_deep_sleep_register_ext1_wakeup(void);
#endif

#if CONFIG_EXAMPLE_TOUCH_WAKEUP
void example_deep_sleep_register_touch_wakeup(void);
#endif

#ifdef __cplusplus
}
#endif
