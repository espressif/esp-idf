/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// MSPI IOMUX PINs Please check iomux_mspi_pin_struct/reg.h
// On S31, SPI pins defined here are all wrong. these pins are individual pins, don't use normal GPIO pins anymore.
#define GPIO_NUM_INVALID            -1
#define MSPI_IOMUX_PIN_NUM_CS1      GPIO_NUM_INVALID
#define MSPI_IOMUX_PIN_NUM_HD       30
#define MSPI_IOMUX_PIN_NUM_WP       28
#define MSPI_IOMUX_PIN_NUM_CS0      26
#define MSPI_IOMUX_PIN_NUM_CLK      31
#define MSPI_IOMUX_PIN_NUM_MISO     27
#define MSPI_IOMUX_PIN_NUM_MOSI     32
#define MSPI_IOMUX_PIN_NUM_D4       GPIO_NUM_INVALID
#define MSPI_IOMUX_PIN_NUM_D5       GPIO_NUM_INVALID
#define MSPI_IOMUX_PIN_NUM_D6       GPIO_NUM_INVALID
#define MSPI_IOMUX_PIN_NUM_D7       GPIO_NUM_INVALID
#define MSPI_IOMUX_PIN_NUM_DQS      GPIO_NUM_INVALID

// GPSPI2 has two sets of Qaud IOMUX pins
#define SPI2_FUNC_NUM_QUAD          2
#define SPI2_IOMUX_PIN_NUM_CLK      20
#define SPI2_IOMUX_PIN_NUM_MOSI     21
#define SPI2_IOMUX_PIN_NUM_MISO     22
#define SPI2_IOMUX_PIN_NUM_CS       23
#define SPI2_IOMUX_PIN_NUM_HD       24
#define SPI2_IOMUX_PIN_NUM_WP       25

#define SPI2_IOMUX_PIN_2_NUM_CS     52
#define SPI2_IOMUX_PIN_2_NUM_CLK    53
#define SPI2_IOMUX_PIN_2_NUM_MOSI   54
#define SPI2_IOMUX_PIN_2_NUM_MISO   55
#define SPI2_IOMUX_PIN_2_NUM_HD     56
#define SPI2_IOMUX_PIN_2_NUM_WP     57

// When using Octal SPI, we make use of SPI2_FUNC_NUM_OCT to route them as follows.
#define SPI2_FUNC_NUM_OCT           0
#define SPI2_IOMUX_PIN_NUM_HD_OCT   9
#define SPI2_IOMUX_PIN_NUM_CS_OCT   10
#define SPI2_IOMUX_PIN_NUM_MOSI_OCT 11
#define SPI2_IOMUX_PIN_NUM_CLK_OCT  12
#define SPI2_IOMUX_PIN_NUM_MISO_OCT 13
#define SPI2_IOMUX_PIN_NUM_WP_OCT   14
#define SPI2_IOMUX_PIN_NUM_IO4_OCT  15
#define SPI2_IOMUX_PIN_NUM_IO5_OCT  16
#define SPI2_IOMUX_PIN_NUM_IO6_OCT  17
#define SPI2_IOMUX_PIN_NUM_IO7_OCT  18
#define SPI2_IOMUX_PIN_NUM_DQS_OCT  19

//SPI3 have no iomux pins
