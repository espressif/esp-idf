/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #pragma once

 #include "esp_err.h"

 #ifdef __cplusplus
 extern "C" {
 #endif

/**
 * @brief Type definition for a brownout callback function.
 *
 * This type defines a callback function that will be invoked when a brownout
 * condition is detected.
 *
 * @note The callback function is executed in an brownout interrupt context, so it
 *       must be designed to execute quickly and must not call blocking functions.
 */
typedef void (*brownout_callback_t)(void);

/**
 * @brief Register a callback to be called during brownout interrupt.
 *
 * @note The callback in brownout must be put in IRAM.
 *
 * @param callback The callback function to register. Set NULL to unregister the callback.
 */
esp_err_t esp_brownout_register_callback(brownout_callback_t callback);

#ifdef __cplusplus
}
#endif
