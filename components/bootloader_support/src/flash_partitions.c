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
#include <string.h>
#include "esp_flash_partitions.h"
#include "esp_log.h"
#include "rom/spi_flash.h"
#include "rom/md5_hash.h"
#include "esp_flash_data_types.h"

static const char *TAG = "flash_parts";

esp_err_t esp_partition_table_basic_verify(const esp_partition_info_t *partition_table, bool log_errors, int *num_partitions)
{
    int md5_found = 0;
    int num_parts;
    uint32_t chip_size = g_rom_flashchip.chip_size;
    *num_partitions = 0;

    for (num_parts = 0; num_parts < ESP_PARTITION_TABLE_MAX_ENTRIES; num_parts++) {
        const esp_partition_info_t *part = &partition_table[num_parts];

        if (part->magic == ESP_PARTITION_MAGIC) {
            const esp_partition_pos_t *pos = &part->pos;
            if (pos->offset > chip_size || pos->offset + pos->size > chip_size) {
                if (log_errors) {
                    ESP_LOGE(TAG, "partition %d invalid - offset 0x%x size 0x%x exceeds flash chip size 0x%x",
                             num_parts, pos->offset, pos->size, chip_size);
                }
                return ESP_ERR_INVALID_SIZE;
            }
        } else if (part->magic == ESP_PARTITION_MAGIC_MD5) {
            if (md5_found) {
                if (log_errors) {
                    ESP_LOGE(TAG, "Only one MD5 checksum is allowed");
                }
                return ESP_ERR_INVALID_STATE;
            }

            struct MD5Context context;
            unsigned char digest[16];
            MD5Init(&context);
            MD5Update(&context, (unsigned char *) partition_table, num_parts * sizeof(esp_partition_info_t));
            MD5Final(digest, &context);

            unsigned char *md5sum = ((unsigned char *) part) + 16; // skip the 2B magic number and the 14B fillup bytes

            if (memcmp(md5sum, digest, sizeof(digest)) != 0) {
                if (log_errors) {
                    ESP_LOGE(TAG, "Incorrect MD5 checksum");
                }
                return ESP_ERR_INVALID_STATE;
            }
            //MD5 checksum matches and we continue with the next interation in
            //order to detect the end of the partition table
            md5_found = 1;
        } else if (part->magic == 0xFFFF
                   && part->type == PART_TYPE_END
                   && part->subtype == PART_SUBTYPE_END) {
            ESP_LOGD(TAG, "partition table verified, %d entries", num_parts);
            *num_partitions = num_parts - md5_found; //do not count the partition where the MD5 checksum is held
            return ESP_OK;
        } else {
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

