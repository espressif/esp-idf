/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs_common.h"
#include "esp_vfs.h"
#include "esp_vfs_ops.h"
#include "esp_err.h"
#include "driver/uart_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get pointer of uart vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of uart.
 *
 * @return pointer to structure esp_vfs_fs_ops_t
 */
const esp_vfs_fs_ops_t *esp_vfs_uart_get_vfs(void);

/**
 * @brief Initialize the UART VFS console port.
 *
 * Registers a UART as a console device in the ESP-IDF virtual filesystem layer.
 * Once initialized, system input and output streams such as `printf` and shell
 * consoles will communicate through the configured UART channel.
 *
 * @param config Pointer to the UART VFS device configuration.
 * @param rx_mode Line ending mode to apply when receiving data.
 * @param tx_mode Line ending mode to apply when transmitting data.
 * @return ESP_OK if initialization completes successfully, or an error code if it fails.
 */
esp_err_t uart_vfs_dev_port_init(const esp_console_dev_uart_config_t *config,
                                 esp_line_endings_t rx_mode,
                                 esp_line_endings_t tx_mode);

/**
 * @brief Deinitialize the UART VFS console port.
 *
 * Unregisters the UART from the VFS layer and restores driver state. Use this
 * when the UART console is no longer required or when switching to another
 * console backend.
 *
 * @param config Pointer to the UART VFS device configuration.
 */
void uart_vfs_dev_port_deinit(const esp_console_dev_uart_config_t *config);

#ifdef __cplusplus
}
#endif
