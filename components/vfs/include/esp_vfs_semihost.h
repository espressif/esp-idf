// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief add virtual filesystem semihosting driver
 *
 * @param base_path VFS path to mount host directory
 * @param host_path host path to mount; if NULL default dirctory will be used (see OpenOCD configuration)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if esp_vfs_semihost_register was already called for specified VFS path
 *      - ESP_ERR_NO_MEM if there are no slots to register new mount point
 */
esp_err_t esp_vfs_semihost_register(const char* base_path, const char* host_path);

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
