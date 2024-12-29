/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include <sys/select.h>

#include "esp_event_base.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/uart.h"
#include "driver/usb_serial_jtag.h"
#include "hal/gpio_types.h"
#include "hal/uart_types.h"
#include "openthread/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OpenThread event declarations
 *
 */
typedef enum {
    OPENTHREAD_EVENT_START,                     /*!< OpenThread stack start */
    OPENTHREAD_EVENT_STOP,                      /*!< OpenThread stack stop */
    OPENTHREAD_EVENT_DETACHED,                  /*!< OpenThread detached */
    OPENTHREAD_EVENT_ATTACHED,                  /*!< OpenThread attached */
    OPENTHREAD_EVENT_ROLE_CHANGED,              /*!< OpenThread role changed */
    OPENTHREAD_EVENT_IF_UP,                     /*!< OpenThread network interface up */
    OPENTHREAD_EVENT_IF_DOWN,                   /*!< OpenThread network interface down */
    OPENTHREAD_EVENT_GOT_IP6,                   /*!< OpenThread stack added IPv6 address */
    OPENTHREAD_EVENT_LOST_IP6,                  /*!< OpenThread stack removed IPv6 address */
    OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN,      /*!< OpenThread stack joined IPv6 multicast group */
    OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE,     /*!< OpenThread stack left IPv6 multicast group */
    OPENTHREAD_EVENT_TREL_ADD_IP6,              /*!< OpenThread stack added TREL IPv6 address */
    OPENTHREAD_EVENT_TREL_REMOVE_IP6,           /*!< OpenThread stack removed TREL IPv6 address */
    OPENTHREAD_EVENT_TREL_MULTICAST_GROUP_JOIN, /*!< OpenThread stack joined TREL IPv6 multicast group */
    OPENTHREAD_EVENT_SET_DNS_SERVER,            /*!< OpenThread stack set DNS server >*/
    OPENTHREAD_EVENT_PUBLISH_MESHCOP_E,         /*!< OpenThread stack start to publish meshcop-e service >*/
    OPENTHREAD_EVENT_REMOVE_MESHCOP_E,          /*!< OpenThread stack start to remove  meshcop-e service >*/
    OPENTHREAD_EVENT_DATASET_CHANGED,           /*!< OpenThread dataset changed >*/
} esp_openthread_event_t;

/**
 * @brief OpenThread event base declaration
 *
 */
ESP_EVENT_DECLARE_BASE(OPENTHREAD_EVENT);

/**
 * @brief OpenThread role changed event data
 *
 */
typedef struct {
    otDeviceRole previous_role; /*!< Previous Thread role */
    otDeviceRole current_role;  /*!< Current Thread role */
} esp_openthread_role_changed_event_t;

/**
 * @brief OpenThread dataset type
 *
 */
typedef enum {
    OPENTHREAD_ACTIVE_DATASET,  /*!< Active dataset */
    OPENTHREAD_PENDING_DATASET, /*!< Pending dataset */
} esp_openthread_dataset_type_t;

/**
 * @brief OpenThread dataset changed event data
 *
 */
typedef struct {
    esp_openthread_dataset_type_t type; /*!< Dataset type */
    otOperationalDataset new_dataset;   /*!< New dataset */
} esp_openthread_dataset_changed_event_t;

/**
 * This structure represents a context for a select() based mainloop.
 *
 */
typedef struct {
    fd_set         read_fds;  /*!< The read file descriptors */
    fd_set         write_fds; /*!< The write file descriptors */
    fd_set         error_fds; /*!< The error file descriptors */
    int            max_fd;    /*!< The max file descriptor */
    struct timeval timeout;   /*!< The timeout */
} esp_openthread_mainloop_context_t;

/**
 * @brief The uart port config for OpenThread.
 *
 */
typedef struct {
    uart_port_t   port;        /*!< UART port number */
    uart_config_t uart_config; /*!< UART configuration, see uart_config_t docs */
    gpio_num_t    rx_pin;      /*!< UART RX pin */
    gpio_num_t    tx_pin;      /*!< UART TX pin */
} esp_openthread_uart_config_t;

/**
 * @brief The spi port config for OpenThread.
 *
 */
typedef struct {
    spi_host_device_t             host_device;      /*!< SPI host device */
    spi_dma_chan_t                dma_channel;      /*!< DMA channel */
    spi_bus_config_t              spi_interface;    /*!< SPI bus */
    spi_device_interface_config_t spi_device;       /*!< SPI peripheral device */
    gpio_num_t                    intr_pin;         /*!< SPI interrupt pin */
} esp_openthread_spi_host_config_t;

/**
 * @brief The spi slave config for OpenThread.
 *
 */
typedef struct {
    spi_host_device_t            host_device;  /*!< SPI host device */
    spi_bus_config_t             bus_config;   /*!< SPI bus config */
    spi_slave_interface_config_t slave_config; /*!< SPI slave config */
    gpio_num_t                   intr_pin;     /*!< SPI interrupt pin */
} esp_openthread_spi_slave_config_t;

/**
 * @brief The radio mode of OpenThread.
 *
 */
typedef enum {
    RADIO_MODE_NATIVE = 0x0,   /*!< Use the native 15.4 radio */
    RADIO_MODE_UART_RCP,       /*!< UART connection to a 15.4 capable radio co-processor (RCP) */
    RADIO_MODE_SPI_RCP,        /*!< SPI connection to a 15.4 capable radio co-processor (RCP) */
    RADIO_MODE_TREL,           /*!< Use the Thread Radio Encapsulation Link (TREL) */
    RADIO_MODE_MAX,            /*!< Using for parameter check */
} esp_openthread_radio_mode_t;

/**
 * @brief How OpenThread connects to the host.
 *
 */
typedef enum {
    HOST_CONNECTION_MODE_NONE = 0x0,     /*!< Disable host connection */
    HOST_CONNECTION_MODE_CLI_UART,       /*!< CLI UART connection to the host */
    HOST_CONNECTION_MODE_CLI_USB,        /*!< CLI USB connection to the host */
    HOST_CONNECTION_MODE_RCP_UART,       /*!< RCP UART connection to the host */
    HOST_CONNECTION_MODE_RCP_SPI,        /*!< RCP SPI connection to the host */
    HOST_CONNECTION_MODE_MAX,            /*!< Using for parameter check */
} esp_openthread_host_connection_mode_t;

/**
 * @brief The OpenThread radio configuration
 *
 */
typedef struct {
    esp_openthread_radio_mode_t radio_mode; /*!< The radio mode */
    union {
        esp_openthread_uart_config_t     radio_uart_config; /*!< The uart configuration to RCP */
        esp_openthread_spi_host_config_t radio_spi_config;  /*!< The spi configuration to RCP */
    };
} esp_openthread_radio_config_t;

/**
 * @brief The OpenThread host connection configuration
 *
 */
typedef struct {
    esp_openthread_host_connection_mode_t host_connection_mode; /*!< The host connection mode */
    union {
        esp_openthread_uart_config_t      host_uart_config; /*!< The uart configuration to host */
        usb_serial_jtag_driver_config_t   host_usb_config;  /*!< The usb configuration to host */
        esp_openthread_spi_slave_config_t spi_slave_config; /*!< The spi configuration to host */
    };
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
    esp_openthread_radio_config_t           radio_config; /*!< The radio configuration */
    esp_openthread_host_connection_config_t host_config;  /*!< The host connection configuration */
    esp_openthread_port_config_t            port_config;  /*!< The port configuration */
} esp_openthread_platform_config_t;

typedef void (*esp_openthread_rcp_failure_handler)(void);

typedef void (*esp_openthread_compatibility_error_callback)(void);

#ifdef __cplusplus
}
#endif
