/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include "sdkconfig.h"

/* I2C port and GPIOs */
#define EXAMPLE_I2C_NUM            (0)
#define EXAMPLE_I2C_SDA_IO         CONFIG_EXAMPLE_I2C_SDA_IO
#define EXAMPLE_I2C_SCL_IO         CONFIG_EXAMPLE_I2C_SCL_IO

/* I2S port and GPIOs */
#define EXAMPLE_I2S_NUM            (0)
#define EXAMPLE_I2S_MCK_IO         CONFIG_EXAMPLE_I2S_MCLK_IO
#define EXAMPLE_I2S_BCK_IO         CONFIG_EXAMPLE_I2S_BCLK_IO
#define EXAMPLE_I2S_WS_IO          CONFIG_EXAMPLE_I2S_WS_IO
#define EXAMPLE_I2S_DI_IO          CONFIG_EXAMPLE_I2S_DIN_IO

/* SD card SPI GPIOs */
#define EXAMPLE_SD_SPI_CLK_IO      CONFIG_EXAMPLE_SD_SPI_CLK_IO
#define EXAMPLE_SD_SPI_MOSI_IO     CONFIG_EXAMPLE_SD_SPI_MOSI_IO
#define EXAMPLE_SD_SPI_MISO_IO     CONFIG_EXAMPLE_SD_SPI_MISO_IO
#define EXAMPLE_SD_SPI_CS_IO       CONFIG_EXAMPLE_SD_SPI_CS_IO
