// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <sys/param.h>

#include "esp_spi_flash.h"
#include "esp_spi_flash_chip.h"
#include "cache_utils.h"
#include "soc/system_reg.h"
#include "soc/soc_memory_layout.h"
#include "esp32s2beta/rom/spi_flash.h"
#include "esp32s2beta/rom/cache.h"

esp_rom_spiflash_result_t IRAM_ATTR spi_flash_write_encrypted_chip(size_t dest_addr, const void *src, size_t size)
{
    const spi_flash_guard_funcs_t *ops = spi_flash_guard_get();
    esp_rom_spiflash_result_t rc;

    if (!esp_ptr_internal(src)) {
        uint8_t block[128]; // Need to buffer in RAM as we write
        while (size > 0) {
            size_t next_block = MIN(size, sizeof(block));
            memcpy(block, src, next_block);

            esp_rom_spiflash_result_t r = spi_flash_write_encrypted_chip(dest_addr, block, next_block);
            if (r != ESP_ROM_SPIFLASH_RESULT_OK) {
                return r;
            }

            size -= next_block;
            dest_addr += next_block;
            src = ((uint8_t *)src) + next_block;
        }
        bzero(block, sizeof(block));

        return ESP_ROM_SPIFLASH_RESULT_OK;
    }
    else { // Already in internal memory
        rc = spi_flash_unlock();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            return rc;
        }

        if (ops && ops->start) {
            ops->start();
        }
        rc = SPI_Encrypt_Write(dest_addr, src, size);
        if (ops && ops->end) {
            ops->end();
        }
        return rc;
    }
}

#define SPICACHE SPIMEM0
#define SPIFLASH SPIMEM1
#define FLASH_WRAP_CMD   0x77
esp_err_t spi_flash_wrap_set(spi_flash_wrap_mode_t mode)
{
    uint32_t reg_bkp_ctrl = SPIFLASH.ctrl.val;
    uint32_t reg_bkp_usr  = SPIFLASH.user.val;
    SPIFLASH.user.fwrite_dio = 0;
    SPIFLASH.user.fwrite_dual = 0;
    SPIFLASH.user.fwrite_qio = 1;
    SPIFLASH.user.fwrite_quad = 0;
    SPIFLASH.ctrl.fcmd_dual = 0;
    SPIFLASH.ctrl.fcmd_quad = 0;
    SPIFLASH.user.usr_dummy = 0;
    SPIFLASH.user.usr_addr = 1;
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;
    SPIFLASH.user2.usr_command_value = FLASH_WRAP_CMD;
    SPIFLASH.user1.usr_addr_bitlen = 23;
    SPIFLASH.addr = 0;
    SPIFLASH.user.usr_miso = 0;
    SPIFLASH.user.usr_mosi = 1;
    SPIFLASH.mosi_dlen.usr_mosi_bit_len = 7;
    SPIFLASH.data_buf[0] = (uint32_t) mode << 4;;
    SPIFLASH.cmd.usr = 1;
    while(SPIFLASH.cmd.usr != 0)
    { }

    SPIFLASH.ctrl.val = reg_bkp_ctrl;
    SPIFLASH.user.val = reg_bkp_usr;
    return ESP_OK;
}

esp_err_t spi_flash_enable_wrap(uint32_t wrap_size)
{
    switch(wrap_size) {
        case 8:
            return spi_flash_wrap_set(FLASH_WRAP_MODE_8B);
        case 16:
            return spi_flash_wrap_set(FLASH_WRAP_MODE_16B);
        case 32:
            return spi_flash_wrap_set(FLASH_WRAP_MODE_32B);
        case 64:
            return spi_flash_wrap_set(FLASH_WRAP_MODE_64B);
        default:
            return ESP_FAIL;
    }
}

void spi_flash_disable_wrap()
{
    spi_flash_wrap_set(FLASH_WRAP_MODE_DISABLE);
}

bool spi_flash_support_wrap_size(uint32_t wrap_size)
{
    if (!REG_GET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FREAD_QIO) || !REG_GET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FASTRD_MODE)){
        return ESP_FAIL;
    }
    switch(wrap_size) {
        case 0:
        case 8:
        case 16:
        case 32:
        case 64:
            return true;
        default:
            return false;
    }
}
