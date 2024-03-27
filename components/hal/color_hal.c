/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "hal/color_hal.h"
#include "hal/assert.h"

uint32_t color_hal_pixel_format_get_bit_depth(color_space_pixel_format_t format)
{
    switch (format.color_type_id) {
    case COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY4):
    case COLOR_TYPE_ID(COLOR_SPACE_ALPHA, COLOR_PIXEL_A4):
    case COLOR_TYPE_ID(COLOR_SPACE_CLUT, COLOR_PIXEL_L4):
        return 4;
    case COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW8):
    case COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8):
    case COLOR_TYPE_ID(COLOR_SPACE_ALPHA, COLOR_PIXEL_A8):
    case COLOR_TYPE_ID(COLOR_SPACE_CLUT, COLOR_PIXEL_L8):
        return 8;
    case COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW10):
        return 10;
    case COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW12):
    case COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420):
    case COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV411):
        return 12;
    case COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565):
    case COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422):
        return 16;
    case COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888):
    case COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV444):
        return 24;
    case COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888):
        return 32;
    default:
        // Unknown color space pixel format, unknown bit depth
        HAL_ASSERT(false);
        return 0;
    }
}
