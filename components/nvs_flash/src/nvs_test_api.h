// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif
    
#include "nvs_flash.h"

/**
 * @brief Initialize NVS flash storage with custom flash sector layout
 *
 * @note  This API is intended to be used in unit tests.
 * 
 * @param partName Partition name of the NVS partition as per partition table
 * @param baseSector Flash sector (units of 4096 bytes) offset to start NVS
 * @param sectorCount Length (in flash sectors) of NVS region. 
 					  NVS partition must be at least 3 sectors long.
 * @return ESP_OK if flash was successfully initialized
 */
esp_err_t nvs_flash_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount);


/**
 * @brief Dump contents of NVS storage to stdout
 *
 * This function may be used for debugging purposes to inspect the state
 * of NVS pages. For each page, list of entries is also dumped.
 *
 * @param partName Partition name of the NVS partition as per partition table
 */
void nvs_dump(const char *partName);


#ifdef __cplusplus
}
#endif
