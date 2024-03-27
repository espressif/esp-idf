/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                      Color Space
---------------------------------------------------------------*/
/**
 * @brief Color Space
 *
 * @note Save enum 0 for special purpose
 */
typedef enum {
    COLOR_SPACE_RAW = 1,     ///< Color space raw
    COLOR_SPACE_RGB,         ///< Color space rgb
    COLOR_SPACE_YUV,         ///< Color space yuv
    COLOR_SPACE_GRAY,        ///< Color space gray
    COLOR_SPACE_ARGB,        ///< Color space argb
    COLOR_SPACE_ALPHA,       ///< Color space alpha (A)
    COLOR_SPACE_CLUT,        ///< Color look-up table (L)
} color_space_t;

/*---------------------------------------------------------------
                      Color Pixel Format
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
    COLOR_PIXEL_RGB666,      ///< 18 bits, 6 bits per R/G/B value
    COLOR_PIXEL_RGB565,      ///< 16 bits, 5 bits per R/B value, 6 bits for G value
} color_pixel_rgb_format_t;

/**
 * @brief YUV Format
 */
typedef enum {
    COLOR_PIXEL_YUV444,    ///< 24 bits, 8 bits per Y/U/V value
    COLOR_PIXEL_YUV422,    ///< 16 bits, 8-bit Y per pixel, 8-bit U and V per two pixels
    COLOR_PIXEL_YUV420,    ///< 12 bits, 8-bit Y per pixel, 8-bit U and V per four pixels
    COLOR_PIXEL_YUV411,    ///< 12 bits, 8-bit Y per pixel, 8-bit U and V per four pixels
} color_pixel_yuv_format_t;

/**
 * @brief Gray Format
 */
typedef enum {
    COLOR_PIXEL_GRAY4,    ///< 4 bits, grayscale
    COLOR_PIXEL_GRAY8,    ///< 8 bits, grayscale
} color_pixel_gray_format_t;

/**
 * @brief ARGB Format
 */
typedef enum {
    COLOR_PIXEL_ARGB8888,   ///< 32 bits, 8 bits per A(alpha)/R/G/B value
} color_pixel_argb_format_t;

/**
 * @brief Alpha(A) Format
 */
typedef enum {
    COLOR_PIXEL_A4,   ///< 4 bits, opacity only
    COLOR_PIXEL_A8,   ///< 8 bits, opacity only
} color_pixel_alpha_format_t;

/**
 * @brief CLUT(L) Format
 */
typedef enum {
    COLOR_PIXEL_L4,   ///< 4 bits, color look-up table
    COLOR_PIXEL_L8,   ///< 8 bits, color look-up table
} color_pixel_clut_format_t;

/*---------------------------------------------------------------
                Color Space Pixel Struct Type
---------------------------------------------------------------*/
///< Bitwidth of the `color_space_pixel_format_t::color_space` field
#define COLOR_SPACE_BITWIDTH                 8
///< Bitwidth of the `color_space_pixel_format_t::pixel_format` field
#define COLOR_PIXEL_FORMAT_BITWIDTH          24
///< Helper to get the color_space from a unique color type ID
#define COLOR_SPACE_TYPE(color_type_id)      (((color_type_id) >> COLOR_PIXEL_FORMAT_BITWIDTH) & ((1 << COLOR_SPACE_BITWIDTH) - 1))
///< Helper to get the pixel_format from a unique color type ID
#define COLOR_PIXEL_FORMAT(color_type_id)    ((color_type_id) & ((1 << COLOR_PIXEL_FORMAT_BITWIDTH) - 1))
///< Make a unique ID of a color based on the value of color space and pixel format
#define COLOR_TYPE_ID(color_space, pixel_format) (((color_space) << COLOR_PIXEL_FORMAT_BITWIDTH) | (pixel_format))

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

/*---------------------------------------------------------------
                      Color Conversion
---------------------------------------------------------------*/
/**
 * @brief Color range
 * @note The difference between a full range color and a limited range color is
 *       the amount of shades of black and white that they can display.
 */
typedef enum {
    COLOR_RANGE_LIMIT, /*!< Limited color range, 16 is the darkest black and 235 is the brightest white */
    COLOR_RANGE_FULL,  /*!< Full color range, 0 is the darkest black and 255 is the brightest white */
} color_range_t;

/**
 * @brief The standard used for conversion between RGB and YUV
 */
typedef enum {
    COLOR_CONV_STD_RGB_YUV_BT601, /*!< YUV<->RGB conversion standard: BT.601 */
    COLOR_CONV_STD_RGB_YUV_BT709, /*!< YUV<->RGB conversion standard: BT.709 */
} color_conv_std_rgb_yuv_t;

/*---------------------------------------------------------------
                      Color Endian
---------------------------------------------------------------*/

/**
 * @brief RGB element order
 */
typedef enum {
    COLOR_RGB_ELEMENT_ORDER_RGB, /*!< RGB element order: RGB */
    COLOR_RGB_ELEMENT_ORDER_BGR, /*!< RGB element order: BGR */
} color_rgb_element_order_t;

#ifdef __cplusplus
}
#endif
