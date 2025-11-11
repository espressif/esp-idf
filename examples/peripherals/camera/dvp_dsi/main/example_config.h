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

#define EXAMPLE_RGB565_BITS_PER_PIXEL      (16)
#define EXAMPLE_DVP_CAM_XCLK_FREQ_HZ       (20000000)
#define EXAMPLE_DVP_CAM_DATA_WIDTH         (8)

#if !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define EXAMPLE_DVP_CAM_SCCB_SCL_IO        (8)
#define EXAMPLE_DVP_CAM_SCCB_SDA_IO        (7)

#define EXAMPLE_DVP_CAM_D0_IO              (2)
#define EXAMPLE_DVP_CAM_D1_IO              (32)
#define EXAMPLE_DVP_CAM_D2_IO              (33)
#define EXAMPLE_DVP_CAM_D3_IO              (23)
#define EXAMPLE_DVP_CAM_D4_IO              (3)
#define EXAMPLE_DVP_CAM_D5_IO              (6)
#define EXAMPLE_DVP_CAM_D6_IO              (5)
#define EXAMPLE_DVP_CAM_D7_IO              (21)

#define EXAMPLE_DVP_CAM_XCLK_IO            (20)
#define EXAMPLE_DVP_CAM_PCLK_IO            (4)
#define EXAMPLE_DVP_CAM_DE_IO              (22)
#define EXAMPLE_DVP_CAM_VSYNC_IO           (37)
#define EXAMPLE_DVP_CAM_HSYNC_IO           (-1)

#else
#define EXAMPLE_DVP_CAM_SCCB_SCL_IO        (33)
#define EXAMPLE_DVP_CAM_SCCB_SDA_IO        (32)

#define EXAMPLE_DVP_CAM_D0_IO              (53)
#define EXAMPLE_DVP_CAM_D1_IO              (54)
#define EXAMPLE_DVP_CAM_D2_IO              (26)
#define EXAMPLE_DVP_CAM_D3_IO              (1)
#define EXAMPLE_DVP_CAM_D4_IO              (0)
#define EXAMPLE_DVP_CAM_D5_IO              (45)
#define EXAMPLE_DVP_CAM_D6_IO              (46)
#define EXAMPLE_DVP_CAM_D7_IO              (47)

#define EXAMPLE_DVP_CAM_XCLK_IO            (20)
#define EXAMPLE_DVP_CAM_PCLK_IO            (21)
#define EXAMPLE_DVP_CAM_DE_IO              (22)
#define EXAMPLE_DVP_CAM_VSYNC_IO           (23)
#define EXAMPLE_DVP_CAM_HSYNC_IO           (-1)
#endif

#if CONFIG_EXAMPLE_CAM_HRES_640

#if CONFIG_EXAMPLE_CAM_VRES_480
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RGB565_640x480_6fps"
#endif

#elif CONFIG_EXAMPLE_CAM_HRES_240

#if CONFIG_EXAMPLE_CAM_VRES_240
#define EXAMPLE_CAM_FORMAT                     "DVP_8bit_20Minput_RGB565_240x240_25fps"
#endif

#endif

#ifndef EXAMPLE_CAM_FORMAT
#error "Unsupported camera format! Please adjust EXAMPLE_CAM_HRES and EXAMPLE_CAM_VRES in menuconfig"
#endif

#ifdef __cplusplus
}
#endif
