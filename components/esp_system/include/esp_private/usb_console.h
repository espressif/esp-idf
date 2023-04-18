/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file usb_console.h
 * This file contains definitions of low-level USB console functions.
 * These functions are not considered to be a public interface and
 * should not be called by applications directly.
 * Application interface to the USB console is provided either by
 * "cdcacm" VFS driver, or by the USB CDC driver in TinyUSB.
 */


/**
 * RX/TX callback function type
 * @param arg  callback-specific context pointer
 */
typedef void (*esp_usb_console_cb_t)(void* arg);

/**
 * Initialize USB console output using ROM USB CDC driver.
 * This function is called by the early startup code if USB CDC is
 * selected as the console output option.
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_NO_MEM
 *  - other error codes from the interrupt allocator
 */
esp_err_t esp_usb_console_init(void);

/**
 * Write a buffer to USB CDC
 * @param buf  data to write
 * @param size  size of the data, in bytes
 * @return -1 on error, otherwise the number of bytes
 */
ssize_t esp_usb_console_write_buf(const char* buf, size_t size);

/**
 * @brief Wait until all buffered USB CDC output is written
 *
 * @return ssize_t  Number of bytes written, or -1 if the driver is not initialized
 */
ssize_t esp_usb_console_flush(void);

/**
 * @brief Read data from USB CDC
 *
 * May read less data than requested.
 *
 * @param buf  Buffer to read data into
 * @param buf_size  Size of the buffer
 * @return ssize_t  Number of bytes written into the buffer, or -1 if the driver is not initialized
 */
ssize_t esp_usb_console_read_buf(char* buf, size_t buf_size);

/**
 * @brief Get the number of bytes available for reading from USB CDC
 *
 * @return ssize_t Number of bytes available, or -1 if the driver is not initialized
 */
ssize_t esp_usb_console_available_for_read(void);

/**
 * @brief Check if data can be written into USB CDC
 *
 * @return true if data can be written now without blocking
 */
bool esp_usb_console_write_available(void);

/**
 * @brief Set RX/TX callback functions to be called from ISR
 *
 * @param rx_cb  RX callback function
 * @param tx_cb  TX callback function
 * @param arg  callback-specific context pointer
 * @return ESP_OK if the callbacks were set, ESP_ERR_INVALID_STATE if the driver is not initialized
 */
esp_err_t esp_usb_console_set_cb(esp_usb_console_cb_t rx_cb, esp_usb_console_cb_t tx_cb, void* arg);

#ifdef __cplusplus
}
#endif
