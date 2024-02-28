/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/param.h> // For MIN/MAX
#include "spi_flash_chip_generic.h"
#include "spi_flash_defs.h"
#include "hal/spi_flash_encrypt_hal.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_private/spi_flash_os.h"
#include "esp_rom_caps.h"

typedef struct flash_chip_dummy {
    uint8_t dio_dummy_bitlen;
    uint8_t qio_dummy_bitlen;
    uint8_t qout_dummy_bitlen;
    uint8_t dout_dummy_bitlen;
    uint8_t fastrd_dummy_bitlen;
    uint8_t slowrd_dummy_bitlen;
} flash_chip_dummy_t;

// These parameters can be placed in the ROM. For now we use the code in IDF.
DRAM_ATTR const static flash_chip_dummy_t default_flash_chip_dummy = {
    .dio_dummy_bitlen = SPI_FLASH_DIO_DUMMY_BITLEN,
    .qio_dummy_bitlen = SPI_FLASH_QIO_DUMMY_BITLEN,
    .qout_dummy_bitlen = SPI_FLASH_QOUT_DUMMY_BITLEN,
    .dout_dummy_bitlen = SPI_FLASH_DOUT_DUMMY_BITLEN,
    .fastrd_dummy_bitlen = SPI_FLASH_FASTRD_DUMMY_BITLEN,
    .slowrd_dummy_bitlen = SPI_FLASH_SLOWRD_DUMMY_BITLEN,
};

DRAM_ATTR const static flash_chip_dummy_t hpm_flash_chip_dummy = {
    .dio_dummy_bitlen = SPI_FLASH_DIO_HPM_DUMMY_BITLEN,
    .qio_dummy_bitlen = SPI_FLASH_QIO_HPM_DUMMY_BITLEN,
    .qout_dummy_bitlen = SPI_FLASH_QOUT_DUMMY_BITLEN,
    .dout_dummy_bitlen = SPI_FLASH_DOUT_DUMMY_BITLEN,
    .fastrd_dummy_bitlen = SPI_FLASH_FASTRD_DUMMY_BITLEN,
    .slowrd_dummy_bitlen = SPI_FLASH_SLOWRD_DUMMY_BITLEN,
};


DRAM_ATTR flash_chip_dummy_t *rom_flash_chip_dummy = (flash_chip_dummy_t *)&default_flash_chip_dummy;

DRAM_ATTR flash_chip_dummy_t *rom_flash_chip_dummy_hpm = (flash_chip_dummy_t *)&hpm_flash_chip_dummy;

// These are the pointer to HW flash encryption. Default using hardware encryption.
DRAM_ATTR static spi_flash_encryption_t esp_flash_encryption_default __attribute__((__unused__)) = {
    .flash_encryption_enable = spi_flash_encryption_hal_enable,
    .flash_encryption_disable = spi_flash_encryption_hal_disable,
    .flash_encryption_data_prepare = spi_flash_encryption_hal_prepare,
    .flash_encryption_done = spi_flash_encryption_hal_done,
    .flash_encryption_destroy = spi_flash_encryption_hal_destroy,
    .flash_encryption_check = spi_flash_encryption_hal_check,
};

#define SPI_FLASH_DEFAULT_IDLE_TIMEOUT_MS           200
#define SPI_FLASH_GENERIC_CHIP_ERASE_TIMEOUT_MS     4000
#define SPI_FLASH_GENERIC_SECTOR_ERASE_TIMEOUT_MS   600  //according to GD25Q127(125°) + 100ms
#define SPI_FLASH_GENERIC_BLOCK_ERASE_TIMEOUT_MS    4100  //according to GD25Q127(125°) + 100ms
#define SPI_FLASH_GENERIC_PAGE_PROGRAM_TIMEOUT_MS   500

#define HOST_DELAY_INTERVAL_US                      1
#define CHIP_WAIT_IDLE_INTERVAL_US                  20

#define SPI_FLASH_LINEAR_DENSITY_LAST_VALUE        (0x19)
#define SPI_FLASH_HEX_A_F_RANGE                    (6)

const DRAM_ATTR flash_chip_op_timeout_t spi_flash_chip_generic_timeout = {
    .idle_timeout = SPI_FLASH_DEFAULT_IDLE_TIMEOUT_MS * 1000,
    .chip_erase_timeout = SPI_FLASH_GENERIC_CHIP_ERASE_TIMEOUT_MS * 1000,
    .block_erase_timeout = SPI_FLASH_GENERIC_BLOCK_ERASE_TIMEOUT_MS * 1000,
    .sector_erase_timeout = SPI_FLASH_GENERIC_SECTOR_ERASE_TIMEOUT_MS * 1000,
    .page_program_timeout = SPI_FLASH_GENERIC_PAGE_PROGRAM_TIMEOUT_MS * 1000,
};

#define SET_FLASH_ERASE_STATUS(CHIP, status) do { \
    if (CHIP->os_func->set_flash_op_status) { \
        CHIP->os_func->set_flash_op_status(status); \
    } \
} while(0)

static const char TAG[] = "chip_generic";

esp_err_t spi_flash_chip_generic_detect_size(esp_flash_t *chip, uint32_t *size)
{
    uint32_t id = chip->chip_id;
    *size = 0;

    /* Can't detect size unless the high byte of the product ID matches the same convention, which is usually 0x40 or
     * 0xC0 or similar. */
    if (((id & 0xFFFF) == 0x0000) || ((id & 0xFFFF) == 0xFFFF)) {
        return ESP_ERR_FLASH_UNSUPPORTED_CHIP;
    }

    /* Get flash capacity from flash chip id depends on different vendors. According to majority of flash datasheets,
       Flash 256Mb to 512Mb directly from 0x19 to 0x20, instead of from 0x19 to 0x1a. So here we leave the common behavior.
       However, some other flash vendors also have their own rule, we will add them in chip specific files.
     */
    uint32_t mem_density = (id & 0xFF);
    if (mem_density > SPI_FLASH_LINEAR_DENSITY_LAST_VALUE ) {
        mem_density -= SPI_FLASH_HEX_A_F_RANGE;
    }

    *size = 1 << mem_density;
    return ESP_OK;
}

#ifndef CONFIG_SPI_FLASH_ROM_IMPL

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
    esp_err_t err = chip->host->driver->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    t = (spi_flash_trans_t) {
        .command = CMD_RST_DEV,
    };
    err = chip->host->driver->common_command(chip->host, &t);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    return err;
}

esp_err_t spi_flash_chip_generic_erase_chip(esp_flash_t *chip)
{
    esp_err_t err;

    err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }
    //The chip didn't accept the previous write command. Ignore this in preparation stage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        SET_FLASH_ERASE_STATUS(chip, SPI_FLASH_OS_IS_ERASING_STATUS_FLAG);
        chip->host->driver->erase_chip(chip->host);
        chip->busy = 1;
#ifdef CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED
        err = chip->chip_drv->wait_idle(chip, ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
#else
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->chip_erase_timeout);
#endif
        SET_FLASH_ERASE_STATUS(chip, 0);
    }
    // Ensure WEL is 0, even if the erase failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }

    return err;
}

esp_err_t spi_flash_chip_generic_erase_sector(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        SET_FLASH_ERASE_STATUS(chip, SPI_FLASH_OS_IS_ERASING_STATUS_FLAG);
        chip->host->driver->erase_sector(chip->host, start_address);
        chip->busy = 1;
#ifdef CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED
        err = chip->chip_drv->wait_idle(chip, ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
#else
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->sector_erase_timeout);
#endif
        SET_FLASH_ERASE_STATUS(chip, 0);
    }
    // Ensure WEL is 0, even if the erase failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }

    return err;
}

esp_err_t spi_flash_chip_generic_erase_block(esp_flash_t *chip, uint32_t start_address)
{
    esp_err_t err = chip->chip_drv->set_chip_write_protect(chip, false);
    if (err == ESP_OK) {
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    }
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        SET_FLASH_ERASE_STATUS(chip, SPI_FLASH_OS_IS_ERASING_STATUS_FLAG);
        chip->host->driver->erase_block(chip->host, start_address);
        chip->busy = 1;
#ifdef CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED
        err = chip->chip_drv->wait_idle(chip, ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
#else
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->block_erase_timeout);
#endif
        SET_FLASH_ERASE_STATUS(chip, 0);
    }
    // Ensure WEL is 0, even if the erase failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }

    return err;
}

esp_err_t spi_flash_chip_generic_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = ESP_OK;
    const uint32_t page_size = chip->chip_drv->page_size;
    uint32_t align_address;
    uint8_t temp_buffer[64]; //spiflash hal max length of read no longer than 64byte
    uint32_t config_io_flags = 0;

    // Configure the host, and return
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

esp_err_t spi_flash_chip_generic_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err;

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        // Perform the actual Page Program command
        chip->host->driver->program_page(chip->host, buffer, address, length);
        chip->busy = 1;

        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->page_program_timeout);
    }
    // Ensure WEL is 0, even if the page program failed.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        err = chip->chip_drv->set_chip_write_protect(chip, true);
    }
    return err;
}

esp_err_t spi_flash_chip_generic_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
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

esp_err_t spi_flash_chip_generic_set_write_protect(esp_flash_t *chip, bool write_protect)
{
    esp_err_t err = ESP_OK;

    err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
    //The chip didn't accept the previous write command. Ignore this in preparationstage.
    if (err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED) {
        chip->host->driver->set_write_protect(chip->host, write_protect);
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
    uint32_t status;
    assert(out_write_protect!=NULL);
    err = chip->chip_drv->read_reg(chip, SPI_FLASH_REG_STATUS, &status);
    if (err != ESP_OK) {
        return err;
    }

    *out_write_protect = ((status & SR_WREN) == 0);
    return err;
}

esp_err_t spi_flash_chip_generic_read_reg(esp_flash_t* chip, spi_flash_register_t reg_id, uint32_t* out_reg)
{
    return chip->host->driver->read_status(chip->host, (uint8_t*)out_reg);
}

esp_err_t spi_flash_chip_generic_yield(esp_flash_t* chip, uint32_t wip)
{
    esp_err_t err = ESP_OK;
    uint32_t flags = wip? 1: 0; //check_yield() and yield() impls should not issue suspend/resume if this flag is zero

    if (chip->os_func->check_yield) {
        uint32_t request;
        //According to the implementation, the check_yield() function may block, poll, delay or do nothing but return
        err = chip->os_func->check_yield(chip->os_func_data, flags, &request);
        if (err == ESP_OK) {
            if (err == ESP_OK && (request & SPI_FLASH_YIELD_REQ_YIELD) != 0) {
                uint32_t status;
                //According to the implementation, the yield() function may block until something happen
                err = chip->os_func->yield(chip->os_func_data, &status);
            }
        } else if (err == ESP_ERR_TIMEOUT) {
            err = ESP_OK;
        } else {
            abort();
        }
    }
    return err;
}

esp_err_t spi_flash_chip_generic_wait_idle(esp_flash_t *chip, uint32_t timeout_us)
{
    bool timeout_en = (timeout_us != ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT);
    if (timeout_us == ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT) {
        timeout_us = 0;// In order to go into while
    }
    timeout_us++; // allow at least one pass before timeout, last one has no sleep cycle

    uint8_t status = 0;
    const int interval = CHIP_WAIT_IDLE_INTERVAL_US;
    while (timeout_us > 0) {
        while (!chip->host->driver->host_status(chip->host) && timeout_us > 0) {

#if HOST_DELAY_INTERVAL_US > 0
            if (timeout_us > 1) {
                int delay = MIN(HOST_DELAY_INTERVAL_US, timeout_us);
                chip->os_func->delay_us(chip->os_func_data, delay);
                timeout_us -= delay;
            }
#endif
        }

        uint32_t read;
        esp_err_t err = chip->chip_drv->read_reg(chip, SPI_FLASH_REG_STATUS, &read);
        if (err != ESP_OK) {
            return err;
        }
        status = read;

        if ((status & SR_WIP) == 0) { // Verify write in progress is complete
            if (chip->busy == 1) {
                chip->busy = 0;
                if ((status & SR_WREN) != 0) { // The previous command is not accepted, leaving the WEL still set.
                    return ESP_ERR_NOT_SUPPORTED;
                }
            }
            break;
        }
        if (timeout_us > 0 && interval > 0) {
            int delay = MIN(interval, timeout_us);
            chip->os_func->delay_us(chip->os_func_data, delay);
            if (timeout_en) {
                timeout_us -= delay;
            }
        }
    }
    return (timeout_us > 0) ?  ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip, uint32_t flags)
{
    uint32_t dummy_cyclelen_base;
    uint32_t addr_bitlen;
    uint32_t read_command;
    bool conf_required = false;
    esp_flash_io_mode_t read_mode = chip->read_mode;
    bool addr_32bit = (flags & SPI_FLASH_CONFIG_IO_MODE_32B_ADDR);

    switch (read_mode & 0xFFFF) {
    case SPI_FLASH_QIO:
        //for QIO mode, the 4 bit right after the address are used for continuous mode, should be set to 0 to avoid that.
        addr_bitlen = SPI_FLASH_QIO_ADDR_BITLEN;
        dummy_cyclelen_base = (chip->hpm_dummy_ena ? rom_flash_chip_dummy_hpm->qio_dummy_bitlen : rom_flash_chip_dummy->qio_dummy_bitlen);
        read_command = (addr_32bit? CMD_FASTRD_QIO_4B: CMD_FASTRD_QIO);
        conf_required = true;
        break;
    case SPI_FLASH_QOUT:
        addr_bitlen = SPI_FLASH_QOUT_ADDR_BITLEN;
        dummy_cyclelen_base = (chip->hpm_dummy_ena ? rom_flash_chip_dummy_hpm->qout_dummy_bitlen : rom_flash_chip_dummy->qout_dummy_bitlen);
        read_command = (addr_32bit? CMD_FASTRD_QUAD_4B: CMD_FASTRD_QUAD);
        break;
    case SPI_FLASH_DIO:
        //for DIO mode, the 4 bit right after the address are used for continuous mode, should be set to 0 to avoid that.
        addr_bitlen = SPI_FLASH_DIO_ADDR_BITLEN;
        dummy_cyclelen_base = (chip->hpm_dummy_ena ? rom_flash_chip_dummy_hpm->dio_dummy_bitlen : rom_flash_chip_dummy->dio_dummy_bitlen);
        read_command = (addr_32bit? CMD_FASTRD_DIO_4B: CMD_FASTRD_DIO);
        conf_required = true;
        break;
    case SPI_FLASH_DOUT:
        addr_bitlen = SPI_FLASH_DOUT_ADDR_BITLEN;
        dummy_cyclelen_base = (chip->hpm_dummy_ena ? rom_flash_chip_dummy_hpm->dout_dummy_bitlen : rom_flash_chip_dummy->dout_dummy_bitlen);
        read_command = (addr_32bit? CMD_FASTRD_DUAL_4B: CMD_FASTRD_DUAL);
        break;
    case SPI_FLASH_FASTRD:
        addr_bitlen = SPI_FLASH_FASTRD_ADDR_BITLEN;
        dummy_cyclelen_base = (chip->hpm_dummy_ena ? rom_flash_chip_dummy_hpm->fastrd_dummy_bitlen : rom_flash_chip_dummy->fastrd_dummy_bitlen);
        read_command = (addr_32bit? CMD_FASTRD_4B: CMD_FASTRD);
        break;
    case SPI_FLASH_SLOWRD:
        addr_bitlen = SPI_FLASH_SLOWRD_ADDR_BITLEN;
        dummy_cyclelen_base = (chip->hpm_dummy_ena ? rom_flash_chip_dummy_hpm->slowrd_dummy_bitlen : rom_flash_chip_dummy->slowrd_dummy_bitlen);
        read_command = (addr_32bit? CMD_READ_4B: CMD_READ);
        break;
    default:
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }
    //For W25Q256 chip, the only difference between 4-Byte address command and 3-Byte version is the command value and the address bit length.
    if (addr_32bit) {
        addr_bitlen += 8;
    }

    if (conf_required) {
        read_mode |= SPI_FLASH_CONFIG_CONF_BITS;
    }

    return chip->host->driver->configure_host_io_mode(chip->host, read_command, addr_bitlen, dummy_cyclelen_base, read_mode);
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
#endif // CONFIG_SPI_FLASH_ROM_IMPL

#if !CONFIG_SPI_FLASH_ROM_IMPL || ESP_ROM_HAS_ENCRYPTED_WRITES_USING_LEGACY_DRV
esp_err_t spi_flash_chip_generic_write_encrypted(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    spi_flash_encryption_t *esp_flash_encryption = &esp_flash_encryption_default;
    esp_err_t err = ESP_OK;
    // Encryption must happen on main flash.
    if (chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Check if the buffer and length can qualify the requirments */
    if (esp_flash_encryption->flash_encryption_check(address, length) != true) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    const uint8_t *data_bytes = (const uint8_t *)buffer;
    esp_flash_encryption->flash_encryption_enable();
    while (length > 0) {
        int block_size;
        /* Write the largest block if possible */
        if (address % 64 == 0 && length >= 64) {
            block_size = 64;
        } else if (address % 32 == 0 && length >= 32) {
            block_size = 32;
        } else {
            block_size = 16;
        }
        // Prepare the flash chip (same time as AES operation, for performance)
        esp_flash_encryption->flash_encryption_data_prepare(address, (uint32_t *)data_bytes, block_size);
        err = chip->chip_drv->set_chip_write_protect(chip, false);
        if (err != ESP_OK) {
            return err;
        }
        // Waiting for encrypting buffer to finish and making result visible for SPI1
        esp_flash_encryption->flash_encryption_done();

        // Note: For encryption function, after write flash command is sent. The hardware will write the encrypted buffer
        // prepared in XTS_FLASH_ENCRYPTION register in function `flash_encryption_data_prepare`, instead of the origin
        // buffer named `data_bytes`.

        err = chip->chip_drv->write(chip, (uint32_t *)data_bytes, address, length);
        if (err != ESP_OK) {
            return err;
        }
        err = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->page_program_timeout);
        if (err != ESP_OK) {
            return err;
        }

        // Note: we don't wait for idle status here, because this way
        // the AES peripheral can start encrypting the next
        // block while the SPI flash chip is busy completing the write

        esp_flash_encryption->flash_encryption_destroy();

        length -= block_size;
        data_bytes += block_size;
        address += block_size;
    }

    esp_flash_encryption->flash_encryption_disable();
    return err;
}
#endif // !CONFIG_SPI_FLASH_ROM_IMPL || ESP_ROM_HAS_ENCRYPTED_WRITES_USING_LEGACY_DRV

esp_err_t spi_flash_chip_generic_read_unique_id(esp_flash_t *chip, uint64_t* flash_unique_id)
{
    uint64_t unique_id_buf = 0;
    spi_flash_trans_t transfer = {
        .command = CMD_RDUID,
        .miso_len = 8,
        .miso_data = ((uint8_t *)&unique_id_buf),
        .dummy_bitlen = 32, //RDUID command followed by 4 bytes (32 bits) of dummy clocks.
    };
    esp_err_t err = chip->host->driver->common_command(chip->host, &transfer);

    if (unique_id_buf == 0 || unique_id_buf == UINT64_MAX) {
        ESP_EARLY_LOGE(TAG, "No response from device when trying to retrieve Unique ID\n");
        *flash_unique_id = unique_id_buf;
        return ESP_ERR_NOT_SUPPORTED;
    }

    *flash_unique_id = __builtin_bswap64(unique_id_buf);
    return err;
}

esp_err_t spi_flash_chip_generic_read_unique_id_none(esp_flash_t *chip, uint64_t* flash_unique_id)
{
    // For flash doesn't support read unique id.
    return ESP_ERR_NOT_SUPPORTED;
}

spi_flash_caps_t spi_flash_chip_generic_get_caps(esp_flash_t *chip)
{
    // For generic part flash capability, take the XMC chip as reference.
    spi_flash_caps_t caps_flags = 0;
    // 32M-bits address support

    // flash suspend support
    // XMC support suspend
    if (chip->chip_id >> 16 == 0x20) {
        caps_flags |= SPI_FLASH_CHIP_CAP_SUSPEND;
    }

    // FM support suspend
    if (chip->chip_id >> 16 == 0xa1) {
        caps_flags |= SPI_FLASH_CHIP_CAP_SUSPEND;
    }
    // flash read unique id.
    caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
    return caps_flags;
}

static const char chip_name[] = "generic";

const spi_flash_chip_t esp_flash_chip_generic = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
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

    .read_reg = spi_flash_chip_generic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_generic_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_generic_read_unique_id,
    .get_chip_caps = spi_flash_chip_generic_get_caps,
    .config_host_io_mode = spi_flash_chip_generic_config_host_io_mode,
};

#ifndef CONFIG_SPI_FLASH_ROM_IMPL
/*******************************************************************************
 * Utility functions
 ******************************************************************************/

static esp_err_t spi_flash_common_read_qe_sr(esp_flash_t *chip, uint8_t qe_rdsr_command, uint8_t qe_sr_bitwidth, uint32_t *sr)
{
    uint32_t sr_buf = 0;
    spi_flash_trans_t t = {
        .command = qe_rdsr_command,
        .miso_data = (uint8_t*) &sr_buf,
        .miso_len = qe_sr_bitwidth / 8,
    };
    esp_err_t ret = chip->host->driver->common_command(chip->host, &t);
    *sr = sr_buf;
    return ret;
}

static esp_err_t spi_flash_common_write_qe_sr(esp_flash_t *chip, uint8_t qe_wrsr_command, uint8_t qe_sr_bitwidth, uint32_t qe)
{
    spi_flash_trans_t t = {
        .command = qe_wrsr_command,
        .mosi_data = ((uint8_t*) &qe),
        .mosi_len = qe_sr_bitwidth / 8,
        .miso_len = 0,
    };
    return chip->host->driver->common_command(chip->host, &t);
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
    /*
     * By default, we don't clear the QE bit even the flash mode is not QIO or QOUT. Force clearing
     * QE bit by the generic chip driver (command 01H with 2 bytes) may cause the output of some
     * chips (MXIC) no longer valid.
     * Enable this option when testing a new flash chip for clearing of QE.
     */
    const bool force_check = false;

    bool need_check = is_quad_mode || force_check;

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
            chip->chip_drv->set_chip_write_protect(chip, true);
            return ret;
        }

        ret = chip->chip_drv->wait_idle(chip, chip->chip_drv->timeout->idle_timeout);
        if (ret == ESP_ERR_NOT_SUPPORTED) {
            chip->chip_drv->set_chip_write_protect(chip, true);
        }
        /* This function is the fallback approach, so we give it higher tolerance.
         *   When the previous WRSR is rejected by the flash,
         *  the result of this function is determined by the result -whether the value of RDSR meets the expectation.
         */
        if (ret != ESP_OK && ret != ESP_ERR_NOT_SUPPORTED) {
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
    }
    return ret;
}

#endif // !CONFIG_SPI_FLASH_ROM_IMPL

esp_err_t spi_flash_chip_generic_suspend_cmd_conf(esp_flash_t *chip)
{
    // chips which support auto-suspend
    if (chip->chip_id >> 16 != 0x20 && chip->chip_id >> 16 != 0xa1) {
        ESP_EARLY_LOGE(TAG, "The flash you use doesn't support auto suspend, only \'XMC\' is supported");
        return ESP_ERR_NOT_SUPPORTED;
    }
    spi_flash_sus_cmd_conf sus_conf = {
        .sus_mask = 0x80,
        .cmd_rdsr = CMD_RDSR2,
        .sus_cmd = CMD_SUSPEND,
        .res_cmd = CMD_RESUME,
    };

    return chip->host->driver->sus_setup(chip->host, &sus_conf);
}
