/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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

#if SOC_HAS(I2S_I80_LCD) || SOC_HAS(LCDCAM_I80_LCD) || SOC_HAS(LCDCAM_RGB_LCD)
/**
 * @brief LCD clock source
 */
typedef soc_periph_lcd_clk_src_t lcd_clock_source_t;
#else
typedef int lcd_clock_source_t;
#endif

/**
 * @brief RGB data endian
 */
typedef enum {
    LCD_RGB_DATA_ENDIAN_BIG = 0, /*!< RGB data endian: MSB first */
    LCD_RGB_DATA_ENDIAN_LITTLE,  /*!< RGB data endian: LSB first */
} lcd_rgb_data_endian_t;

/**
 * @brief LCD color type, in FourCC format
 */
typedef enum {
    LCD_COLOR_FMT_GRAY8  = ESP_COLOR_FOURCC_GREY,      ///< 8-bit gray scale
    LCD_COLOR_FMT_RGB565 = ESP_COLOR_FOURCC_RGB16,     ///< RGB565
    LCD_COLOR_FMT_RGB888 = ESP_COLOR_FOURCC_BGR24,     ///< RGB888
    LCD_COLOR_FMT_YUV422_YUYV = ESP_COLOR_FOURCC_YUYV, ///< YUV422 packed in YUYV order
    LCD_COLOR_FMT_YUV422_YVYU = ESP_COLOR_FOURCC_YVYU, ///< YUV422 packed in YVYU order
    LCD_COLOR_FMT_YUV422_VYUY = ESP_COLOR_FOURCC_VYUY, ///< YUV422 packed in VYUY order
    LCD_COLOR_FMT_YUV422_UYVY = ESP_COLOR_FOURCC_UYVY, ///< YUV422 packed in UYVY order
    LCD_COLOR_FMT_YUV420_OUYY_EVYY = ESP_COLOR_FOURCC_OUYY_EVYY, ///< YUV420 packed in OUYY/EVYY order
} lcd_color_format_t;

/**
 * @brief LCD color range
 */
typedef enum {
    LCD_COLOR_RANGE_LIMIT = COLOR_RANGE_LIMIT, /*!< Limited color range */
    LCD_COLOR_RANGE_FULL = COLOR_RANGE_FULL,   /*!< Full color range */
} lcd_color_range_t;

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
