/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "Partition.h"
#include <inttypes.h>

static const char *TAG = "wl_partition";

Partition::Partition(const esp_partition_t *partition)
{
    this->partition = partition;
}

size_t Partition::get_flash_size()
{
    return this->partition->size;
}

esp_err_t Partition::erase_sector(size_t sector)
{
    esp_err_t result = ESP_OK;
    result = erase_range(sector * this->partition->erase_size, this->partition->erase_size);
    return result;
}

esp_err_t Partition::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = esp_partition_erase_range(this->partition, start_address, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_range - start_address=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) start_address, (uint32_t) size, result);
    } else {
        ESP_LOGE(TAG, "erase_range - start_address=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) start_address, (uint32_t) size, result);
    }
    return result;
}

esp_err_t Partition::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = ESP_OK;
    result = esp_partition_write(this->partition, dest_addr, src, size);
    return result;
}

esp_err_t Partition::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = ESP_OK;
    result = esp_partition_read(this->partition, src_addr, dest, size);
    return result;
}

size_t Partition::get_sector_size()
{
    return this->partition->erase_size;
}

bool Partition::is_readonly()
{
    return this->partition->readonly;
}

Partition::~Partition()
{

}
