/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_assert.h"
#include "hal/lcd_types.h"
#include "hal/gpio_types.h"
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
 * @brief Configuration of LCD color conversion: YUV <-> RGB
 */
typedef struct {
    lcd_color_range_t in_color_range;  /*!< Color range of the input color */
    lcd_color_range_t out_color_range; /*!< Color range of the output color */
    lcd_yuv_conv_std_t conv_std;       /*!< YUV conversion standard: BT601, BT709 */
} esp_lcd_color_conv_yuv_config_t;

#ifdef __cplusplus
}
#endif
