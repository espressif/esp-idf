/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_flash_chips/spi_flash_defs.h"

//MXIC OPI mode needs two bytes of command - 2nd byte is the inversion of the command (1st) byte. S3 HW send LSB first
 #define MXIC_CMD16(cmd8)   ( (uint8_t)(cmd8) | ((uint8_t)(~(cmd8)) << 8) )

#define OPI_CMD_FORMAT_MXIC_STR() {   \
    .rdid = {              \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_RDID), \
        .addr = 0, \
        .addr_bit_len = 4*8, \
        .dummy_bit_len = 4, \
        .data_bit_len = 4 * 8, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .rdsr = { \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_RDSR), \
        .addr = 0, \
        .addr_bit_len = 4*8, \
        .dummy_bit_len = 4, \
        .data_bit_len = 1 * 8, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .wren = { \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_WREN), \
        .addr = 0, \
        .addr_bit_len = 0, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .se = { \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_SECTOR_ERASE_4B), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 1, \
    }, \
    .be64k = { \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_LARGE_BLOCK_ERASE_4B), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 1, \
    }, \
    .read = { \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_8READ), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 20, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .pp = { \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_PROGRAM_PAGE_4B), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 1, \
    }, \
    .cache_rd_cmd = { \
        .addr_bit_len = 32, \
        .dummy_bit_len = 20, \
        .cmd = MXIC_CMD16(CMD_8READ), \
        .cmd_bit_len = 16, \
        .var_dummy_en = 1, \
    } \
}

#define OPI_CMD_FORMAT_MXIC_DTR() {   \
    .rdid = {              \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_RDID), \
        .addr = 0, \
        .addr_bit_len = 4*8, \
        .dummy_bit_len = 4*2, \
        .data_bit_len = 4 * 8, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .rdsr = { \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_RDSR), \
        .addr = 0, \
        .addr_bit_len = 4*8, \
        .dummy_bit_len = 4*2, \
        .data_bit_len = 2 * 8, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .wren = { \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_WREN), \
        .addr = 0, \
        .addr_bit_len = 0, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .se = { \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_SECTOR_ERASE_4B), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 1, \
    }, \
    .be64k = { \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_LARGE_BLOCK_ERASE_4B), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 1, \
    }, \
    .read = { \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_8DTRD), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 20*2, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 0, \
    }, \
    .pp = { \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = MXIC_CMD16(CMD_PROGRAM_PAGE_4B), \
        .addr = 0, \
        .addr_bit_len = 32, \
        .dummy_bit_len = 0, \
        .data_bit_len = 0, \
        .cs_sel = 0x1, \
        .is_pe = 1, \
    }, \
    .cache_rd_cmd = { \
        .addr_bit_len = 32, \
        .dummy_bit_len = 20*2, \
        .cmd = MXIC_CMD16(CMD_8DTRD), \
        .cmd_bit_len = 16, \
        .var_dummy_en = 1, \
    } \
}
