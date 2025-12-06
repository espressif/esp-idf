/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_panel_t esp_lcd_panel_t;  /*!< Type of LCD panel */

/**
 * @brief LCD panel interface
 */
struct esp_lcd_panel_t {
    /**
     * @brief Reset LCD panel
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @return
     *          - ESP_OK on success
     */
    esp_err_t (*reset)(esp_lcd_panel_t *panel);

    /**
     * @brief Initialize LCD panel
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @return
     *          - ESP_OK on success
     */
    esp_err_t (*init)(esp_lcd_panel_t *panel);

    /**
     * @brief Destroy LCD panel
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @return
     *          - ESP_OK on success
     */
    esp_err_t (*del)(esp_lcd_panel_t *panel);

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
    esp_err_t (*draw_bitmap)(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);

    /**
     * @brief Draw partial bitmap on LCD panel
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] x_start Start pixel index in the target frame buffer, on x-axis (x_start is included)
     * @param[in] y_start Start pixel index in the target frame buffer, on y-axis (y_start is included)
     * @param[in] x_end End pixel index in the target frame buffer, on x-axis (x_end is not included)
     * @param[in] y_end End pixel index in the target frame buffer, on y-axis (y_end is not included)
     * @param[in] src_data Source bitmap data
     * @param[in] src_x_size Size of the source bitmap in the x-axis
     * @param[in] src_y_size Size of the source bitmap in the y-axis
     * @param[in] src_x_start Start pixel index in the source bitmap, on x-axis (src_x_start is included)
     * @param[in] src_y_start Start pixel index in the source bitmap, on y-axis (src_y_start is included)
     * @param[in] src_x_end End pixel index in the source bitmap, on x-axis (src_x_end is not included)
     * @param[in] src_y_end End pixel index in the source bitmap, on y-axis (src_y_end is not included)
     * @return
     *          - ESP_OK on success
     */
    esp_err_t (*draw_bitmap_2d)(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end,
                                const void *src_data, size_t src_x_size, size_t src_y_size, int src_x_start, int src_y_start, int src_x_end, int src_y_end);

    /**
     * @brief Mirror the LCD panel on specific axis
     *
     * @note Combine this function with `swap_xy`, one can realize screen rotatation
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] x_axis Whether the panel will be mirrored about the x_axis
     * @param[in] y_axis Whether the panel will be mirrored about the y_axis
     * @return
     *          - ESP_OK on success
     *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
     */
    esp_err_t (*mirror)(esp_lcd_panel_t *panel, bool x_axis, bool y_axis);

    /**
     * @brief Swap/Exchange x and y axis
     *
     * @note Combine this function with `mirror`, one can realize screen rotatation
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] swap_axes Whether to swap the x and y axis
     * @return
     *          - ESP_OK on success
     *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
     */
    esp_err_t (*swap_xy)(esp_lcd_panel_t *panel, bool swap_axes);

    /**
     * @brief Set extra gap in x and y axis
     *
     * @note The gap is only used for calculating the real coordinates.
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] x_gap Extra gap on x axis, in pixels
     * @param[in] y_gap Extra gap on y axis, in pixels
     * @return
     *          - ESP_OK on success
     */
    esp_err_t (*set_gap)(esp_lcd_panel_t *panel, int x_gap, int y_gap);

    /**
     * @brief Invert the color (bit 1 -> 0 for color data line, and vice versa)
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] invert_color_data Whether to invert the color data
     * @return
     *          - ESP_OK on success
     */
    esp_err_t (*invert_color)(esp_lcd_panel_t *panel, bool invert_color_data);

    /**
     * @brief Turn on or off the display
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] on_off True to turns on display, False to turns off display
     * @return
     *          - ESP_OK on success
     *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
     */
    esp_err_t (*disp_on_off)(esp_lcd_panel_t *panel, bool on_off);

    /**
     * @brief Enter or exit sleep mode
     *
     * @param[in] panel LCD panel handle, which is created by other factory API like `esp_lcd_new_panel_st7789()`
     * @param[in] sleep True to enter sleep mode, False to wake up
     * @return
     *          - ESP_OK on success
     *          - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
     */
    esp_err_t (*disp_sleep)(esp_lcd_panel_t *panel, bool sleep);

    void *user_data;    /*!< User data, used to store externally customized data */
};

#ifdef __cplusplus
}
#endif
