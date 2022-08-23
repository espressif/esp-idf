/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_H_RES         800
#define TEST_LCD_V_RES         480

#define TEST_LCD_VSYNC_GPIO    3
#define TEST_LCD_HSYNC_GPIO    46
#define TEST_LCD_DE_GPIO       0
#define TEST_LCD_PCLK_GPIO     9
#define TEST_LCD_DATA0_GPIO    14 // B0
#define TEST_LCD_DATA1_GPIO    13 // B1
#define TEST_LCD_DATA2_GPIO    12 // B2
#define TEST_LCD_DATA3_GPIO    11 // B3
#define TEST_LCD_DATA4_GPIO    10 // B4
#define TEST_LCD_DATA5_GPIO    39 // G0
#define TEST_LCD_DATA6_GPIO    38 // G1
#define TEST_LCD_DATA7_GPIO    45 // G2
#define TEST_LCD_DATA8_GPIO    48 // G3
#define TEST_LCD_DATA9_GPIO    47 // G4
#define TEST_LCD_DATA10_GPIO   21 // G5
#define TEST_LCD_DATA11_GPIO   1  // R0
#define TEST_LCD_DATA12_GPIO   2  // R1
#define TEST_LCD_DATA13_GPIO   42 // R2
#define TEST_LCD_DATA14_GPIO   41 // R3
#define TEST_LCD_DATA15_GPIO   40 // R4
#define TEST_LCD_DISP_EN_GPIO  -1

#define TEST_LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)

#ifdef __cplusplus
}
#endif
