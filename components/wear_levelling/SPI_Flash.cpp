/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "SPI_Flash.h"
#include "spi_flash_mmap.h"
#include "esp_rom_spiflash.h"
static const char *TAG = "spi_flash";

SPI_Flash::SPI_Flash()
{
}

size_t SPI_Flash::chip_size()
{
    return g_rom_flashchip.chip_size;
}

esp_err_t SPI_Flash::erase_sector(size_t sector)
{
    esp_err_t result = esp_rom_spiflash_erase_sector(sector);
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
    esp_err_t result = esp_rom_spiflash_erase_area(start_address, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "erase_range - start_address=0x%08x, size=0x%08x, result=0x%08x", start_address, size, result);
    } else {
        ESP_LOGE(TAG, "erase_range - start_address=0x%08x, size=0x%08x, result=0x%08x", start_address, size, result);
    }
    return result;
}

esp_err_t SPI_Flash::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = esp_rom_spiflash_write(dest_addr, (uint32_t*)src, size);
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "write - dest_addr=0x%08x, size=0x%08x, result=0x%08x", dest_addr, size, result);
    } else {
        ESP_LOGE(TAG, "write - dest_addr=0x%08x, size=0x%08x, result=0x%08x", dest_addr, size, result);
    }
    return result;
}

esp_err_t SPI_Flash::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = esp_rom_spiflash_read(src_addr, (uint32_t*)dest, size);
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
