/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/param.h> // For MIN/MAX
#include "esp_log.h"
#include "spi_flash_chip_generic.h"
#include "spi_flash_defs.h"

#define W25N_FAMILY 0xFEAA

//static const char TAG[] = "chip_w25n";

/* Driver for Winbond flash chip */
static esp_err_t spi_flash_command_w25n_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);
static esp_err_t spi_flash_command_w25n_erase_block(esp_flash_t *chip, uint32_t start_address);

esp_err_t spi_flash_chip_w25n_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0xEF;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    if ((flash_id >> 8) != W25N_FAMILY) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_w25n_reset(esp_flash_t *chip)
{
    //this is written following the winbond spec..
    spi_flash_trans_t t;
    esp_err_t err;
    t = (spi_flash_trans_t) {
        .command = 0xFF,
    };
    err = chip->host->driver->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    return err;
}

esp_err_t spi_flash_chip_w25n_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err;

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);

    if (err == ESP_OK) {
        // Perform the actual Page Program command
        err = spi_flash_command_w25n_program(chip, buffer, address, length);
        if (err != ESP_OK) {
            return err;
        }

        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->page_program_timeout);
    }
    return err;
}

esp_err_t spi_flash_chip_w25n_erase_block(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }

    if (err == ESP_OK) {
        err = spi_flash_command_w25n_erase_block(chip, start_address);
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

esp_err_t spi_flash_chip_w25n_read_reg(esp_flash_t* chip, uint8_t reg_id, uint32_t* out_reg)
{
    uint32_t stat_buf = 0;

    if (reg_id == SPI_FLASH_REG_STATUS) {
        // We use the default status address to identify if we need to query the status
        // We need to do this because read_reg it's used by non-overriden generic functions
        reg_id = 0xC0;
    }

    spi_flash_trans_t t = {
        .command = CMD_RDSR,
        .mosi_data = ((uint8_t*) &reg_id),
        .mosi_len = 1,
        .miso_data = ((uint8_t*) &stat_buf),
        .miso_len = 1
    };

    esp_err_t err = chip->host->driver->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }
    *out_reg = stat_buf;
    return ESP_OK;
}

spi_flash_caps_t spi_flash_chip_w25n_get_caps(esp_flash_t *chip)
{
    // For generic part flash capability, take the XMC chip as reference.
    spi_flash_caps_t caps_flags = 0;

    // flash read unique id.
    caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
    return caps_flags;
}

static const char chip_name[] = "winbond_w25n";

// The issi chip can use the functions for generic chips except from set read mode and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_winbond_w25n = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_w25n_probe,
    .reset = spi_flash_chip_w25n_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = NULL,
    .erase_sector = NULL,
    .erase_block = spi_flash_chip_w25n_erase_block,
    .block_erase_size = 128 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_generic_read,
    .write = spi_flash_chip_generic_write,
    .program_page = spi_flash_chip_w25n_page_program,
    .page_size = 2048,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,
    .set_io_mode = spi_flash_chip_generic_set_io_mode,
    .get_io_mode = spi_flash_chip_generic_get_io_mode,

    .read_reg = spi_flash_chip_w25n_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_generic_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_generic_read_unique_id,
    .get_chip_caps = spi_flash_chip_w25n_get_caps,
    .config_host_io_mode = spi_flash_chip_generic_config_host_io_mode,
};


static esp_err_t spi_flash_command_w25n_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    spi_flash_trans_t t = {
        .command = CMD_PROGRAM_PAGE,
        .address_bitlen = 16,
        .address = address,
        .mosi_len = length,
        .mosi_data = buffer,
    };
    return chip->host->driver->common_command(chip->host, &t);
}

esp_err_t spi_flash_command_w25n_erase_block(esp_flash_t *chip, uint32_t start_address)
{
    // 128KB block erase
    spi_flash_trans_t t = {
        .command = CMD_LARGE_BLOCK_ERASE,
        .dummy_bitlen = 8,
        .address_bitlen = 16,
        .address = start_address, // Page address
    };
    return chip->host->driver->common_command(chip->host, &t);
}
