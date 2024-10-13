/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "SPI_Flash.h"
#include "esp_flash.h"
#include <inttypes.h>
#include "esp_partition.h"

static const char *TAG = "spi_flash";

SPI_Flash::SPI_Flash()
{
}

size_t SPI_Flash::get_flash_size()
{
    uint32_t chip_size;
    esp_flash_get_size(NULL, &chip_size);
    return chip_size;
}

esp_err_t SPI_Flash::erase_sector(size_t sector)
{
    const uint32_t sec_size = esp_partition_get_main_flash_sector_size();
    esp_err_t result = esp_flash_erase_region(NULL, sector * sec_size, sec_size);;
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_sector - sector=0x%08" PRIx32 ", result=0x%08x", (uint32_t) sector, result);
    } else {
        ESP_LOGE(TAG, "erase_sector - sector=0x%08" PRIx32 ", result=0x%08x", (uint32_t) sector, result);
    }
    return result;
}
esp_err_t SPI_Flash::erase_range(size_t start_address, size_t size)
{
    const uint32_t sec_size = esp_partition_get_main_flash_sector_size();
    size = (size + sec_size - 1) / sec_size;
    size = size * sec_size;
    esp_err_t result = esp_flash_erase_region(NULL, start_address, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_range - start_address=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) start_address, (uint32_t) size, result);
    } else {
        ESP_LOGE(TAG, "erase_range - start_address=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) start_address, (uint32_t) size, result);
    }
    return result;
}

esp_err_t SPI_Flash::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = esp_flash_write(NULL, src, dest_addr, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "write - dest_addr=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) dest_addr, (uint32_t) size, result);
    } else {
        ESP_LOGE(TAG, "write - dest_addr=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) dest_addr, (uint32_t) size, result);
    }
    return result;
}

esp_err_t SPI_Flash::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = esp_flash_read(NULL, dest, src_addr, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "read - src_addr=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) src_addr, (uint32_t) size, result);
    } else {
        ESP_LOGE(TAG, "read - src_addr=0x%08" PRIx32 ", size=0x%08" PRIx32 ", result=0x%08x", (uint32_t) src_addr, (uint32_t) size, result);
    }
    return result;
}

size_t SPI_Flash::get_sector_size()
{
    return esp_partition_get_main_flash_sector_size();;
}

SPI_Flash::~SPI_Flash()
{
}
