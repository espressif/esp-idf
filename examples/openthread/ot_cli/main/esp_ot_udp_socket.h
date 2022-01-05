/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief User command "mcast" process.
 *
 */
void esp_ot_process_mcast_group(void *aContext, uint8_t aArgsLength, char *aArgs[]);

/**
 * @brief User command "udpsockserver" process.
 *
 */
void esp_ot_process_udp_server(void *aContext, uint8_t aArgsLength, char *aArgs[]);

/**
 * @brief User command "udpsockclient" process.
 *
 */
void esp_ot_process_udp_client(void *aContext, uint8_t aArgsLength, char *aArgs[]);

#ifdef __cplusplus
}
#endif
