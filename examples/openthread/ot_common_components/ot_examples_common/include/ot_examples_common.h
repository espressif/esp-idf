/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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

#include "sdkconfig.h"

#if CONFIG_ESP_COEX_EXTERNAL_COEXIST_ENABLE
#if CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_1
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
    }
#elif CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_2
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
        .grant = CONFIG_EXTERNAL_COEX_GRANT_PIN,        \
    }
#elif CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_3
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
        .priority = CONFIG_EXTERNAL_COEX_PRIORITY_PIN,  \
        .grant = CONFIG_EXTERNAL_COEX_GRANT_PIN,        \
    }
#elif CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_4
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
        .priority = CONFIG_EXTERNAL_COEX_PRIORITY_PIN,  \
        .grant = CONFIG_EXTERNAL_COEX_GRANT_PIN,        \
        .tx_line = CONFIG_EXTERNAL_COEX_TX_LINE_PIN,    \
    }
#endif
#endif // CONFIG_ESP_COEX_EXTERNAL_COEXIST_ENABLE

/**
 * @brief   Initializes the external coexistence.
 *
 */
void ot_external_coexist_init(void);

/**
 * @brief   Initializes the console.
 *
 */
void ot_console_start(void);

/**
 * @brief   De-initializes the console.
 *
 */
void ot_console_stop(void);

/**
 * @brief   Form or join the Thread network automatically.
 *
 */
void ot_network_auto_start(void);

/**
 * @brief   Register external system commands (e.g., iperf).
 *
 */
void ot_register_external_commands(void);
