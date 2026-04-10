/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Four Character Code type definition
 */
typedef uint32_t esp_color_fourcc_t;

#define ESP_COLOR_FOURCC(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

/**
 * BGRA32
 * Memory Layout:
 *            | bit7 - bit0 |
 *    Byte 3: |  A7  -  A0  |
 *    Byte 2: |  R7  -  R0  |
 *    Byte 1: |  G7  -  G0  |
 *    Byte 0: |  B7  -  B0  |
 */
#define ESP_COLOR_FOURCC_BGRA32         ESP_COLOR_FOURCC('B', 'A', '2', '4') /* 32 bpp BGRA-8-8-8-8 */

/**
 * BGR24
 * Memory Layout:
 *            | bit7 - bit0 |
 *    Byte 2: |  R7  -  R0  |
 *    Byte 1: |  G7  -  G0  |
 *    Byte 0: |  B7  -  B0  |
 */
#define ESP_COLOR_FOURCC_BGR24          ESP_COLOR_FOURCC('B', 'G', 'R', '3') /* 24 bpp BGR-8-8-8 */

/**
 * RGB24 (RGB888)
 * Memory Layout:
 *            | bit7 - bit0 |
 *    Byte 2: |  B7  -  B0  |
 *    Byte 1: |  G7  -  G0  |
 *    Byte 0: |  R7  -  R0  |
 */
#define ESP_COLOR_FOURCC_RGB24          ESP_COLOR_FOURCC('R', 'G', 'B', '3') /* 24 bpp RGB-8-8-8 */

/**
 * RGB565
 * Memory Layout:
 *            | bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0 |
 *    Byte 1: |  R4   R3   R2   R1   R0 | G5   G4   G3  |
 *    Byte 0: |  G2   G1   G0 | B4   B3   B2   B1   B0  |
 */
#define ESP_COLOR_FOURCC_RGB16          ESP_COLOR_FOURCC('R', 'G', 'B', 'L') /* 16 bpp RGB-5-6-5, little endian */

/**
 * RGB565 (Big Endian)
 * Memory Layout:
 *            | bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0 |
 *    Byte 1: |  G2   G1   G0 | B4   B3   B2   B1   B0  |
 *    Byte 0: |  R4   R3   R2   R1   R0 | G5   G4   G3  |
 */
#define ESP_COLOR_FOURCC_RGB16_BE       ESP_COLOR_FOURCC('R', 'G', 'B', 'E') /* 16 bpp RGB-5-6-5, big endian */

/**
 * Grey8
 * Memory Layout:
 *            | bit7 - bit0 |
 *    Byte 0: |  G7  -  G0  |
 */
#define ESP_COLOR_FOURCC_GREY           ESP_COLOR_FOURCC('G', 'R', 'E', 'Y') /* 8 bpp Greyscale */

/**
 * ALPHA4 (Alpha 4 bits)
 * Memory Layout:
 *    Addr0  Addr1  Addr2  Addr3
 *    A0A1   A2A3   A4A5   A6A7
 */
#define ESP_COLOR_FOURCC_ALPHA4         ESP_COLOR_FOURCC('A', 'L', 'P', '4') /* 4 bpp, an 4-bit alpha-only format */

/**
 * ALPHA8 (Alpha 8 bits)
 * Memory Layout:
 *    Addr0  Addr1  Addr2  Addr3
 *    A0     A1     A2     A3
 */
#define ESP_COLOR_FOURCC_ALPHA8         ESP_COLOR_FOURCC('A', 'L', 'P', '8') /* 8 bpp, an 8-bit alpha-only format */

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
 *                 +--+--+--+--+ +--+--+--+--+
 *  (lowest byte)  |U0|Y0|V0|Y1| |U2|Y2|V2|Y3|
 *                 +--+--+--+--+ +--+--+--+--+
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

/**
 * RAW8
 * Memory Layout:
 *    Addr0    Addr1    Addr2    Addr3
 *    R0        R1       R2       R3
 */
#define ESP_COLOR_FOURCC_RAW8           ESP_COLOR_FOURCC('R', 'A', 'W', '8') /* 8 bpp, raw8 */

/**
 * RAW10
 * Memory Layout:
 *    Addr0      Addr1      Addr2      Addr3
 *    R0[7:0]    R0[1:0]    R1[7:0]    R1[1:0]
 */
#define ESP_COLOR_FOURCC_RAW10          ESP_COLOR_FOURCC('R', 'A', 'W', 'A') /* 10 bpp, raw10 */

/**
 * RAW12
 * Memory Layout:
 *    Addr0      Addr1      Addr2      Addr3
 *    R0[7:0]    R0[3:0]    R1[7:0]    R1[3:0]
 */
#define ESP_COLOR_FOURCC_RAW12          ESP_COLOR_FOURCC('R', 'A', 'W', 'C') /* 12 bpp, raw12 */

/**
 * RAW16
 * Memory Layout:
 *    Addr0      Addr1      Addr2      Addr3
 *    R0[7:0]    R0[7:0]    R1[7:0]    R1[7:0]
 */
#define ESP_COLOR_FOURCC_RAW16          ESP_COLOR_FOURCC('R', 'A', 'W', 'G') /* 16 bpp, raw16 */


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

#ifdef __cplusplus
}
#endif
