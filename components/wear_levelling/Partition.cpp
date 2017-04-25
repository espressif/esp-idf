// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_log.h"
#include "Partition.h"
static const char *TAG = "wl_partition";

Partition::Partition(const esp_partition_t *partition)
{
    this->partition = partition;
}

size_t Partition::chip_size()
{
    return this->partition->size;
}

esp_err_t Partition::erase_sector(size_t sector)
{
    esp_err_t result = ESP_OK;
    result = erase_range(sector * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
    return result;
}
esp_err_t Partition::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = esp_partition_erase_range(this->partition, start_address, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_range - start_address=0x%08x, size=0x%08x, result=0x%08x", start_address, size, result);
    } else {
        ESP_LOGE(TAG, "erase_range - start_address=0x%08x, size=0x%08x, result=0x%08x", start_address, size, result);
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

size_t Partition::sector_size()
{
    return SPI_FLASH_SEC_SIZE;
}

Partition::~Partition()
{

}
