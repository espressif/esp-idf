/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <assert.h>
#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp32s3/rom/spi_flash.h"
#include "soc/efuse_reg.h"
#include "soc/spi_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/soc_caps.h"
#include "flash_qio_mode.h"
#include "bootloader_flash_config.h"
#include "bootloader_common.h"

#define FLASH_IO_MATRIX_DUMMY_40M   0
#define FLASH_IO_MATRIX_DUMMY_80M   0
#define FLASH_IO_DRIVE_GD_WITH_1V8PSRAM    3
#define FLASH_CS_SETUP_TIME 3
#define FLASH_CS_HOLD_TIME  3
#define FLASH_CS_HOLD_DELAY 2

void bootloader_flash_update_id()
{
    esp_rom_spiflash_chip_t *chip = &rom_spiflash_legacy_data->chip;
    chip->device_id = bootloader_read_flash_id();
}

void IRAM_ATTR bootloader_flash_cs_timing_config()
{
    //SPI0/1 share the cs_hold / cs_setup, cd_hold_time / cd_setup_time, cs_hold_delay registers for FLASH, so we only need to set SPI0 related registers here
#if CONFIG_ESPTOOLPY_OCT_FLASH
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME_V, FLASH_CS_HOLD_TIME, SPI_MEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_SETUP_TIME_V, FLASH_CS_SETUP_TIME, SPI_MEM_CS_SETUP_TIME_S);
    //CS high time
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_DELAY_V, FLASH_CS_HOLD_DELAY, SPI_MEM_CS_HOLD_DELAY_S);
#else
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME_V, 0, SPI_MEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_SETUP_TIME_V, 0, SPI_MEM_CS_SETUP_TIME_S);
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
#endif
}

void IRAM_ATTR bootloader_flash_clock_config(const esp_image_header_t *pfhdr)
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
    esp_rom_spiflash_config_clk(spi_clk_div, 1);
}

void IRAM_ATTR bootloader_flash_set_dummy_out(void)
{
    REG_SET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FDUMMY_OUT | SPI_MEM_D_POL | SPI_MEM_Q_POL);
    REG_SET_BIT(SPI_MEM_CTRL_REG(1), SPI_MEM_FDUMMY_OUT | SPI_MEM_D_POL | SPI_MEM_Q_POL);
}

void IRAM_ATTR bootloader_flash_dummy_config(const esp_image_header_t *pfhdr)
{
    bootloader_configure_spi_pins(1);
    bootloader_flash_set_dummy_out();
}
