/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
 * @brief   Start the border router features of OpenThread.
 *
 * @note Calling this function will make the device connect to the Wi-Fi or Ethernet,
 *       and initialize the border router feature.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 *
 */
esp_err_t esp_openthread_border_router_start(void);

/**
 * @brief   Stop the border router features of OpenThread.
 *
 * @note Calling this function will make the device deinitialize the border router feature.
 *
 */
void esp_openthread_border_router_stop(void);
