/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD clock source
 * @note User should select the clock source based on the real requirement:
 * @verbatim embed:rst:leading-asterisk
 * +---------------------+-------------------------+----------------------------+
 * | LCD clock source    | Features                | Power Management           |
 * +=====================+=========================+============================+
 * | LCD_CLK_SRC_PLL160M | High resolution         | ESP_PM_APB_FREQ_MAX lock   |
 * +---------------------+-------------------------+----------------------------+
 * | LCD_CLK_SRC_PLL240M | High resolution         | ESP_PM_APB_FREQ_MAX lock   |
 * +---------------------+-------------------------+----------------------------+
 * | LCD_CLK_SRC_APLL    | Configurable resolution | ESP_PM_NO_LIGHT_SLEEP lock |
 * +---------------------+-------------------------+----------------------------+
 * | LCD_CLK_SRC_XTAL    | Medium resolution       | No PM lock                 |
 * +---------------------+-------------------------+----------------------------+
 * @endverbatim
 */
typedef enum {
    LCD_CLK_SRC_PLL160M, /*!< Select PLL160M as the source clock */
    LCD_CLK_SRC_PLL240M, /*!< Select PLL240M as the source clock */
    LCD_CLK_SRC_APLL,    /*!< Select APLL as the source clock */
    LCD_CLK_SRC_XTAL,    /*!< Select XTAL as the source clock */
} lcd_clock_source_t;

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
