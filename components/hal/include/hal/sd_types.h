/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD bus width
 */
typedef enum {
    SD_BUS_WIDTH_1_BIT,    ///< 1 bit
    SD_BUS_WIDTH_4_BIT,    ///< 4 bit
    SD_BUS_WIDTH_8_BIT,    ///< 8 bit
} sd_bus_width_t;

#ifdef __cplusplus
}
#endif
