/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves the I2C master bus handle for a specified I2C port number.
 *
 * This function retrieves the I2C master bus handle for the
 * given I2C port number. Please make sure the handle has already been initialized, and this
 * function would simply returns the existing handle. Note that the returned handle still can't be used concurrently
 *
 * @param port_num I2C port number for which the handle is to be retrieved.
 * @param ret_handle Pointer to a variable where the retrieved handle will be stored.
 * @return
 *     - ESP_OK: Success. The handle is retrieved successfully.
 *     - ESP_ERR_INVALID_ARG: Invalid argument, such as invalid port number
 *     - ESP_ERR_INVALID_STATE: Invalid state, such as the I2C port is not initialized.
 */
esp_err_t i2c_master_get_bus_handle(i2c_port_num_t port_num, i2c_master_bus_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
