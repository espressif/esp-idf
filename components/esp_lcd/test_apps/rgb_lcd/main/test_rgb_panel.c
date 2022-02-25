/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_system.h"
#include "test_rgb_board.h"

void test_app_include_rgb_lcd(void)
{
}

TEST_CASE("lcd_rgb_lcd_panel", "[lcd]")
{
#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .psram_trans_align = 64,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .disp_gpio_num = TEST_LCD_DISP_EN_GPIO,
        .pclk_gpio_num = TEST_LCD_PCLK_GPIO,
        .vsync_gpio_num = TEST_LCD_VSYNC_GPIO,
        .hsync_gpio_num = TEST_LCD_HSYNC_GPIO,
        .de_gpio_num = TEST_LCD_DE_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
            TEST_LCD_DATA8_GPIO,
            TEST_LCD_DATA9_GPIO,
            TEST_LCD_DATA10_GPIO,
            TEST_LCD_DATA11_GPIO,
            TEST_LCD_DATA12_GPIO,
            TEST_LCD_DATA13_GPIO,
            TEST_LCD_DATA14_GPIO,
            TEST_LCD_DATA15_GPIO,
        },
        .timings = {
            .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
            .h_res = TEST_LCD_H_RES,
            .v_res = TEST_LCD_V_RES,
            .hsync_back_porch = 68,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 5,
            .vsync_back_porch = 18,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
            .flags.pclk_active_neg = 1, // RGB data is clocked out on falling edge
        },
        .flags.fb_in_psram = 1, // allocate frame buffer in PSRAM
    };
    // Test stream mode and one-off mode
    for (int i = 0; i < 2; i++) {
        panel_config.flags.relax_on_idle = i;
        TEST_ESP_OK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
        TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));
        TEST_ESP_OK(esp_lcd_panel_init(panel_handle));

        for (int i = 0; i < 200; i++) {
            uint8_t color_byte = esp_random() & 0xFF;
            int x_start = esp_random() % (TEST_LCD_H_RES - 100);
            int y_start = esp_random() % (TEST_LCD_V_RES - 100);
            memset(img, color_byte, TEST_IMG_SIZE);
            esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        }

        TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    }
    free(img);
#undef TEST_IMG_SIZE
}
