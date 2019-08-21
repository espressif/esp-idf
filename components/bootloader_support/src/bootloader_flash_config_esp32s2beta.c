// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#include <stdbool.h>
#include <assert.h>
#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp32s2beta/rom/gpio.h"
#include "esp32s2beta/rom/spi_flash.h"
#include "esp32s2beta/rom/efuse.h"
#include "soc/gpio_periph.h"
#include "soc/efuse_reg.h"
#include "soc/spi_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/spi_caps.h"
#include "flash_qio_mode.h"
#include "bootloader_flash_config.h"
#include "bootloader_common.h"

#define FLASH_IO_MATRIX_DUMMY_40M   0
#define FLASH_IO_MATRIX_DUMMY_80M   0

#define FLASH_IO_DRIVE_GD_WITH_1V8PSRAM    3

void bootloader_flash_update_id()
{
    g_rom_flashchip.device_id = bootloader_read_flash_id();
}

void IRAM_ATTR bootloader_flash_cs_timing_config()
{
    SET_PERI_REG_MASK(SPI_USER_REG(0), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(0), SPI_CS_HOLD_TIME_V, 1, SPI_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(0), SPI_CS_SETUP_TIME_V, 0, SPI_CS_SETUP_TIME_S);
    SET_PERI_REG_MASK(SPI_USER_REG(1), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(1), SPI_CS_HOLD_TIME_V, 1, SPI_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(1), SPI_CS_SETUP_TIME_V, 0, SPI_CS_SETUP_TIME_S);
}

void IRAM_ATTR bootloader_flash_clock_config(const esp_image_header_t* pfhdr)
{
    uint32_t spi_clk_div = 0;
    switch (pfhdr->spi_speed) {
        case ESP_IMAGE_SPI_SPEED_80M:
            spi_clk_div = 1;
            break;
        case ESP_IMAGE_SPI_SPEED_40M:
            spi_clk_div = 2;
            break;
        case ESP_IMAGE_SPI_SPEED_26M:
            spi_clk_div = 3;
            break;
        case ESP_IMAGE_SPI_SPEED_20M:
            spi_clk_div = 4;
            break;
        default:
            break;
    }
    esp_rom_spiflash_config_clk(spi_clk_div, 0);
}

void IRAM_ATTR bootloader_flash_gpio_config(const esp_image_header_t* pfhdr)
{

}

void IRAM_ATTR bootloader_flash_dummy_config(const esp_image_header_t* pfhdr)
{
    int spi_cache_dummy = 0;
    int drv = 2;
    switch (pfhdr->spi_mode) {
    case ESP_IMAGE_SPI_MODE_QIO:
        spi_cache_dummy = SPI0_R_QIO_DUMMY_CYCLELEN;
        break;
    case ESP_IMAGE_SPI_MODE_DIO:
        spi_cache_dummy = SPI0_R_DIO_DUMMY_CYCLELEN;   //qio 3
        break;
    case ESP_IMAGE_SPI_MODE_QOUT:
    case ESP_IMAGE_SPI_MODE_DOUT:
    default:
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
        break;
    }

    /* dummy_len_plus values defined in ROM for SPI flash configuration */
    extern uint8_t g_rom_spiflash_dummy_len_plus[];
    switch (pfhdr->spi_speed) {
    case ESP_IMAGE_SPI_SPEED_80M:
        g_rom_spiflash_dummy_len_plus[0] = FLASH_IO_MATRIX_DUMMY_80M;
        g_rom_spiflash_dummy_len_plus[1] = FLASH_IO_MATRIX_DUMMY_80M;
        SET_PERI_REG_BITS(SPI_MEM_USER1_REG(0), SPI_MEM_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + FLASH_IO_MATRIX_DUMMY_80M,
                          SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY
        drv = 3;
        break;
    case ESP_IMAGE_SPI_SPEED_40M:
        g_rom_spiflash_dummy_len_plus[0] = FLASH_IO_MATRIX_DUMMY_40M;
        g_rom_spiflash_dummy_len_plus[1] = FLASH_IO_MATRIX_DUMMY_40M;
        SET_PERI_REG_BITS(SPI_MEM_USER1_REG(0), SPI_MEM_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + FLASH_IO_MATRIX_DUMMY_40M,
                          SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY
        break;
    default:
        break;
    }

    bootloader_configure_spi_pins(drv);
}
