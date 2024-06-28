// Copyright 2022-2023 Black Pearl Technology, Inc.
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
#include "spi_flash_chip_at25.h"
#include "spi_flash_defs.h"
#include "esp_log.h"


/* Driver for AT25SF321B flash chip */

esp_err_t spi_flash_chip_at25_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0x1F;
    const uint8_t FAM_ID = 0x80;
    const uint8_t FAM_MASK = 0xE0;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }
    if (((flash_id >> 8) & FAM_MASK) != FAM_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_at25_detect_size(esp_flash_t *chip, uint32_t *size)
{
    uint32_t id = chip->chip_id;
    uint32_t SIZE_MASK = 0x1F;
    *size = 0;
    
    //density mapping
    //1001b = 128Mbit = 16
    //1000b = 64Mbit = 8
    //111b = 32Mbit = 4
    //110b = 16MBit = 2
    //101b = 8MBit = 1
    //100b = 4MBit = 0

    /* middle byte contains chip size data */
    *size = (1 << (((id >> 8) & SIZE_MASK) - 5)) * 1024 * 1024;
    return ESP_OK;
}

spi_flash_caps_t spi_flash_chip_at25_get_caps(esp_flash_t *chip)
{
    spi_flash_caps_t caps_flags = 0;
    // 32-bit-address flash is not supported
    // flash-suspend is not supported
    // flash read unique id.
    caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
    return caps_flags;
}

static const char chip_name[] = "at25";

// The at25 chip can use the functions for generic chips except for get_size and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_at25 = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_at25_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_at25_detect_size,
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
    .set_io_mode = spi_flash_chip_generic_set_io_mode,
    .get_io_mode = spi_flash_chip_generic_get_io_mode,

    .read_reg = spi_flash_chip_generic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_generic_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_generic_read_unique_id,
    .get_chip_caps = spi_flash_chip_at25_get_caps,
    .config_host_io_mode = spi_flash_chip_generic_config_host_io_mode,
};
