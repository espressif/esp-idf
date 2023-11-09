/* board.c - Board-specific hooks */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "board.h"
#include "led_strip.h"

#define TAG "BOARD"

struct _led_state led_state[3] = {
    { LED_OFF, LED_OFF, LED_R, "red"   },
    { LED_OFF, LED_OFF, LED_G, "green" },
    { LED_OFF, LED_OFF, LED_B, "blue"  },
};

#if defined(CONFIG_BLE_MESH_ESP32C3_DEV) || defined(CONFIG_BLE_MESH_ESP32S3_DEV)
static led_strip_t *pStrip_a;

void board_led_operation(uint8_t color_type, uint8_t onoff)
{
    uint32_t color_R =  led_state[0].previous ? 16 : 0;
    uint32_t color_G =  led_state[1].previous ? 16 : 0;
    uint32_t color_B =  led_state[2].previous ? 16 : 0;

    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    if(color_type == LED_COLOR_R)
    {
        color_R =  onoff ? 16 : 0;
    }
    else if(color_type == LED_COLOR_G)
    {
        color_G =  onoff ? 16 : 0;
    }
    else if(color_type == LED_COLOR_B)
    {
        color_B =  onoff ? 16 : 0;
    }
    else
        return;

    led_state[color_type].previous = onoff;
    pStrip_a->set_pixel(pStrip_a, 0, color_R, color_G, color_B);
        
    /* Refresh the strip to send data */
    pStrip_a->refresh(pStrip_a, 100);
}

void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(0, LED_G, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}
#else
void board_led_operation(uint8_t pin, uint8_t onoff)
{
    for (int i = 0; i < 3; i++) {
        if (led_state[i].pin != pin) {
            continue;
        }
        if (onoff == led_state[i].previous) {
            ESP_LOGW(TAG, "led %s is already %s",
                     led_state[i].name, (onoff ? "on" : "off"));
            return;
        }
        gpio_set_level(pin, onoff);
        led_state[i].previous = onoff;
        return;
    }

    ESP_LOGE(TAG, "LED is not found!");
}
#endif


static void board_led_init(void)
{
    for (int i = 0; i < 3; i++) {
        gpio_reset_pin(led_state[i].pin);
        gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
#if !defined(CONFIG_BLE_MESH_ESP32C3_DEV) && !defined(CONFIG_BLE_MESH_ESP32S3_DEV)
        gpio_set_level(led_state[i].pin, LED_OFF);
#endif
        led_state[i].previous = LED_OFF;
    }
#if defined(CONFIG_BLE_MESH_ESP32C3_DEV) || defined(CONFIG_BLE_MESH_ESP32S3_DEV)
    configure_led();
    board_led_operation(LED_COLOR_R, LED_OFF);
    board_led_operation(LED_COLOR_G, LED_OFF);
    board_led_operation(LED_COLOR_B, LED_OFF);
#endif
}

void board_init(void)
{
    board_led_init();
}
