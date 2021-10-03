/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_panel_io_t *esp_lcd_panel_io_handle_t; /*!< Type of LCD panel IO handle */
typedef struct esp_lcd_panel_t *esp_lcd_panel_handle_t;       /*!< Type of LCD panel handle */

/**
 * @brief LCD color space type definition
 */
typedef enum {
    ESP_LCD_COLOR_SPACE_RGB,        /*!< Color space: RGB */
    ESP_LCD_COLOR_SPACE_BGR,        /*!< Color space: BGR */
    ESP_LCD_COLOR_SPACE_MONOCHROME, /*!< Color space: monochrome */
} esp_lcd_color_space_t;

#ifdef __cplusplus
}
#endif
