/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * Zigbee switch driver
 *
 * This driver reads a physical GPIO switch/button (e.g. BOOT button on devkit),
 * uses GPIO interrupt and a queue to report press/release events to the application
 * via a callback. Typical use: Zigbee HA On/Off Switch sending commands on button press.
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle for the switch driver instance.
 */
typedef void* switch_driver_handle_t;

/**
 * @brief Invalid handle value.
 */
#define SWITCH_INV_HANDLE NULL

/**
 * @brief Callback invoked from GPIO ISR (low-level) when the switch level changes.
 *
 * Runs in interrupt context: keep it minimal (e.g. set a flag or post to a queue).
 * Do not block or call blocking / non-ISR-safe APIs.
 *
 * @param user_ctx User context passed at init
 */
typedef void (*switch_event_cb_t)(switch_driver_handle_t handle);

/**
 * @brief Switch driver configuration.
 */
typedef struct {
    gpio_num_t        gpio_num;     /*!< GPIO number used as switch input (e.g. BOOT button) */
    switch_event_cb_t event_cb;     /*!< Callback on level change (invoked from ISR); may be NULL if only polling */
} switch_driver_config_t;

/**
 * @brief Initialize the switch driver.
 *
 * Configures the GPIO in switch_driver_init() as input with pull-up enabled,
 * installs the GPIO ISR, and hooks the callback. The callback is triggered in low-level
 * (GPIO ISR) context on level change.
 *
 * @param config  Driver configuration (gpio_num required; event_cb optional)
 * @return  Handle
 */
switch_driver_handle_t switch_driver_init(const switch_driver_config_t *config);

/**
 * @brief Deinitialize the switch driver.
 *
 * Removes ISR handler and resets GPIO.
 *
 * @param handle  Handle from switch_driver_init; may be NULL
 */
void switch_driver_deinit(switch_driver_handle_t handle);

#ifdef __cplusplus
}
#endif
