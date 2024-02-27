/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_random.h"
#include "esp_attr.h"
#include "test_mipi_dsi_board.h"
#include "esp_lcd_ili9881c.h"

TEST_CASE("MIPI DSI Pattern Generator (ILI9881C)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;
    esp_lcd_panel_handle_t ili9881c_ctrl_panel;

    test_bsp_enable_dsi_phy_power();

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = -1,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_init(ili9881c_ctrl_panel));
    // turn on display
    TEST_ESP_OK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    TEST_ESP_OK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_BAR_HORIZONTAL));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_BAR_VERTICAL));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_BER_VERTICAL));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_NONE));

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_del(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));

    test_bsp_disable_dsi_phy_power();
}

#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))

TEST_CASE("MIPI DSI draw bitmap (ILI9881C)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;
    esp_lcd_panel_handle_t ili9881c_ctrl_panel;

    test_bsp_enable_dsi_phy_power();

    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = -1,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_init(ili9881c_ctrl_panel));
    // turn on display
    TEST_ESP_OK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    TEST_ESP_OK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    for (int i = 0; i < 100; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, img);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_del(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));
    free(img);

    test_bsp_disable_dsi_phy_power();
}

TEST_CASE("MIPI DSI with multiple frame buffers (ILI9881C)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;
    esp_lcd_panel_handle_t ili9881c_ctrl_panel;

    test_bsp_enable_dsi_phy_power();

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = -1,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_init(ili9881c_ctrl_panel));
    // turn on display
    TEST_ESP_OK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .num_fbs = 3, // the driver will allocate and maintain 3 frame buffers
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    TEST_ESP_OK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    uint16_t *fbs[3];
    TEST_ESP_OK(esp_lcd_dpi_panel_get_frame_buffer(mipi_dpi_panel, 3, (void **)&fbs[0], (void **)&fbs[1], (void **)&fbs[2]));

    for (int i = 0; i < 3; i++) {
        uint16_t color_byte = rand() & 0xFFFF;
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        for (int j = y_start; j < y_start + 100; j++) {
            for (int k = x_start; k < x_start + 100; k++) {
                fbs[i][j * MIPI_DSI_LCD_H_RES + k] = color_byte;
            }
        }
        esp_lcd_panel_draw_bitmap(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, fbs[i]);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_del(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));

    test_bsp_disable_dsi_phy_power();
}
