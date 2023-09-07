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
/**
 * @brief LCD color space type definition (WRONG!)
 * @deprecated RGB and BGR should belong to the same color space, but this enum take them both as two different color spaces.
 *             If you want to use a enum to describe a color space, please use lcd_color_space_t instead.
 */
typedef enum {
    ESP_LCD_COLOR_SPACE_RGB,        /*!< Color space: RGB */
    ESP_LCD_COLOR_SPACE_BGR,        /*!< Color space: BGR */
    ESP_LCD_COLOR_SPACE_MONOCHROME, /*!< Color space: monochrome */
} esp_lcd_color_space_t __attribute__((deprecated));

// Ensure binary compatibility with lcd_color_rgb_endian_t
ESP_STATIC_ASSERT((lcd_rgb_element_order_t)ESP_LCD_COLOR_SPACE_RGB == LCD_RGB_ELEMENT_ORDER_RGB, "ESP_LCD_COLOR_SPACE_RGB is not compatible with LCD_RGB_ORDER_RGB");
ESP_STATIC_ASSERT((lcd_rgb_element_order_t)ESP_LCD_COLOR_SPACE_BGR == LCD_RGB_ELEMENT_ORDER_BGR, "ESP_LCD_COLOR_SPACE_BGR is not compatible with LCD_RGB_ORDER_BGR");
/** @endcond */

#ifdef __cplusplus
}
#endif
