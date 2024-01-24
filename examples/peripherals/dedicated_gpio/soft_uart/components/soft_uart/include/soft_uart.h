/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration for the usable baudrates by the software UART
 */
typedef enum {
    SOFT_UART_115200,
    SOFT_UART_230400,
    SOFT_UART_460800,
    SOFT_UART_921600,
    SOFT_UART_BAUD_END
} soft_uart_baudrate_t;

/**
 * @brief Structure defining the configuration for the software UART port
 */
typedef struct {
    uint32_t tx_pin;
    uint32_t rx_pin;
    soft_uart_baudrate_t baudrate;
} soft_uart_config_t;

/**
 * @brief Abstract type representing a software UART port.
 */
typedef struct soft_uart_port_impl_t* soft_uart_port_t;

/**
 * @brief Create and configure the software UART port.
 *
 * @param config Configuration to apply to the initialized port.
 * @param port Output structure representing the freshly initialized software UART port.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_uart_new(soft_uart_config_t *config, soft_uart_port_t *port);

/**
 * @brief Delete a previously initialized software UART port.
 *
 * @param port Port to delete, must have been initialized with `soft_uart_new` first.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_uart_del(soft_uart_port_t port);

/**
 * @brief Send the given bytes on the software UART port.
 *
 * @note Since toggling fast GPIOs for the first time may be slow (~1us), the first byte may be corrupted.
 *       If you are seeing this issue, prepend a dummy byte to the buffer to send when calling this
 *       function for the first time.
 *
 * @param port Software UART port to send data on.
 * @param write_buffer Buffer containing the bytes to send on the buffer. Must not be NULL.
 * @param write_size Size of the write buffer. Must not be 0.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_uart_send(soft_uart_port_t port, const uint8_t* write_buffer, size_t write_size);

/**
 * @brief Receive bytes from the software UART port.
 *
 * @param port Software UART port to receive data from.
 * @param read_buffer Buffer that will contain the bytes received. Must not be NULL.
 * @param read_size Size of the read buffer. Must not be 0.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_uart_receive(soft_uart_port_t port, uint8_t* read_buffer, size_t read_size);

#ifdef __cplusplus
}
#endif
