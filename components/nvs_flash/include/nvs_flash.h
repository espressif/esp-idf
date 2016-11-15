// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef nvs_flash_h
#define nvs_flash_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize NVS flash storage with layout given in the partition table.
 *
 * @return ESP_OK if storage was successfully initialized.
 */
esp_err_t nvs_flash_init(void);

/**
 * @brief Initialize NVS flash storage with custom flash sector layout
 * @note  Make sure specified sectors don't fall within ranges occupied
 *        by other partitions.
 * @param baseSector Flash sector (units of 4096 bytes) offset to start NVS
 * @param sectorCount Length (in flash sectors) of NVS region
 * @return ESP_OK if flash was successfully initialized
 */
esp_err_t nvs_flash_init_custom(uint32_t baseSector, uint32_t sectorCount);


#ifdef __cplusplus
}
#endif


#endif /* nvs_flash_h */
