/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_H_RES          240
#define TEST_LCD_V_RES          280

#if CONFIG_IDF_TARGET_ESP32H2
#define TEST_LCD_BK_LIGHT_GPIO  10
#define TEST_LCD_RST_GPIO       5
#define TEST_LCD_CS_GPIO        3
#define TEST_LCD_DC_GPIO        4
#define TEST_LCD_PCLK_GPIO      2
#define TEST_LCD_DATA0_GPIO     1
#elif CONFIG_IDF_TARGET_ESP32C2
#define TEST_LCD_BK_LIGHT_GPIO  18
#define TEST_LCD_RST_GPIO       5
#define TEST_LCD_CS_GPIO        0
#define TEST_LCD_DC_GPIO        1
#define TEST_LCD_PCLK_GPIO      2
#define TEST_LCD_DATA0_GPIO     4
#else
#define TEST_LCD_BK_LIGHT_GPIO  18
#define TEST_LCD_RST_GPIO       5
#define TEST_LCD_CS_GPIO        0
#define TEST_LCD_DC_GPIO        19
#define TEST_LCD_PCLK_GPIO      2
#define TEST_LCD_DATA0_GPIO     4
#define TEST_LCD_DATA1_GPIO     7
#define TEST_LCD_DATA2_GPIO     8
#define TEST_LCD_DATA3_GPIO     9
#define TEST_LCD_DATA4_GPIO     10
#define TEST_LCD_DATA5_GPIO     11
#define TEST_LCD_DATA6_GPIO     12
#define TEST_LCD_DATA7_GPIO     13
#endif

#define TEST_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)

#ifdef __cplusplus
}
#endif
