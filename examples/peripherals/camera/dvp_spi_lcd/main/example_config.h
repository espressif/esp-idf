/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------CAM Config------------//
#define EXAMPLE_RGB565_BITS_PER_PIXEL      16

#define EXAMPLE_DVP_CAM_SCCB_SCL_IO        (5)
#define EXAMPLE_DVP_CAM_SCCB_SDA_IO        (4)

#define EXAMPLE_DVP_CAM_XCLK_FREQ_HZ       (20000000)

#define EXAMPLE_DVP_CAM_DATA_WIDTH         (8)

#define EXAMPLE_DVP_CAM_D0_IO              (11)
#define EXAMPLE_DVP_CAM_D1_IO              (9)
#define EXAMPLE_DVP_CAM_D2_IO              (8)
#define EXAMPLE_DVP_CAM_D3_IO              (10)
#define EXAMPLE_DVP_CAM_D4_IO              (12)
#define EXAMPLE_DVP_CAM_D5_IO              (18)
#define EXAMPLE_DVP_CAM_D6_IO              (17)
#define EXAMPLE_DVP_CAM_D7_IO              (16)

#define EXAMPLE_DVP_CAM_XCLK_IO            (15)
#define EXAMPLE_DVP_CAM_PCLK_IO            (13)
#define EXAMPLE_DVP_CAM_DE_IO              (7)
#define EXAMPLE_DVP_CAM_VSYNC_IO           (6)
#define EXAMPLE_DVP_CAM_HSYNC_IO           (-1)

#if CONFIG_SPIRAM
#define EXAMPLE_DVP_CAM_BUF_ALLOC_CAPS     (MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA)
#else
#define EXAMPLE_DVP_CAM_BUF_ALLOC_CAPS     (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)
#endif

#if CONFIG_EXAMPLE_CAM_INPUT_FORMAT_YUV422
#define EXAMPLE_CAM_FORMAT                  "DVP_8bit_20Minput_YUV422_240x240_25fps" // ov2640
#elif CONFIG_EXAMPLE_CAM_INPUT_FORMAT_RGB565
#define EXAMPLE_CAM_FORMAT                  "DVP_8bit_20Minput_RGB565_240x240_25fps" // ov2640
#endif

#ifndef EXAMPLE_CAM_FORMAT
#error "Unsupported camera format! Please adjust EXAMPLE_CAM_HRES and EXAMPLE_CAM_VRES in menuconfig"
#endif

//----------LCD Config------------//
#define EXAMPLE_LEDC_DVP_XCLK       (LEDC_TIMER_0)
#define EXAMPLE_LEDC_LCD_BACKLIGHT  (LEDC_TIMER_1)
#define EXAMPLE_LCD_SPI_NUM         (SPI3_HOST)
#define EXAMPLE_LCD_CMD_BITS        (8)
#define EXAMPLE_LCD_PARAM_BITS      (8)

/* LCD Display */
#define EXAMPLE_LCD_SPI_MOSI        (GPIO_NUM_47)
#define EXAMPLE_LCD_SPI_CLK         (GPIO_NUM_21)
#define EXAMPLE_LCD_SPI_CS          (GPIO_NUM_44)
#define EXAMPLE_LCD_DC              (GPIO_NUM_43)
#define EXAMPLE_LCD_RST             (GPIO_NUM_NC)
#define EXAMPLE_LCD_BACKLIGHT       (GPIO_NUM_48)

#define EXAMPLE_LCD_PIXEL_CLOCK_HZ  (80 * 1000 * 1000)

#ifdef __cplusplus
}
#endif
