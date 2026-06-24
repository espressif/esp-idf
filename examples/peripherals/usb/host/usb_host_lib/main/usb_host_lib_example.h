/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include "esp_err.h"

/**
 * @brief Register USB-related console commands
 *
 * Registers `usb_install`, `usb_uninstall`, `usb_info`, `usb_suspend`, `usb_resume`, and
 * `usb_power` with the ESP console REPL. Call once after `esp_console` is initialized.
 */
void register_usb(void);

/**
 * @brief Install the USB Host Library and start the example class driver
 *
 * Creates the USB host library task (which calls `usb_host_install()`) and the class driver task.
 *
 * @param peripheral_map Value for `usb_host_config_t.peripheral_map` passed to `usb_host_install()`.
 * @return
 *     - ESP_OK on success or if the stack is already installed
 *     - ESP_ERR_INVALID_STATE if lifecycle mutexes were not created (call from `app_main` first)
 */
esp_err_t usb_example_install(unsigned peripheral_map);

/**
 * @brief FreeRTOS task entry for the example pseudo class driver
 *
 * @param[in] arg `TaskHandle_t` of the task to notify with `xTaskNotifyGive()` after client registration
 */
extern void class_driver_task(void *arg);

/**
 * @brief Request shutdown of the class driver task and client deregistration
 */
extern void class_driver_client_deregister(void);
