/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTS_LCD_CAMERA || SOC_LCDCAM_SUPPORTED
/**
 * @brief LCD clock source
 */
typedef soc_periph_lcd_clk_src_t lcd_clock_source_t;
#else
typedef int lcd_clock_source_t;
#endif // SOC_I2S_SUPPORTS_LCD_CAMERA || SOC_LCDCAM_SUPPORTED

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
    LCD_COLOR_SPACE_RGB = COLOR_SPACE_RGB, /*!< Color space: RGB */
    LCD_COLOR_SPACE_YUV = COLOR_SPACE_YUV, /*!< Color space: YUV */
} lcd_color_space_t;

/**
 * @brief LCD color pixel format in RGB color space
 */
typedef enum {
    LCD_COLOR_PIXEL_FORMAT_RGB565 = COLOR_PIXEL_RGB565, /*!< 16 bits, 5 bits per R/B value, 6 bits for G value */
    LCD_COLOR_PIXEL_FORMAT_RGB666 = COLOR_PIXEL_RGB666, /*!< 18 bits, 6 bits per R/G/B value */
    LCD_COLOR_PIXEL_FORMAT_RGB888 = COLOR_PIXEL_RGB888, /*!< 24 bits, 8 bits per R/G/B value */
} lcd_color_rgb_pixel_format_t;

/**
 * @brief LCD color range
 */
typedef enum {
    LCD_COLOR_RANGE_LIMIT = COLOR_RANGE_LIMIT, /*!< Limited color range */
    LCD_COLOR_RANGE_FULL = COLOR_RANGE_FULL,   /*!< Full color range */
} lcd_color_range_t;

/**
 * @brief YUV sampling method
 */
typedef enum {
    LCD_YUV_SAMPLE_422 = COLOR_PIXEL_YUV422, /*!< YUV 4:2:2 sampling */
    LCD_YUV_SAMPLE_420 = COLOR_PIXEL_YUV420, /*!< YUV 4:2:0 sampling */
    LCD_YUV_SAMPLE_411 = COLOR_PIXEL_YUV411, /*!< YUV 4:1:1 sampling */
} lcd_yuv_sample_t;

/**
 * @brief The standard used for conversion between RGB and YUV
 */
typedef enum {
    LCD_YUV_CONV_STD_BT601 = COLOR_CONV_STD_RGB_YUV_BT601, /*!< YUV<->RGB conversion standard: BT.601 */
    LCD_YUV_CONV_STD_BT709 = COLOR_CONV_STD_RGB_YUV_BT709, /*!< YUV<->RGB conversion standard: BT.709 */
} lcd_yuv_conv_std_t;

#ifdef __cplusplus
}
#endif
