/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize LVGL on top of the HDMI DPI panel
 *
 * Creates the LVGL display, flush path, tick timer and LVGL task, then starts the demo UI.
 *
 * @param panel LCD panel handle returned from the HDMI bridge driver
 */
void example_lvgl_init(esp_lcd_panel_handle_t panel);

#ifdef __cplusplus
}
#endif
