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

#ifdef CONFIG_SPI_FLASH_SUPPORT_TH_CHIP
extern const spi_flash_chip_t esp_flash_chip_th;
#else
extern __attribute__((weak)) const spi_flash_chip_t esp_flash_chip_th;
#endif
