// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#pragma once

#include "hal/uart_types.h"
#include "sys/select.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure represents a context for a select() based mainloop.
 *
 */
typedef struct {
    fd_set         read_fds;    /*!< The read file descriptors.*/
    fd_set         write_fds;   /*!< The write file descriptors.*/
    fd_set         error_fds;   /*!< The error file descriptors.*/
    int            max_fd;      /*!< The max file descriptor.*/
    struct timeval timeout;     /*!< The timeout.*/
} esp_openthread_mainloop_context_t;

/**
 * @brief The uart port config for OpenThread.
 *
 */
typedef struct {
    uart_port_t port;               /*!< UART port number*/
    uart_config_t uart_config;      /*!< UART configuration, see uart_config_t docs*/
    int rx_pin;                     /*!< UART RX pin */
    int tx_pin;                     /*!< UART TX pin */
} esp_openthread_uart_config_t;

/**
 * @brief The radio mode of OpenThread.
 *
 */
typedef enum {
    RADIO_MODE_UART_RCP = 0x0,      /*!< UART connection to a 15.4 capable radio co-processor(RCP)*/
} esp_openthread_radio_mode_t;

/**
 * @brief How OpenThread connects to the host.
 *
 */
typedef enum {
    HOST_CONNECTION_MODE_NONE = 0x0, /*!< Disable host connection*/
    HOST_CONNECTION_MODE_UART = 0x1, /*!< UART connection to the host*/
} esp_openthread_host_connection_mode_t;

/**
 * @brief The OpenThread radio configuration
 *
 */
typedef struct {
    esp_openthread_radio_mode_t     radio_mode;         /*!< The radio mode*/
    esp_openthread_uart_config_t    radio_uart_config;  /*!< The uart configuration to RCP*/
} esp_openthread_radio_config_t;

/**
 * @brief The OpenThread host connection configuration
 *
 */
typedef struct {
    esp_openthread_host_connection_mode_t   host_connection_mode;   /*!< The host connection mode*/
    esp_openthread_uart_config_t            host_uart_config;       /*!< The uart configuration to host*/
} esp_openthread_host_connection_config_t;

/**
 * @brief The OpenThread platform configuration
 *
 */
typedef struct {
    esp_openthread_radio_config_t               radio_config;   /*!< The radio configuration*/
    esp_openthread_host_connection_config_t     host_config;    /*!< The host connection configuration*/
} esp_openthread_platform_config_t;

#ifdef __cplusplus
}
#endif
