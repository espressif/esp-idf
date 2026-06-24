/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Refresh Rate = 18000000/(1+40+20+800)/(1+10+5+480) = 42Hz
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define EXAMPLE_LCD_H_RES              800
#define EXAMPLE_LCD_V_RES              480
#define EXAMPLE_LCD_HSYNC              1
#define EXAMPLE_LCD_HBP                40
#define EXAMPLE_LCD_HFP                20
#define EXAMPLE_LCD_VSYNC              1
#define EXAMPLE_LCD_VBP                10
#define EXAMPLE_LCD_VFP                5

#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL                   1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL                  !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT                        -1
#define EXAMPLE_PIN_NUM_DISP_EN                         -1

#define EXAMPLE_PIN_NUM_HSYNC                           CONFIG_EXAMPLE_LCD_HSYNC_GPIO
#define EXAMPLE_PIN_NUM_VSYNC                           CONFIG_EXAMPLE_LCD_VSYNC_GPIO
#define EXAMPLE_PIN_NUM_DE                              CONFIG_EXAMPLE_LCD_DE_GPIO
#define EXAMPLE_PIN_NUM_PCLK                            CONFIG_EXAMPLE_LCD_PCLK_GPIO

#define EXAMPLE_PIN_NUM_DATA0                           CONFIG_EXAMPLE_LCD_DATA0_GPIO
#define EXAMPLE_PIN_NUM_DATA1                           CONFIG_EXAMPLE_LCD_DATA1_GPIO
#define EXAMPLE_PIN_NUM_DATA2                           CONFIG_EXAMPLE_LCD_DATA2_GPIO
#define EXAMPLE_PIN_NUM_DATA3                           CONFIG_EXAMPLE_LCD_DATA3_GPIO
#define EXAMPLE_PIN_NUM_DATA4                           CONFIG_EXAMPLE_LCD_DATA4_GPIO
#define EXAMPLE_PIN_NUM_DATA5                           CONFIG_EXAMPLE_LCD_DATA5_GPIO
#define EXAMPLE_PIN_NUM_DATA6                           CONFIG_EXAMPLE_LCD_DATA6_GPIO
#define EXAMPLE_PIN_NUM_DATA7                           CONFIG_EXAMPLE_LCD_DATA7_GPIO
#define EXAMPLE_PIN_NUM_DATA8                           CONFIG_EXAMPLE_LCD_DATA8_GPIO
#define EXAMPLE_PIN_NUM_DATA9                           CONFIG_EXAMPLE_LCD_DATA9_GPIO
#define EXAMPLE_PIN_NUM_DATA10                          CONFIG_EXAMPLE_LCD_DATA10_GPIO
#define EXAMPLE_PIN_NUM_DATA11                          CONFIG_EXAMPLE_LCD_DATA11_GPIO
#define EXAMPLE_PIN_NUM_DATA12                          CONFIG_EXAMPLE_LCD_DATA12_GPIO
#define EXAMPLE_PIN_NUM_DATA13                          CONFIG_EXAMPLE_LCD_DATA13_GPIO
#define EXAMPLE_PIN_NUM_DATA14                          CONFIG_EXAMPLE_LCD_DATA14_GPIO
#define EXAMPLE_PIN_NUM_DATA15                          CONFIG_EXAMPLE_LCD_DATA15_GPIO
#if CONFIG_EXAMPLE_LCD_DATA_LINES > 16
#define EXAMPLE_PIN_NUM_DATA16                          CONFIG_EXAMPLE_LCD_DATA16_GPIO
#define EXAMPLE_PIN_NUM_DATA17                          CONFIG_EXAMPLE_LCD_DATA17_GPIO
#define EXAMPLE_PIN_NUM_DATA18                          CONFIG_EXAMPLE_LCD_DATA18_GPIO
#define EXAMPLE_PIN_NUM_DATA19                          CONFIG_EXAMPLE_LCD_DATA19_GPIO
#define EXAMPLE_PIN_NUM_DATA20                          CONFIG_EXAMPLE_LCD_DATA20_GPIO
#define EXAMPLE_PIN_NUM_DATA21                          CONFIG_EXAMPLE_LCD_DATA21_GPIO
#define EXAMPLE_PIN_NUM_DATA22                          CONFIG_EXAMPLE_LCD_DATA22_GPIO
#define EXAMPLE_PIN_NUM_DATA23                          CONFIG_EXAMPLE_LCD_DATA23_GPIO
#endif

#if CONFIG_EXAMPLE_USE_DOUBLE_FB
#define EXAMPLE_RGB_PANEL_NUM_FBS                       2
#else
#define EXAMPLE_RGB_PANEL_NUM_FBS                       1
#endif

#if CONFIG_EXAMPLE_LCD_DATA_LINES_16
#define EXAMPLE_PIXEL_SIZE             2
#elif CONFIG_EXAMPLE_LCD_DATA_LINES_24
#define EXAMPLE_PIXEL_SIZE             3
#endif

esp_err_t example_rgb_panel_init_backlight(void);
void example_rgb_panel_set_backlight(bool on);
esp_err_t example_rgb_panel_new(esp_lcd_panel_handle_t *panel_handle);
esp_err_t example_rgb_panel_init(esp_lcd_panel_handle_t panel_handle);
esp_err_t example_rgb_panel_deinit(esp_lcd_panel_handle_t panel_handle);

#ifdef __cplusplus
}
#endif
