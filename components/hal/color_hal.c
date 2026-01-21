/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "hal/color_hal.h"
#include "hal/assert.h"

uint32_t color_hal_pixel_format_fourcc_get_bit_depth(esp_color_fourcc_t four_character_code)
{
    switch (four_character_code) {
    case ESP_COLOR_FOURCC_ALPHA4:
        return 4;
    case ESP_COLOR_FOURCC_ALPHA8:
    case ESP_COLOR_FOURCC_GREY:
    case ESP_COLOR_FOURCC_RAW8:
        return 8;
    case ESP_COLOR_FOURCC_RAW10:
        return 10;
    case ESP_COLOR_FOURCC_RAW12:
    case ESP_COLOR_FOURCC_OUYY_EVYY:
        return 12;
    case ESP_COLOR_FOURCC_YUYV:
    case ESP_COLOR_FOURCC_UYVY:
    case ESP_COLOR_FOURCC_YVYU:
    case ESP_COLOR_FOURCC_VYUY:
    case ESP_COLOR_FOURCC_RGB16:
    case ESP_COLOR_FOURCC_RGB16_BE:
        return 16;
    case ESP_COLOR_FOURCC_BGR24:
    case ESP_COLOR_FOURCC_RGB24:
    case ESP_COLOR_FOURCC_YUV:
        return 24;
    case ESP_COLOR_FOURCC_BGRA32:
        return 32;
    default:
        // Unknown color space pixel format, unknown bit depth
        HAL_ASSERT(false);
        return 0;
    }
}
