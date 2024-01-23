/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get pointer of uart vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of uart.
 *
 * @return pointer to structure esp_vfs_t
 */
const esp_vfs_t *esp_vfs_uart_get_vfs(void);

#ifdef __cplusplus
}
#endif
