/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_flash.h"
#include "esp_flash_chips/spi_flash_chip_driver.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * GD (GigaDevice) SPI flash chip_drv, uses all the above functions for its operations. In
 * default autodetection, this is used as a catchall if a more specific chip_drv
 * is not found.
 *
 * Note that this is for GD chips with product ID 40H (GD25Q) and 60H (GD25LQ). The chip diver uses
 * different commands to write the SR2 register according to the chip ID. For GD25Q40 - GD25Q16
 * chips, and GD25LQ chips, WRSR (01H) command is used; while WRSR2 (31H) is used for GD25Q32 -
 * GD25Q127 chips.
 */
esp_err_t spi_flash_chip_gd_probe(esp_flash_t *chip, uint32_t flash_id);
esp_err_t spi_flash_chip_gd_set_io_mode(esp_flash_t *chip);
esp_err_t spi_flash_chip_gd_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode);

#ifdef CONFIG_SPI_FLASH_SUPPORT_GD_CHIP
extern const spi_flash_chip_t esp_flash_chip_gd;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_gd;
#endif

#ifdef __cplusplus
}
#endif
