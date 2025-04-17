/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#define EXAMPLE_ISP_DVP_CAM_D0_IO              (53)
#define EXAMPLE_ISP_DVP_CAM_D1_IO              (54)
#define EXAMPLE_ISP_DVP_CAM_D2_IO              (26)
#define EXAMPLE_ISP_DVP_CAM_D3_IO              (1)
#define EXAMPLE_ISP_DVP_CAM_D4_IO              (0)
#define EXAMPLE_ISP_DVP_CAM_D5_IO              (45)
#define EXAMPLE_ISP_DVP_CAM_D6_IO              (46)
#define EXAMPLE_ISP_DVP_CAM_D7_IO              (47)
#define EXAMPLE_ISP_DVP_CAM_XCLK_IO            (20)
#define EXAMPLE_ISP_DVP_CAM_PCLK_IO            (21)
#define EXAMPLE_ISP_DVP_CAM_DE_IO              (22)
#define EXAMPLE_ISP_DVP_CAM_VSYNC_IO           (23)
#define EXAMPLE_ISP_DVP_CAM_HSYNC_IO           (-1)

#if CONFIG_EXAMPLE_CAM_HRES_640

#if CONFIG_EXAMPLE_CAM_VRES_480
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_640x480_26fps" // sc030iot
#endif

#elif CONFIG_EXAMPLE_CAM_HRES_800

#if CONFIG_EXAMPLE_CAM_VRES_640
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_800x640_15fps" // ov2640
#elif CONFIG_EXAMPLE_CAM_VRES_800
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_800x800_15fps" // ov2640
#endif

#elif CONFIG_EXAMPLE_CAM_HRES_1024

#if CONFIG_EXAMPLE_CAM_VRES_600
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RAW8_1024x600_15fps" // ov2640
#endif

#endif

#ifndef EXAMPLE_CAM_FORMAT
#error "Unsupported camera format! Please adjust EXAMPLE_CAM_HRES and EXAMPLE_CAM_VRES in menuconfig"
#endif

#ifdef __cplusplus
}
#endif
