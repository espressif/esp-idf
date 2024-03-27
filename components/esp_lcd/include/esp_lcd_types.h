/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_assert.h"
#include "hal/lcd_types.h"
#include "hal/mipi_dsi_types.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timing parameters for the video data transmission
 */
typedef struct {
    uint32_t h_size;            /*!< Horizontal resolution, i.e. the number of pixels in a line */
    uint32_t v_size;            /*!< Vertical resolution, i.e. the number of lines in the frame  */
    uint32_t hsync_pulse_width; /*!< Horizontal sync width, in pixel clock */
    uint32_t hsync_back_porch;  /*!< Horizontal back porch, number of pixel clock between hsync and start of line active data */
    uint32_t hsync_front_porch; /*!< Horizontal front porch, number of pixel clock between the end of active data and the next hsync */
    uint32_t vsync_pulse_width; /*!< Vertical sync width, in number of lines */
    uint32_t vsync_back_porch;  /*!< Vertical back porch, number of invalid lines between vsync and start of frame */
    uint32_t vsync_front_porch; /*!< Vertical front porch, number of invalid lines between the end of frame and the next vsync */
} esp_lcd_video_timing_t;

typedef struct esp_lcd_panel_io_t *esp_lcd_panel_io_handle_t; /*!< Type of LCD panel IO handle */
typedef struct esp_lcd_panel_t *esp_lcd_panel_handle_t;       /*!< Type of LCD panel handle */

/**
 * @brief RGB element order
 */
typedef enum {
    LCD_RGB_ELEMENT_ORDER_RGB = COLOR_RGB_ELEMENT_ORDER_RGB, /*!< RGB element order: RGB */
    LCD_RGB_ELEMENT_ORDER_BGR = COLOR_RGB_ELEMENT_ORDER_BGR, /*!< RGB element order: BGR */
} lcd_rgb_element_order_t;

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

/// for backward compatible
typedef lcd_rgb_element_order_t lcd_color_rgb_endian_t;
#define LCD_RGB_ENDIAN_RGB LCD_RGB_ELEMENT_ORDER_RGB
#define LCD_RGB_ENDIAN_BGR LCD_RGB_ELEMENT_ORDER_BGR
/** @endcond */

#ifdef __cplusplus
}
#endif
