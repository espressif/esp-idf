/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "hal/uart_types.h"

/**
 * @brief Send data to the LP UART port if there is space available in the Tx FIFO
 *
 * This function will not wait for enough space in the Tx FIFO to be available.
 * It will just fill the available Tx FIFO slots and return when the FIFO is full.
 * If there are no empty slots in the Tx FIFO, this function will not write any data.
 *
 * @param lp_uart_num   LP UART port number
 * @param src           data buffer address
 * @param size          data length to send
 *
 * @return              - (-1) Error
 *                      - OTHERS (>=0) The number of bytes pushed to the Tx FIFO
 */
int lp_core_uart_tx_chars(uart_port_t lp_uart_num, const void *src, size_t size);

/**
 * @brief Write data to the LP UART port
 *
 * This function will write data to the Tx FIFO. If a timeout value is configured, this function will timeout once the number of CPU cycles expire.
 *
 * @param lp_uart_num   LP UART port number
 * @param src           data buffer address
 * @param size          data length to send
 * @param timeout       Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t lp_core_uart_write_bytes(uart_port_t lp_uart_num, const void *src, size_t size, int32_t timeout);

/**
 * @brief Read data from the LP UART port
 *
 * This function will read data from the Rx FIFO. If a timeout value is configured, then this function will timeout once the number of CPU cycles expire.
 *
 * @param lp_uart_num   LP UART port number
 * @param buf           data buffer address
 * @param size          data length to send
 * @param timeout       Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return              - (-1) Error
 *                      - OTHERS (>=0) The number of bytes read from the Rx FIFO
 */
int lp_core_uart_read_bytes(uart_port_t lp_uart_num, void *buf, size_t size, int32_t timeout);

#ifdef __cplusplus
}
#endif
