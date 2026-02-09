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
 * Winbond SPI flash chip_drv, uses all the above functions for its operations. In
 * default autodetection, this is used as a catchall if a more specific chip_drv
 * is not found.
 */

#ifdef CONFIG_SPI_FLASH_SUPPORT_WINBOND_CHIP
extern const spi_flash_chip_t esp_flash_chip_winbond;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_winbond;
#endif

#ifdef __cplusplus
}
#endif
