/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
#if CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
#define OPI_CMD_FORMAT_MXIC() {   \
    .rdid = {              \
        .mode = ESP_ROM_SPIFLASH_OPI_STR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = 0x609f, \
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
        .cmd = 0xfa05, \
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
        .cmd = 0xf906, \
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
        .cmd = 0xde21, \
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
        .cmd = 0x23dc, \
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
        .cmd = 0x13ec, \
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
        .cmd = 0xed12, \
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
        .cmd = 0x13ec, \
        .cmd_bit_len = 16, \
        .var_dummy_en = 1, \
    } \
}

#elif CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
#define OPI_CMD_FORMAT_MXIC() {   \
    .rdid = {              \
        .mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE, \
        .cmd_bit_len = 16, \
        .cmd = 0x609f, \
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
        .cmd = 0xfa05, \
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
        .cmd = 0xf906, \
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
        .cmd = 0xde21, \
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
        .cmd = 0x23dc, \
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
        .cmd = 0x11ee, \
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
        .cmd = 0xed12, \
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
        .cmd = 0x11ee, \
        .cmd_bit_len = 16, \
        .var_dummy_en = 1, \
    } \
}
#endif  // DTR / STR
#endif  // #if CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
