/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/uart_types.h"
#include "sys/_stdint.h"
#include "sys/select.h"
#include "esp_event_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief OpenThread event declarations
*
*/
typedef enum {
    OPENTHREAD_EVENT_START,                         /*!< OpenThread stack start */
    OPENTHREAD_EVENT_STOP,                          /*!< OpenThread stack stop */
    OPENTHREAD_EVENT_IF_UP,                         /*!< OpenThread network interface up */
    OPENTHREAD_EVENT_IF_DOWN,                       /*!< OpenThread network interface down */
    OPENTHREAD_EVENT_GOT_IP6,                       /*!< OpenThread stack added IPv6 address */
    OPENTHREAD_EVENT_LOST_IP6,                      /*!< OpenThread stack removed IPv6 address */
    OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN,          /*!< OpenThread stack joined IPv6 multicast group */
    OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE,         /*!< OpenThread stack left IPv6 multicast group */
    OPENTHREAD_EVENT_TREL_ADD_IP6,                  /*!< OpenThread stack added TREL IPv6 address */
    OPENTHREAD_EVENT_TREL_REMOVE_IP6,               /*!< OpenThread stack removed TREL IPv6 address */
    OPENTHREAD_EVENT_TREL_MULTICAST_GROUP_JOIN,     /*!< OpenThread stack joined TREL IPv6 multicast group */
} esp_openthread_event_t;

/**
* @brief OpenThread event base declaration
*
*/
ESP_EVENT_DECLARE_BASE(OPENTHREAD_EVENT);

/**
 * This structure represents a context for a select() based mainloop.
 *
 */
typedef struct {
    fd_set         read_fds;    /*!< The read file descriptors */
    fd_set         write_fds;   /*!< The write file descriptors */
    fd_set         error_fds;   /*!< The error file descriptors */
    int            max_fd;      /*!< The max file descriptor */
    struct timeval timeout;     /*!< The timeout */
} esp_openthread_mainloop_context_t;

/**
 * @brief The uart port config for OpenThread.
 *
 */
typedef struct {
    uart_port_t port;               /*!< UART port number */
    uart_config_t uart_config;      /*!< UART configuration, see uart_config_t docs */
    int rx_pin;                     /*!< UART RX pin */
    int tx_pin;                     /*!< UART TX pin */
} esp_openthread_uart_config_t;

/**
 * @brief The radio mode of OpenThread.
 *
 */
typedef enum {
    RADIO_MODE_NATIVE   = 0x0,      /*!< Use the native 15.4 radio */
    RADIO_MODE_UART_RCP = 0x1,      /*!< UART connection to a 15.4 capable radio co-processor (RCP) */
    RADIO_MODE_SPI_RCP  = 0x2,      /*!< SPI connection to a 15.4 capable radio co-processor (RCP) */
} esp_openthread_radio_mode_t;

/**
 * @brief How OpenThread connects to the host.
 *
 */
typedef enum {
    HOST_CONNECTION_MODE_NONE     = 0x0, /*!< Disable host connection */
    HOST_CONNECTION_MODE_CLI_UART = 0x1, /*!< CLI UART connection to the host */
    HOST_CONNECTION_MODE_RCP_UART = 0x2, /*!< RCP UART connection to the host */
} esp_openthread_host_connection_mode_t;

/**
 * @brief The OpenThread radio configuration
 *
 */
typedef struct {
    esp_openthread_radio_mode_t     radio_mode;         /*!< The radio mode */
    esp_openthread_uart_config_t    radio_uart_config;  /*!< The uart configuration to RCP */
} esp_openthread_radio_config_t;

/**
 * @brief The OpenThread host connection configuration
 *
 */
typedef struct {
    esp_openthread_host_connection_mode_t   host_connection_mode;   /*!< The host connection mode */
    esp_openthread_uart_config_t            host_uart_config;       /*!< The uart configuration to host */
} esp_openthread_host_connection_config_t;

/**
 * @brief The OpenThread port specific configuration
 *
 */
typedef struct {
    const char *storage_partition_name; /*!< The partition for storing OpenThread dataset */
    uint8_t     netif_queue_size;       /*!< The packet queue size for the network interface */
    uint8_t     task_queue_size;        /*!< The task queue size */
} esp_openthread_port_config_t;

/**
 * @brief The OpenThread platform configuration
 *
 */
typedef struct {
    esp_openthread_radio_config_t               radio_config;   /*!< The radio configuration */
    esp_openthread_host_connection_config_t     host_config;    /*!< The host connection configuration */
    esp_openthread_port_config_t                port_config;    /*!< The port configuration */
} esp_openthread_platform_config_t;

#ifdef __cplusplus
}
#endif
