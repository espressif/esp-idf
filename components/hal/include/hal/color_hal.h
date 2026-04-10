/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
 * @param four_character_code Value constructed in four character code
 *
 * @return Number of bits per pixel
 */
uint32_t color_hal_pixel_format_fourcc_get_bit_depth(esp_color_fourcc_t four_character_code);

#ifdef __cplusplus
}
#endif
