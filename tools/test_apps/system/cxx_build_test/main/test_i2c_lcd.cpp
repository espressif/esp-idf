/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_lcd_panel_vendor.h"

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
