/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_VFS_DEV_CONSOLE "/dev/console"

/**
 * @brief add uart/usb_serial_jtag/usb_otg_acmcdc virtual filesystem driver
 *
 * This function is called from startup code to enable serial output
 */
esp_err_t esp_vfs_console_register(void);

#ifdef __cplusplus
}
#endif
