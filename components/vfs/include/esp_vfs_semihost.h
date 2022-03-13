/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief add virtual filesystem semihosting driver
 *
 * @param base_path VFS path to mount host directory
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if esp_vfs_semihost_register was already called for specified VFS path
 *      - ESP_ERR_NO_MEM if there are no slots to register new mount point
 */
esp_err_t esp_vfs_semihost_register(const char* base_path);

/**
 * @brief Un-register semihosting driver from VFS
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if semihosting driver is not registered in VFS at that path
 */
esp_err_t esp_vfs_semihost_unregister(const char* base_path);

#ifdef __cplusplus
}
#endif
