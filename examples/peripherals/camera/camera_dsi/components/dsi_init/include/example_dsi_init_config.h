/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

<<<<<<< HEAD
=======
#include "sdkconfig.h"

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
#ifdef __cplusplus
extern "C" {
#endif

<<<<<<< HEAD
#define EXAMPLE_MIPI_DSI_IMAGE_HSYNC            40
#define EXAMPLE_MIPI_DSI_IMAGE_HBP              140
#define EXAMPLE_MIPI_DSI_IMAGE_HFP              40
#define EXAMPLE_MIPI_DSI_IMAGE_VSYNC            4
#define EXAMPLE_MIPI_DSI_IMAGE_VBP              16
#define EXAMPLE_MIPI_DSI_IMAGE_VFP              16
=======
#if CONFIG_EXAMPLE_LCD_PATTERN_ILI9881C
// Refresh Rate = 80000000/(40+140+40+800)/(4+16+16+1280) = 60Hz
#define EXAMPLE_MIPI_DSI_DPI_CLK_MHZ              80
#define EXAMPLE_MIPI_DSI_IMAGE_HSYNC              40
#define EXAMPLE_MIPI_DSI_IMAGE_HBP                140
#define EXAMPLE_MIPI_DSI_IMAGE_HFP                40
#define EXAMPLE_MIPI_DSI_IMAGE_VSYNC              4
#define EXAMPLE_MIPI_DSI_IMAGE_VBP                16
#define EXAMPLE_MIPI_DSI_IMAGE_VFP                16
#elif CONFIG_EXAMPLE_LCD_PATTERN_EK79007
// Refresh Rate = 48000000/(10+120+120+1024)/(1+20+10+600) = 60Hz
#define EXAMPLE_MIPI_DSI_DPI_CLK_MHZ              48
#define EXAMPLE_MIPI_DSI_IMAGE_HSYNC              10
#define EXAMPLE_MIPI_DSI_IMAGE_HBP                120
#define EXAMPLE_MIPI_DSI_IMAGE_HFP                120
#define EXAMPLE_MIPI_DSI_IMAGE_VSYNC              1
#define EXAMPLE_MIPI_DSI_IMAGE_VBP                20
#define EXAMPLE_MIPI_DSI_IMAGE_VFP                10
#endif
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

#ifdef __cplusplus
}
#endif
