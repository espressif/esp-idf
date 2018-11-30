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
#ifndef __ESP_FLASH_PARTITIONS_H
#define __ESP_FLASH_PARTITIONS_H

#include "esp_err.h"
#include "esp_flash_data_types.h"
#include <stdbool.h>
#include "sdkconfig.h"

/* Pre-partition table fixed flash offsets */
#define ESP_BOOTLOADER_DIGEST_OFFSET 0x0
#define ESP_BOOTLOADER_OFFSET 0x1000 /* Offset of bootloader image. Has matching value in bootloader KConfig.projbuild file. */
#define ESP_PARTITION_TABLE_OFFSET CONFIG_PARTITION_TABLE_OFFSET /* Offset of partition table. Backwards-compatible name.*/

#define ESP_PARTITION_TABLE_MAX_LEN 0xC00 /* Maximum length of partition table data */
#define ESP_PARTITION_TABLE_MAX_ENTRIES (ESP_PARTITION_TABLE_MAX_LEN / sizeof(esp_partition_info_t)) /* Maximum length of partition table data, including terminating entry */

/* @brief Verify the partition table
 *
 * @param partition_table Pointer to at least ESP_PARTITION_TABLE_MAX_ENTRIES of potential partition table data. (ESP_PARTITION_TABLE_MAX_LEN bytes.)
 * @param log_errors Log errors if the partition table is invalid.
 * @param num_partitions If result is ESP_OK, num_partitions is updated with total number of partitions (not including terminating entry).
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if partition table is not valid.
 */
esp_err_t esp_partition_table_verify(const esp_partition_info_t *partition_table, bool log_errors, int *num_partitions);


/* This function is included for compatibility with the ESP-IDF v3.x API */
inline static __attribute__((deprecated)) esp_err_t esp_partition_table_basic_verify(const esp_partition_info_t *partition_table, bool log_errors, int *num_partitions)
{
    return esp_partition_table_verify(partition_table, log_errors, num_partitions);
}

#endif
