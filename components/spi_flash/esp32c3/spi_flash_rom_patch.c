/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp32c3/rom/spi_flash.h"
#include "soc/spi_periph.h"
#include "spi_flash_defs.h"
#include "esp_rom_spiflash.h"

#define SPI_IDX   1

esp_rom_spiflash_result_t esp_rom_spiflash_write_disable(void)
{
    REG_WRITE(SPI_MEM_CMD_REG(SPI_IDX), SPI_MEM_FLASH_WRDI);
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}
