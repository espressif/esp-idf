/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_H_RES         480
#define TEST_LCD_V_RES         272

#define TEST_LCD_VSYNC_GPIO    48
#define TEST_LCD_HSYNC_GPIO    47
#define TEST_LCD_DE_GPIO       45
#define TEST_LCD_PCLK_GPIO     21
#define TEST_LCD_DATA0_GPIO    3  // B0
#define TEST_LCD_DATA1_GPIO    4  // B1
#define TEST_LCD_DATA2_GPIO    5  // B2
#define TEST_LCD_DATA3_GPIO    6  // B3
#define TEST_LCD_DATA4_GPIO    7  // B4
#define TEST_LCD_DATA5_GPIO    8  // G0
#define TEST_LCD_DATA6_GPIO    9  // G1
#define TEST_LCD_DATA7_GPIO    10 // G2
#define TEST_LCD_DATA8_GPIO    11 // G3
#define TEST_LCD_DATA9_GPIO    12 // G4
#define TEST_LCD_DATA10_GPIO   13 // G5
#define TEST_LCD_DATA11_GPIO   14 // R0
#define TEST_LCD_DATA12_GPIO   15 // R1
#define TEST_LCD_DATA13_GPIO   16 // R2
#define TEST_LCD_DATA14_GPIO   17 // R3
#define TEST_LCD_DATA15_GPIO   18 // R4
#define TEST_LCD_DISP_EN_GPIO  -1

#define TEST_LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)

#ifdef __cplusplus
}
#endif
