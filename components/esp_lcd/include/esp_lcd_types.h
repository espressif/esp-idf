/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
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
/// for backward compatible
typedef lcd_rgb_element_order_t lcd_color_rgb_endian_t;
#define LCD_RGB_ENDIAN_RGB (lcd_color_rgb_endian_t)LCD_RGB_ELEMENT_ORDER_RGB
#define LCD_RGB_ENDIAN_BGR (lcd_color_rgb_endian_t)LCD_RGB_ELEMENT_ORDER_BGR

typedef lcd_rgb_element_order_t esp_lcd_color_space_t;
#define ESP_LCD_COLOR_SPACE_RGB (esp_lcd_color_space_t)LCD_RGB_ELEMENT_ORDER_RGB
#define ESP_LCD_COLOR_SPACE_BGR (esp_lcd_color_space_t)LCD_RGB_ELEMENT_ORDER_BGR
#define ESP_LCD_COLOR_SPACE_MONOCHROME (esp_lcd_color_space_t)2
/** @endcond */

/**
 * @brief Type of LCD panel IO event data
 */
typedef struct {
} esp_lcd_panel_io_event_data_t;

/**
 * @brief Declare the prototype of the function that will be invoked when panel IO finishes transferring color data
 *
 * @param[in] panel_io LCD panel IO handle, which is created by factory API like `esp_lcd_new_panel_io_spi()`
 * @param[in] edata Panel IO event data, fed by driver
 * @param[in] user_ctx User data, passed from `esp_lcd_panel_io_xxx_config_t`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_panel_io_color_trans_done_cb_t)(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);

/**
 * @brief Type of LCD panel IO callbacks
 */
typedef struct {
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data transfer has finished */
} esp_lcd_panel_io_callbacks_t;

/**
 * @brief Configuration of LCD color conversion
 */
typedef struct {
    lcd_color_range_t in_color_range;    /*!< Color range of the input color */
    lcd_color_range_t out_color_range;   /*!< Color range of the output color */
    union {
        struct {
            lcd_yuv_conv_std_t conv_std; /*!< YUV conversion standard: BT601, BT709 */
            struct {
                lcd_yuv422_pack_order_t in_pack_order; /*!< YUV422 packing order of the input color */
            } yuv422; /*!< YUV422 specific */
        } yuv; /*!< YUV specific */
    } spec; /*!< Extra configuration for specific color conversion */
} esp_lcd_color_conv_config_t;

#ifdef __cplusplus
}
#endif
