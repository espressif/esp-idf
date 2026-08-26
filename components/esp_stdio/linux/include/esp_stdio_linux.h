/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration for a Linux host-side console port.
 *
 * Represents the configuration used when running the console interface on a
 * Linux host environment. This is typically used in host-based testing,
 * emulation, or development workflows where the console backend is not a
 * hardware UART or USB device.
 */
typedef struct linux_port_config {

} linux_port_config_t;

/**
 * @brief Install the Linux console driver.
 *
 * Sets up the Linux console port as the active input/output backend for console
 * operations. After installation, standard I/O (such as printf or terminal input)
 * will pass through this Linux-based interface.
 *
 * @param config Pointer to the Linux console port configuration.
 * @return ESP_OK if the driver was successfully installed, or an error otherwise.
 */
esp_err_t linux_vfs_dev_port_init(linux_port_config_t *config);

/**
 * @brief Uninstall the Linux console driver.
 *
 * Releases resources allocated during driver installation and restores state
 * associated with the Linux console backend. Call this when shutting down or
 * switching to another console interface.
 *
 * @param config Pointer to the Linux console port configuration.
 */
void linux_vfs_dev_port_deinit(linux_port_config_t *config);

#ifdef __cplusplus
}
#endif
