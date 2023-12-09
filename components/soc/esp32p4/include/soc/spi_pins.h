/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// Normal IOMUX pins
#define SPI2_FUNC_NUM               3
#define SPI2_IOMUX_PIN_NUM_HD       6
#define SPI2_IOMUX_PIN_NUM_CS       7
#define SPI2_IOMUX_PIN_NUM_MOSI     8
#define SPI2_IOMUX_PIN_NUM_CLK      9
#define SPI2_IOMUX_PIN_NUM_MISO     10
#define SPI2_IOMUX_PIN_NUM_WP       11

// When using Octal SPI, we make use of SPI2_FUNC_NUM_OCT to route them as follows.
#define SPI2_FUNC_NUM_OCT           2
#define SPI2_IOMUX_PIN_NUM_HD_OCT   32
#define SPI2_IOMUX_PIN_NUM_CS_OCT   28
#define SPI2_IOMUX_PIN_NUM_MOSI_OCT 29
#define SPI2_IOMUX_PIN_NUM_CLK_OCT  30
#define SPI2_IOMUX_PIN_NUM_MISO_OCT 31
#define SPI2_IOMUX_PIN_NUM_WP_OCT   33
#define SPI2_IOMUX_PIN_NUM_IO4_OCT  34
#define SPI2_IOMUX_PIN_NUM_IO5_OCT  35
#define SPI2_IOMUX_PIN_NUM_IO6_OCT  36
#define SPI2_IOMUX_PIN_NUM_IO7_OCT  37

//SPI3 have no iomux pins
