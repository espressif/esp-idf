
/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_VFS_SUPPORT_IO
/**
 * @brief Set the pointer of primary dev vfs.
 *
 * This function is called in each vfs dev driver as a system initialization function registered via ESP_SYSTEM_INIT_FN
 *
 * @param vfs pointer to structure esp_vfs_t
 */
void esp_vfs_set_primary_dev_vfs_def_struct(const esp_vfs_t *vfs);

#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
/**
 * @brief Set the pointer of secondary dev vfs.
 *
 * This function is called in each vfs dev driver as a system initialization function registered via ESP_SYSTEM_INIT_FN
 *
 * @param vfs pointer to structure esp_vfs_t
 */
void esp_vfs_set_secondary_dev_vfs_def_struct(const esp_vfs_t *vfs);
#endif //CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
#endif // CONFIG_VFS_SUPPORT_IO

#ifdef __cplusplus
}
#endif
