/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN YOUR APPLICATIONS
// The following APIs are for internal use, public to other IDF components, but not for users' applications.

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register an I2S or I2S variant driver object to platform
 *
 * @note This private API is used to avoid applications from using the same I2S instance for different purpose.
 * @note This function will help enable the peripheral APB clock as well.
 *
 * @param driver_obj Driver object
 * @param port_id I2S port number
 * @return
 *      - ESP_OK: The specific I2S port is free and register the new device object successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument, e.g. wrong port_id
 *      - ESP_ERR_NOT_FOUND: Specific I2S port is not available
 */
esp_err_t i2s_priv_register_object(void *driver_obj, int port_id);

/**
 * @brief Deregister I2S or I2S variant driver object from platform
 *
 * @note This function will help disable the peripheral APB clock as well.
 *
 * @param port_id I2S port number
 * @return
 *      - ESP_OK: Deregister I2S port successfully (i.e. that I2S port can used used by other users after this function returns)
 *      - ESP_ERR_INVALID_ARG: Invalid argument, e.g. wrong port_id
 *      - ESP_ERR_INVALID_STATE: Specific I2S port is free already
 */
esp_err_t i2s_priv_deregister_object(int port_id);

#ifdef __cplusplus
}
#endif
