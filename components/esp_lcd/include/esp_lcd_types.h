/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_assert.h"
#include "esp_err.h"
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

/**
 * @brief Type of draw bitmap hook data
 */
typedef struct {
    void *dst_data;       /*!< Destination buffer (usually frame buffer) */
    int dst_x_size;       /*!< Destination bitmap width */
    int dst_y_size;       /*!< Destination bitmap height */
    int dst_x_start;      /*!< Destination start x coordinate */
    int dst_y_start;      /*!< Destination start y coordinate */
    int dst_x_end;        /*!< Destination end x coordinate (exclusive) */
    int dst_y_end;        /*!< Destination end y coordinate (exclusive) */
    const void *src_data; /*!< Source bitmap data */
    int src_x_size;       /*!< Source bitmap width */
    int src_y_size;       /*!< Source bitmap height */
    int src_x_start;      /*!< Source start x coordinate */
    int src_y_start;      /*!< Source start y coordinate */
    int src_x_end;        /*!< Source end x coordinate (exclusive) */
    int src_y_end;        /*!< Source end y coordinate (exclusive) */
    int bits_per_pixel;   /*!< Bits per pixel */
    bool (*on_hook_end)(esp_lcd_panel_handle_t panel); /*!< Callback to be invoked by an asynchronous hook after the custom draw
                                                            operation completes. This notifies the panel driver to finish the draw
                                                            transaction. If a color transfer done callback has been registered, it
                                                            also invokes that callback */
} esp_lcd_draw_bitmap_hook_data_t;

/**
 * @brief draw bitmap hook function type for custom pixel processing operations
 *
 * This hook allows users to implement custom operations like scaling, rotation,
 * color space conversion, etc. using hardware accelerators like PPA or DMA2D.
 *
 * @note For asynchronous operations, the hook should call hook_data->on_hook_end() after the operation is complete.
 *       The panel driver does not wait for a previous draw to finish; the hook must handle synchronization itself.
 *       The simplest approach is to serialize draws. To queue multiple transactions (e.g. via PPA), keep per-transaction
 *       hook_data, keep source buffers valid until completion, and handle overlapping destinations carefully.
 *
 * @param[in] panel LCD panel handle
 * @param[in] hook_data Hook data
 * @param[in] hook_ctx Hook context
 * @return
 *          - ESP_OK on success
 *          - Other error codes on failure
 */
typedef esp_err_t (*esp_lcd_panel_draw_bitmap_hook_t)(esp_lcd_panel_handle_t panel, const esp_lcd_draw_bitmap_hook_data_t *hook_data, void* hook_ctx);

/**
 * @brief Type of LCD panel hooks
 */
typedef struct {
    esp_lcd_panel_draw_bitmap_hook_t draw_bitmap_hook; /*!< Draw bitmap hook function */
} esp_lcd_panel_hooks_t;

#ifdef __cplusplus
}
#endif
