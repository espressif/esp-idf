// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

/* Driver for ISSI flash chip, as used in ESP32 D2WD */

esp_err_t spi_flash_chip_issi_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0x9D;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    const uint16_t FLASH_ID_MASK = 0xCF00;
    const uint16_t FLASH_ID_VALUE = 0x4000;
    if ((flash_id & FLASH_ID_MASK) != FLASH_ID_VALUE) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_issi_set_io_mode(esp_flash_t *chip)
{
    /* ISSI uses bit 6 of "basic" SR as Quad Enable */
    const uint8_t BIT_QE = 1 << 6;
    return spi_flash_common_set_io_mode(chip,
                                        spi_flash_common_write_status_8b_wrsr,
                                        spi_flash_common_read_status_8b_rdsr,
                                        BIT_QE);
}

esp_err_t spi_flash_chip_issi_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode)
{
    /* ISSI uses bit 6 of "basic" SR as Quad Enable */
    const uint8_t BIT_QE = 1 << 6;
    uint32_t sr;
    esp_err_t ret = spi_flash_common_read_status_8b_rdsr(chip, &sr);
    if (ret == ESP_OK) {
        *out_io_mode = ((sr & BIT_QE)? SPI_FLASH_QOUT: 0);
    }
    return ret;
}


static const char chip_name[] = "issi";

// The issi chip can use the functions for generic chips except from set read mode and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_issi = {
    .name = chip_name,
    .probe = spi_flash_chip_issi_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_generic_erase_sector,
    .erase_block = spi_flash_chip_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    // TODO support protected regions on ISSI flash
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
    .set_io_mode = spi_flash_chip_issi_set_io_mode,
    .get_io_mode = spi_flash_chip_issi_get_io_mode,
};
