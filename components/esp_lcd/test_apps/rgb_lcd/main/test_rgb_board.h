/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_H_RES         800
#define TEST_LCD_V_RES         480
#define TEST_LCD_HSYNC         1
#define TEST_LCD_HBP           40
#define TEST_LCD_HFP           20
#define TEST_LCD_VSYNC         1
#define TEST_LCD_VBP           10
#define TEST_LCD_VFP           5

#define TEST_LCD_PIXEL_CLOCK_HZ (18 * 1000 * 1000)

#if CONFIG_IDF_TARGET_ESP32S3

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

#elif CONFIG_IDF_TARGET_ESP32P4

#define TEST_LCD_VSYNC_GPIO    41
#define TEST_LCD_HSYNC_GPIO    39
#define TEST_LCD_DE_GPIO       43
#define TEST_LCD_PCLK_GPIO     33
#define TEST_LCD_DATA0_GPIO    40 // B0
#define TEST_LCD_DATA1_GPIO    42 // B1
#define TEST_LCD_DATA2_GPIO    27 // B2
#define TEST_LCD_DATA3_GPIO    29 // B3
#define TEST_LCD_DATA4_GPIO    31 // B4
#define TEST_LCD_DATA5_GPIO    21 // G0
#define TEST_LCD_DATA6_GPIO    23 // G1
#define TEST_LCD_DATA7_GPIO    26 // G2
#define TEST_LCD_DATA8_GPIO    28 // G3
#define TEST_LCD_DATA9_GPIO    30 // G4
#define TEST_LCD_DATA10_GPIO   32 // G5
#define TEST_LCD_DATA11_GPIO   6  // R0
#define TEST_LCD_DATA12_GPIO   0  // R1
#define TEST_LCD_DATA13_GPIO   15 // R2
#define TEST_LCD_DATA14_GPIO   17 // R3
#define TEST_LCD_DATA15_GPIO   19 // R4
#define TEST_LCD_DISP_EN_GPIO  -1

#else
#error "Unsupported target"
#endif

#ifdef __cplusplus
}
#endif
