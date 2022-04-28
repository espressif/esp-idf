/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_gpio.h
 *
 * This file contains declarations of GPIO related functions in light sleep mode.
 */

#if SOC_GPIO_SUPPORT_SLP_SWITCH && CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL

/**
 * @brief Save GPIO pull-up and pull-down configuration information in the wake-up state
 *
 * In light sleep mode, the pull-up and pull-down resistors of GPIO will cause
 * leakage current when the system sleeps. In order to reduce the power
 * consumption of system sleep, it needs to save the configuration information
 * of all GPIO pull-up and pull-down resistors and disable the pull-up and
 * pull-down resistors of GPIO before the system enters sleep.
 */
void gpio_sleep_mode_config_apply(void);

/**
 * @brief Restore GPIO pull-up and pull-down configuration information in the wake-up state
 *
 * In light sleep mode, after the system wakes up, it needs to restore all GPIO
 * pull-up and pull-down configurations before the last sleep.
 */
void gpio_sleep_mode_config_unapply(void);

#endif // SOC_GPIO_SUPPORT_SLP_SWITCH && CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL

#ifdef __cplusplus
}
#endif
