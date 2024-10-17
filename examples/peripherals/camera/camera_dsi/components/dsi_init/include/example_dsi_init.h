/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
<<<<<<< HEAD
#include "esp_lcd_ili9881c.h"
=======
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DSI init function
 *
<<<<<<< HEAD
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
=======
 * @param[out] mipi_dsi_bus         MIPI DSI bus handle
 * @param[out] mipi_dbi_io          MIPI DBI io handle
 * @param[out] mipi_dpi_panel       MIPI DPI panel handle
 * @param[out] frame_buffer         frame buffer
 */
void example_dsi_resource_alloc(esp_lcd_dsi_bus_handle_t *mipi_dsi_bus, esp_lcd_panel_io_handle_t *mipi_dbi_io, esp_lcd_panel_handle_t *mipi_dpi_panel, void **frame_buffer);

/**
 * @brief DPI panel reset function
 *
 * @param[in] mipi_dpi_panel MIPI DPI panel handle
 */
void example_dpi_panel_reset(esp_lcd_panel_handle_t mipi_dpi_panel);
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

/**
 * @brief DPI panel init function
 *
 * @param[in] mipi_dpi_panel MIPI DPI panel handle
 */
void example_dpi_panel_init(esp_lcd_panel_handle_t mipi_dpi_panel);

<<<<<<< HEAD
=======
/**
 * @brief Destroy DSI related resources
 *
 * @param[in] mipi_dsi_bus         MIPI DSI bus handle
 * @param[in] mipi_dbi_io          MIPI DBI io handle
 * @param[in] mipi_dpi_panel       MIPI DPI panel handle
 */
void example_dsi_resource_destroy(esp_lcd_dsi_bus_handle_t mipi_dsi_bus, esp_lcd_panel_io_handle_t mipi_dbi_io, esp_lcd_panel_handle_t mipi_dpi_panel);

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
#ifdef __cplusplus
}
#endif
