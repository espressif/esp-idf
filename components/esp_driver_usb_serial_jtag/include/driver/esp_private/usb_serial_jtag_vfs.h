/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_vfs.h"
#include "esp_vfs_common.h"
#include "esp_vfs_ops.h"
#include "driver/usb_serial_jtag_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get pointer of usb_serial_jtag vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of usb_serial_jtag.
 *
 * @return pointer to structure esp_vfs_fs_ops_t
 */
const esp_vfs_fs_ops_t *esp_vfs_usb_serial_jtag_get_vfs(void);

/**
 * @brief Initialize the USB Serial JTAG VFS console port.
 *
 * Registers the USB Serial JTAG interface as a standard console I/O device
 * through the ESP-IDF virtual filesystem. Once initialized, standard input
 * and output streams (such as `printf` and interactive console REPL) will be
 * routed through the USB Serial JTAG peripheral.
 *
 * @param config Pointer to the USB Serial JTAG VFS device configuration.
 * @param rx_mode Line ending mode to apply to received data.
 * @param tx_mode Line ending mode to apply to transmitted data.
 * @return ESP_OK on successful initialization, or an error code if setup fails.
 */
esp_err_t usb_serial_jtag_vfs_dev_port_init(const esp_console_dev_usb_serial_jtag_config_t *config,
                                            esp_line_endings_t rx_mode,
                                            esp_line_endings_t tx_mode);

/**
 * @brief Deinitialize the USB Serial JTAG VFS console port.
 *
 * Unregisters the USB Serial JTAG VFS device and restores driver state. Call
 * this when the console is no longer needed or when switching to a different
 * console backend.
 *
 * @param config Pointer to the USB Serial JTAG VFS device configuration.
 */
void usb_serial_jtag_vfs_dev_port_deinit(const esp_console_dev_usb_serial_jtag_config_t *config);

#ifdef __cplusplus
}
#endif
