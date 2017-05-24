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
#include <sys/param.h> // For MIN/MAX
#include <xtensa/hal.h>

#include "spi_flash_lowlevel_driver.h"
#include "spi_flash_lowlevel_generic.h"

#define SPI_FLASH_GENERIC_CHIP_ERASE_TIMEOUT 4000
#define SPI_FLASH_GENERIC_SECTOR_ERASE_TIMEOUT 500
#define SPI_FLASH_GENERIC_BLOCK_ERASE_TIMEOUT 1000

#define DEFAULT_IDLE_TIMEOUT 200
#define DEFAULT_PAGE_PROGRAM_TIMEOUT 500

/* Hardware host-specific constants */
#define MAX_WRITE_BYTES 32
#define MAX_READ_BYTES 64

#define ADDRESS_MASK_24BIT 0xFFFFFF

uint32_t spi_flash_common_command(const esp_flash_chip_t *chip, uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
{
    typeof(chip->spi->user2) user2 = {
        .usr_command_value = command,
        .usr_command_bitlen = (8 -1),
    };
    chip->spi->user2 = user2;

    typeof(chip->spi->user) user = {
        .usr_miso = miso_len > 0,
        .usr_mosi = mosi_len > 0,
        .usr_dummy = 0,
        .usr_command = 1,
    };
    chip->spi->user = user;

    chip->spi->ctrl.val = 0;
    chip->spi->miso_dlen.usr_miso_dbitlen = miso_len ? (miso_len - 1) : 0;
    chip->spi->mosi_dlen.usr_mosi_dbitlen = mosi_len ? (mosi_len - 1) : 0;

    // TODO: there's a bug(?) here where if multiple bytes are written
    // with each byte MSB-first (correct), but the bytes are
    // written out LSB first...
    //
    // May be easier to just document this in the function interface...
    chip->spi->data_buf[0] = mosi_data;

    if (spi_flash_uses_gpio_matrix(chip)) {
        /* When flash pins are mapped via GPIO matrix, need a dummy cycle before reading via MISO */
        if (chip->speed == ESP_FLASH_80MHZ) {
            chip->spi->user.usr_dummy = 1;
            chip->spi->user1.usr_dummy_cyclelen = 1;
        } else {
            chip->spi->user.usr_dummy = 1;
            chip->spi->user1.usr_dummy_cyclelen = 0;
        }
    }

    chip->spi->cmd.usr = 1;
    while(chip->spi->cmd.usr != 0)
    { }

    uint32_t miso = chip->spi->data_buf[0];

    return miso;
}

esp_flash_err_t spi_flash_generic_probe(esp_flash_chip_t *chip, uint32_t flash_id)
{
    // This is the catch-all probe function, claim the chip always if nothing
    // else has claimed it yet.
    return FLASH_OK;
}

esp_flash_err_t spi_flash_generic_read_id(const esp_flash_chip_t *chip, uint32_t *id)
{
    uint32_t raw_flash_id = spi_flash_common_command(chip, CMD_RDID, 0, 0, 24);
    if (raw_flash_id == 0xFFFFFF || raw_flash_id == 0) {
        return FLASH_ERR_NO_RESPONSE;
    }

    // Byte swap the flash id as it's usually written the other way around
    uint8_t mfg_id = raw_flash_id & 0xFF;
    uint16_t flash_id = (raw_flash_id >> 16) | (raw_flash_id & 0xFF00);

    *id = ((uint32_t)mfg_id << 16) | flash_id;
    return FLASH_OK;
}


esp_flash_err_t spi_flash_generic_detect_size(const esp_flash_chip_t *chip, uint32_t *size)
{
    uint32_t id = 0;
    *size = 0;
    esp_flash_err_t err = chip->drv->read_id(chip, &id);
    if (err != FLASH_OK) {
        return err;
    }

    /* Can't detect size unless the high byte of the product ID matches the same convention, which is usually 0x40 or
     * 0xC0 or similar. */
    if ((id & 0x0F00) != 0) {
        return FLASH_ERR_UNSUPPORTED_CHIP;
    }

    *size = 1 << (id & 0xFF);

    return FLASH_OK;
}


esp_flash_err_t spi_flash_generic_erase_chip(const esp_flash_chip_t *chip)
{
    esp_flash_err_t err;

    err = chip->drv->write_enable(chip);
    if (err == FLASH_OK) {
        err = chip->drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    }
    if (err == FLASH_OK) {
        chip->spi->ctrl.val = 0;
        chip->spi->cmd.flash_ce = 1;
        while(chip->spi->cmd.val != 0) { }
        err = chip->drv->wait_idle(chip, SPI_FLASH_GENERIC_CHIP_ERASE_TIMEOUT);
    }

    return err;
}

esp_flash_err_t spi_flash_generic_erase_sector(const esp_flash_chip_t *chip, uint32_t start_address)
{
    esp_flash_err_t err = chip->drv->write_enable(chip);
    if (err == FLASH_OK) {
        err = chip->drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    }
    if (err == FLASH_OK) {
        chip->spi->user1.usr_addr_bitlen = (24 - 1);
        chip->spi->addr = start_address & ADDRESS_MASK_24BIT;
        chip->spi->ctrl.val = 0;
        chip->spi->cmd.flash_se = 1;
        while(chip->spi->cmd.val != 0) { }
        err = chip->drv->wait_idle(chip, SPI_FLASH_GENERIC_SECTOR_ERASE_TIMEOUT);
    }
    return err;
}

esp_flash_err_t spi_flash_generic_erase_block(const esp_flash_chip_t *chip, uint32_t start_address)
{
    esp_flash_err_t err = chip->drv->write_enable(chip);
    if (err == FLASH_OK) {
        err = chip->drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);
    }
    if (err == FLASH_OK) {
        chip->spi->user1.usr_addr_bitlen = (24 - 1);
        chip->spi->addr = start_address & ADDRESS_MASK_24BIT;
        chip->spi->cmd.flash_be = 1;
        while(chip->spi->cmd.val != 0) { }
        err = chip->drv->wait_idle(chip, SPI_FLASH_GENERIC_BLOCK_ERASE_TIMEOUT);
    }
    return err;
}

esp_flash_err_t spi_flash_generic_read(const esp_flash_chip_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    esp_flash_err_t err = FLASH_OK;

    while (err == FLASH_OK && length > 0) {
        uint32_t read_len = MIN(length, MAX_READ_BYTES);
        chip->spi->miso_dlen.usr_miso_dbitlen = (read_len * 8) - 1;
        chip->spi->addr = address << 8;

        chip->spi->cmd.usr = 1;
        while(chip->spi->cmd.val != 0) {}

        if(((intptr_t)buffer % 4 == 0) && (read_len % 4 == 0)) {
            // If everything is word-aligned, do a faster memcpy
            xthal_memcpy(buffer, (void *)chip->spi->data_buf, read_len);
            length -= read_len;
            buffer = (void *)((intptr_t)buffer + read_len);
            address += read_len;
        } else {
            // Otherwise, slow(er) path copies word by word
            for (int i = 0; i < (read_len+3)/4; i++) {
                int word_len = MIN(sizeof(uint32_t), length);
                uint32_t word = chip->spi->data_buf[i];
                xthal_memcpy(buffer, &word, word_len);
                length -= word_len;
                buffer = (void *)((intptr_t)buffer + word_len);
                address += word_len;
            }
        }
    }


    return err;
}

esp_flash_err_t spi_flash_generic_page_program(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    esp_flash_err_t err;

    err = chip->drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);

    if (err == FLASH_OK) {
        err = chip->drv->write_enable(chip);
    }

    if (err == FLASH_OK) {
        // Perform the actual Page Program command
        chip->spi->user.usr_dummy = 0;
        chip->spi->user1.usr_addr_bitlen = (24 - 1);
        chip->spi->addr = (address & ADDRESS_MASK_24BIT) | (length << 24);

        // Load data registers, word at a time
        int num_words = (length+3) / 4;
        for (int i = 0; i < num_words; i++) {
            uint32_t word = 0;
            uint32_t word_len = MIN(length, sizeof(word));
            xthal_memcpy(&word, buffer, word_len);
            chip->spi->data_buf[i] = word;
            length -= word_len;
            buffer = (void *)((intptr_t)buffer + word_len);
        }

        chip->spi->cmd.flash_pp = 1;
        while (chip->spi->cmd.val != 0) { }

        err = chip->drv->wait_idle(chip, DEFAULT_PAGE_PROGRAM_TIMEOUT);
    }


    return err;
}

esp_flash_err_t spi_flash_generic_write(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    esp_flash_err_t err = FLASH_OK;
    const uint32_t page_size = chip->drv->page_size;

    while (err == FLASH_OK && length > 0) {
        uint32_t page_len = MIN(MAX_WRITE_BYTES, MIN(page_size, length));
        if ((address + page_len) / page_size != address / page_size) {
            // Most flash chips can't page write across a page boundary
            page_len = page_size - (address % page_size);
        }
        err = chip->drv->page_program(chip, address, buffer, page_len);
        address += page_len;
        buffer = (void *)((intptr_t)buffer + page_len);
        length -= page_len;
    }


    return err;
}

esp_flash_err_t spi_flash_generic_write_encrypted(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    return FLASH_ERR_UNSUPPORTED_HOST; // TODO
}

esp_flash_err_t spi_flash_generic_write_enable(const esp_flash_chip_t *chip)
{
    esp_flash_err_t err = FLASH_OK;

    err = chip->drv->wait_idle(chip, DEFAULT_IDLE_TIMEOUT);

    if (err == FLASH_OK) {
        chip->spi->cmd.flash_wren = 1;
        while(chip->spi->cmd.val != 0) { }
    }

    uint8_t status = spi_flash_common_command(chip, CMD_RDSR, 0, 0, 8);
    if ((status & SR_WREN) == 0) {
        // WREN flag has not been set!
        err = FLASH_ERR_NOT_FOUND;
    }


    return err;
}

esp_flash_err_t spi_flash_generic_wait_host_idle(const esp_flash_chip_t *chip, uint32_t *timeout_ms)
{
    while(chip->spi->ext2.st != 0 && *timeout_ms > 0) {
        if (*timeout_ms > 1) {
            esp_flash_os_functions->delay_ms(1);
        }
        (*timeout_ms)--;
    }

    // Not clear if this is necessary, or only necessary if
    // chip->spi == SPI1. But probably doesn't hurt...
    while(SPI0.ext2.st != 0 && *timeout_ms > 0) {
        if (*timeout_ms > 1) {
            esp_flash_os_functions->delay_ms(1);
        }
        (*timeout_ms)--;
    }

    return (*timeout_ms > 0) ? FLASH_OK : FLASH_ERR_NO_RESPONSE;
}

esp_flash_err_t spi_flash_generic_wait_idle(const esp_flash_chip_t *chip, uint32_t timeout_ms)
{
    timeout_ms++; // allow at least one pass before timeout, last one has no sleep cycle

    uint8_t status = 0;
    while(timeout_ms > 0) {

        esp_flash_err_t err = spi_flash_generic_wait_host_idle(chip, &timeout_ms);
        if (err != FLASH_OK) {
            return err;
        }

        status = spi_flash_common_command(chip, CMD_RDSR, 0, 0, 8);
        if ((status & SR_WIP) == 0) {
            break; // Write in progress is complete
        }
        if (timeout_ms > 1) {
            esp_flash_os_functions->delay_ms(1);
        }
        timeout_ms--;
    }

    return (timeout_ms > 0) ?  FLASH_OK : FLASH_ERR_NO_RESPONSE;
}

esp_flash_err_t spi_flash_common_configure_host_read_mode(const esp_flash_chip_t *chip)
{
    int dummy_cyclelen, addr_bitlen, read_command;
    switch(chip->read_mode) {
    case ESP_FLASH_QIO:
        addr_bitlen = 32;
        dummy_cyclelen = 4; // TODO check this works
        read_command = CMD_FASTRD_QIO;
        break;
    case ESP_FLASH_QOUT:
        addr_bitlen = 24;
        dummy_cyclelen = 8; // TODO check this works
        read_command = CMD_FASTRD_QUAD;
        break;
    case ESP_FLASH_DIO:
        addr_bitlen = 32;
        dummy_cyclelen = 0;
        read_command = CMD_FASTRD_DIO;
        break;
    case ESP_FLASH_DOUT:
        addr_bitlen = 24;
        dummy_cyclelen = 8;
        read_command = CMD_FASTRD_DUAL;
        break;
    case ESP_FLASH_FASTRD:
        addr_bitlen = 24;
        dummy_cyclelen = 8;
        read_command = CMD_FASTRD;
        break;
    case ESP_FLASH_SLOWRD:
        addr_bitlen = 24;
        dummy_cyclelen = 0;
        read_command = CMD_READ;
        break;
    default:
        return FLASH_ERR_NOT_INITIALISED;
    }

    // Add dummy cycles to compensate for GPIO matrix
    // latency, if necessary...
    if (spi_flash_uses_gpio_matrix(chip)) {
        if (chip->speed == ESP_FLASH_80MHZ) {
            dummy_cyclelen += 2;
        } else if (chip->speed == ESP_FLASH_40MHZ) {
            dummy_cyclelen += 1;
        }
    }

    chip->spi->user1.usr_dummy_cyclelen = (dummy_cyclelen - 1);
    chip->spi->user1.usr_addr_bitlen = (addr_bitlen - 1);
    chip->spi->user2.usr_command_value = read_command;
    chip->spi->user2.usr_command_bitlen = (8 - 1);

    typeof (chip->spi->user) user = {
        .usr_command = 1,
        .usr_mosi = 0,
        .usr_miso = 1,
        .usr_dummy = (dummy_cyclelen > 0) ? 1 : 0,
        .usr_addr = 1,
    };
    chip->spi->user = user;

    typeof (chip->spi->ctrl) ctrl = {
        .fread_qio   = (chip->read_mode == ESP_FLASH_QIO),
        .fread_quad  = (chip->read_mode == ESP_FLASH_QOUT),
        .fread_dio   = (chip->read_mode == ESP_FLASH_DIO),
        .fread_dual  = (chip->read_mode == ESP_FLASH_DOUT),
        .fastrd_mode = (chip->read_mode != ESP_FLASH_SLOWRD),
    };
    chip->spi->ctrl = ctrl;

    return FLASH_OK;
}

#include <rom/ets_sys.h>

esp_flash_err_t spi_flash_common_set_read_mode(const esp_flash_chip_t *chip, uint8_t qe_rdsr_command, uint8_t qe_wrsr_command, uint8_t qe_sr_bitwidth, unsigned qe_sr_bit)
{
    if (spi_flash_is_quad_mode(chip)) {
        // Ensure quad modes are enabled, using the Quad Enable parameters supplied.
        unsigned sr = spi_flash_common_command(chip, qe_rdsr_command, 0, 0, qe_sr_bitwidth);
        ets_printf("before 0x%x\n", sr);
        if ((sr & qe_sr_bit) == 0) {
            sr |= qe_sr_bit;
            spi_flash_common_command(chip, qe_wrsr_command, sr, qe_sr_bitwidth, 0);

            /* Check the new QE bit has stayed set */
            sr = spi_flash_common_command(chip, qe_rdsr_command, 0, 0, qe_sr_bitwidth);
            ets_printf("after 0x%x\n", sr);
            if ((sr & qe_sr_bit) == 0) {
                return FLASH_ERR_NO_RESPONSE;
            }
        }
    }

    // Configure the host, and return
    return spi_flash_common_configure_host_read_mode(chip);
}

esp_flash_err_t spi_flash_generic_set_read_mode(const esp_flash_chip_t *chip)
{
    // On "generic" chips, this involves checking
    // bit 1 (QE) of RDSR2 (35h) result
    // (it works this way on GigaDevice & Fudan Micro chips, probably others...)
    const uint8_t BIT_QE = 1<<1;
    return spi_flash_common_set_read_mode(chip, CMD_RDSR2, CMD_WRSR2, 8, BIT_QE);
}

bool spi_flash_uses_gpio_matrix(const esp_flash_chip_t *chip)
{
    if (chip->pins == NULL) {
        return false;
    }
    if (chip->pins->mosi_io_num != -1
        || chip->pins->miso_io_num != -1
        || chip->pins->sclk_io_num != -1) {
        return true;
    }
    if (spi_flash_is_quad_mode(chip)) {
        if (chip->pins->quadwp_io_num != -1
            || chip->pins->quadhd_io_num != -1) {
            return true;
        }
    }
    return false;
}

const esp_flash_driver_t esp_flash_generic_chip_driver = {
    .probe = spi_flash_generic_probe,
    .read_id = spi_flash_generic_read_id,
    .detect_size = spi_flash_generic_detect_size,
    .erase_chip = spi_flash_generic_erase_chip,
    .erase_sector = spi_flash_generic_erase_sector,
    .erase_block = spi_flash_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    // TODO: figure out if generic chip-wide protection bits exist across some manufacturers
    .get_chip_write_protect = NULL,
    .set_chip_write_protect = NULL,

    // Chip write protection regions do not appear to be standardised
    // at all, this is implemented in chip-specific drivers only.
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
    .set_read_mode = spi_flash_generic_set_read_mode,
};
