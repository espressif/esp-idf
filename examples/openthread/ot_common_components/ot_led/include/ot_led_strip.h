/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_err.h"
#include "esp_openthread.h"

/**
 * @brief Initilize the LED of OpenThread device.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_state_indicator_init(otInstance *instance);

/**
 * @brief Set the LED color of OpenThread device.
 *
 * @param[in] index index of pixel to set
 * @param[in] red   red part of color
 * @param[in] green green part of color
 * @param[in] blue  blue part of color
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG: Set LED color failed because of invalid parameters
 *      - ESP_FAIL on other failure
 *
 */
esp_err_t esp_openthread_state_indicator_set(uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

/**
 * @brief Clear the LED color of OpenThread device.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG: Clear LED color failed because of invalid parameters
 *      - ESP_FAIL on other failure
 *
 */
esp_err_t esp_openthread_state_indicator_clear(void);
