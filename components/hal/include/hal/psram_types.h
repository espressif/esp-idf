/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of PSRAM command types
 */
typedef enum {
    PSRAM_HAL_CMD_QPI,     /*!< Quad command for psram command */
    PSRAM_HAL_CMD_SPI,     /*!< SPI command for psram command */
} psram_hal_cmd_mode_t;

#ifdef __cplusplus
}
#endif
