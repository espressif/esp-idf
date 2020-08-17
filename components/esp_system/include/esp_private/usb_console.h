// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
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

ssize_t esp_usb_console_flush(void);

ssize_t esp_usb_console_read_buf(char* buf, size_t buf_size);

bool esp_usb_console_read_available(void);

bool esp_usb_console_write_available(void);

esp_err_t esp_usb_console_set_cb(esp_usb_console_cb_t rx_cb, esp_usb_console_cb_t tx_cb, void* arg);

#ifdef __cplusplus
}
#endif
