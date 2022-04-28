// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <sys/param.h> // For MIN/MAX
#include "esp_log.h"
#include "spi_flash_chip_generic.h"
#include "spi_flash_defs.h"


#define REGION_32BIT(start, len)    ((start) + (len) > (1<<24))
#define ADDR_32BIT(addr)            (addr >= (1<<24))


static const char TAG[] = "chip_wb";

/* Driver for Winbond flash chip */
static esp_err_t spi_flash_command_winbond_program_4B(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);
static esp_err_t spi_flash_command_winbond_erase_sector_4B(esp_flash_t *chip, uint32_t start_address);
static esp_err_t spi_flash_command_erase_block_4B(esp_flash_t *chip, uint32_t start_address);

esp_err_t spi_flash_chip_winbond_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0xEF;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_winbond_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = ESP_OK;
    const uint32_t page_size = chip->chip_drv->page_size;
    uint32_t align_address;
    uint8_t temp_buffer[64]; //spiflash hal max length of read no longer than 64byte
    uint32_t config_io_flags = 0;

    // Configure the host, and return
    if (REGION_32BIT(address, length)) {
        config_io_flags |= SPI_FLASH_CONFIG_IO_MODE_32B_ADDR;
    }
    err = chip->chip_drv->config_host_io_mode(chip, config_io_flags);

    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGE(TAG, "configure host io mode failed - unsupported");
        return err;
    }

    while (err == ESP_OK && length > 0) {
        memset(temp_buffer, 0xFF, sizeof(temp_buffer));
        uint32_t read_len = chip->host->driver->read_data_slicer(chip->host, address, length, &align_address, page_size);
        uint32_t left_off = address - align_address;
        uint32_t data_len = MIN(align_address + read_len, address + length) - address;
        err = chip->host->driver->read(chip->host, temp_buffer, align_address, read_len);

        memcpy(buffer, temp_buffer + left_off, data_len);

        address += data_len;
        buffer = (void *)((intptr_t)buffer + data_len);
        length = length - data_len;
    }

    return err;
}

esp_err_t spi_flash_chip_winbond_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err;

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);

    if (err == ESP_OK) {
        // Perform the actual Page Program command
        err = spi_flash_command_winbond_program_4B(chip, buffer, address, length);
        if (err != ESP_OK) {
            return err;
        }

        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->page_program_timeout);
    }
    return err;
}

esp_err_t spi_flash_chip_winbond_erase_sector(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }

    if (err == ESP_OK) {
        err = spi_flash_command_winbond_erase_sector_4B(chip, start_address);
        if (err != ESP_OK) {
            return err;
        }
        //to save time, flush cache here
        if (chip->host->driver->flush_cache) {
            err = chip->host->driver->flush_cache(chip->host, start_address, chip->chip_drv->sector_size);
            if (err != ESP_OK) {
                return err;
            }
        }
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->sector_erase_timeout);
    }
    return err;
}

esp_err_t spi_flash_chip_winbond_erase_block(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }

    if (err == ESP_OK) {
        err = spi_flash_command_erase_block_4B(chip, start_address);
        if (err != ESP_OK) {
            return err;
        }
        //to save time, flush cache here
        if (chip->host->driver->flush_cache) {
            err = chip->host->driver->flush_cache(chip->host, start_address, chip->chip_drv->block_erase_size);
            if (err != ESP_OK) {
                return err;
            }
        }
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->block_erase_timeout);
    }
    return err;
}

spi_flash_caps_t spi_flash_chip_winbond_get_caps(esp_flash_t *chip)
{
    spi_flash_caps_t caps_flags = 0;
    // 32M-bits address support
    if ((chip->chip_id & 0xFF) >= 0x19) {
        caps_flags |= SPI_FLASH_CHIP_CAP_32MB_SUPPORT;
    }
    // flash-suspend is not supported
    // flash read unique id.
    caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
    return caps_flags;
}

static const char chip_name[] = "winbond";

// The issi chip can use the functions for generic chips except from set read mode and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_winbond = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_winbond_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_winbond_erase_sector,
    .erase_block = spi_flash_chip_winbond_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_winbond_read,
    .write = spi_flash_chip_generic_write,
    .program_page = spi_flash_chip_winbond_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,
    .set_io_mode = spi_flash_chip_generic_set_io_mode,
    .get_io_mode = spi_flash_chip_generic_get_io_mode,

    .read_reg = spi_flash_chip_generic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_generic_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_generic_read_unique_id,
    .get_chip_caps = spi_flash_chip_winbond_get_caps,
    .config_host_io_mode = spi_flash_chip_generic_config_host_io_mode,
};


static esp_err_t spi_flash_command_winbond_program_4B(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    bool addr_4b = ADDR_32BIT(address);
    spi_flash_trans_t t = {
        .command = (addr_4b? CMD_PROGRAM_PAGE_4B: CMD_PROGRAM_PAGE),
        .address_bitlen = (addr_4b? 32: 24),
        .address = address,
        .mosi_len = length,
        .mosi_data = buffer,
    };
    return chip->host->driver->common_command(chip->host, &t);
}

esp_err_t spi_flash_command_winbond_erase_sector_4B(esp_flash_t *chip, uint32_t start_address)
{
    bool addr_4b = ADDR_32BIT(start_address);
    spi_flash_trans_t t = {
        .command = (addr_4b? CMD_SECTOR_ERASE_4B: CMD_SECTOR_ERASE),
        .address_bitlen = (addr_4b? 32: 24),
        .address = start_address,
    };
    return chip->host->driver->common_command(chip->host, &t);
}

esp_err_t spi_flash_command_erase_block_4B(esp_flash_t *chip, uint32_t start_address)
{
    bool addr_4b = ADDR_32BIT(start_address);
    spi_flash_trans_t t = {
        .command = (addr_4b? CMD_LARGE_BLOCK_ERASE_4B: CMD_LARGE_BLOCK_ERASE),
        .address_bitlen = (addr_4b? 32: 24),
        .address = start_address,
    };
    return chip->host->driver->common_command(chip->host, &t);
}
