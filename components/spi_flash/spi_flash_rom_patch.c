// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "rom/ets_sys.h"
#include "rom/gpio.h"
#include "rom/spi_flash.h"
#include "sdkconfig.h"

#define SPI_IDX   1
#define OTH_IDX   0

extern esp_rom_spiflash_chip_t g_rom_spiflash_chip;

esp_rom_spiflash_result_t esp_rom_spiflash_wait_idle(esp_rom_spiflash_chip_t *spi)
{
    uint32_t status;

    //wait for spi control ready
    while ((REG_READ(SPI_EXT2_REG(1)) & SPI_ST)) {
    }
    while ((REG_READ(SPI_EXT2_REG(0)) & SPI_ST)) {
    }
    //wait for flash status ready
    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_read_status(spi, &status)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }
    return  ESP_ROM_SPIFLASH_RESULT_OK;
}


/* Modified version of esp_rom_spiflash_unlock() that replaces version in ROM.

   This works around a bug where esp_rom_spiflash_unlock sometimes reads the wrong
   high status byte (RDSR2 result) and then copies it back to the
   flash status, which can cause the CMP bit or Status Register
   Protect bit to become set.

   Like other ROM SPI functions, this function is not designed to be
   called directly from an RTOS environment without taking precautions
   about interrupts, CPU coordination, flash mapping. However some of
   the functions in esp_spi_flash.c call it.
 */
esp_rom_spiflash_result_t esp_rom_spiflash_unlock()
{
    uint32_t status;

    esp_rom_spiflash_wait_idle(&g_rom_spiflash_chip);

    if (esp_rom_spiflash_read_statushigh(&g_rom_spiflash_chip, &status) != ESP_ROM_SPIFLASH_RESULT_OK) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    /* Clear all bits except QIE, if it is set.
     (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
    */
    status &= ESP_ROM_SPIFLASH_QE;

    esp_rom_spiflash_wait_idle(&g_rom_spiflash_chip);
    REG_WRITE(SPI_CMD_REG(SPI_IDX), SPI_FLASH_WREN);
    while (REG_READ(SPI_CMD_REG(SPI_IDX)) != 0) {
    }
    esp_rom_spiflash_wait_idle(&g_rom_spiflash_chip);

    SET_PERI_REG_MASK(SPI_CTRL_REG(SPI_IDX), SPI_WRSR_2B);
    if (esp_rom_spiflash_write_status(&g_rom_spiflash_chip, status) != ESP_ROM_SPIFLASH_RESULT_OK) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}


#if CONFIG_SPI_FLASH_ROM_DRIVER_PATCH

extern uint8_t g_rom_spiflash_dummy_len_plus[];


static esp_rom_spiflash_result_t esp_rom_spiflash_enable_write(esp_rom_spiflash_chip_t *spi);

//only support spi1
static esp_rom_spiflash_result_t esp_rom_spiflash_erase_chip_internal(esp_rom_spiflash_chip_t *spi)
{
    esp_rom_spiflash_wait_idle(spi);

    // Chip erase.
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_CE);
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);

    // check erase is finished.
    esp_rom_spiflash_wait_idle(spi);

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

//only support spi1
static esp_rom_spiflash_result_t esp_rom_spiflash_erase_sector_internal(esp_rom_spiflash_chip_t *spi, uint32_t addr)
{
    //check if addr is 4k alignment
    if (0 != (addr & 0xfff)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    esp_rom_spiflash_wait_idle(spi);

    // sector erase  4Kbytes erase is sector erase.
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, addr & 0xffffff);
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_SE);
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);

    esp_rom_spiflash_wait_idle(spi);

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

//only support spi1
static esp_rom_spiflash_result_t esp_rom_spiflash_erase_block_internal(esp_rom_spiflash_chip_t *spi, uint32_t addr)
{
    esp_rom_spiflash_wait_idle(spi);

    // sector erase  4Kbytes erase is sector erase.
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, addr & 0xffffff);
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_BE);
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);

    esp_rom_spiflash_wait_idle(spi);

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

//only support spi1
static esp_rom_spiflash_result_t esp_rom_spiflash_program_page_internal(esp_rom_spiflash_chip_t *spi, uint32_t spi_addr,
        uint32_t *addr_source, int32_t byte_length)
{
    uint32_t  temp_addr;
    int32_t  temp_bl;
    uint8_t   i;
    uint8_t   remain_word_num;

    //check 4byte alignment
    if (0 != (byte_length & 0x3)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    //check if write in one page
    if ((spi->page_size) < ((spi_addr % (spi->page_size)) + byte_length)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    esp_rom_spiflash_wait_idle(spi);

    temp_addr = spi_addr;
    temp_bl = byte_length;

    while (temp_bl > 0 ) {
        if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_enable_write(spi)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
        if ( temp_bl >= ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM ) {
            WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, (temp_addr & 0xffffff) | ( ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM << ESP_ROM_SPIFLASH_BYTES_LEN )); // 32 byte a block

            for (i = 0; i < (ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM >> 2); i++) {
                WRITE_PERI_REG(PERIPHS_SPI_FLASH_C0 + i * 4, *addr_source++);
            }
            temp_bl = temp_bl - 32;
            temp_addr = temp_addr + 32;
        } else {
            WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, (temp_addr & 0xffffff) | (temp_bl << ESP_ROM_SPIFLASH_BYTES_LEN ));

            remain_word_num = (0 == (temp_bl & 0x3)) ? (temp_bl >> 2) : (temp_bl >> 2) + 1;
            for (i = 0; i < remain_word_num; i++) {
                WRITE_PERI_REG(PERIPHS_SPI_FLASH_C0 + i * 4, *addr_source++);
                temp_bl = temp_bl - 4;
            }
            temp_bl = 0;
        }

        WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_PP);
        while ( READ_PERI_REG(PERIPHS_SPI_FLASH_CMD ) != 0 );

        esp_rom_spiflash_wait_idle(spi);
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

//only support spi1
static esp_rom_spiflash_result_t esp_rom_spiflash_read_data(esp_rom_spiflash_chip_t *spi, uint32_t flash_addr,
        uint32_t *addr_dest, int32_t byte_length)
{
    uint32_t  temp_addr;
    int32_t  temp_length;
    uint8_t   i;
    uint8_t   remain_word_num;

    //address range check
    if ((flash_addr + byte_length) > (spi->chip_size)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    temp_addr = flash_addr;
    temp_length = byte_length;

    esp_rom_spiflash_wait_idle(spi);

    while (temp_length > 0) {
        if (temp_length >= ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM) {
            //WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, temp_addr |(ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM << ESP_ROM_SPIFLASH_BYTES_LEN));
            REG_WRITE(SPI_MISO_DLEN_REG(1),  ((ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM << 3) - 1) << SPI_USR_MISO_DBITLEN_S);
            WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, temp_addr << 8);
            REG_WRITE(PERIPHS_SPI_FLASH_CMD, SPI_USR);
            while (REG_READ(PERIPHS_SPI_FLASH_CMD) != 0);

            for (i = 0; i < (ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM >> 2); i++) {
                *addr_dest++ = READ_PERI_REG(PERIPHS_SPI_FLASH_C0 + i * 4);
            }
            temp_length = temp_length - ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM;
            temp_addr = temp_addr + ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM;
        } else {
            //WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, temp_addr |(temp_length << ESP_ROM_SPIFLASH_BYTES_LEN ));
            WRITE_PERI_REG(PERIPHS_SPI_FLASH_ADDR, temp_addr << 8);
            REG_WRITE(SPI_MISO_DLEN_REG(1),  ((ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM << 3) - 1) << SPI_USR_MISO_DBITLEN_S);
            REG_WRITE(PERIPHS_SPI_FLASH_CMD, SPI_USR);
            while (REG_READ(PERIPHS_SPI_FLASH_CMD) != 0);

            remain_word_num = (0 == (temp_length & 0x3)) ? (temp_length >> 2) : (temp_length >> 2) + 1;
            for (i = 0; i < remain_word_num; i++) {
                *addr_dest++ = READ_PERI_REG(PERIPHS_SPI_FLASH_C0 + i * 4);
            }
            temp_length = 0;
        }
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_read_status(esp_rom_spiflash_chip_t *spi, uint32_t *status)
{
    uint32_t status_value = ESP_ROM_SPIFLASH_BUSY_FLAG;

    if (g_rom_spiflash_dummy_len_plus[1] == 0) {
        while (ESP_ROM_SPIFLASH_BUSY_FLAG == (status_value & ESP_ROM_SPIFLASH_BUSY_FLAG)) {
            WRITE_PERI_REG(PERIPHS_SPI_FLASH_STATUS, 0);       // clear regisrter
            WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_RDSR);
            while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);

            status_value = READ_PERI_REG(PERIPHS_SPI_FLASH_STATUS) & (spi->status_mask);
        }
    } else {
        while (ESP_ROM_SPIFLASH_BUSY_FLAG == (status_value & ESP_ROM_SPIFLASH_BUSY_FLAG)) {
            esp_rom_spiflash_read_user_cmd(&status_value, 0x05);
        }
    }
    *status = status_value;

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_read_statushigh(esp_rom_spiflash_chip_t *spi, uint32_t *status)
{
    esp_rom_spiflash_result_t ret;
    esp_rom_spiflash_wait_idle(&g_rom_spiflash_chip);
    ret = esp_rom_spiflash_read_user_cmd(status, 0x35);
    *status = *status << 8;
    return ret;
}

esp_rom_spiflash_result_t esp_rom_spiflash_write_status(esp_rom_spiflash_chip_t *spi, uint32_t status_value)
{
    esp_rom_spiflash_wait_idle(spi);

    // update status value by status_value
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_STATUS, status_value);    // write status regisrter
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_WRSR);
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);
    esp_rom_spiflash_wait_idle(spi);

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

static esp_rom_spiflash_result_t esp_rom_spiflash_enable_write(esp_rom_spiflash_chip_t *spi)
{
    uint32_t flash_status = 0;

    esp_rom_spiflash_wait_idle(spi);

    //enable write
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_WREN);     // enable write operation
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);

    // make sure the flash is ready for writing
    while (ESP_ROM_SPIFLASH_WRENABLE_FLAG != (flash_status & ESP_ROM_SPIFLASH_WRENABLE_FLAG)) {
        esp_rom_spiflash_read_status(spi, &flash_status);
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

static void spi_cache_mode_switch(uint32_t  modebit)
{
    if ((modebit & SPI_FREAD_QIO) && (modebit & SPI_FASTRD_MODE)) {
        REG_CLR_BIT(SPI_USER_REG(0), SPI_USR_MOSI);
        REG_SET_BIT(SPI_USER_REG(0), SPI_USR_MISO | SPI_USR_DUMMY | SPI_USR_ADDR);
        REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_ADDR_BITLEN, SPI0_R_QIO_ADDR_BITSLEN);
        REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN, SPI0_R_QIO_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[0]);
        REG_SET_FIELD(SPI_USER2_REG(0), SPI_USR_COMMAND_VALUE, 0xEB);
    } else if (modebit & SPI_FASTRD_MODE) {
        REG_CLR_BIT(SPI_USER_REG(0), SPI_USR_MOSI);
        REG_SET_BIT(SPI_USER_REG(0), SPI_USR_MISO | SPI_USR_DUMMY | SPI_USR_ADDR);
        REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_ADDR_BITLEN, SPI0_R_FAST_ADDR_BITSLEN);
        if ((modebit & SPI_FREAD_QUAD)) {
            REG_SET_FIELD(SPI_USER2_REG(0), SPI_USR_COMMAND_VALUE, 0x6B);
            REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN, SPI0_R_FAST_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[0]);
        } else if ((modebit & SPI_FREAD_DIO)) {
            REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN, SPI0_R_DIO_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[0]);
            REG_SET_FIELD(SPI_USER2_REG(0), SPI_USR_COMMAND_VALUE, 0xBB);
        } else if ((modebit & SPI_FREAD_DUAL)) {
            REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN, SPI0_R_FAST_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[0]);
            REG_SET_FIELD(SPI_USER2_REG(0), SPI_USR_COMMAND_VALUE, 0x3B);
        } else {
            REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN, SPI0_R_FAST_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[0]);
            REG_SET_FIELD(SPI_USER2_REG(0), SPI_USR_COMMAND_VALUE, 0x0B);
        }
    } else {
        REG_CLR_BIT(SPI_USER_REG(0), SPI_USR_MOSI);
        if (g_rom_spiflash_dummy_len_plus[0] == 0) {
            REG_CLR_BIT(SPI_USER_REG(0), SPI_USR_DUMMY);
        } else {
            REG_SET_BIT(SPI_USER_REG(0), SPI_USR_DUMMY);
            REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN, g_rom_spiflash_dummy_len_plus[0] - 1);
        }
        REG_SET_BIT(SPI_USER_REG(0), SPI_USR_MISO | SPI_USR_ADDR);
        REG_SET_FIELD(SPI_USER1_REG(0), SPI_USR_ADDR_BITLEN, SPI0_R_SIO_ADDR_BITSLEN);
        REG_SET_FIELD(SPI_USER2_REG(0), SPI_USR_COMMAND_VALUE, 0x03);
    }

}

esp_rom_spiflash_result_t esp_rom_spiflash_lock()
{
    uint32_t status;

    //read QE bit, not write if not QE
    if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_read_statushigh(&g_rom_spiflash_chip, &status)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }
    //enable 2 byte status writing
    SET_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, ESP_ROM_SPIFLASH_TWO_BYTE_STATUS_EN);

    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_enable_write(&g_rom_spiflash_chip)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_write_status(&g_rom_spiflash_chip, status | ESP_ROM_SPIFLASH_WR_PROTECT)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}


esp_rom_spiflash_result_t esp_rom_spiflash_config_readmode(esp_rom_spiflash_read_mode_t mode)
{
    uint32_t  modebit;

    while ((REG_READ(SPI_EXT2_REG(1)) & SPI_ST)) {
    }
    while ((REG_READ(SPI_EXT2_REG(0)) & SPI_ST)) {
    }
    //clear old mode bit
    CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_FREAD_QIO | SPI_FREAD_QUAD | SPI_FREAD_DIO | SPI_FREAD_DUAL | SPI_FASTRD_MODE);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(0), SPI_FREAD_QIO | SPI_FREAD_QUAD | SPI_FREAD_DIO | SPI_FREAD_DUAL | SPI_FASTRD_MODE);
    //configure read mode
    switch (mode) {
    case ESP_ROM_SPIFLASH_QIO_MODE   :  modebit = SPI_FREAD_QIO  | SPI_FASTRD_MODE; break;
    case ESP_ROM_SPIFLASH_QOUT_MODE  :  modebit = SPI_FREAD_QUAD | SPI_FASTRD_MODE; break;
    case ESP_ROM_SPIFLASH_DIO_MODE   :  modebit = SPI_FREAD_DIO  | SPI_FASTRD_MODE; break;
    case ESP_ROM_SPIFLASH_DOUT_MODE  :  modebit = SPI_FREAD_DUAL | SPI_FASTRD_MODE; break;
    case ESP_ROM_SPIFLASH_FASTRD_MODE:  modebit = SPI_FASTRD_MODE; break;
    case ESP_ROM_SPIFLASH_SLOWRD_MODE:  modebit = 0; break;
    default : modebit = 0;
    }

    SET_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, modebit);
    SET_PERI_REG_MASK(SPI_CTRL_REG(0), modebit);
    spi_cache_mode_switch(modebit);

    return  ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_chip()
{
    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_enable_write(&g_rom_spiflash_chip)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_erase_chip_internal(&g_rom_spiflash_chip)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_block(uint32_t block_num)
{
    // flash write is always 1 line currently
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
    REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, ESP_ROM_SPIFLASH_W_SIO_ADDR_BITSLEN);

    //check program size
    if (block_num >= ((g_rom_spiflash_chip.chip_size) / (g_rom_spiflash_chip.block_size))) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_enable_write(&g_rom_spiflash_chip)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_erase_block_internal(&g_rom_spiflash_chip, block_num * (g_rom_spiflash_chip.block_size))) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_sector(uint32_t sector_num)
{
    // flash write is always 1 line currently
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
    REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, ESP_ROM_SPIFLASH_W_SIO_ADDR_BITSLEN);

    //check program size
    if (sector_num >= ((g_rom_spiflash_chip.chip_size) / (g_rom_spiflash_chip.sector_size))) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_enable_write(&g_rom_spiflash_chip)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_erase_sector_internal(&g_rom_spiflash_chip, sector_num * (g_rom_spiflash_chip.sector_size))) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_write(uint32_t target, const uint32_t *src_addr, int32_t len)
{
    uint32_t  page_size;
    uint32_t  pgm_len, pgm_num;
    uint8_t    i;

    // flash write is always 1 line currently
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
    REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, ESP_ROM_SPIFLASH_W_SIO_ADDR_BITSLEN);

    //check program size
    if ( (target + len) > (g_rom_spiflash_chip.chip_size)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    page_size = g_rom_spiflash_chip.page_size;
    pgm_len = page_size - (target % page_size);
    if (len < pgm_len) {
        if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_program_page_internal(&g_rom_spiflash_chip,
                target, (uint32_t *)src_addr, len)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
    } else {
        if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_program_page_internal(&g_rom_spiflash_chip,
                target, (uint32_t *)src_addr, pgm_len)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }

        //whole page program
        pgm_num = (len - pgm_len) / page_size;
        for (i = 0; i < pgm_num; i++) {
            if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_program_page_internal(&g_rom_spiflash_chip,
                    target + pgm_len, (uint32_t *)src_addr + (pgm_len >> 2), page_size)) {
                return ESP_ROM_SPIFLASH_RESULT_ERR;
            }
            pgm_len += page_size;
        }

        //remain parts to program
        if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_program_page_internal(&g_rom_spiflash_chip,
                target + pgm_len, (uint32_t *)src_addr + (pgm_len >> 2), len - pgm_len)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
    }
    return  ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_write_encrypted(uint32_t flash_addr, uint32_t *data, uint32_t len)
{
    esp_rom_spiflash_result_t ret = ESP_ROM_SPIFLASH_RESULT_OK;
    uint32_t i;

    if ((flash_addr & 0x1f) || (len & 0x1f)) {  //check 32 byte alignment
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    esp_rom_spiflash_write_encrypted_enable();

    for (i = 0; i < (len >> 5); i++) {
        if ((ret = esp_rom_spiflash_prepare_encrypted_data(flash_addr + (i << 5), data + (i << 3))) != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

        if ((ret = esp_rom_spiflash_write(flash_addr + (i << 5), data, 32)) != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }
    }

    esp_rom_spiflash_write_encrypted_disable();

    return ret;
}


esp_rom_spiflash_result_t esp_rom_spiflash_read(uint32_t target, uint32_t *dest_addr, int32_t len)
{
    // QIO or SIO, non-QIO regard as SIO
    uint32_t modebit;
    modebit = READ_PERI_REG(PERIPHS_SPI_FLASH_CTRL);
    if ((modebit & SPI_FREAD_QIO) && (modebit & SPI_FASTRD_MODE)) {
        REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_MOSI);
        REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_MISO | SPI_USR_DUMMY | SPI_USR_ADDR);
        REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, SPI1_R_QIO_ADDR_BITSLEN);
        REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_DUMMY_CYCLELEN, SPI1_R_QIO_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[1]);
        //REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG2, SPI_USR_COMMAND_VALUE, 0xEB);
        REG_WRITE(PERIPHS_SPI_FLASH_USRREG2, (0x7 << SPI_USR_COMMAND_BITLEN_S) | 0xEB);
    } else if (modebit & SPI_FASTRD_MODE) {
        REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_MOSI);
        REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_MISO | SPI_USR_ADDR);
        if (modebit & SPI_FREAD_DIO) {
            if (g_rom_spiflash_dummy_len_plus[1] == 0) {
                REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
                REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, SPI1_R_DIO_ADDR_BITSLEN);
                REG_WRITE(PERIPHS_SPI_FLASH_USRREG2, (0x7 << SPI_USR_COMMAND_BITLEN_S) | 0xBB);
            } else {
                REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
                REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, SPI1_R_DIO_ADDR_BITSLEN);
                REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_DUMMY_CYCLELEN, g_rom_spiflash_dummy_len_plus[1] - 1);
                REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG2, SPI_USR_COMMAND_VALUE, 0xBB);
            }
        } else {
            if ((modebit & SPI_FREAD_QUAD)) {
                //REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG2, SPI_USR_COMMAND_VALUE, 0x6B);
                REG_WRITE(PERIPHS_SPI_FLASH_USRREG2, (0x7 << SPI_USR_COMMAND_BITLEN_S) | 0x6B);
            } else if ((modebit & SPI_FREAD_DUAL)) {
                //REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG2, SPI_USR_COMMAND_VALUE, 0x3B);
                REG_WRITE(PERIPHS_SPI_FLASH_USRREG2, (0x7 << SPI_USR_COMMAND_BITLEN_S) | 0x3B);
            } else {
                //REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG2, SPI_USR_COMMAND_VALUE, 0x0B);
                REG_WRITE(PERIPHS_SPI_FLASH_USRREG2, (0x7 << SPI_USR_COMMAND_BITLEN_S) | 0x0B);
            }
            REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
            REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, SPI1_R_FAST_ADDR_BITSLEN);
            REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_DUMMY_CYCLELEN, SPI1_R_FAST_DUMMY_CYCLELEN + g_rom_spiflash_dummy_len_plus[1]);
        }
    } else {
        REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_MOSI);
        if (g_rom_spiflash_dummy_len_plus[1] == 0) {
            REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
        } else {
            REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_DUMMY);
            REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_DUMMY_CYCLELEN,  g_rom_spiflash_dummy_len_plus[1] - 1);
        }
        REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_USR_MISO | SPI_USR_ADDR);
        REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG1, SPI_USR_ADDR_BITLEN, SPI1_R_SIO_ADDR_BITSLEN);
        //REG_SET_FIELD(PERIPHS_SPI_FLASH_USRREG2, SPI_USR_COMMAND_VALUE, 0x03);
        REG_WRITE(PERIPHS_SPI_FLASH_USRREG2, (0x7 << SPI_USR_COMMAND_BITLEN_S) | 0x03);
    }

    if ( ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_read_data(&g_rom_spiflash_chip, target, dest_addr, len)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_area(uint32_t start_addr, uint32_t area_len)
{
    int32_t total_sector_num;
    int32_t head_sector_num;
    uint32_t sector_no;
    uint32_t sector_num_per_block;

    //set read mode to Fastmode ,not QDIO mode for erase
    //
    // TODO: this is probably a bug as it doesn't re-enable QIO mode, not serious as this
    // function is not used in IDF.
    esp_rom_spiflash_config_readmode(ESP_ROM_SPIFLASH_SLOWRD_MODE);

    //check if area is oversize of flash
    if ((start_addr + area_len) > g_rom_spiflash_chip.chip_size) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    //start_addr is aligned as sector boundary
    if (0 != (start_addr % g_rom_spiflash_chip.sector_size)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    //Unlock flash to enable erase
    if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_unlock(/*&g_rom_spiflash_chip*/)) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    sector_no = start_addr / g_rom_spiflash_chip.sector_size;
    sector_num_per_block = g_rom_spiflash_chip.block_size / g_rom_spiflash_chip.sector_size;
    total_sector_num = (0 == (area_len % g_rom_spiflash_chip.sector_size)) ? area_len / g_rom_spiflash_chip.sector_size :
                       1 + (area_len / g_rom_spiflash_chip.sector_size);

    //check if erase area reach over block boundary
    head_sector_num = sector_num_per_block - (sector_no % sector_num_per_block);

    head_sector_num = (head_sector_num >= total_sector_num) ? total_sector_num : head_sector_num;

    //JJJ, BUG of 6.0 erase
    //middle part of area is aligned by blocks
    total_sector_num -= head_sector_num;

    //head part of area is erased
    while (0 != head_sector_num) {
        if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_erase_sector(sector_no)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
        sector_no++;
        head_sector_num--;
    }
    while (total_sector_num > sector_num_per_block) {
        if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_erase_block(sector_no / sector_num_per_block)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
        sector_no += sector_num_per_block;
        total_sector_num -= sector_num_per_block;
    }

    //tail part of area burn
    while (0 < total_sector_num) {
        if (ESP_ROM_SPIFLASH_RESULT_OK != esp_rom_spiflash_erase_sector(sector_no)) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
        sector_no++;
        total_sector_num--;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

#endif
