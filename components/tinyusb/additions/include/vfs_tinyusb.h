/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register TinyUSB CDC at VFS with path
 * @param cdc_intf - interface number of TinyUSB's CDC
 * @param path - path where the CDC will be registered, `/dev/tusb_cdc` will be used if left NULL.
 *
 * @return esp_err_t ESP_OK or ESP_FAIL
 */
esp_err_t esp_vfs_tusb_cdc_register(int cdc_intf, char const *path);

/**
 * @brief Unregister TinyUSB CDC from VFS
 * @param path - path where the CDC will be unregistered if NULL will be used `/dev/tusb_cdc`
 *
 * @return esp_err_t ESP_OK or ESP_FAIL
 */
esp_err_t esp_vfs_tusb_cdc_unregister(char const *path);

#ifdef __cplusplus
}
#endif
