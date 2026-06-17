/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_vfs_common.h"
#include "../esp_vfs_cdcacm.h" /* use relative path to make sure we include the correct header */
#include "esp_vfs_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get pointer of cdcacm vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of cdcacm.
 *
 * @return pointer to structure esp_vfs_fs_ops_t
 */
const esp_vfs_fs_ops_t *esp_vfs_cdcacm_get_vfs(void);

/**
 * @brief Initialize the USB CDC-ACM VFS console port.
 *
 * Registers the USB CDC-ACM interface as a console device in the ESP-IDF
 * virtual filesystem layer. Once initialized, standard I/O functions such as
 * `printf` and interactive console input will use the USB CDC-ACM channel.
 *
 * @param config Pointer to the USB CDC-ACM VFS device configuration.
 * @param rx_mode Line ending mode to apply when receiving data.
 * @param tx_mode Line ending mode to apply when transmitting data.
 * @return ESP_OK if initialization completed successfully, or a related error code.
 */
esp_err_t cdcacm_vfs_dev_port_init(const esp_console_dev_usb_cdc_config_t *config,
                                   esp_line_endings_t rx_mode,
                                   esp_line_endings_t tx_mode);

/**
 * @brief Deinitialize the USB CDC-ACM VFS console port.
 *
 * Unregisters the USB CDC-ACM VFS device and releases resources allocated during
 * initialization. Call this when the console is being shut down or replaced by
 * another console backend.
 *
 * @param config Pointer to the USB CDC-ACM VFS device configuration.
 */
void cdcacm_vfs_dev_port_deinit(const esp_console_dev_usb_cdc_config_t *config);

#ifdef __cplusplus
}
#endif
