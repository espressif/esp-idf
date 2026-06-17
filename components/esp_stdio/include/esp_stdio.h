/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_stdio_cli_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_VFS_DEV_CONSOLE "/dev/console"

/**
 * @brief add uart/usb_serial_jtag/usb_otg_acmcdc virtual filesystem driver
 *
 * This function is called from startup code to enable serial output
 */
esp_err_t esp_stdio_register(void);

/**
 * @brief Install and enable the default stdio driver.
 *
 * Initializes the selected console backend and registers it as the active
 * input and output stream source. After calling this function, standard I/O
 * functions such as `printf` and terminal input will use the configured backend.
 *
 * Line endings are automatically configured for typical terminal use:
 * - RX: CR (terminals send CR when Enter is pressed)
 * - TX: CRLF (move cursor to beginning of next line on newline)
 *
 * @return ESP_OK if the driver is successfully installed, or an appropriate error code otherwise.
 */
esp_err_t esp_stdio_install_io_driver(void);

/**
 * @brief Uninstall the active stdio driver.
 *
 * Restores driver state and detaches the console backend from the standard I/O
 * streams. Call this when shutting down or when replacing the current console
 * driver with a different one.
 */
void esp_stdio_uninstall_io_driver(void);

#ifdef __cplusplus
}
#endif
