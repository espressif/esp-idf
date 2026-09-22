/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mount the LittleFS partition used to store the LVGL image files
 */
void example_init_filesystem(void);

/**
 * @brief Initialize the LVGL library on top of the given i80 LCD panel
 *
 * This creates a LVGL display, registers the flush callback, the tick timer and
 * the dedicated LVGL task, and finally starts the demo UI.
 *
 * @param io_handle     The i80 panel IO handle
 * @param panel_handle  The LCD panel handle
 * @param h_res         Horizontal resolution in pixels
 * @param v_res         Vertical resolution in pixels
 */
void example_init_lvgl(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t panel_handle, int h_res, int v_res);

#ifdef __cplusplus
}
#endif
