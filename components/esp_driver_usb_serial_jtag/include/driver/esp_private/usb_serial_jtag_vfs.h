/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get pointer of usb_serial_jtag vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of usb_serial_jtag.
 *
 * @return pointer to structure esp_vfs_t
 */
const esp_vfs_fs_ops_t *esp_vfs_usb_serial_jtag_get_vfs(void);

#ifdef __cplusplus
}
#endif
