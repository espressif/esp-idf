/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                      Four Character Code
---------------------------------------------------------------*/

#define ESP_COLOR_FOURCC(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

/**
 * RGB24
 * Memory Layout:
 *            | bit7 - bit0 |
 *    Byte 2: |  B7  -  B0  |
 *    Byte 1: |  G7  -  G0  |
 *    Byte 0: |  R7  -  R0  |
 */
#define ESP_COLOR_FOURCC_RGB24          ESP_COLOR_FOURCC('R', 'G', 'B', '3') /* 24 bpp RGB-8-8-8 */

/**
 * RGB565_BE
 * Memory Layout:
 *            | bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0 |
 *    Byte 1: |  G2   G1   G0 | B4   B3   B2   B1   B0  |
 *    Byte 0: |  R4   R3   R2   R1   R0 | G5   G4   G3  |
 */
#define ESP_COLOR_FOURCC_RGB16_BE       ESP_COLOR_FOURCC('R', 'G', 'B', 'B') /* 16 bpp RGB-5-6-5, big endian */

/**
 * Grey8
 * Memory Layout:
 *            | bit7 - bit0 |
 *    Byte 0: |  G7  -  G0  |
 */
#define ESP_COLOR_FOURCC_GREY           ESP_COLOR_FOURCC('G', 'R', 'E', 'Y') /* 8 bpp Greyscale */

/**
 * YUV444
 * Memory Layout:
 *    +--+--+--+ +--+--+--+
 *    |Y0|U0|V0| |Y1|U1|V1|
 *    +--+--+--+ +--+--+--+
 */
#define ESP_COLOR_FOURCC_YUV            ESP_COLOR_FOURCC('V', '3', '0', '8') /* 24 bpp, Y-U-V 4:4:4 */

/**
 * YUYV422
 * Memory Layout:
 *    +--+--+--+--+ +--+--+--+--+
 *    |Y0|U0|Y1|V0| |Y2|U2|Y3|V2|
 *    +--+--+--+--+ +--+--+--+--+
 */
#define ESP_COLOR_FOURCC_YUYV           ESP_COLOR_FOURCC('Y', 'U', 'Y', 'V') /* 16 bpp, YUYV 4:2:2 */

/**
 * YVYU422
 * Memory Layout:
 *    +--+--+--+--+ +--+--+--+--+
 *    |Y0|V0|Y1|U0| |Y2|V2|Y3|U2|
 *    +--+--+--+--+ +--+--+--+--+
 */
#define ESP_COLOR_FOURCC_YVYU           ESP_COLOR_FOURCC('Y', 'V', 'Y', 'U') /* 16 bpp, YVYU 4:2:2 */

/**
 * UYVY422
 * Memory Layout:
 *    +--+--+--+--+ +--+--+--+--+
 *    |U0|Y0|V0|Y1| |U2|Y2|V2|Y3|
 *    +--+--+--+--+ +--+--+--+--+
 */
#define ESP_COLOR_FOURCC_UYVY           ESP_COLOR_FOURCC('U', 'Y', 'V', 'Y') /* 16 bpp, UYVY 4:2:2 */

/**
 * VYUY422
 * Memory Layout:
 *    +--+--+--+--+ +--+--+--+--+
 *    |V0|Y0|U0|Y1| |V2|Y2|U2|Y3|
 *    +--+--+--+--+ +--+--+--+--+
 */
#define ESP_COLOR_FOURCC_VYUY           ESP_COLOR_FOURCC('V', 'Y', 'U', 'Y') /* 16 bpp, VYUY 4:2:2 */

/**
 * Espressif YUV420, U00 V10 shared Y00 Y01 Y10 Y11, U02 V12 shared Y02 Y03 Y12 Y13
 * Memory Layout:
 *       +---+---+---+ +---+---+---+
 * Line1 |U00|Y00|Y01| |U02|Y02|Y03|
 *       +---+---+---+ +---+---+---+
 * Line2 |V10|Y10|Y11| |V12|Y12|Y13|
 *       +---+---+---+ +---+---+---+
 */
#define ESP_COLOR_FOURCC_OUYY_EVYY      ESP_COLOR_FOURCC('O', 'U', 'E', 'V') /* 12 bpp, Espressif Y-U-V 4:2:0 */


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
 * @brief RAW element order
 */
typedef enum {
    COLOR_RAW_ELEMENT_ORDER_BGGR,    ///< BGGR order
    COLOR_RAW_ELEMENT_ORDER_GBRG,    ///< GBRG order
    COLOR_RAW_ELEMENT_ORDER_GRBG,    ///< GRBG order
    COLOR_RAW_ELEMENT_ORDER_RGGB,    ///< RGGB order
} color_raw_element_order_t;

/**
 * @brief RGB element order
 */
typedef enum {
    COLOR_RGB_ELEMENT_ORDER_RGB, /*!< RGB element order: RGB */
    COLOR_RGB_ELEMENT_ORDER_BGR, /*!< RGB element order: BGR */
} color_rgb_element_order_t;

/*---------------------------------------------------------------
                Data Structure for Color Pixel Unit
---------------------------------------------------------------*/

/**
 * @brief Data structure for ARGB8888 pixel unit
 */
typedef union {
    struct {
        uint32_t b: 8;      /*!< B component [0, 255] */
        uint32_t g: 8;      /*!< G component [0, 255] */
        uint32_t r: 8;      /*!< R component [0, 255] */
        uint32_t a: 8;      /*!< A component [0, 255] */
    };
    uint32_t val;           /*!< 32-bit ARGB8888 value */
} color_pixel_argb8888_data_t;

/**
 * @brief Data structure for RGB888 pixel unit
 */
typedef union {
    struct {
        uint8_t b;          /*!< B component [0, 255] */
        uint8_t g;          /*!< G component [0, 255] */
        uint8_t r;          /*!< R component [0, 255] */
    };
    uint32_t val;           /*!< 32-bit RGB888 value */
} color_pixel_rgb888_data_t;

/**
 * @brief Data structure for RGB565 pixel unit
 */
typedef union {
    struct {
        uint16_t b: 5;      /*!< B component [0, 31] */
        uint16_t g: 6;      /*!< G component [0, 63] */
        uint16_t r: 5;      /*!< R component [0, 31] */
    };
    uint16_t val;           /*!< 16-bit RGB565 value */
} color_pixel_rgb565_data_t;

/**
 * @brief Data structure for GRAY8 pixel unit
 */
typedef union {
    struct {
        uint8_t gray;      /*!< Gray component [0, 255] */
    };
    uint8_t val;           /*!< 8-bit GRAY8 value */
} color_pixel_gray8_data_t;

/**
 * @brief Data structure for YUV macroblock unit
 *
 * For YUV420, a macroblock is 2x2 pixels
 * For YUV422, a macroblock is 2x1 pixels
 * For YUV444, a macro block is a 1x1 pixel
 */
typedef struct {
    uint8_t y;      /*!< Y component [0, 255] */
    uint8_t u;      /*!< U component [0, 255] */
    uint8_t v;      /*!< V component [0, 255] */
} color_macroblock_yuv_data_t;

/*---------------------------------------------------------------
                        Color Components
---------------------------------------------------------------*/

/**
 * @brief Color component
 */
typedef enum {
    COLOR_COMPONENT_R,          /*!< R component */
    COLOR_COMPONENT_G,          /*!< G component */
    COLOR_COMPONENT_B,          /*!< B component */
    COLOR_COMPONENT_INVALID,    /*!< Invalid color component */
} color_component_t;

/**
 * @brief The order of the components per pack in the YUV422 format
 */
typedef enum {
    COLOR_YUV422_PACK_ORDER_YUYV, /*!< YUYV */
    COLOR_YUV422_PACK_ORDER_YVYU, /*!< YVYU */
    COLOR_YUV422_PACK_ORDER_UYVY, /*!< UYVY */
    COLOR_YUV422_PACK_ORDER_VYUY, /*!< VYUY */
} color_yuv422_pack_order_t;

///< The following values are deprecated, please use the FOURCC values instead, IDF-14284

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


#ifdef __cplusplus
}
#endif
