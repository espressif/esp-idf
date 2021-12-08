/*
 * AliGenie - Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"

#include "iot_button.h"
#include "light_driver.h"

#include "genie_event.h"

#define BUTTON_ON_OFF          0   /* on/off button */
#define BUTTON_ACTIVE_LEVEL    0

static const char *TAG  = "board";

static uint32_t dev_on_btn_num = BUTTON_ON_OFF;

extern void user_genie_event_handle(genie_event_t event, void *p_arg);

void button_tap_cb(void* arg)
{
    user_genie_event_handle(GENIE_EVT_BUTTON_TAP, NULL);
}

static void board_led_init(void)
{
    /**
     * NOTE:
     *  If the module has SPI flash, GPIOs 6-11 are connected to the module’s integrated SPI flash and PSRAM.
     *  If the module has PSRAM, GPIOs 16 and 17 are connected to the module’s integrated PSRAM.
     */
    light_driver_config_t driver_config = {
        .gpio_red        = CONFIG_LIGHT_GPIO_RED,
        .gpio_green      = CONFIG_LIGHT_GPIO_GREEN,
        .gpio_blue       = CONFIG_LIGHT_GPIO_BLUE,
        .gpio_cold       = CONFIG_LIGHT_GPIO_COLD,
        .gpio_warm       = CONFIG_LIGHT_GPIO_WARM,
        .fade_period_ms  = CONFIG_LIGHT_FADE_PERIOD_MS,
        .blink_period_ms = CONFIG_LIGHT_BLINK_PERIOD_MS,
    };

    /**
     * @brief Light driver initialization
     */
    ESP_ERROR_CHECK(light_driver_init(&driver_config));
    light_driver_set_mode(MODE_HSL);
    // light_driver_set_switch(true);

    button_handle_t dev_on_off_btn = iot_button_create(BUTTON_ON_OFF, BUTTON_ACTIVE_LEVEL);
    iot_button_set_evt_cb(dev_on_off_btn, BUTTON_CB_TAP, button_tap_cb, &dev_on_btn_num);
}

void board_init(void)
{
    board_led_init();
}

/**
 * hsl
 */
void board_led_hsl(uint8_t elem_index, uint16_t hue, uint16_t saturation, uint16_t lightness)
{
    static uint16_t last_hue        = 0xFFFF;
    static uint16_t last_saturation = 0xFFFF;
    static uint16_t last_lightness  = 0xFFFF;

    ESP_LOGD(TAG, "hue last state %d, state %d", last_hue, hue);
    ESP_LOGD(TAG, "saturation last state %d, state %d", last_saturation, saturation);
    ESP_LOGD(TAG, "lightness last state %d, state %d", last_lightness, lightness);

    if(last_hue != hue || last_saturation != saturation || last_lightness != lightness ) {
        last_hue        = hue;
        last_saturation = saturation;
        last_lightness  = lightness;

        uint16_t actual_hue        = (float)last_hue / (UINT16_MAX / 360.0);
        uint8_t  actual_saturation = (float)last_saturation / (UINT16_MAX / 100.0);
        uint8_t  actual_lightness  = (float)last_lightness / (UINT16_MAX / 100.0);

        ESP_LOGD(TAG, "hsl: %d, %d, %d operation", actual_hue, actual_saturation, actual_lightness);
        light_driver_set_hsl(actual_hue, actual_saturation, actual_lightness);
    }
}

/**
 * temperature light temp
 */
void board_led_temperature(uint8_t elem_index, uint16_t temperature)
{
    static uint16_t last_temperature = 0xFFFF;

    ESP_LOGD(TAG, "temperature last state %d, state %d", last_temperature, temperature);

    if(last_temperature != temperature) {
        last_temperature = temperature;

        uint16_t actual_temperature = (float)last_temperature / (UINT16_MAX / 100.0);
        ESP_LOGD(TAG, "temperature %d %%%d operation", last_temperature, actual_temperature);
        light_driver_set_color_temperature(actual_temperature);
    }
}

/**
 * actual lightness
 */
void board_led_lightness(uint8_t elem_index, uint16_t actual)
{
    static uint16_t last_acual = 0xFFFF;

    ESP_LOGD(TAG, "actual last state %d, state %d", last_acual, actual);

    if(last_acual != actual) {
        last_acual = actual;

        uint16_t actual_lightness = (float)last_acual / (UINT16_MAX / 100.0);
        ESP_LOGD(TAG, "lightness %d %%%d operation", last_acual, actual_lightness);
        light_driver_set_lightness(actual_lightness);
    }
}

/**
 * onoff on/off
 */
void board_led_switch(uint8_t elem_index, uint8_t onoff)
{
    static uint8_t last_onoff = 0xFF;

    ESP_LOGD(TAG, "onoff last state %d, state %d", last_onoff, onoff);
    if(last_onoff != onoff) {
        last_onoff = onoff;
        if (last_onoff) {
            ESP_LOGD(TAG, "onoff %d operation", last_onoff);
            light_driver_set_switch(true);
        } else {
            ESP_LOGD(TAG, "onoff %d operation", last_onoff);
            light_driver_set_switch(false);
        }
    }
}

#define MINDIFF (2.25e-308)

static float bt_mesh_sqrt(float square)
{
    float root, last, diff;

    root = square / 3.0;
    diff = 1;

    if (square <= 0) {
        return 0;
    }

    do {
        last = root;
        root = (root + square / root) / 2.0;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);

    return root;
}

static int32_t bt_mesh_ceiling(float num)
{
    int32_t inum = (int32_t)num;
    if (num == (float)inum) {
        return inum;
    }
    return inum + 1;
}

uint16_t convert_lightness_actual_to_linear(uint16_t actual)
{
    float tmp = ((float) actual / UINT16_MAX);
    return bt_mesh_ceiling(UINT16_MAX * tmp * tmp);
}

uint16_t convert_lightness_linear_to_actual(uint16_t linear)
{
    return (uint16_t)(UINT16_MAX * bt_mesh_sqrt(((float) linear / UINT16_MAX)));
}

int16_t convert_temperature_to_level(uint16_t temp, uint16_t min, uint16_t max)
{
    float tmp = (temp - min) * UINT16_MAX / (max - min);
    return (int16_t) (tmp + INT16_MIN);
}

uint16_t covert_level_to_temperature(int16_t level, uint16_t min, uint16_t max)
{
    float diff = (float) (max - min) / UINT16_MAX;
    uint16_t tmp = (uint16_t) ((level - INT16_MIN) * diff);
    return (uint16_t) (min + tmp);
}

/* swap octets */
void swap_buf(uint8_t *dst, const uint8_t *src, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        dst[len - 1 - i] = src[i];
    }
}

uint8_t *mac_str2hex(const char *mac_str, uint8_t *mac_hex)
{
    uint32_t mac_data[6] = {0};

    sscanf(mac_str, "%02x%02x%02x%02x%02x%02x",
           mac_data, mac_data + 1, mac_data + 2, mac_data + 3, mac_data + 4, mac_data + 5);

    for (int i = 0; i < 6; i++) {
        mac_hex[i] = mac_data[i];
    }

    return mac_hex;
}
