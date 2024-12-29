/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/spi_mem_c_struct.h"
#include "soc/spi1_mem_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spi1_mem_dev_s spi_mem_dev_t;
extern spi_mem_dev_t SPIMEM1;
extern spi_mem_c_dev_t SPIMEM0;

#ifdef __cplusplus
}
#endif
