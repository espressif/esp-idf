/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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

esp_err_t esp_vfs_dev_usb_serial_jtag_register(void) __attribute__((deprecated("Please use usb_serial_jtag_vfs_register instead")));

void esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(esp_line_endings_t mode) __attribute__((deprecated("Please use usb_serial_jtag_vfs_set_rx_line_endings instead")));

void esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(esp_line_endings_t mode) __attribute__((deprecated("Please use usb_serial_jtag_vfs_set_tx_line_endings instead")));

#ifdef __cplusplus
}
#endif
