/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_vfs.h"
#include "esp_vfs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief add /dev/usbserjtag virtual filesystem driver
 *
 * This function is called from startup code to enable console output
 */
esp_err_t esp_vfs_dev_usb_serial_jtag_register(void);

/**
 * @brief Set the line endings expected to be received
 *
 * This specifies the conversion between line endings received and
 * newlines ('\n', LF) passed into stdin:
 *
 * - ESP_LINE_ENDINGS_CRLF: convert CRLF to LF
 * - ESP_LINE_ENDINGS_CR: convert CR to LF
 * - ESP_LINE_ENDINGS_LF: no modification
 *
 * @note this function is not thread safe w.r.t. reading
 *
 * @param mode line endings expected
 */
void esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(esp_line_endings_t mode);

/**
 * @brief Set the line endings to sent
 *
 * This specifies the conversion between newlines ('\n', LF) on stdout and line
 * endings sent:
 *
 * - ESP_LINE_ENDINGS_CRLF: convert LF to CRLF
 * - ESP_LINE_ENDINGS_CR: convert LF to CR
 * - ESP_LINE_ENDINGS_LF: no modification
 *
 * @note this function is not thread safe w.r.t. writing
 *
 * @param mode line endings to send
 */
void esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(esp_line_endings_t mode);

#ifdef __cplusplus
}
#endif
