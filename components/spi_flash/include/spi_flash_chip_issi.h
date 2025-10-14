/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_flash.h"
#include "spi_flash_chip_driver.h"
#include "sdkconfig.h"


/**
 * ISSI SPI flash chip_drv, uses all the above functions for its operations. In
 * default autodetection, this is used as a catchall if a more specific chip_drv
 * is not found.
 */
esp_err_t spi_flash_chip_issi_probe(esp_flash_t *chip, uint32_t flash_id);
esp_err_t spi_flash_chip_issi_set_io_mode(esp_flash_t *chip);
esp_err_t spi_flash_chip_issi_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode);

#ifdef CONFIG_SPI_FLASH_SUPPORT_ISSI_CHIP
extern const spi_flash_chip_t esp_flash_chip_issi;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_issi;
#endif
