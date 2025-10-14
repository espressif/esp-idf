/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// MSPI IOMUX PINs
#define MSPI_FUNC_NUM               1
#define MSPI_IOMUX_PIN_NUM_CLK      6
#define MSPI_IOMUX_PIN_NUM_MISO     7
#define MSPI_IOMUX_PIN_NUM_MOSI     8
#define MSPI_IOMUX_PIN_NUM_HD       9
#define MSPI_IOMUX_PIN_NUM_WP       10
#define MSPI_IOMUX_PIN_NUM_CS0      11

//For D2WD and PICO-D4 chip
#define SPI_D2WD_PIN_NUM_MISO       17
#define SPI_D2WD_PIN_NUM_MOSI       8
#define SPI_D2WD_PIN_NUM_CLK        6
#define SPI_D2WD_PIN_NUM_CS         16
#define SPI_D2WD_PIN_NUM_WP         7
#define SPI_D2WD_PIN_NUM_HD         11

#define SPI2_FUNC_NUM           1
#define SPI2_IOMUX_PIN_NUM_MISO 12
#define SPI2_IOMUX_PIN_NUM_MOSI 13
#define SPI2_IOMUX_PIN_NUM_CLK  14
#define SPI2_IOMUX_PIN_NUM_CS   15
#define SPI2_IOMUX_PIN_NUM_WP   2
#define SPI2_IOMUX_PIN_NUM_HD   4

#define SPI3_FUNC_NUM           1
#define SPI3_IOMUX_PIN_NUM_MISO 19
#define SPI3_IOMUX_PIN_NUM_MOSI 23
#define SPI3_IOMUX_PIN_NUM_CLK  18
#define SPI3_IOMUX_PIN_NUM_CS   5
#define SPI3_IOMUX_PIN_NUM_WP   22
#define SPI3_IOMUX_PIN_NUM_HD   21
