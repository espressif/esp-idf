/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * Zigbee light driver example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include "led_strip.h"
#include "light_driver.h"

static led_strip_handle_t s_led_strip;
static uint8_t            s_red = 255, s_green = 255, s_blue = 255, s_level = 255;

void light_driver_set_color_xy(uint16_t color_current_x, uint16_t color_current_y)
{
    float red_f = 0, green_f = 0, blue_f = 0, color_x, color_y;
    color_x = (float)color_current_x / 65535;
    color_y = (float)color_current_y / 65535;
    /* assume color_Y is full light level value 1  (0-1.0) */
    float color_X = color_x / color_y;
    float color_Z = (1 - color_x - color_y) / color_y;
    /* change from xy to linear RGB NOT sRGB */
    XYZ_to_RGB(color_X, 1, color_Z, red_f, green_f, blue_f);
    float ratio = (float)s_level / 255;
    s_red       = (uint8_t)(red_f * (float)255);
    s_green     = (uint8_t)(green_f * (float)255);
    s_blue      = (uint8_t)(blue_f * (float)255);
    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * ratio, s_green * ratio, s_blue * ratio));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_color_hue_sat(uint8_t hue, uint8_t sat)
{
    float red_f, green_f, blue_f;
    HSV_to_RGB(hue, sat, UINT8_MAX, red_f, green_f, blue_f);
    float ratio = (float)s_level / 255;
    s_red       = (uint8_t)red_f;
    s_green     = (uint8_t)green_f;
    s_blue      = (uint8_t)blue_f;
    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * ratio, s_green * ratio, s_blue * ratio));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_color_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    float ratio = (float)s_level / 255;
    s_red       = red;
    s_green     = green;
    s_blue      = blue;
    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, red * ratio, green * ratio, blue * ratio));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_power(bool power)
{
    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * power, s_green * power, s_blue * power));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_level(uint8_t level)
{
    s_level     = level;
    float ratio = (float)s_level / 255;
    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * ratio, s_green * ratio, s_blue * ratio));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_init(bool power)
{
    led_strip_config_t led_strip_conf = {
        .max_leds       = CONFIG_EXAMPLE_STRIP_LED_NUMBER,
        .strip_gpio_num = CONFIG_EXAMPLE_STRIP_LED_GPIO,
    };
    led_strip_rmt_config_t rmt_conf = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_conf, &rmt_conf, &s_led_strip));

    light_driver_set_power(power);
}
