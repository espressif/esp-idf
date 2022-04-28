/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Border Router Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "esp_netif.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief User command "wifi" process.
 *
 */
void esp_ot_process_wifi_cmd(void *aContext, uint8_t aArgsLength, char *aArgs[]);

/**
 * @brief Wifi netif init.
 *
 */
void esp_ot_wifi_netif_init(void);

#ifdef __cplusplus
}
#endif
