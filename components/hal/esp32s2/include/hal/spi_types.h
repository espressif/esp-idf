/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include_next "hal/spi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_HOST    _Pragma ("GCC warning \"SPI_HOST is deprecated in favor of SPI1_HOST\"")  SPI1_HOST
#define FSPI_HOST   _Pragma ("GCC warning \"FSPI_HOST is deprecated in favor of SPI2_HOST\"") SPI2_HOST
#define HSPI_HOST   _Pragma ("GCC warning \"HSPI_HOST is deprecated in favor of SPI3_HOST\"") SPI3_HOST

#ifdef __cplusplus
}
#endif
