/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "esp_log.h"
#include "Partition.h"

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
    return erase_range(sector * this->partition->erase_size, this->partition->erase_size);
}

esp_err_t Partition::erase_range(size_t start_address, size_t size)
{
    ESP_LOGV(TAG, "%s - start_address=0x%08" PRIx32 ", size=0x%08" PRIx32, __func__, (uint32_t)start_address, (uint32_t)size);
    return esp_partition_erase_range(this->partition, start_address, size);
}

esp_err_t Partition::write(size_t dest_addr, const void *src, size_t size)
{
    ESP_LOGV(TAG, "%s - dest_addr=0x%08" PRIx32 ", size=0x%08" PRIx32, __func__, (uint32_t)dest_addr, (uint32_t)size);
    return esp_partition_write(this->partition, dest_addr, src, size);
}

esp_err_t Partition::read(size_t src_addr, void *dest, size_t size)
{
    ESP_LOGV(TAG, "%s - src_addr=0x%08" PRIx32 ", size=0x%08" PRIx32, __func__, (uint32_t)src_addr, (uint32_t)size);
    return esp_partition_read(this->partition, src_addr, dest, size);
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
