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

#include "nvs.h"

/**
 * @brief Initialize NVS flash storage with layout given in the partition table.
 *
 * @return
 *      - ESP_OK if storage was successfully initialized.
 *      - ESP_ERR_NVS_NO_FREE_PAGES if the NVS storage contains no empty pages
 *        (which may happen if NVS partition was truncated)
 *      - one of the error codes from the underlying flash storage driver
 */
esp_err_t nvs_flash_init(void);


/**
 * @brief Erase NVS partition
 *
 * This function erases all contents of NVS partition
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if there is no NVS partition in the partition table
 */
esp_err_t nvs_flash_erase(void);

#ifdef __cplusplus
}
#endif


#endif /* nvs_flash_h */
