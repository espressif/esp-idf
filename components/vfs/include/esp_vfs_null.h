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
const esp_vfs_t *esp_vfs_null_get_vfs(void);

/**
 * @brief Register filesystem for /dev/null
 *
 * @return ESP_OK on success; any other value indicates an error
 */
esp_err_t esp_vfs_null_register(void);

#ifdef __cplusplus
}
#endif
