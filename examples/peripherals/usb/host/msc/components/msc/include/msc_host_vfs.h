/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs_fat.h"
#include "msc_host.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct msc_host_vfs *msc_host_vfs_handle_t;           /**< VFS handle to attached Mass Storage device */

/**
 * @brief Register MSC device to Virtual filesystem.
 *
 * @param[in]  device  Device handle obtained from MSC callback provided upon initialization
 * @param[in]  base_path Base VFS path to be used to access file storage
 * @param[in]  mount_config Mount configuration.
 * @param[out] vfs_handle Handle to MSC device associated with registered VFS
 * @return esp_err_t
 */
esp_err_t msc_host_vfs_register(msc_host_device_handle_t device,
                                const char *base_path,
                                const esp_vfs_fat_mount_config_t *mount_config,
                                msc_host_vfs_handle_t *vfs_handle);


/**
 * @brief Unregister MSC device from Virtual filesystem.
 *
 * @param[in]  vfs_handle  VFS handle obtained from MSC callback provided upon initialization
 * @return esp_err_t
 */
esp_err_t msc_host_vfs_unregister(msc_host_vfs_handle_t vfs_handle);

#ifdef __cplusplus
}
#endif
