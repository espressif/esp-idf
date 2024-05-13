/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_random.h"
#include "esp_attr.h"
#include "test_rgb_board.h"

#define TEST_IMG_SIZE (320 * 320 * sizeof(uint16_t))

// YUV images are embedded in the firmware binary
extern const uint8_t image_hello_yuv_start[] asm("_binary_hello_yuv_start");
extern const uint8_t image_hello_yuv_end[]   asm("_binary_hello_yuv_end");
extern const uint8_t image_world_yuv_start[] asm("_binary_world_yuv_start");
extern const uint8_t image_world_yuv_end[]   asm("_binary_world_yuv_end");

TEST_CASE("lcd_rgb_panel_yuv422_conversion", "[lcd]")
{
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .dma_burst_size = 64,
        .bits_per_pixel = 16, // YUV422: 16bits per pixel
        .clk_src = LCD_CLK_SRC_DEFAULT,
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
        },
        .flags.fb_in_psram = 1, // allocate frame buffer in PSRAM
    };

    printf("Create RGB LCD panel\r\n");
    TEST_ESP_OK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));

    printf("Set YUV-RGB conversion profile\r\n");
    esp_lcd_yuv_conv_config_t conv_config = {
        .std = LCD_YUV_CONV_STD_BT601,
        .src = {
            .color_range = LCD_COLOR_RANGE_FULL,
            .color_space = LCD_COLOR_SPACE_RGB,
        },
        .dst = {
            .color_range = LCD_COLOR_RANGE_FULL,
            .color_space = LCD_COLOR_SPACE_RGB,
        },
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_lcd_rgb_panel_set_yuv_conversion(panel_handle, &conv_config));

    conv_config.src.color_space = LCD_COLOR_SPACE_YUV;
    conv_config.src.yuv_sample = LCD_YUV_SAMPLE_422;
    TEST_ESP_OK(esp_lcd_rgb_panel_set_yuv_conversion(panel_handle, &conv_config));

    TEST_ESP_OK(esp_lcd_panel_init(panel_handle));

    printf("Draw YUV images\r\n");
    for (int i = 0; i < 4; i++) {
        TEST_ESP_OK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 320, 320, image_hello_yuv_start));
        vTaskDelay(pdMS_TO_TICKS(1000));
        TEST_ESP_OK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 320, 320, image_world_yuv_start));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
}
