// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Line ending settings
 */
typedef enum {
    ESP_LINE_ENDINGS_CRLF,//!< CR + LF
    ESP_LINE_ENDINGS_CR,  //!< CR
    ESP_LINE_ENDINGS_LF,  //!< LF
} esp_line_endings_t;

/**
 * @brief add /dev/uart virtual filesystem driver
 *
 * This function is called from startup code to enable serial output
 */
void esp_vfs_dev_uart_register(void);

/**
 * @brief Set the line endings expected to be received on UART
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
 * @param mode line endings expected on UART
 */
void esp_vfs_dev_uart_set_rx_line_endings(esp_line_endings_t mode);

/**
 * @brief Set the line endings to sent to UART
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
 * @param mode line endings to send to UART
 */
void esp_vfs_dev_uart_set_tx_line_endings(esp_line_endings_t mode);

/**
 * @brief set VFS to use simple functions for reading and writing UART
 * Read is non-blocking, write is busy waiting until TX FIFO has enough space.
 * These functions are used by default.
 * @param uart_num UART peripheral number
 */
void esp_vfs_dev_uart_use_nonblocking(int uart_num);

/**
 * @brief set VFS to use UART driver for reading and writing
 * @note application must configure UART driver before calling these functions
 * With these functions, read and write are blocking and interrupt-driven.
 * @param uart_num UART peripheral number
 */
void esp_vfs_dev_uart_use_driver(int uart_num);

#ifdef __cplusplus
}
#endif
