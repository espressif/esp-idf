/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "esp_log.h"

#include "hal/spi_flash_hal.h"
#include "esp_flash_chips/spi_flash_defs.h"
#include "esp_flash_chips/spi_flash_chip_generic.h"

/* Driver for MXIC flash chip */

esp_err_t spi_flash_chip_mxic_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0xC2;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }
    if (chip->read_mode >= SPI_FLASH_OPI_FLAG) {
        // The code here serve for ordinary mxic chip. If opi mode has been selected, go `spi_flash_chip_mxic_opi.c`
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_mxic_detect_size(esp_flash_t *chip, uint32_t *size)
{
    uint32_t id = chip->chip_id;
    *size = 0;

    /* Can't detect size unless the high byte of the product ID matches the same convention, which is usually 0x40 or
     * 0xC0 or similar. */
    if (((id & 0xFFFF) == 0x0000) || ((id & 0xFFFF) == 0xFFFF)) {
        return ESP_ERR_FLASH_UNSUPPORTED_CHIP;
    }

    uint32_t mem_density = (id & 0xFF);
    if (mem_density > 0x30) { // For OPI chips
        mem_density -= 0x20;
    }

    *size = 1 << mem_density;
    return ESP_OK;
}

esp_err_t spi_flash_chip_issi_set_io_mode(esp_flash_t *chip);
esp_err_t spi_flash_chip_issi_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode);

// Use the same implementation as ISSI chips
#define spi_flash_chip_mxic_set_io_mode spi_flash_chip_issi_set_io_mode
#define spi_flash_chip_mxic_get_io_mode spi_flash_chip_issi_get_io_mode
#define spi_flash_chip_mxic_read_reg        spi_flash_chip_generic_read_reg

static const char chip_name[] = "mxic";

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
    .detect_size = spi_flash_chip_mxic_detect_size,
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
    .read_unique_id = spi_flash_chip_generic_read_unique_id_none,
    .get_chip_caps = spi_flash_chip_mxic_get_caps,
    .config_host_io_mode = spi_flash_chip_generic_config_host_io_mode,
};
