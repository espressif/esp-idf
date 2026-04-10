/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "driver/parlio_tx.h"
#include "test_parlio_board.h"

#define TEST_SPI_DATA_WIDTH 1
#define TEST_I80_DATA_WIDTH 8

#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))
static void lcd_parlio_panel_with_st7789_interface(esp_lcd_panel_io_handle_t io_handle)
{
    uint8_t *img = heap_caps_calloc(1, TEST_IMG_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(img);

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    TEST_ESP_OK(gpio_config(&bk_gpio_config));

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

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(gpio_reset_pin(TEST_LCD_BK_LIGHT_GPIO));
    free(img);
}

TEST_CASE("lcd_panel_simulate_SPI_interface(st7789)", "[lcd]")
{
    esp_lcd_panel_io_parl_config_t io_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .clk_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
        },
        .data_width = TEST_SPI_DATA_WIDTH,
        .max_transfer_bytes = TEST_IMG_SIZE,
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_cmd_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    esp_lcd_panel_io_handle_t spi_io_handle = NULL;
    TEST_ESP_OK(esp_lcd_new_panel_io_parl(&io_config, &spi_io_handle));
    esp_lcd_panel_io_handle_t another_io_handle = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, esp_lcd_new_panel_io_parl(&io_config, &another_io_handle));
    lcd_parlio_panel_with_st7789_interface(spi_io_handle);
}

#if SOC_PARLIO_SUPPORT_I80_LCD
TEST_CASE("lcd_panel_simulate_I80_interface(st7789)", "[lcd]")
{
    esp_lcd_panel_io_parl_config_t io_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .clk_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
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
        .data_width = TEST_I80_DATA_WIDTH,
        .max_transfer_bytes = TEST_IMG_SIZE,
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_cmd_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    esp_lcd_panel_io_handle_t i80_io_handle = NULL;
    TEST_ESP_OK(esp_lcd_new_panel_io_parl(&io_config, &i80_io_handle));
    lcd_parlio_panel_with_st7789_interface(i80_io_handle);
}
#endif
#undef TEST_IMG_SIZE

static bool on_color_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    uint32_t *isr_counter = (uint32_t *)user_ctx;
    (*isr_counter)++;
    return false;
}

static void lcd_parlio_send_colors_to_fixed_region(size_t data_width)
{
    int x_start = 100;
    int y_start = 100;
    int x_end = 200;
    int y_end = 200;
    size_t color_size = (x_end - x_start) * (y_end - y_start) * 2;
    void *color_data = heap_caps_calloc(1, color_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(color_data);
    uint8_t color_byte = esp_random() & 0xFF;
    memset(color_data, color_byte, color_size);
    uint32_t isr_counter = 0;
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_parl_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_cmd_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .clk_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
#if SOC_PARLIO_SUPPORT_I80_LCD
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
#endif
        },
        .data_width = data_width,
        .max_transfer_bytes = color_size * 2,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_parl(&io_config, &io_handle));

    printf("Register io panel event callback");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = on_color_trans_done,
    };
    /* Register done callback */
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, &isr_counter));

    printf("creating LCD panel\r\n");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // we don't use the panel handle in this test, creating the panel just for a quick initialization
    printf("initialize LCD panel\r\n");
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

    printf("set the flush window for only once\r\n");
    esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_RAMWR, NULL, 0);

    printf("send colors to the fixed region in multiple steps\r\n");
    const int steps = 10;
    int color_size_per_step = color_size / steps;
    for (int i = 0; i < steps; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_tx_color(io_handle, -1, color_data + i * color_size_per_step, color_size_per_step));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ASSERT_EQUAL(steps, isr_counter);
    // change to another color
    color_byte = esp_random() & 0xFF;
    memset(color_data, color_byte, color_size);
    for (int i = 0; i < steps; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_tx_color(io_handle, -1, color_data + i * color_size_per_step, color_size_per_step));
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL(steps * 2, isr_counter);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    free(color_data);
}

TEST_CASE("lcd_parlio_send_colors_to_fixed_region(SPI)", "[lcd]")
{
    lcd_parlio_send_colors_to_fixed_region(TEST_SPI_DATA_WIDTH);
}

#if SOC_PARLIO_SUPPORT_I80_LCD
TEST_CASE("lcd_parlio_send_colors_to_fixed_region(I80)", "[lcd]")
{
    lcd_parlio_send_colors_to_fixed_region(TEST_I80_DATA_WIDTH);
}
#endif
