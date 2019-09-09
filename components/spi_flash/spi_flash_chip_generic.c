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
#include <sys/param.h> // For MIN/MAX
#include "spi_flash_chip_generic.h"
#include "spi_flash_defs.h"
#include "esp_log.h"

static const char TAG[] = "chip_generic";

#define SPI_FLASH_GENERIC_CHIP_ERASE_TIMEOUT 4000
#define SPI_FLASH_GENERIC_SECTOR_ERASE_TIMEOUT 500
#define SPI_FLASH_GENERIC_BLOCK_ERASE_TIMEOUT 1000

#define DEFAULT_IDLE_TIMEOUT 200
#define DEFAULT_PAGE_PROGRAM_TIMEOUT 500

esp_err_t spi_flash_chip_generic_probe(esp_flash_t *chip, uint32_t flash_id)
{
    // This is the catch-all probe function, claim the chip always if nothing
    // else has claimed it yet.
    return ESP_OK;
}

esp_err_t spi_flash_chip_generic_reset(esp_flash_t *chip)
{
    //this is written following the winbond spec..
    spi_flash_trans_t t;
    t = (spi_flash_trans_t) {
        .command = CMD_RST_EN,
    };
    esp_err_t err = chip->host->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    t = (spi_flash_trans_t) {
        .command = CMD_RST_DEV,
    };
    err = chip->host->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    return err;
}

esp_err_t spi_flash_chip_generic_detect_size(esp_flash_t *chip, uint32_t *size)
{
    uint32_t id = chip->chip_id;
    *size = 0;

    /* Can't detect size unless the high byte of the product ID matches the same convention, which is usually 0x40 or
     * 0xC0 or similar. */
    if ((id & 0x0F00) != 0) {
        return ESP_ERR_FLASH_UNSUPPORTED_CHIP;
    }

    *size = 1 << (id & 0xFF);
    return ESP_OK;
}


esp_err_t spi_flash_chip_generic_erase_chip(esp_flash_t *chip)
{
    esp_err_t err;

    err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    }
    if (err == ESP_OK) {
        chip->host->erase_chip(chip->host);
        //to save time, flush cache here
        if (chip->host->flush_cache) {
            err = chip->host->flush_cache(chip->host, 0, chip->size);
            if (err != ESP_OK) {
                return err;
            }
        }
        err = chip->chip_drv->wait_idle(chip, SPI_FLASH_GENERIC_CHIP_ERASE_TIMEOUT);
    }
    return err;
}

esp_err_t spi_flash_chip_generic_erase_sector(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    }
    if (err == ESP_OK) {
        chip->host->erase_sector(chip->host, start_address);
        //to save time, flush cache here
        if (chip->host->flush_cache) {
            err = chip->host->flush_cache(chip->host, start_address, chip->chip_drv->sector_size);
            if (err != ESP_OK) {
                return err;
            }
        }
        err = chip->chip_drv->wait_idle(chip, SPI_FLASH_GENERIC_SECTOR_ERASE_TIMEOUT);
    }
    return err;
}

esp_err_t spi_flash_chip_generic_erase_block(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    }
    if (err == ESP_OK) {
        chip->host->erase_block(chip->host, start_address);
        //to save time, flush cache here
        if (chip->host->flush_cache) {
            err = chip->host->flush_cache(chip->host, start_address, chip->chip_drv->block_erase_size);
            if (err != ESP_OK) {
                return err;
            }
        }
        err = chip->chip_drv->wait_idle(chip, SPI_FLASH_GENERIC_BLOCK_ERASE_TIMEOUT);
    }
    return err;
}

esp_err_t spi_flash_chip_generic_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = ESP_OK;
    // Configure the host, and return
    spi_flash_chip_generic_config_host_io_mode(chip);

    while (err == ESP_OK && length > 0) {
        uint32_t read_len = MIN(length, chip->host->max_read_bytes);
        err = chip->host->read(chip->host, buffer, address, read_len);

        buffer += read_len;
        length -= read_len;
        address += read_len;
    }

    return err;
}

esp_err_t spi_flash_chip_generic_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err;

    err = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);

    if (err == ESP_OK) {
        // Perform the actual Page Program command
        chip->host->program_page(chip->host, buffer, address, length);

        err = chip->chip_drv->wait_idle(chip, DEFAULT_PAGE_PROGRAM_TIMEOUT);
    }
    return err;
}

esp_err_t spi_flash_chip_generic_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = ESP_OK;
    const uint32_t page_size = chip->chip_drv->page_size;

    while (err == ESP_OK && length > 0) {
        uint32_t page_len = MIN(chip->host->max_write_bytes, MIN(page_size, length));
        if ((address + page_len) / page_size != address / page_size) {
            // Most flash chips can't page write across a page boundary
            page_len = page_size - (address % page_size);
        }

        err = chip->chip_drv->set_chip_write_protect(chip, false);

        if (err == ESP_OK) {
            err = chip->chip_drv->program_page(chip, buffer, address, page_len);
            address += page_len;
            buffer = (void *)((intptr_t)buffer + page_len);
            length -= page_len;
        }
    }
    if (err == ESP_OK && chip->host->flush_cache) {
        err = chip->host->flush_cache(chip->host, address, length);
    }
    return err;
}

esp_err_t spi_flash_chip_generic_write_encrypted(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    return ESP_ERR_FLASH_UNSUPPORTED_HOST; // TODO
}

esp_err_t spi_flash_chip_generic_set_write_protect(esp_flash_t *chip, bool write_protect)
{
    esp_err_t err = ESP_OK;

    err = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);

    if (err == ESP_OK) {
        chip->host->set_write_protect(chip->host, write_protect);
    }

    bool wp_read;
    err = chip->chip_drv->get_chip_write_protect(chip, &wp_read);
    if (err == ESP_OK && wp_read != write_protect) {
        // WREN flag has not been set!
        err = ESP_ERR_NOT_FOUND;
    }
    return err;
}

esp_err_t spi_flash_chip_generic_get_write_protect(esp_flash_t *chip, bool *out_write_protect)
{
    esp_err_t err = ESP_OK;
    uint8_t status;
    assert(out_write_protect!=NULL);
    err = chip->host->read_status(chip->host, &status);
    if (err != ESP_OK) {
        return err;
    }

    *out_write_protect = ((status & SR_WREN) == 0);
    return err;
}

esp_err_t spi_flash_generic_wait_host_idle(esp_flash_t *chip, uint32_t *timeout_ms)
{
    while (chip->host->host_idle(chip->host) && *timeout_ms > 0) {
        if (*timeout_ms > 1) {
            chip->os_func->delay_ms(chip->os_func_data, 1);
        }
        (*timeout_ms)--;
    }
    return (*timeout_ms > 0) ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t spi_flash_chip_generic_wait_idle(esp_flash_t *chip, uint32_t timeout_ms)
{
    timeout_ms++; // allow at least one pass before timeout, last one has no sleep cycle

    uint8_t status = 0;
    while (timeout_ms > 0) {

        esp_err_t err = spi_flash_generic_wait_host_idle(chip, &timeout_ms);
        if (err != ESP_OK) {
            return err;
        }

        err = chip->host->read_status(chip->host, &status);
        if (err != ESP_OK) {
            return err;
        }
        if ((status & SR_WIP) == 0) {
            break; // Write in progress is complete
        }
        if (timeout_ms > 1) {
            chip->os_func->delay_ms(chip->os_func_data, 1);
        }
        timeout_ms--;
    }

    return (timeout_ms > 0) ?  ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip)
{
    uint32_t dummy_cyclelen_base;
    uint32_t addr_bitlen;
    uint32_t read_command;

    switch (chip->read_mode) {
    case SPI_FLASH_QIO:
        //for QIO mode, the 4 bit right after the address are used for continuous mode, should be set to 0 to avoid that.
        addr_bitlen = 32;
        dummy_cyclelen_base = 4;
        read_command = CMD_FASTRD_QIO;
        break;
    case SPI_FLASH_QOUT:
        addr_bitlen = 24;
        dummy_cyclelen_base = 8;
        read_command = CMD_FASTRD_QUAD;
        break;
    case SPI_FLASH_DIO:
        //for DIO mode, the 4 bit right after the address are used for continuous mode, should be set to 0 to avoid that.
        addr_bitlen = 28;
        dummy_cyclelen_base = 2;
        read_command = CMD_FASTRD_DIO;
        break;
    case SPI_FLASH_DOUT:
        addr_bitlen = 24;
        dummy_cyclelen_base = 8;
        read_command = CMD_FASTRD_DUAL;
        break;
    case SPI_FLASH_FASTRD:
        addr_bitlen = 24;
        dummy_cyclelen_base = 8;
        read_command = CMD_FASTRD;
        break;
    case SPI_FLASH_SLOWRD:
        addr_bitlen = 24;
        dummy_cyclelen_base = 0;
        read_command = CMD_READ;
        break;
    default:
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }

    return chip->host->configure_host_io_mode(chip->host, read_command, addr_bitlen, dummy_cyclelen_base,
                                                chip->read_mode);
}

esp_err_t spi_flash_chip_generic_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode)
{
    // On "generic" chips, this involves checking
    // bit 1 (QE) of RDSR2 (35h) result
    // (it works this way on GigaDevice & Fudan Micro chips, probably others...)
    const uint8_t BIT_QE = 1 << 1;
    uint32_t sr;
    esp_err_t ret = spi_flash_common_read_status_8b_rdsr2(chip, &sr);
    if (ret == ESP_OK) {
        *out_io_mode = ((sr & BIT_QE)? SPI_FLASH_QOUT: 0);
    }
    return ret;
}

esp_err_t spi_flash_chip_generic_set_io_mode(esp_flash_t *chip)
{
    // On "generic" chips, this involves checking
    // bit 9 (QE) of RDSR (05h) result
    const uint32_t BIT_QE = 1 << 9;
    return spi_flash_common_set_io_mode(chip,
                                        spi_flash_common_write_status_16b_wrsr,
                                        spi_flash_common_read_status_16b_rdsr_rdsr2,
                                        BIT_QE);
}

static const char chip_name[] = "generic";

const spi_flash_chip_t esp_flash_chip_generic = {
    .name = chip_name,
    .probe = spi_flash_chip_generic_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_generic_erase_sector,
    .erase_block = spi_flash_chip_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    // TODO: figure out if generic chip-wide protection bits exist across some manufacturers
    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    // Chip write protection regions do not appear to be standardised
    // at all, this is implemented in chip-specific drivers only.
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
};

/*******************************************************************************
 * Utility functions
 ******************************************************************************/

static esp_err_t spi_flash_common_read_qe_sr(esp_flash_t *chip, uint8_t qe_rdsr_command, uint8_t qe_sr_bitwidth, uint32_t *sr)
{
    spi_flash_trans_t t = {
        .command = qe_rdsr_command,
        .mosi_data = 0,
        .mosi_len = 0,
        .miso_len = qe_sr_bitwidth,
    };
    esp_err_t ret = chip->host->common_command(chip->host, &t);
    *sr = t.miso_data[0];
    return ret;
}

static esp_err_t spi_flash_common_write_qe_sr(esp_flash_t *chip, uint8_t qe_wrsr_command, uint8_t qe_sr_bitwidth, uint32_t qe)
{
    spi_flash_trans_t t = {
        .command = qe_wrsr_command,
        .mosi_data = qe,
        .mosi_len = qe_sr_bitwidth,
        .miso_len = 0,
    };
    return chip->host->common_command(chip->host, &t);
}

esp_err_t spi_flash_common_read_status_16b_rdsr_rdsr2(esp_flash_t* chip, uint32_t* out_sr)
{
    uint32_t sr, sr2;
    esp_err_t ret = spi_flash_common_read_qe_sr(chip, CMD_RDSR2, 8, &sr2);
    if (ret == ESP_OK) {
        ret = spi_flash_common_read_qe_sr(chip, CMD_RDSR, 8, &sr);
    }
    if (ret == ESP_OK) {
        *out_sr = (sr & 0xff) | ((sr2 & 0xff) << 8);
    }
    return ret;
}

esp_err_t spi_flash_common_read_status_8b_rdsr2(esp_flash_t* chip, uint32_t* out_sr)
{
    return spi_flash_common_read_qe_sr(chip, CMD_RDSR2, 8, out_sr);
}

esp_err_t spi_flash_common_read_status_8b_rdsr(esp_flash_t* chip, uint32_t* out_sr)
{
    return spi_flash_common_read_qe_sr(chip, CMD_RDSR, 8, out_sr);
}

esp_err_t spi_flash_common_write_status_16b_wrsr(esp_flash_t* chip, uint32_t sr)
{
    return spi_flash_common_write_qe_sr(chip, CMD_WRSR, 16, sr);
}

esp_err_t spi_flash_common_write_status_8b_wrsr(esp_flash_t* chip, uint32_t sr)
{
    return spi_flash_common_write_qe_sr(chip, CMD_WRSR, 8, sr);
}

esp_err_t spi_flash_common_write_status_8b_wrsr2(esp_flash_t* chip, uint32_t sr)
{
    return spi_flash_common_write_qe_sr(chip, CMD_WRSR2, 8, sr);
}

esp_err_t spi_flash_common_set_io_mode(esp_flash_t *chip, esp_flash_wrsr_func_t wrsr_func, esp_flash_rdsr_func_t rdsr_func, uint32_t qe_sr_bit)
{
    esp_err_t ret = ESP_OK;
    const bool is_quad_mode = esp_flash_is_quad_mode(chip);
    bool update_config = false;
    const bool force_check = true; //in case some chips doesn't support erase QE

    bool need_check = is_quad_mode;
    if (force_check) {
        need_check = true;
    }

    uint32_t sr_update;
    if (need_check) {
        // Ensure quad modes are enabled, using the Quad Enable parameters supplied.
        uint32_t sr;
        ret = (*rdsr_func)(chip, &sr);
        if (ret != ESP_OK) {
            return ret;
        }
        ESP_EARLY_LOGD(TAG, "set_io_mode: status before 0x%x", sr);
        if (is_quad_mode) {
            sr_update = sr | qe_sr_bit;
        } else {
            sr_update = sr & (~qe_sr_bit);
        }
        ESP_EARLY_LOGV(TAG, "set_io_mode: status update 0x%x", sr_update);
        if (sr != sr_update) {
            update_config = true;
        }
    }

    if (update_config) {
        //some chips needs the write protect to be disabled before writing to Status Register
        chip->chip_drv->set_chip_write_protect(chip, false);

        ret = (*wrsr_func)(chip, sr_update);
        if (ret != ESP_OK) {
            return ret;
        }

        ret = chip->chip_drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
        if (ret != ESP_OK) {
            return ret;
        }

        /* Check the new QE bit has stayed set */
        uint32_t sr;
        ret = (*rdsr_func)(chip, &sr);
        if (ret != ESP_OK) {
            return ret;
        }
        ESP_EARLY_LOGD(TAG, "set_io_mode: status after 0x%x", sr);
        if (sr != sr_update) {
            ret = ESP_ERR_FLASH_NO_RESPONSE;
        }

        chip->chip_drv->set_chip_write_protect(chip, true);
    }
    return ret;
}
