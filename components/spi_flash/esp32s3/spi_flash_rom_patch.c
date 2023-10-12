/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// We keep this file here only for future use

#include "sdkconfig.h"
#include "soc/spi_periph.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"

extern void esp_rom_spi_set_address_bit_len(int spi, int addr_bits);
void esp_rom_opiflash_cache_mode_config(esp_rom_spiflash_read_mode_t mode, const esp_rom_opiflash_spi0rd_t *cache)
{
    esp_rom_spi_set_op_mode(0, mode);
    REG_CLR_BIT(SPI_MEM_USER_REG(0), SPI_MEM_USR_MOSI);
    REG_SET_BIT(SPI_MEM_USER_REG(0), SPI_MEM_USR_MISO | SPI_MEM_USR_ADDR);

    if (cache) {
        esp_rom_spi_set_address_bit_len(0, cache->addr_bit_len);
        // Patch for ROM function `esp_rom_opiflash_cache_mode_config`, because when dummy is 0,
        // `SPI_MEM_USR_DUMMY` should be 0. `esp_rom_opiflash_cache_mode_config` doesn't handle this
        // properly.
        if (cache->dummy_bit_len == 0) {
            REG_CLR_BIT(SPI_MEM_USER_REG(0), SPI_MEM_USR_DUMMY);
        } else {
            REG_SET_BIT(SPI_MEM_USER_REG(0), SPI_MEM_USR_DUMMY);
            REG_SET_FIELD(SPI_MEM_USER1_REG(0), SPI_MEM_USR_DUMMY_CYCLELEN, cache->dummy_bit_len - 1 + rom_spiflash_legacy_data->dummy_len_plus[0]);
        }
        REG_SET_FIELD(SPI_MEM_USER2_REG(0), SPI_MEM_USR_COMMAND_VALUE, cache->cmd);
        REG_SET_FIELD(SPI_MEM_USER2_REG(0), SPI_MEM_USR_COMMAND_BITLEN, cache->cmd_bit_len - 1);
        REG_SET_FIELD(SPI_MEM_DDR_REG(0), SPI_MEM_SPI_FMEM_VAR_DUMMY, cache->var_dummy_en);
    }
}
