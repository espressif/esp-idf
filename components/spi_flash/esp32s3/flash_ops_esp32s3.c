/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>

#include "spi_flash_mmap.h"
#include "soc/system_reg.h"
#include "soc/soc_memory_layout.h"
#include "esp32s3/rom/cache.h"
#include "bootloader_flash.h"
#include "hal/spi_flash_hal.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_rom_spiflash.h"

#define SPICACHE SPIMEM0
#define SPIFLASH SPIMEM1

esp_err_t spi_flash_wrap_set(spi_flash_wrap_mode_t mode)
{
    return bootloader_flash_wrap_set(mode);
}

esp_err_t spi_flash_enable_wrap(uint32_t wrap_size)
{
    switch (wrap_size) {
    case 8:
        return bootloader_flash_wrap_set(FLASH_WRAP_MODE_8B);
    case 16:
        return bootloader_flash_wrap_set(FLASH_WRAP_MODE_16B);
    case 32:
        return bootloader_flash_wrap_set(FLASH_WRAP_MODE_32B);
    case 64:
        return bootloader_flash_wrap_set(FLASH_WRAP_MODE_64B);
    default:
        return ESP_FAIL;
    }
}

void spi_flash_disable_wrap(void)
{
    bootloader_flash_wrap_set(FLASH_WRAP_MODE_DISABLE);
}

bool spi_flash_support_wrap_size(uint32_t wrap_size)
{
    if (!REG_GET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FREAD_QIO) || !REG_GET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FASTRD_MODE)) {
        return ESP_FAIL;
    }
    switch (wrap_size) {
    case 0:
    case 8:
    case 16:
    case 32:
    case 64:
        return true;
    default:
        return false;
    }
}
