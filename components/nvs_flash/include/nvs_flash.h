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
 * @brief Initialize the default NVS partition.
 *
 * This API initialises the default NVS partition. The default NVS partition
 * is the one that is labeled "nvs" in the partition table.
 *
 * @return
 *      - ESP_OK if storage was successfully initialized.
 *      - ESP_ERR_NVS_NO_FREE_PAGES if the NVS storage contains no empty pages
 *        (which may happen if NVS partition was truncated)
 *      - ESP_ERR_NOT_FOUND if no partition with label "nvs" is found in the partition table
 *      - one of the error codes from the underlying flash storage driver
 */
esp_err_t nvs_flash_init(void);

/**
 * @brief Initialize NVS flash storage for the specified partition.
 *
 * @param[in]  partition_label   Label of the partition. Note that internally a reference to
 *                               passed value is kept and it should be accessible for future operations
 *
 * @return
 *      - ESP_OK if storage was successfully initialized.
 *      - ESP_ERR_NVS_NO_FREE_PAGES if the NVS storage contains no empty pages
 *        (which may happen if NVS partition was truncated)
 *      - ESP_ERR_NOT_FOUND if specified partition is not found in the partition table
 *      - one of the error codes from the underlying flash storage driver
 */
esp_err_t nvs_flash_init_partition(const char *partition_label);

/**
 * @brief Deinitialize NVS storage for the default NVS partition
 *
 * Default NVS partition is the partition with "nvs" label in the partition table.
 *
 * @return
 *      - ESP_OK on success (storage was deinitialized)
 *      - ESP_ERR_NVS_NOT_INITIALIZED if the storage was not initialized prior to this call
 */
esp_err_t nvs_flash_deinit(void);

/**
 * @brief Deinitialize NVS storage for the given NVS partition
 *
 * @param[in]  partition_label   Label of the partition
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NVS_NOT_INITIALIZED if the storage for given partition was not
 *        initialized prior to this call
 */
esp_err_t nvs_flash_deinit_partition(const char* partition_label);

/**
 * @brief Erase the default NVS partition
 *
 * This function erases all contents of the default NVS partition (one with label "nvs")
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if there is no NVS partition labeled "nvs" in the
 *        partition table
 */
esp_err_t nvs_flash_erase(void);

/**
 * @brief Erase specified NVS partition
 *
 * This function erases all contents of specified NVS partition
 *
 * @param[in]  part_name    Name (label) of the partition to be erased
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if there is no NVS partition with the specified name
 *        in the partition table
 */
esp_err_t nvs_flash_erase_partition(const char *part_name);

#ifdef __cplusplus
}
#endif


#endif /* nvs_flash_h */
