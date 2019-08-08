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
#include <stdbool.h>
#include <assert.h>
#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp32/rom/gpio.h"
#include "esp32/rom/spi_flash.h"
#include "esp32/rom/efuse.h"
#include "soc/gpio_periph.h"
#include "soc/efuse_reg.h"
#include "soc/spi_reg.h"
#include "soc/spi_caps.h"
#include "flash_qio_mode.h"
#include "bootloader_flash_config.h"

void bootloader_flash_update_id(void)
{
    g_rom_flashchip.device_id = bootloader_read_flash_id();
}

void IRAM_ATTR bootloader_flash_cs_timing_config(void)
{
    SET_PERI_REG_MASK(SPI_USER_REG(0), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(0), SPI_HOLD_TIME_V, 1, SPI_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(0), SPI_SETUP_TIME_V, 0, SPI_SETUP_TIME_S);
    SET_PERI_REG_MASK(SPI_USER_REG(1), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(1), SPI_HOLD_TIME_V, 1, SPI_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_CTRL2_REG(1), SPI_SETUP_TIME_V, 0, SPI_SETUP_TIME_S);
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
    uint32_t drv = 2;
    if (pfhdr->spi_speed == ESP_IMAGE_SPI_SPEED_80M) {
        drv = 3;
    }

    uint32_t chip_ver = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG);
    uint32_t pkg_ver = chip_ver & 0x7;

    if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5) {
        // For ESP32D2WD the SPI pins are already configured
        // flash clock signal should come from IO MUX.
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, drv, FUN_DRV_S);
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2) {
        // For ESP32PICOD2 the SPI pins are already configured
        // flash clock signal should come from IO MUX.
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, drv, FUN_DRV_S);
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4) {
        // For ESP32PICOD4 the SPI pins are already configured
        // flash clock signal should come from IO MUX.
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, drv, FUN_DRV_S);
    } else {
        const uint32_t spiconfig = ets_efuse_get_spiconfig();
        if (spiconfig == EFUSE_SPICONFIG_SPI_DEFAULTS) {
            gpio_matrix_out(SPI_IOMUX_PIN_NUM_CS, SPICS0_OUT_IDX, 0, 0);
            gpio_matrix_out(SPI_IOMUX_PIN_NUM_MISO, SPIQ_OUT_IDX, 0, 0);
            gpio_matrix_in(SPI_IOMUX_PIN_NUM_MISO, SPIQ_IN_IDX, 0);
            gpio_matrix_out(SPI_IOMUX_PIN_NUM_MOSI, SPID_OUT_IDX, 0, 0);
            gpio_matrix_in(SPI_IOMUX_PIN_NUM_MOSI, SPID_IN_IDX, 0);
            gpio_matrix_out(SPI_IOMUX_PIN_NUM_WP, SPIWP_OUT_IDX, 0, 0);
            gpio_matrix_in(SPI_IOMUX_PIN_NUM_WP, SPIWP_IN_IDX, 0);
            gpio_matrix_out(SPI_IOMUX_PIN_NUM_HD, SPIHD_OUT_IDX, 0, 0);
            gpio_matrix_in(SPI_IOMUX_PIN_NUM_HD, SPIHD_IN_IDX, 0);
            //select pin function gpio
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U, PIN_FUNC_GPIO);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U, PIN_FUNC_GPIO);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA2_U, PIN_FUNC_GPIO);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, PIN_FUNC_GPIO);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, PIN_FUNC_GPIO);
            // flash clock signal should come from IO MUX.
            // set drive ability for clock
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, drv, FUN_DRV_S);

            uint32_t flash_id = g_rom_flashchip.device_id;
            if (flash_id == FLASH_ID_GD25LQ32C) {
                // Set drive ability for 1.8v flash in 80Mhz.
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA0_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA1_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA2_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA3_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CMD_U,   FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U,   FUN_DRV, 3, FUN_DRV_S);
            }
        }
    }
}

void IRAM_ATTR bootloader_flash_dummy_config(const esp_image_header_t* pfhdr)
{
    int spi_cache_dummy = 0;
    uint32_t modebit = READ_PERI_REG(SPI_CTRL_REG(0));
    if (modebit & SPI_FASTRD_MODE) {
        if (modebit & SPI_FREAD_QIO) {    //SPI mode is QIO
            spi_cache_dummy = SPI0_R_QIO_DUMMY_CYCLELEN;
        } else if (modebit & SPI_FREAD_DIO) {    //SPI mode is DIO
            spi_cache_dummy = SPI0_R_DIO_DUMMY_CYCLELEN;
            SET_PERI_REG_BITS(SPI_USER1_REG(0), SPI_USR_ADDR_BITLEN_V, SPI0_R_DIO_ADDR_BITSLEN, SPI_USR_ADDR_BITLEN_S);
        } else if(modebit & (SPI_FREAD_QUAD | SPI_FREAD_DUAL))  {    //SPI mode is QOUT or DIO
            spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
        }
    }

    extern uint8_t g_rom_spiflash_dummy_len_plus[];
    switch (pfhdr->spi_speed) {
        case ESP_IMAGE_SPI_SPEED_80M:
            g_rom_spiflash_dummy_len_plus[0] = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_80M;
            g_rom_spiflash_dummy_len_plus[1] = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_80M;
            break;
        case ESP_IMAGE_SPI_SPEED_40M:
            g_rom_spiflash_dummy_len_plus[0] = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_40M;
            g_rom_spiflash_dummy_len_plus[1] = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_40M;
            break;
        case ESP_IMAGE_SPI_SPEED_26M:
        case ESP_IMAGE_SPI_SPEED_20M:
            g_rom_spiflash_dummy_len_plus[0] = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_20M;
            g_rom_spiflash_dummy_len_plus[1] = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_20M;
            break;
        default:
            break;
    }

    SET_PERI_REG_BITS(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + g_rom_spiflash_dummy_len_plus[0],
            SPI_USR_DUMMY_CYCLELEN_S);
}