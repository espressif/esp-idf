/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of pixel color space format
 */
typedef enum {
    COLOR_SPACE_RAW8,           /*!< 8 bits per pixel */
    COLOR_SPACE_RAW10,          /*!< 10 bits per pixel */
    COLOR_SPACE_RAW12,          /*!< 12 bits per pixel */
    COLOR_SPACE_RGB888,         /*!< 24 bits, 8 bits per R/G/B value */
    COLOR_SPACE_RGB565,         /*!< 16 bits, 5 bits per R/B value, 6 bits for G value */
    COLOR_SPACE_YUV422,         /*!< 16 bits, 8-bit Y per pixel, 8-bit U and V per two pixels */
    COLOR_SPACE_YUV420,         /*!< 12 bits, 8-bit Y per pixel, 8-bit U and V per four pixels */
} color_space_pixel_format_t;

#ifdef __cplusplus
}
#endif
