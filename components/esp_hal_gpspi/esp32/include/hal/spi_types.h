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

#define SPI_HOST  SPI1_HOST
#define HSPI_HOST SPI2_HOST
#define VSPI_HOST SPI3_HOST

#ifdef __cplusplus
}
#endif
