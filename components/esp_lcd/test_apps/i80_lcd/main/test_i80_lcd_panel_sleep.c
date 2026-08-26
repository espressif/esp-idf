/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_random.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "test_i80_board.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

static void test_i80_lcd_sleep_retention(bool allow_pd)
{
#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))
    uint8_t *img = heap_caps_malloc(TEST_IMG_SIZE, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(img);

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    TEST_ESP_OK(gpio_config(&bk_gpio_config));

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_IMG_SIZE + 10,
        .flags = {
            .allow_pd = allow_pd,
        },
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 8000000, // 8MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // turn off backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 20);
    // turn on display
    esp_lcd_panel_disp_on_off(panel_handle, true);
    // turn on backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 1);

    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    }
    // turn off screen
    esp_lcd_panel_disp_on_off(panel_handle, false);

    // go to sleep
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 2 seconds\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if I80 LCD driver can still work...\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_LCDCAM_LCD_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    // turn on screen
    esp_lcd_panel_disp_on_off(panel_handle, true);

    printf("Testing I80 LCD after sleep...\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    }

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
    TEST_ESP_OK(gpio_reset_pin(TEST_LCD_BK_LIGHT_GPIO));
    free(img);
#undef TEST_IMG_SIZE
}

TEST_CASE("i80_lcd light sleep", "[lcd]")
{
    test_i80_lcd_sleep_retention(false);
#if SOC_LCDCAM_LCD_SUPPORT_SLEEP_RETENTION
    test_i80_lcd_sleep_retention(true);
#endif
}
