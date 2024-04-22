/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DSI init function
 *
 * @param[out] ili9881c_ctrl_panel  ILI9881C panel handle
 * @param[out] mipi_dpi_panel       MIPI DPI panel handle
 * @param[out] frame_buffer         frame buffer
 */
void example_dsi_resource_alloc(esp_lcd_panel_handle_t *ili9881c_ctrl_panel, esp_lcd_panel_handle_t *mipi_dpi_panel, void **frame_buffer);

/**
 * @brief DSI ILI9881C panel init function
 *
 * @param[in] ili9881c_ctrl_panel ILI9881C panel handle
 */
void example_dsi_ili9881c_panel_init(esp_lcd_panel_handle_t ili9881c_ctrl_panel);

/**
 * @brief DPI panel init function
 *
 * @param[in] mipi_dpi_panel MIPI DPI panel handle
 */
void example_dpi_panel_init(esp_lcd_panel_handle_t mipi_dpi_panel);

#ifdef __cplusplus
}
#endif
