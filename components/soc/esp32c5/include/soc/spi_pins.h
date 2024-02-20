/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// TODO: [ESP32C5] IDF-8698 (inherit from C6)

#define SPI_FUNC_NUM            0
#define SPI_IOMUX_PIN_NUM_CS    24
#define SPI_IOMUX_PIN_NUM_CLK   29
#define SPI_IOMUX_PIN_NUM_MOSI  30
#define SPI_IOMUX_PIN_NUM_MISO  25
#define SPI_IOMUX_PIN_NUM_WP    26
#define SPI_IOMUX_PIN_NUM_HD    28

#define SPI2_FUNC_NUM           2
#define SPI2_IOMUX_PIN_NUM_MISO 2
#define SPI2_IOMUX_PIN_NUM_HD   4
#define SPI2_IOMUX_PIN_NUM_WP   5
#define SPI2_IOMUX_PIN_NUM_CLK  6
#define SPI2_IOMUX_PIN_NUM_MOSI 7
#define SPI2_IOMUX_PIN_NUM_CS   16
