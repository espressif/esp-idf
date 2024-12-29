/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/

#include "esp_partition.h"
#include "esp_flash_partitions.h"  // esp_partition_table_verify
#include "bootloader_flash_priv.h" // bootloader_ssupport/bootloader_flash/include
#include "esp_log.h"

static const char* TAG = "partition_bootloader";

// populates the internal_partition_list from flash
const esp_partition_t* esp_partition_find_first(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* label)
{
    static esp_partition_t internal_partition = {0};
    int partition_count = 0;

    // Inspired by the implementation in bootloader_support component in bootloader_support.c
    const esp_partition_info_t *partitions = NULL;

    partitions = bootloader_mmap(ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
    if (!partitions) {
        ESP_LOGV(TAG, "bootloader_mmap(0x%x, 0x%x) failed", ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
        return NULL;
    }
    ESP_LOGV(TAG, "mapped partition table 0x%x at 0x%x", ESP_PARTITION_TABLE_OFFSET, (intptr_t)partitions);

    esp_err_t err = esp_partition_table_verify(partitions, true, &partition_count);
    if (err != ESP_OK) {
        ESP_LOGV(TAG, "Failed to verify partition table");

        bootloader_munmap(partitions);
        return NULL;
    }

    ESP_LOGV(TAG, "Partition Table:");

    // in the loop, try to match the partition type, subtype and label
    for (int i = 0; i < partition_count; i++) {
        const esp_partition_info_t *partition = &partitions[i];
        ESP_LOGV(TAG, "Partition table entry 0x%x", (intptr_t)partition);
        ESP_LOGV(TAG, "type=%x subtype=%x label=%-16s", partition->type, partition->subtype, partition->label);

        // if everything matches, populate the internal_partition
        if (partition->type == type
                && partition->subtype == subtype
                && strncmp((char*) partition->label, label, sizeof(partition->label) - 1) == 0) {

            ESP_LOGV(TAG, "Matched", partition->type, partition->subtype, partition->label);
            internal_partition.flash_chip = NULL; //esp_flash_default_chip;
            internal_partition.type = partition->type;
            internal_partition.subtype = partition->subtype;
            internal_partition.address = partition->pos.offset;
            internal_partition.size = partition->pos.size;
            internal_partition.erase_size = SPI_FLASH_SEC_SIZE;
            strncpy(internal_partition.label, (char*) partition->label, sizeof(internal_partition.label) - 1);
            internal_partition.encrypted = partition->flags & PART_FLAG_ENCRYPTED;
            internal_partition.readonly = partition->flags & PART_FLAG_READONLY;

            bootloader_munmap(partitions);
            return &internal_partition;
        }
    }
    ESP_LOGV(TAG, "Nothing matched, end of partition table");

    bootloader_munmap(partitions);

    return NULL;
}

esp_err_t esp_partition_read(const esp_partition_t *partition,
                             size_t src_offset, void *dst, size_t size)
{
    const void *buf;

    // log call to mmap
    ESP_LOGV(TAG, "mmap(0x%x, 0x%x)", partition->address + src_offset, size);

    buf = bootloader_mmap(partition->address + src_offset, size);

    if (buf == NULL) {
        ESP_LOGV(TAG, "bootloader_mmap(0x%x, 0x%x) failed", partition->address, size);
        return ESP_ERR_NO_MEM;
    }
    memcpy(dst, buf, size);
    bootloader_munmap(buf);
    return ESP_OK;
}
