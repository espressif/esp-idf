/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "esp_log.h"
#include "string.h"
#include <sys/param.h> // For MIN/MAX

#include "hal/spi_flash_hal.h"
#include "esp_flash_chips/spi_flash_defs.h"
#include "esp_flash_chips/spi_flash_chip_generic.h"

#define CMD_OPI_FLASH_MXIC(cmd)              ((((~(cmd) & 0xff) << 8)) | ((cmd) & 0xff))
#define CMD_OPI_FLASH_MXIC_CHIP_ERASE        0x9F60
#define CMD_OPI_FLASH_MXIC_READ_STR          0x13EC
#define CMD_OPI_FLASH_MXIC_READ_DTR          0x11EE
#define CMD_OPI_FLASH_MXIC_RDCR2             0x8E71
#define CMD_OPI_FLASH_MXIC_WRCR2             0x8D72

/* Driver for MXIC OPI flash chip */
static const char chip_name[] = "mxic (opi)";

esp_err_t spi_flash_chip_mxic_opi_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0xC2;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    if (chip->read_mode < SPI_FLASH_OPI_FLAG) {
        // The code here serve for opi flash under opi mode only, for ordinary mxic chip, go `spi_flash_chip_mxic.c`
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_mxic_opi_detect_size(esp_flash_t *chip, uint32_t *size)
{
    uint32_t id = chip->chip_id;
    *size = 0;

    /* Can't detect size unless the high byte of the product ID matches the same convention, which is usually 0x40 or
     * 0xC0 or similar. */
    if (((id & 0xFFFF) == 0x0000) || ((id & 0xFFFF) == 0xFFFF)) {
        return ESP_ERR_FLASH_UNSUPPORTED_CHIP;
    }

    *size = 1 << ((id & 0xFF) - 0x20);
    return ESP_OK;
}

spi_flash_caps_t spi_flash_chip_mxic_opi_get_caps(esp_flash_t *chip)
{
    spi_flash_caps_t caps_flags = 0;
    caps_flags |= SPI_FLASH_CHIP_CAP_32MB_SUPPORT;
    // flash-suspend is not supported yet. // IDF-3852
    // reading unique id is not supported.
    return caps_flags;
}

esp_err_t spi_flash_chip_mxic_opi_set_write_protect(esp_flash_t *chip, bool write_protect)
{
    esp_err_t err = ESP_OK;

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    spi_flash_trans_t t = {};
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        if(write_protect) {
            t.command = CMD_OPI_FLASH_MXIC(CMD_WRDI);
        } else {
            t.command = CMD_OPI_FLASH_MXIC(CMD_WREN);
        }
        err = chip->host->driver->common_command(chip->host, &t);
    }

    bool wp_read;
    err = chip->chip_drv->get_chip_write_protect(chip, &wp_read);
    if (err == ESP_OK && wp_read != write_protect) {
        err = ESP_ERR_NOT_FOUND;
    }
    return err;
}

static void spi_flash_chip_mxic_opi_get_data_length_zoom(esp_flash_io_mode_t io_mode, uint32_t *length_zoom)
{
    /* Under STR mode, one byte occupies one single clock. While under DTR mode, one byte occupies half clock.
       For example, if an operation needs 3 clock dummy, host send 3 dummy bytes under STR mode, while 6 dummy bytes under DTR mode.
       Therefore, we need to adjust data zoom to fit the clock here. */
    assert((io_mode == SPI_FLASH_OPI_STR) || (io_mode == SPI_FLASH_OPI_DTR));
    *length_zoom = (io_mode == SPI_FLASH_OPI_STR) ? 1 : 2;
}

esp_err_t spi_flash_chip_mxic_opi_read_id(esp_flash_t *chip, uint32_t* out_chip_id)
{
    uint64_t id_buf = 0;
    uint32_t length_zoom;
    spi_flash_chip_mxic_opi_get_data_length_zoom(chip->read_mode, &length_zoom);
    spi_flash_trans_t t = {
        .command = CMD_OPI_FLASH_MXIC(CMD_RDID),
        .miso_len = 3 * length_zoom,
        .dummy_bitlen = 4 * length_zoom,
        .address_bitlen = 32,
        .miso_data = ((uint8_t*) &id_buf),
    };

    chip->host->driver->common_command(chip->host, &t);

    if(chip->read_mode == SPI_FLASH_OPI_DTR) {
        // Adjust the id_buf in DTR mode, because in DTR mode, the data back in STR rule.
        // So it looks like [MD, MD, MT, MT, MID, MID], adjust it to [MD, MT, MID] here.
        ESP_EARLY_LOGV(chip_name, "raw_chip_id: %llx\n", id_buf);
        id_buf = (id_buf & 0xff) | ((id_buf & 0xff0000) >> 8) | ((id_buf & 0xff00000000) >> 16);
    } else {
        ESP_EARLY_LOGV(chip_name, "raw_chip_id: %X\n", id_buf);
    }

    uint32_t raw_flash_id = __builtin_bswap32(id_buf);
    if (raw_flash_id == 0xFFFFFF || raw_flash_id == 0) {
        ESP_EARLY_LOGE(chip_name, "no response\n");
        return ESP_ERR_FLASH_NO_RESPONSE;
    }

    *out_chip_id = (raw_flash_id >> 8);
    ESP_EARLY_LOGV(chip_name, "chip_id: %X\n", *out_chip_id);
    return ESP_OK;
}

esp_err_t spi_flash_chip_mxic_opi_read_reg(esp_flash_t *chip, spi_flash_register_t reg_id, uint32_t* out_reg)
{
    uint32_t stat_buf = 0;
    uint32_t length_zoom;
    spi_flash_chip_mxic_opi_get_data_length_zoom(chip->read_mode, &length_zoom);
    spi_flash_trans_t t = {
        .command = CMD_OPI_FLASH_MXIC(CMD_RDSR),
        .miso_data = ((uint8_t*) &stat_buf),
        .miso_len = 1 * length_zoom,
        .address_bitlen = 32,
        .dummy_bitlen = 4 * length_zoom,
    };
    esp_err_t err = chip->host->driver->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    // For DTR mode, RDSR result like [SR1, SR1], just keeping one SR1.
    *out_reg = (stat_buf & 0xff);
    return ESP_OK;
}

esp_err_t spi_flash_chip_mxic_opi_get_write_protect(esp_flash_t *chip, bool *out_write_protected)
{
    esp_err_t err = ESP_OK;
    uint32_t status;
    assert(out_write_protected!=NULL);
    err = chip->chip_drv->read_reg(chip, SPI_FLASH_REG_STATUS, &status);
    if (err != ESP_OK) {
        return err;
    }

    *out_write_protected = ((status & SR_WREN) == 0);
    return err;
}

esp_err_t spi_flash_chip_mxic_opi_erase_chip(esp_flash_t *chip)
{
    esp_err_t err;

    err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }

    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        // Do erase chip here.
        spi_flash_trans_t t = {
            .command = CMD_OPI_FLASH_MXIC_CHIP_ERASE,
        };
        err = chip->host->driver->common_command(chip->host, &t);
        chip->busy = 1;
#ifdef CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED
        err = chip->chip_drv->wait_idle(chip, ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
#else
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->chip_erase_timeout);
#endif
    }
    // Ensure WEL is 0, even if the erase failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        chip->chip_drv->set_chip_write_protect(chip, true);
    }

    return err;

}

esp_err_t spi_flash_chip_mxic_opi_erase_sector(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        spi_flash_trans_t t = {
            .command = CMD_OPI_FLASH_MXIC(CMD_SECTOR_ERASE_4B),
            .address_bitlen = 32,
            .address = start_address,
        };
        err = chip->host->driver->common_command(chip->host, &t);
        chip->busy = 1;
#ifdef CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED
        err = chip->chip_drv->wait_idle(chip, ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
#else
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->sector_erase_timeout);
#endif
    }
    // Ensure WEL is 0, even if the erase failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }

    return err;
}

esp_err_t spi_flash_chip_mxic_opi_erase_block(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        spi_flash_trans_t t = {
            .command = CMD_OPI_FLASH_MXIC(CMD_LARGE_BLOCK_ERASE_4B),
            .address_bitlen = 32,
            .address = start_address,
        };
        err = chip->host->driver->common_command(chip->host, &t);
        chip->busy = 1;
#ifdef CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED
        err = chip->chip_drv->wait_idle(chip, ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
#else
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->block_erase_timeout);
#endif
    }
    // Ensure WEL is 0, even if the erase failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }

    return err;
}

esp_err_t spi_flash_chip_mxic_opi_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err;

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        // Perform the actual Page Program command
        spi_flash_trans_t t = {
            .command = CMD_OPI_FLASH_MXIC(CMD_PROGRAM_PAGE_4B),
            .address_bitlen = 32,
            .address = address,
            .mosi_len = length,
            .mosi_data = buffer,
        };
        chip->host->driver->common_command(chip->host, &t);
        chip->busy = 1;

        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->page_program_timeout);
    }
    // Ensure WEL is 0, even if the page program failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }
    return err;
}

esp_err_t spi_flash_chip_mxic_opi_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = ESP_OK;
    const uint32_t page_size = chip->chip_drv->page_size;
    uint32_t align_address;
    uint8_t temp_buffer[64]; //spiflash hal max length of write no longer than 64byte

    while (err == ESP_OK && length > 0) {
        memset(temp_buffer, 0xFF, sizeof(temp_buffer));
        uint32_t page_len = chip->host->driver->write_data_slicer(chip->host, address, length, &align_address, page_size);
        uint32_t left_off = address - align_address;
        uint32_t write_len = MIN(align_address + page_len, address + length) - address;
        memcpy(temp_buffer + left_off, buffer, write_len);

        err = chip->chip_drv->set_chip_write_protect(chip, false);
        if (err == ESP_OK && length > 0) {
            err = chip->chip_drv->program_page(chip, temp_buffer, align_address, page_len);

            address += write_len;
            buffer = (void *)((intptr_t)buffer + write_len);
            length -= write_len;
        }
    }
    // The caller is responsible to do host->driver->flush_cache, because this function may be
    // called in small pieces. Frequency call of flush cache will do harm to the performance.
    return err;
}

esp_err_t spi_flash_chip_mxic_opi_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode)
{
    uint32_t stat_buf = 0;
    uint32_t length_zoom;
    spi_flash_chip_mxic_opi_get_data_length_zoom(chip->read_mode, &length_zoom);

    spi_flash_trans_t t = {
        .command = CMD_OPI_FLASH_MXIC_RDCR2,
        .dummy_bitlen = 4 * length_zoom,
        .miso_data = ((uint8_t*) &stat_buf),
        .miso_len = 1 * length_zoom,
        .address_bitlen = 32,
    };
    esp_err_t err = chip->host->driver->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    // For DTR mode, RDSR result like [CR1, CR1], just keeping one CR1.
    switch (stat_buf & 0xff)
    {
    case 0x1:
        *out_io_mode = SPI_FLASH_OPI_STR;
        break;
    case 0x2:
        *out_io_mode = SPI_FLASH_OPI_DTR;
        break;
    default:
        // wrong mode.
        *out_io_mode = 0;
        break;
    }
    if (*out_io_mode != chip->read_mode) {
        // Current chip mode is not the mode we configured.
        *out_io_mode = 0;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_xmic_opi_set_io_mode(esp_flash_t *chip)
{
    // TODO: configure opi flash chip set io mode, only useful for external flash currently.
    // For main flash, we already set io mode when chip starts up. But for external flash,
    // We need to set mode when flash initialized, so keeping this for future usage.
    return ESP_OK;
}

// This function should only be called after opi mode initialization. So, only configure for OPI-STR/OPI-DTR mode
// not support other mode in this file, return `ESP_ERR_FLASH_NOT_INITIALISED` directly.
esp_err_t spi_flash_chip_xmic_opi_config_host_io_mode(esp_flash_t *chip, uint32_t flags)
{
    uint32_t dummy_cyclelen_base;
    uint32_t addr_bitlen;
    uint32_t read_command;
    esp_flash_io_mode_t read_mode = chip->read_mode;

    switch (read_mode & 0xFFFF) {
    case SPI_FLASH_OPI_STR:
        addr_bitlen = SPI_FLASH_OPISTR_ADDR_BITLEN;
        dummy_cyclelen_base = SPI_FLASH_OPISTR_DUMMY_BITLEN;
        read_command = CMD_OPI_FLASH_MXIC_READ_STR;
        break;
    case SPI_FLASH_OPI_DTR:
        addr_bitlen = SPI_FLASH_OPIDTR_ADDR_BITLEN;
        dummy_cyclelen_base = SPI_FLASH_OPIDTR_DUMMY_BITLEN;
        read_command = CMD_OPI_FLASH_MXIC_READ_DTR;
        break;
    default:
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }

    return chip->host->driver->configure_host_io_mode(chip->host, read_command, addr_bitlen, dummy_cyclelen_base, read_mode);
}

// Most of mxic opi implementations are totally different from that is generic.
// Replace them to opi implementation.
const spi_flash_chip_t esp_flash_chip_mxic_opi = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_mxic_opi_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_mxic_opi_detect_size,
    .erase_chip = spi_flash_chip_mxic_opi_erase_chip,
    .erase_sector = spi_flash_chip_mxic_opi_erase_sector,
    .erase_block = spi_flash_chip_mxic_opi_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_mxic_opi_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_mxic_opi_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_generic_read,
    .write = spi_flash_chip_mxic_opi_write,
    .program_page = spi_flash_chip_mxic_opi_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,

    .set_io_mode = spi_flash_chip_xmic_opi_set_io_mode,
    .get_io_mode = spi_flash_chip_mxic_opi_get_io_mode,

    .read_id = spi_flash_chip_mxic_opi_read_id,
    .read_reg = spi_flash_chip_mxic_opi_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_generic_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_generic_read_unique_id_none,
    .get_chip_caps = spi_flash_chip_mxic_opi_get_caps,
    .config_host_io_mode = spi_flash_chip_xmic_opi_config_host_io_mode,
};
