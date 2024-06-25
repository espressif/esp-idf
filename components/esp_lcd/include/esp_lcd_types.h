/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "hal/lcd_types.h"
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_panel_io_t *esp_lcd_panel_io_handle_t; /*!< Type of LCD panel IO handle */
typedef struct esp_lcd_panel_t *esp_lcd_panel_handle_t;       /*!< Type of LCD panel handle */

/** @cond */
/// for backward compatible
typedef lcd_rgb_element_order_t lcd_color_rgb_endian_t;
#define LCD_RGB_ENDIAN_RGB (lcd_color_rgb_endian_t)LCD_RGB_ELEMENT_ORDER_RGB
#define LCD_RGB_ENDIAN_BGR (lcd_color_rgb_endian_t)LCD_RGB_ELEMENT_ORDER_BGR

typedef lcd_rgb_element_order_t esp_lcd_color_space_t;
#define ESP_LCD_COLOR_SPACE_RGB (esp_lcd_color_space_t)LCD_RGB_ELEMENT_ORDER_RGB
#define ESP_LCD_COLOR_SPACE_BGR (esp_lcd_color_space_t)LCD_RGB_ELEMENT_ORDER_BGR
#define ESP_LCD_COLOR_SPACE_MONOCHROME (esp_lcd_color_space_t)2
/** @endcond */

#ifdef __cplusplus
}
#endif
