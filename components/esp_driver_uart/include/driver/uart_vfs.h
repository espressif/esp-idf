/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Add /dev/uart virtual filesystem driver
 *
 * This function is called from startup code to enable serial output
 */
void uart_vfs_dev_register(void);

/**
 * @brief Set the line endings expected to be received on specified UART
 *
 * This specifies the conversion between line endings received on UART and
 * newlines ('\n', LF) passed into stdin:
 *
 * - ESP_LINE_ENDINGS_CRLF: convert CRLF to LF
 * - ESP_LINE_ENDINGS_CR: convert CR to LF
 * - ESP_LINE_ENDINGS_LF: no modification
 *
 * @note this function is not thread safe w.r.t. reading from UART
 *
 * @param uart_num the UART number
 * @param mode line endings to send to UART
 *
 * @return  0 if succeeded, or -1
 *              when an error (specified by errno) have occurred.
 */
int uart_vfs_dev_port_set_rx_line_endings(int uart_num, esp_line_endings_t mode);

/**
 * @brief Set the line endings to sent to specified UART
 *
 * This specifies the conversion between newlines ('\n', LF) on stdout and line
 * endings sent over UART:
 *
 * - ESP_LINE_ENDINGS_CRLF: convert LF to CRLF
 * - ESP_LINE_ENDINGS_CR: convert LF to CR
 * - ESP_LINE_ENDINGS_LF: no modification
 *
 * @note this function is not thread safe w.r.t. writing to UART
 *
 * @param uart_num the UART number
 * @param mode line endings to send to UART
 *
 * @return  0 if succeeded, or -1
 *              when an error (specified by errno) have occurred.
 */
int uart_vfs_dev_port_set_tx_line_endings(int uart_num, esp_line_endings_t mode);

/**
 * @brief set VFS to use simple functions for reading and writing UART
 *
 * Read is non-blocking, write is busy waiting until TX FIFO has enough space.
 * These functions are used by default.
 *
 * @param uart_num UART peripheral number
 */
void uart_vfs_dev_use_nonblocking(int uart_num);

/**
 * @brief set VFS to use UART driver for reading and writing
 *
 * @note Application must configure UART driver before calling these functions
 * With these functions, read and write are blocking and interrupt-driven.
 *
 * @param uart_num UART peripheral number
 */
void uart_vfs_dev_use_driver(int uart_num);

#ifdef __cplusplus
}
#endif
