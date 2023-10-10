/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#define ESP_VFS_DEV_CONSOLE "/dev/console"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief add uart/usb_serial_jtag/usb_otg_acmcdc virtual filesystem driver
 *
 * This function is called from startup code to enable serial output
 */
esp_err_t esp_vfs_console_register(void);

#ifdef __cplusplus
}
#endif
