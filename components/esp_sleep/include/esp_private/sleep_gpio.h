/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "hal/gpio_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_gpio.h
 *
 * This file contains declarations of GPIO related functions in sleep modes.
 */

#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief Save GPIO pull-up and pull-down configuration information in the wake-up state
 *
 * In light sleep mode, the pull-up and pull-down resistors of GPIO will cause
 * leakage current when the system sleeps. In order to reduce the power
 * consumption of system sleep, it needs to save the configuration information
 * of all GPIO pull-up and pull-down resistors and disable the pull-up and
 * pull-down resistors of GPIO before the system enters sleep.
 */
void esp_sleep_gpio_pupd_config_workaround_apply(void);

/**
 * @brief Restore GPIO pull-up and pull-down configuration information in the wake-up state
 *
 * In light sleep mode, after the system wakes up, it needs to restore all GPIO
 * pull-up and pull-down configurations before the last sleep.
 */
void esp_sleep_gpio_pupd_config_workaround_unapply(void);

#endif // CONFIG_IDF_TARGET_ESP32

/**
 * @brief Clear all GPIO dedicated control signals
 */
FORCE_INLINE_ATTR void esp_sleep_gpio_clear_dedicated_ctrl(void)
{
    gpio_ll_clear_dedicated_ctrl();
}

#ifdef __cplusplus
}
#endif
