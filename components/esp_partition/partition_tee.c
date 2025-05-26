/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_tee_flash.h"
#include "esp_log.h"

static __attribute__((unused)) const char *TAG = "partition_tee";

const esp_partition_t *esp_partition_find_first(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* label)
{
    static esp_partition_t internal_partition = {0};
    esp_partition_info_t partition_info = {0};

    esp_err_t err = esp_tee_flash_find_partition(type, subtype, label, &partition_info);
    if (err != ESP_OK) {
        return NULL;
    }

    // Populate the internal partition structure
    internal_partition.flash_chip = NULL;
    internal_partition.type = partition_info.type;
    internal_partition.subtype = partition_info.subtype;
    internal_partition.address = partition_info.pos.offset;
    internal_partition.size = partition_info.pos.size;
    internal_partition.erase_size = SPI_FLASH_SEC_SIZE;
    strncpy(internal_partition.label, (char *)partition_info.label, sizeof(internal_partition.label) - 1);
    internal_partition.encrypted = partition_info.flags & PART_FLAG_ENCRYPTED;
    internal_partition.readonly = partition_info.flags & PART_FLAG_READONLY;

    return &internal_partition;
}

esp_err_t esp_partition_read(const esp_partition_t *partition,
                            size_t src_offset, void *dst, size_t size)
{
    assert(partition != NULL);
    if (src_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size > partition->size - src_offset) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (!partition->encrypted) {
        return esp_tee_flash_read(partition->address + src_offset, dst, size, false);
    }

    const void *buf = esp_tee_flash_mmap(partition->address + src_offset, size);
    if (buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(dst, buf, size);
    esp_tee_flash_munmap(buf);
    return ESP_OK;
}

esp_err_t esp_partition_write(const esp_partition_t *partition,
                             size_t dst_offset, const void *src, size_t size)
{
    assert(partition != NULL);
    if (partition->readonly) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if (dst_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size > partition->size - dst_offset) {
        return ESP_ERR_INVALID_SIZE;
    }
    dst_offset = partition->address + dst_offset;
    if (!partition->encrypted) {
        return esp_tee_flash_write(dst_offset, (void *)src, size, false);
    }

    return esp_tee_flash_write(dst_offset, (void *)src, size, true);
}

esp_err_t esp_partition_read_raw(const esp_partition_t *partition,
                                size_t src_offset, void *dst, size_t size)
{
    assert(partition != NULL);
    if (src_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size > partition->size - src_offset) {
        return ESP_ERR_INVALID_SIZE;
    }

    return esp_tee_flash_read(partition->address + src_offset, dst, size, false);
}

esp_err_t esp_partition_write_raw(const esp_partition_t *partition,
                                 size_t dst_offset, const void *src, size_t size)
{
    assert(partition != NULL);
    if (partition->readonly) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if (dst_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size > partition->size - dst_offset) {
        return ESP_ERR_INVALID_SIZE;
    }
    dst_offset = partition->address + dst_offset;

    return esp_tee_flash_write(dst_offset, (void *)src, size, false);
}

esp_err_t esp_partition_erase_range(const esp_partition_t *partition,
                                   size_t offset, size_t size)
{
    assert(partition != NULL);
    if (partition->readonly) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if (offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size > partition->size - offset) {
        return ESP_ERR_INVALID_SIZE;
    }

    return esp_tee_flash_erase_range(partition->address + offset, size);
}

uint32_t esp_partition_get_main_flash_sector_size(void)
{
    return SPI_FLASH_SEC_SIZE;
}
