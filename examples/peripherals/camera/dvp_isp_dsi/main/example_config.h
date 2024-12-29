/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_RGB565_BITS_PER_PIXEL           16

#define EXAMPLE_ISP_DVP_CAM_SCCB_SCL_IO        (33)
#define EXAMPLE_ISP_DVP_CAM_SCCB_SDA_IO        (32)

#define EXAMPLE_ISP_DVP_CAM_XCLK_FREQ_HZ       (20000000)

#define EXAMPLE_ISP_DVP_CAM_DATA_WIDTH         (8)
#define EXAMPLE_ISP_DVP_CAM_D0_IO              (8)
#define EXAMPLE_ISP_DVP_CAM_D1_IO              (9)
#define EXAMPLE_ISP_DVP_CAM_D2_IO              (10)
#define EXAMPLE_ISP_DVP_CAM_D3_IO              (4)
#define EXAMPLE_ISP_DVP_CAM_D4_IO              (5)
#define EXAMPLE_ISP_DVP_CAM_D5_IO              (45)
#define EXAMPLE_ISP_DVP_CAM_D6_IO              (46)
#define EXAMPLE_ISP_DVP_CAM_D7_IO              (47)
#define EXAMPLE_ISP_DVP_CAM_XCLK_IO            (20)
#define EXAMPLE_ISP_DVP_CAM_PCLK_IO            (21)
#define EXAMPLE_ISP_DVP_CAM_DE_IO              (2)
#define EXAMPLE_ISP_DVP_CAM_VSYNC_IO           (23)
#define EXAMPLE_ISP_DVP_CAM_HSYNC_IO           (-1)

#if CONFIG_EXAMPLE_CAM_HRES_800

#if CONFIG_EXAMPLE_CAM_VRES_640
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_800x640_15fps"
#elif CONFIG_EXAMPLE_CAM_VRES_800
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_800x800_15fps"
#endif

#elif CONFIG_EXAMPLE_CAM_HRES_1024

#if CONFIG_EXAMPLE_CAM_VRES_600
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_1024x600_15fps"
#endif

#endif

#ifdef __cplusplus
}
#endif
