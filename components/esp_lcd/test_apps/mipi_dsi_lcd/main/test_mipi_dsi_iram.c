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
#include "unity_test_utils.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_random.h"
#include "esp_attr.h"
#include "test_mipi_dsi_board.h"
#include "esp_lcd_ili9881c.h"

IRAM_ATTR static bool test_rgb_panel_count_in_callback(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    uint32_t *count = (uint32_t *)user_ctx;
    *count = *count + 1;
    return false;
}

static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(200000);
}

#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))

TEST_CASE("MIPI DSI draw bitmap (ILI9881C) IRAM Safe", "[mipi_dsi]")
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
    uint32_t callback_calls = 0;
    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_refresh_done = test_rgb_panel_count_in_callback,
    };
    TEST_ESP_OK(esp_lcd_dpi_panel_register_event_callbacks(mipi_dpi_panel, &cbs, &callback_calls));

    uint8_t color_byte = rand() & 0xFF;
    int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
    int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
    memset(img, color_byte, TEST_IMG_SIZE);
    esp_lcd_panel_draw_bitmap(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, img);
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("The LCD driver should keep flushing the color block in the background\r\n");

    // disable the cache for a while, the LCD driver should stay working
    printf("disable the cache for a while\r\n");
    unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    printf("callback calls: %"PRIu32"\r\n", callback_calls);
    TEST_ASSERT(callback_calls > 2);

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_del(ili9881c_ctrl_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));
    free(img);

    test_bsp_disable_dsi_phy_power();
}
