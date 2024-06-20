/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCD_I80_SUPPORTED || SOC_LCD_RGB_SUPPORTED
/**
 * @brief LCD clock source
 */
typedef soc_periph_lcd_clk_src_t lcd_clock_source_t;
#endif

/**
 * @brief RGB color endian
 */
typedef enum {
    LCD_RGB_ELEMENT_ORDER_RGB, /*!< RGB element order: RGB */
    LCD_RGB_ELEMENT_ORDER_BGR, /*!< RGB element order: BGR */
} lcd_rgb_element_order_t;

/**
 * @brief RGB data endian
 */
typedef enum {
    LCD_RGB_DATA_ENDIAN_BIG = 0, /*!< RGB data endian: MSB first */
    LCD_RGB_DATA_ENDIAN_LITTLE,  /*!< RGB data endian: LSB first */
} lcd_rgb_data_endian_t;

/**
 * @brief LCD color space
 */
typedef enum {
    LCD_COLOR_SPACE_RGB, /*!< Color space: RGB */
    LCD_COLOR_SPACE_YUV, /*!< Color space: YUV */
} lcd_color_space_t;

/**
 * @brief LCD color range
 */
typedef enum {
    LCD_COLOR_RANGE_LIMIT, /*!< Limited color range */
    LCD_COLOR_RANGE_FULL,  /*!< Full color range */
} lcd_color_range_t;

/**
 * @brief YUV sampling method
 */
typedef enum {
    LCD_YUV_SAMPLE_422, /*!< YUV 4:2:2 sampling */
    LCD_YUV_SAMPLE_420, /*!< YUV 4:2:0 sampling */
    LCD_YUV_SAMPLE_411, /*!< YUV 4:1:1 sampling */
} lcd_yuv_sample_t;

/**
 * @brief The standard used for conversion between RGB and YUV
 */
typedef enum {
    LCD_YUV_CONV_STD_BT601, /*!< YUV<->RGB conversion standard: BT.601 */
    LCD_YUV_CONV_STD_BT709, /*!< YUV<->RGB conversion standard: BT.709 */
} lcd_yuv_conv_std_t;

#ifdef __cplusplus
}
#endif
