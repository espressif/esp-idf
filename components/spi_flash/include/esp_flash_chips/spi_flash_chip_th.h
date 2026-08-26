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

#ifdef CONFIG_SPI_FLASH_SUPPORT_TH_CHIP
extern const spi_flash_chip_t esp_flash_chip_th;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_th;
#endif

#ifdef __cplusplus
}
#endif
