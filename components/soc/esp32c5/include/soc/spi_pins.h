/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/io_mux_reg.h"

// MSPI IO_MUX pin
#define SPI_FUNC_NUM            0
#define SPI_IOMUX_PIN_NUM_CS    SPI_CS0_GPIO_NUM
#define SPI_IOMUX_PIN_NUM_CLK   SPI_CLK_GPIO_NUM
#define SPI_IOMUX_PIN_NUM_MOSI  SPI_D_GPIO_NUM
#define SPI_IOMUX_PIN_NUM_MISO  SPI_Q_GPIO_NUM
#define SPI_IOMUX_PIN_NUM_WP    SPI_WP_GPIO_NUM
#define SPI_IOMUX_PIN_NUM_HD    SPI_HD_GPIO_NUM

#define SPI2_FUNC_NUM           2
#define SPI2_IOMUX_PIN_NUM_MISO 2
#define SPI2_IOMUX_PIN_NUM_HD   4
#define SPI2_IOMUX_PIN_NUM_WP   5
#define SPI2_IOMUX_PIN_NUM_CLK  6
#define SPI2_IOMUX_PIN_NUM_MOSI 7
#define SPI2_IOMUX_PIN_NUM_CS   10
