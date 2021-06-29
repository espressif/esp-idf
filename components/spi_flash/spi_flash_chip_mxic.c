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
#include "spi_flash_chip_generic.h"
#include "spi_flash_defs.h"
#include "esp_log.h"
#include "string.h"
#include <sys/param.h> // For MIN/MAX

/* Driver for MXIC flash chip */

extern flash_chip_dummy_t *rom_flash_chip_dummy;

esp_err_t spi_flash_chip_mxic_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0xC2;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_issi_set_io_mode(esp_flash_t *chip);
esp_err_t spi_flash_chip_issi_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode);

// Use the same implementation as ISSI chips
#define spi_flash_chip_mxic_set_io_mode spi_flash_chip_issi_set_io_mode
#define spi_flash_chip_mxic_get_io_mode spi_flash_chip_issi_get_io_mode
#define spi_flash_chip_mxic_read_reg        spi_flash_chip_generic_read_reg

static const char chip_name[] = "mxic";

esp_err_t spi_flash_chip_mxic_read_unique_id(esp_flash_t *chip, uint64_t* flash_unique_id)
{
    //MXIC not support read unique id.
    ESP_LOGE(chip_name, "chip %s doesn't support reading unique id", chip->chip_drv->name);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t spi_flash_chip_mxic_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = ESP_OK;
    const uint32_t page_size = chip->chip_drv->page_size;
    uint32_t align_address;
    uint8_t temp_buffer[64]; //spiflash hal max length of read no longer than 64byte

    // Configure the host, and return
    uint32_t addr_bitlen = SPI_FLASH_FASTRD_ADDR_BITLEN;
    uint32_t dummy_cyclelen_base = rom_flash_chip_dummy->fastrd_dummy_bitlen;;
    uint32_t read_command = CMD_FASTRD;
    uint32_t read_mode = SPI_FLASH_FASTRD;

    err = chip->host->driver->configure_host_io_mode(chip->host, read_command, addr_bitlen, dummy_cyclelen_base, read_mode);

    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGE(chip_name, "configure host io mode failed - unsupported");
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

spi_flash_caps_t spi_flash_chip_mxic_get_caps(esp_flash_t *chip)
{
    spi_flash_caps_t caps_flags = 0;
    // 32-bit-address flash is not supported
    // flash-suspend is not supported
    // reading unique id is not supported.
    return caps_flags;
}

// The mxic chip can use the functions for generic chips except from set read mode and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_mxic = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_mxic_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_generic_erase_sector,
    .erase_block = spi_flash_chip_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_generic_read,
    .write = spi_flash_chip_generic_write,
    .program_page = spi_flash_chip_generic_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,
    .set_io_mode = spi_flash_chip_mxic_set_io_mode,
    .get_io_mode = spi_flash_chip_mxic_get_io_mode,

    .read_reg = spi_flash_chip_mxic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_generic_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_mxic_read_unique_id,
    .get_chip_caps = spi_flash_chip_mxic_get_caps,
};
