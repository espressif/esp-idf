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
#include "SPI_Flash.h"
#include "esp_spi_flash.h"
static const char *TAG = "spi_flash";

SPI_Flash::SPI_Flash()
{
}

size_t SPI_Flash::chip_size()
{
    return spi_flash_get_chip_size();
}

esp_err_t SPI_Flash::erase_sector(size_t sector)
{
    esp_err_t result = spi_flash_erase_sector(sector);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_sector - sector=0x%08x, result=0x%08x", sector, result);
    } else {
        ESP_LOGE(TAG, "erase_sector - sector=0x%08x, result=0x%08x", sector, result);
    }
    return result;
}
esp_err_t SPI_Flash::erase_range(size_t start_address, size_t size)
{
    size = (size + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE;
    size = size * SPI_FLASH_SEC_SIZE;
    esp_err_t result = spi_flash_erase_range(start_address, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_range - start_address=0x%08x, size=0x%08x, result=0x%08x", start_address, size, result);
    } else {
        ESP_LOGE(TAG, "erase_range - start_address=0x%08x, size=0x%08x, result=0x%08x", start_address, size, result);
    }
    return result;
}

esp_err_t SPI_Flash::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = spi_flash_write(dest_addr, src, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "write - dest_addr=0x%08x, size=0x%08x, result=0x%08x", dest_addr, size, result);
    } else {
        ESP_LOGE(TAG, "write - dest_addr=0x%08x, size=0x%08x, result=0x%08x", dest_addr, size, result);
    }
    return result;
}

esp_err_t SPI_Flash::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = spi_flash_read(src_addr, dest, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "read - src_addr=0x%08x, size=0x%08x, result=0x%08x", src_addr, size, result);
    } else {
        ESP_LOGE(TAG, "read - src_addr=0x%08x, size=0x%08x, result=0x%08x", src_addr, size, result);
    }
    return result;
}

size_t SPI_Flash::sector_size()
{
    return SPI_FLASH_SEC_SIZE;
}

SPI_Flash::~SPI_Flash()
{
}
