/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_flash_partitions.h"
#include "esp_log.h"
#include "esp_rom_md5.h"
#include "esp_rom_spiflash.h"

static const char *TAG = "flash_parts";

esp_err_t esp_partition_table_verify(const esp_partition_info_t *partition_table, bool log_errors, int *num_partitions)
{
    int md5_found = 0;
    size_t num_parts;
    uint32_t chip_size = g_rom_flashchip.chip_size;
    *num_partitions = 0;

    for (num_parts = 0; num_parts < ESP_PARTITION_TABLE_MAX_ENTRIES; num_parts++) {
        const esp_partition_info_t *part = &partition_table[num_parts];

        if (part->magic == ESP_PARTITION_MAGIC) {
            const esp_partition_pos_t *pos = &part->pos;
            if (pos->offset > chip_size || pos->offset + pos->size > chip_size) {
                if (log_errors) {
                    ESP_LOGE(TAG, "partition %d invalid - offset 0x%"PRIx32" size 0x%"PRIx32" exceeds flash chip size 0x%"PRIx32,
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

            md5_context_t context;
            unsigned char digest[16];
            esp_rom_md5_init(&context);
            esp_rom_md5_update(&context, (unsigned char *) partition_table, num_parts * sizeof(esp_partition_info_t));
            esp_rom_md5_final(digest, &context);

            unsigned char *md5sum = ((unsigned char *) part) + ESP_PARTITION_MD5_OFFSET;

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
