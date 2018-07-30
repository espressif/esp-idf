// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "spiffs.h"
#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPIFFS definition structure
 */
typedef struct {
    spiffs *fs;                             /*!< Handle to the underlying SPIFFS */
    SemaphoreHandle_t lock;                 /*!< FS lock */
    const esp_partition_t* partition;       /*!< The partition on which SPIFFS is located */
    char base_path[ESP_VFS_PATH_MAX+1];     /*!< Mount point */
    bool by_label;                          /*!< Partition was mounted by label */
    spiffs_config cfg;                      /*!< SPIFFS Mount configuration */
    uint8_t *work;                          /*!< Work Buffer */
    uint8_t *fds;                           /*!< File Descriptor Buffer */
    uint32_t fds_sz;                        /*!< File Descriptor Buffer Length */
    uint8_t *cache;                         /*!< Cache Buffer */
    uint32_t cache_sz;                      /*!< Cache Buffer Length */
} esp_spiffs_t;

s32_t spiffs_api_read(spiffs *fs, uint32_t addr, uint32_t size, uint8_t *dst);

s32_t spiffs_api_write(spiffs *fs, uint32_t addr, uint32_t size, uint8_t *src);

s32_t spiffs_api_erase(spiffs *fs, uint32_t addr, uint32_t size);

void spiffs_api_check(spiffs *fs, spiffs_check_type type,
                            spiffs_check_report report, uint32_t arg1, uint32_t arg2);

#ifdef __cplusplus
}
#endif