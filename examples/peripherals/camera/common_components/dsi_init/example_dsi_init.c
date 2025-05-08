/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_ili9881c.h"
#include "esp_lcd_ek79007.h"
#include "example_dsi_init.h"
#include "example_dsi_init_config.h"
#include "sdkconfig.h"

void example_dsi_resource_alloc(esp_lcd_dsi_bus_handle_t *mipi_dsi_bus, esp_lcd_panel_io_handle_t *mipi_dbi_io, esp_lcd_panel_handle_t *mipi_dpi_panel, void **frame_buffer)
{
    //---------------DSI resource allocation------------------//
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(*mipi_dsi_bus, &dbi_config, mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = EXAMPLE_MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .video_timing = {
            .h_size = CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES,
            .v_size = CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES,
            .hsync_back_porch = EXAMPLE_MIPI_DSI_IMAGE_HBP,
            .hsync_pulse_width = EXAMPLE_MIPI_DSI_IMAGE_HSYNC,
            .hsync_front_porch = EXAMPLE_MIPI_DSI_IMAGE_HFP,
            .vsync_back_porch = EXAMPLE_MIPI_DSI_IMAGE_VBP,
            .vsync_pulse_width = EXAMPLE_MIPI_DSI_IMAGE_VSYNC,
            .vsync_front_porch = EXAMPLE_MIPI_DSI_IMAGE_VFP,
        },
#if CONFIG_EXAMPLE_MIPI_DSI_DISP_USE_DMA2D
        .flags.use_dma2d = true,
#endif
    };

#if CONFIG_EXAMPLE_LCD_PATTERN_ILI9881C
    ili9881c_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = *mipi_dsi_bus,
            .dpi_config = &dpi_config,
            .lane_num = 2,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(*mipi_dbi_io, &lcd_dev_config, mipi_dpi_panel));
#elif CONFIG_EXAMPLE_LCD_PATTERN_EK79007
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = *mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ek79007(*mipi_dbi_io, &lcd_dev_config, mipi_dpi_panel));
#endif

    ESP_ERROR_CHECK(esp_lcd_dpi_panel_get_frame_buffer(*mipi_dpi_panel, 1, frame_buffer));
}

void example_dpi_panel_reset(esp_lcd_panel_handle_t mipi_dpi_panel)
{
    //---------------DPI Panel Reset------------------//
    ESP_ERROR_CHECK(esp_lcd_panel_reset(mipi_dpi_panel));
}

void example_dpi_panel_init(esp_lcd_panel_handle_t mipi_dpi_panel)
{
    //---------------DPI Panel Init------------------//
    ESP_ERROR_CHECK(esp_lcd_panel_init(mipi_dpi_panel));
}

void example_dsi_resource_destroy(esp_lcd_dsi_bus_handle_t mipi_dsi_bus, esp_lcd_panel_io_handle_t mipi_dbi_io, esp_lcd_panel_handle_t mipi_dpi_panel)
{
    ESP_ERROR_CHECK(esp_lcd_panel_del(mipi_dpi_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_io_del(mipi_dbi_io));
    ESP_ERROR_CHECK(esp_lcd_del_dsi_bus(mipi_dsi_bus));
}
