// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#include <stdlib.h>
#include "spi_flash_lowlevel_driver.h"
#include "spi_flash_lowlevel_generic.h"

/* Driver for ISSI flash chip, as used in ESP32 D2WD */

esp_flash_err_t issi_probe(esp_flash_chip_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0x9D;
    if (flash_id >> 16 != MFG_ID) {
        return FLASH_ERR_NOT_FOUND;
    }

    const uint16_t FLASH_ID_MASK = 0xCF00;
    const uint16_t FLASH_ID_VALUE = 0x4000;
    if ((flash_id & FLASH_ID_MASK) != FLASH_ID_VALUE) {
        return FLASH_ERR_NOT_FOUND;
    }

    return FLASH_OK;
}

esp_flash_err_t issi_set_read_mode(const esp_flash_chip_t *chip)
{
    /* ISSI uses bit 6 of "basic" SR as Quad Enable */
    const uint8_t BIT_QE = 1<<6;
    return spi_flash_common_set_read_mode(chip, CMD_RDSR, CMD_WRSR, 8, BIT_QE);
}


const esp_flash_driver_t esp_flash_issi_chip_driver = {
    .probe = issi_probe,
    .read_id = spi_flash_generic_read_id,
    .detect_size = spi_flash_generic_detect_size,
    .erase_chip = spi_flash_generic_erase_chip,
    .erase_sector = spi_flash_generic_erase_sector,
    .erase_block = spi_flash_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    // TODO: support get/set chip write protect for ISSI flash
    .get_chip_write_protect = NULL,
    .set_chip_write_protect = NULL,

    // TODO support protected regions on ISSI flash
    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_generic_read,
    .write = spi_flash_generic_write,
    .page_program = spi_flash_generic_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_generic_write_encrypted,

    .write_enable = spi_flash_generic_write_enable,
    .wait_idle = spi_flash_generic_wait_idle,
    .set_read_mode = issi_set_read_mode,
};
