/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the bit depth (bits/pixel) of each color space pixel format
 *
 * @param color_type_id Value constructed in color_space_pixel_format_t struct
 *
 * @return Number of bits per pixel
 */
uint32_t color_hal_pixel_format_get_bit_depth(color_space_pixel_format_t color_type_id);

#ifdef __cplusplus
}
#endif
