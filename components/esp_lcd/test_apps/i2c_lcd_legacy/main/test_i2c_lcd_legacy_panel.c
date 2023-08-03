/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_system.h"
#include "test_i2c_board.h"

TEST_CASE("lcd_panel_with_i2c_interface legacy_(ssd1306)", "[lcd]")
{
    const uint8_t pattern[][16] = {{
            0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00,
            0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00
        },
        {
            0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81,
            0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81
        }
    };

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = TEST_I2C_SDA_GPIO,
        .scl_io_num = TEST_I2C_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = TEST_LCD_PIXEL_CLOCK_HZ,
    };
    TEST_ESP_OK(i2c_param_config(TEST_I2C_HOST_ID, &conf));
    TEST_ESP_OK(i2c_driver_install(TEST_I2C_HOST_ID, I2C_MODE_MASTER, 0, 0, 0));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = TEST_I2C_DEV_ADDR,
        .control_phase_bytes = 1, // According to SSD1306 datasheet
        .dc_bit_offset = 6,       // According to SSD1306 datasheet
        .lcd_cmd_bits = 8,        // According to SSD1306 datasheet
        .lcd_param_bits = 8,      // According to SSD1306 datasheet
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)TEST_I2C_HOST_ID, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_init(panel_handle));
    // turn on display
    TEST_ESP_OK(esp_lcd_panel_disp_on_off(panel_handle, true));

    for (int i = 0; i < TEST_LCD_H_RES / 16; i++) {
        for (int j = 0; j < TEST_LCD_V_RES / 8; j++) {
            TEST_ESP_OK(esp_lcd_panel_draw_bitmap(panel_handle, i * 16, j * 8, i * 16 + 16, j * 8 + 8, pattern[i & 0x01]));
        }
    }

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(i2c_driver_delete(TEST_I2C_HOST_ID));
}
