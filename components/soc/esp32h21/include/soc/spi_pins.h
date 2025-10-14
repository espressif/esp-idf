/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_SPI_PINS_H_
#define _SOC_SPI_PINS_H_

// MSPI IOMUX PINs
#define MSPI_FUNC_NUM               0
#define MSPI_IOMUX_PIN_NUM_HD       23
#define MSPI_IOMUX_PIN_NUM_WP       22
#define MSPI_IOMUX_PIN_NUM_CS0      20
#define MSPI_IOMUX_PIN_NUM_CLK      24
#define MSPI_IOMUX_PIN_NUM_MOSI     25
#define MSPI_IOMUX_PIN_NUM_MISO     21

// GPSPI2 IOMUX PINs
#define SPI2_FUNC_NUM               2
#define SPI2_IOMUX_PIN_NUM_WP       0
#define SPI2_IOMUX_PIN_NUM_HD       1
#define SPI2_IOMUX_PIN_NUM_CLK      2
#define SPI2_IOMUX_PIN_NUM_MOSI     3
#define SPI2_IOMUX_PIN_NUM_MISO     4
#define SPI2_IOMUX_PIN_NUM_CS       12

#endif
