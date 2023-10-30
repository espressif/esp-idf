/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @background
 *
 * Color Space: a specific representation of colors, e.g. RGB, YUV, etc.
 * Color Pixel Format: a specific pixel format of a certain color space, e.g. RGB565, YUV422, etc.
 */

/*---------------------------------------------------------------
                      Color Space
---------------------------------------------------------------*/
/**
 * @brief Color Space
 */
typedef enum {
    COLOR_SPACE_RAW,     ///< Color space raw
    COLOR_SPACE_RGB,     ///< Color space rgb
    COLOR_SPACE_YUV,     ///< Color space yuv
    COLOR_SPACE_GRAY,    ///< Color space gray
} color_space_t;

/*---------------------------------------------------------------
                      Color Space Format
---------------------------------------------------------------*/
/**
 * @brief Raw Format
 */
typedef enum {
    COLOR_PIXEL_RAW8,     ///< 8 bits per pixel
    COLOR_PIXEL_RAW10,    ///< 10 bits per pixel
    COLOR_PIXEL_RAW12,    ///< 12 bits per pixel
} color_pixel_raw_format_t;

/**
 * @brief RGB Format
 */
typedef enum {
    COLOR_PIXEL_RGB888,      ///< 24 bits, 8 bits per R/G/B value
    COLOR_PIXEL_RGB565,      ///< 16 bits, 5 bits per R/B value, 6 bits for G value
} color_pixel_rgb_format_t;

/**
 * @brief YUV Format
 */
typedef enum {
    COLOR_PIXEL_YUV444,    ///< 24 bits, 8 bits per Y/U/V value
    COLOR_PIXEL_YUV422,    ///< 16 bits, 8-bit Y per pixel, 8-bit U and V per two pixels
    COLOR_PIXEL_YUV420,    ///< 12 bits, 8-bit Y per pixel, 8-bit U and V per four pixels
} color_pixel_yuv_format_t;

/**
 * @brief Gray Format
 */
typedef enum {
    COLOR_PIXEL_GRAY4,    ///< 4 bits, grayscale
    COLOR_PIXEL_GRAY8,    ///< 8 bits, grayscale
} color_pixel_gray_format_t;

/*---------------------------------------------------------------
                Color Space Struct Type
---------------------------------------------------------------*/
///< Bitwidth of the `color_space_format_t:color_space` field
#define COLOR_SPACE_BITWIDTH                 8
///< Bitwidth of the `color_space_format_t:pixel_format` field
#define COLOR_PIXEL_FORMAT_BITWIDTH          24
///< Helper to get `color_space_format_t:color_space` from its `color_space_pixel_format_t:color_type_id`
#define COLOR_SPACE_TYPE(color_type_id)      (((color_type_id) >> COLOR_PIXEL_FORMAT_BITWIDTH) & ((1 << COLOR_SPACE_BITWIDTH) - 1))
///< Helper to get `color_space_format_t:pixel_format` from its `color_space_pixel_format_t:color_type_id`
#define COLOR_PIXEL_FORMAT(color_type_id)    ((color_type_id) & ((1 << COLOR_PIXEL_FORMAT_BITWIDTH) - 1))

/**
 * @brief Color Space Info Structure
 */
typedef union {
    struct {
        uint32_t pixel_format: COLOR_PIXEL_FORMAT_BITWIDTH;    ///< Format of a certain color space type
        uint32_t color_space: COLOR_SPACE_BITWIDTH;            ///< Color space type
    };
    uint32_t color_type_id;                                    ///< Unique type of a certain color pixel format
} color_space_pixel_format_t;


#ifdef __cplusplus
}
#endif
