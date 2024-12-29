/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_vfs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get VFS structure for /dev/null
 *
 * @return VFS structure for /dev/null
 */
const esp_vfs_fs_ops_t *esp_vfs_null_get_vfs(void);

#ifdef __cplusplus
}
#endif
