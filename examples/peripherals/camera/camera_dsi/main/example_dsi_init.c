/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9881c.h"
#include "esp_ldo_regulator.h"
#include "esp_cache.h"
#include "example_dsi_init.h"
#include "example_config.h"

void example_dsi_resource_alloc(esp_lcd_panel_handle_t *ili9881c_ctrl_panel, esp_lcd_panel_handle_t *mipi_dpi_panel, void **frame_buffer)
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;

    //---------------DSI resource allocation------------------//
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = -1,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, ili9881c_ctrl_panel));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = 80,
        .virtual_channel = 0,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .video_timing = {
            .h_size = EXAMPLE_MIPI_DSI_IMAGE_HSIZE,
            .v_size = EXAMPLE_MIPI_DSI_IMAGE_VSIZE,
            .hsync_back_porch = EXAMPLE_MIPI_DSI_IMAGE_HBP,
            .hsync_pulse_width = EXAMPLE_MIPI_DSI_IMAGE_HSYNC,
            .hsync_front_porch = EXAMPLE_MIPI_DSI_IMAGE_HFP,
            .vsync_back_porch = EXAMPLE_MIPI_DSI_IMAGE_VBP,
            .vsync_pulse_width = EXAMPLE_MIPI_DSI_IMAGE_VSYNC,
            .vsync_front_porch = EXAMPLE_MIPI_DSI_IMAGE_VFP,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, mipi_dpi_panel));
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_get_frame_buffer(*mipi_dpi_panel, 1, frame_buffer));
}

void example_dsi_ili9881c_panel_init(esp_lcd_panel_handle_t ili9881c_ctrl_panel)
{
    //---------------DSI Panel Init------------------//
    ESP_ERROR_CHECK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(ili9881c_ctrl_panel));
    // turn on display
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));
}

void example_dpi_panel_init(esp_lcd_panel_handle_t mipi_dpi_panel)
{
    //---------------DPI Panel Init------------------//
    ESP_ERROR_CHECK(esp_lcd_panel_init(mipi_dpi_panel));
}
