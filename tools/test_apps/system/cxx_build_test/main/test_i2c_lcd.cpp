/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"
#include "driver/i2c_master.h"

const esp_lcd_panel_dev_config_t panel_config0 = {
    .reset_gpio_num = 0,
    .color_space = ESP_LCD_COLOR_SPACE_MONOCHROME,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    .bits_per_pixel = 16,
    .flags = {
        .reset_active_high = false,
    },
    .vendor_config = NULL,
};

const esp_lcd_panel_dev_config_t panel_config1 = {
    .reset_gpio_num = 0,
    .color_space = ESP_LCD_COLOR_SPACE_BGR,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    .bits_per_pixel = 16,
    .flags = {
        .reset_active_high = false,
    },
    .vendor_config = NULL,
};

const esp_lcd_panel_dev_config_t panel_config2 = {
    .reset_gpio_num = 0,
    .rgb_endian = LCD_RGB_ENDIAN_BGR,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    .bits_per_pixel = 16,
    .flags = {
        .reset_active_high = false,
    },
    .vendor_config = NULL,
};

void test_i2c_lcd_apis(void)
{
    i2c_master_bus_config_t i2c_bus_conf = {
        .i2c_port = -1,
        .sda_io_num = GPIO_NUM_0,
        .scl_io_num = GPIO_NUM_2,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 0,
        .intr_priority = 1,
        .trans_queue_depth = 4,
        .flags = {
            .enable_internal_pullup = true,
            .allow_pd = false,
        }
    };

    i2c_master_bus_handle_t bus_handle;
    i2c_new_master_bus(&i2c_bus_conf, &bus_handle);

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x3c,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
        .control_phase_bytes = 1,
        .dc_bit_offset = 6,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .flags = {
            .dc_low_on_data = false,
            .disable_control_phase = false,
        },
        .scl_speed_hz = 10 * 1000,
    };

    esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle);
}
