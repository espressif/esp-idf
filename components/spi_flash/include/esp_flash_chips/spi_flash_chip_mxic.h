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
 * MXIC SPI flash chip_drv, uses all the above functions for its operations. In
 * default autodetection, this is used as a catchall if a more specific chip_drv
 * is not found.
 */
#ifdef CONFIG_SPI_FLASH_SUPPORT_MXIC_CHIP
extern const spi_flash_chip_t esp_flash_chip_mxic;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_mxic;
#endif

/**
 * MXIC OPI flash chip_drv, uses all the above functions for its operations. In
 * default autodetection, this is used as a catchall if a more specific chip_drv
 * is not found.
 */
#ifdef CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
extern const spi_flash_chip_t esp_flash_chip_mxic_opi;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_mxic_opi;
#endif

#ifdef __cplusplus
}
#endif
