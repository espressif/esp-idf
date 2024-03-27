/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset LCD panel
 *
 * @note Panel reset must be called before attempting to initialize the panel using `esp_lcd_panel_init()`.
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel);

/**
 * @brief Initialize LCD panel
 *
 * @note Before calling this function, make sure the LCD panel has finished the `reset` stage by `esp_lcd_panel_reset()`.
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel);

/**
 * @brief Deinitialize the LCD panel
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel);

/**
 * @brief Draw bitmap on LCD panel
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] x_start Start pixel index in the target frame buffer, on x-axis (x_start is included)
 * @param[in] y_start Start pixel index in the target frame buffer, on y-axis (y_start is included)
 * @param[in] x_end End pixel index in the target frame buffer, on x-axis (x_end is not included)
 * @param[in] y_end End pixel index in the target frame buffer, on y-axis (y_end is not included)
 * @param[in] color_data RGB color data that will be dumped to the specific window range
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);

/**
 * @brief Mirror the LCD panel on specific axis
 *
 * @note Combined with `esp_lcd_panel_swap_xy()`, one can realize screen rotation
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] mirror_x Whether the panel will be mirrored about the x axis
 * @param[in] mirror_y Whether the panel will be mirrored about the y axis
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t panel, bool mirror_x, bool mirror_y);

/**
 * @brief Swap/Exchange x and y axis
 *
 * @note Combined with `esp_lcd_panel_mirror()`, one can realize screen rotation
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] swap_axes Whether to swap the x and y axis
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t panel, bool swap_axes);

/**
 * @brief Set extra gap in x and y axis
 *
 * The gap is the space (in pixels) between the left/top sides of the LCD panel and the first row/column respectively of the actual contents displayed.
 *
 * @note Setting a gap is useful when positioning or centering a frame that is smaller than the LCD.
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] x_gap Extra gap on x axis, in pixels
 * @param[in] y_gap Extra gap on y axis, in pixels
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t panel, int x_gap, int y_gap);

/**
 * @brief Invert the color (bit-wise invert the color data line)
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] invert_color_data Whether to invert the color data
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_lcd_panel_invert_color(esp_lcd_panel_handle_t panel, bool invert_color_data);

/**
 * @brief Turn on or off the display
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] on_off True to turns on display, False to turns off display
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool on_off);

/**
 * @brief Turn off the display
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] off Whether to turn off the screen
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t esp_lcd_panel_disp_off(esp_lcd_panel_handle_t panel, bool off)
__attribute__((deprecated("use esp_lcd_panel_disp_on_off instead")));

/**
 * @brief Enter or exit sleep mode
 *
 * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
 * @param[in] sleep True to enter sleep mode, False to wake up
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t esp_lcd_panel_disp_sleep(esp_lcd_panel_handle_t panel, bool sleep);

#ifdef __cplusplus
}
#endif
