/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file records the flash vendor that we offically supported.
 * we have many chip-specific files, like ``spi_flash_chip_gd.c``,
 * which means that this file is used for GD flash chips.
 *
 * The following definations illustrate what flash vendor is officially
 * supported by ESP chips. If a flash vendor is officially supported, the chip
 * specific file will be linked by default, vice versa. You can also adjust this
 * manually in Kconfig options.
 *
 * For example:
 * Following `SPI_FLASH_VENDOR_ISSI_SUPPORTED` is (1), which means file `spi_flash_chip_issi.c`
 * will be linked.
 *
 */

#pragma once

#define SPI_FLASH_VENDOR_XMC_SUPPORTED       (1)
#define SPI_FLASH_VENDOR_GD_SUPPORTED        (1)
#define SPI_FLASH_VENDOR_ISSI_SUPPORTED      (1)
#define SPI_FLASH_VENDOR_MXIC_SUPPORTED      (1)
#define SPI_FLASH_VENDOR_WINBOND_SUPPORTED   (1)
