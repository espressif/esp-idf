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
#include "esp_flash_partitions.h"
#include "esp_log.h"

static const char *TAG = "flash_parts";

esp_err_t esp_partition_table_basic_verify(const esp_partition_info_t *partition_table, bool log_errors, int *num_partitions)
{
  int num_parts;
  *num_partitions = 0;

  for(num_parts = 0; num_parts < ESP_PARTITION_TABLE_MAX_ENTRIES; num_parts++) {
    const esp_partition_info_t *part = &partition_table[num_parts];

    if(part->magic == 0xFFFF
       && part->type == PART_TYPE_END
       && part->subtype == PART_SUBTYPE_END) {
      /* TODO: check md5 */
      ESP_LOGD(TAG, "partition table verified, %d entries", num_parts);
      *num_partitions = num_parts;
      return ESP_OK;
    }

    if(part->magic != ESP_PARTITION_MAGIC) {
        if (log_errors) {
            ESP_LOGE(TAG, "partition %d invalid magic number 0x%x", num_parts, part->magic);
        }
        return ESP_ERR_INVALID_STATE;
    }
  }

  if (log_errors) {
      ESP_LOGE(TAG, "partition table has no terminating entry, not valid");
  }
  return ESP_ERR_INVALID_STATE;
}

