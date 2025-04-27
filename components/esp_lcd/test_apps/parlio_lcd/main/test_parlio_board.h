/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_H_RES         (240)
#define TEST_LCD_V_RES         (280)

#if CONFIG_IDF_TARGET_ESP32P4
#define TEST_LCD_BK_LIGHT_GPIO (48)
#define TEST_LCD_RST_GPIO      (35)
#define TEST_LCD_PCLK_GPIO     (33)
#define TEST_LCD_CS_GPIO       (32)
#define TEST_LCD_DC_GPIO       (34)
#define TEST_LCD_DATA0_GPIO    (24)
#define TEST_LCD_DATA1_GPIO    (25)
#define TEST_LCD_DATA2_GPIO    (26)
#define TEST_LCD_DATA3_GPIO    (27)
#define TEST_LCD_DATA4_GPIO    (28)
#define TEST_LCD_DATA5_GPIO    (29)
#define TEST_LCD_DATA6_GPIO    (30)
#define TEST_LCD_DATA7_GPIO    (31)
#elif CONFIG_IDF_TARGET_ESP32H2
#define TEST_LCD_BK_LIGHT_GPIO (2)
#define TEST_LCD_RST_GPIO      (14)
#define TEST_LCD_CS_GPIO       (3)
#define TEST_LCD_DC_GPIO       (13)
#define TEST_LCD_PCLK_GPIO     (5)
#define TEST_LCD_DATA0_GPIO    (4)
#elif CONFIG_IDF_TARGET_ESP32C5
#define TEST_LCD_BK_LIGHT_GPIO (5)
#define TEST_LCD_RST_GPIO      (2)
#define TEST_LCD_CS_GPIO       (0)
#define TEST_LCD_DC_GPIO       (3)
#define TEST_LCD_PCLK_GPIO     (1)
#define TEST_LCD_DATA0_GPIO    (4)
#define TEST_LCD_DATA1_GPIO    (9)
#define TEST_LCD_DATA2_GPIO    (28)
#define TEST_LCD_DATA3_GPIO    (24)
#define TEST_LCD_DATA4_GPIO    (14)
#define TEST_LCD_DATA5_GPIO    (23)
#define TEST_LCD_DATA6_GPIO    (13)
#define TEST_LCD_DATA7_GPIO    (27)

#endif

#define TEST_LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)

#ifdef __cplusplus
}
#endif
