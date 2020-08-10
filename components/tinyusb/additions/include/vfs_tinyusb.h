// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
