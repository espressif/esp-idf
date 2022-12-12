/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>

#include "spi_flash_mmap.h"
#include "soc/system_reg.h"
#include "soc/soc_memory_layout.h"
#include "esp32h4/rom/cache.h"
#include "hal/spi_flash_hal.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_rom_spiflash.h"
#include "esp_private/spi_flash_os.h"

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
    while (SPIFLASH.cmd.usr != 0)
    { }

    SPIFLASH.ctrl.val = reg_bkp_ctrl;
    SPIFLASH.user.val = reg_bkp_usr;
    return ESP_OK;
}

esp_err_t spi_flash_enable_wrap(uint32_t wrap_size)
{
    switch (wrap_size) {
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

void spi_flash_disable_wrap(void)
{
    spi_flash_wrap_set(FLASH_WRAP_MODE_DISABLE);
}

bool spi_flash_support_wrap_size(uint32_t wrap_size)
{
    if (!REG_GET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FREAD_QIO) || !REG_GET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FASTRD_MODE)) {
        return ESP_FAIL;
    }
    switch (wrap_size) {
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
