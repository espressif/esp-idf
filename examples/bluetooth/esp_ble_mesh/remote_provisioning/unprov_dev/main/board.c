/* board.c - Board-specific hooks */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "lightbulb.h"
#include "esp_log.h"
#include "board.h"

#define TAG "BOARD"

void board_led_operation(uint8_t r, uint8_t g, uint8_t b)
{
#ifdef BLE_MESH_LED_STRIP_IO
    ws2812_set_rgb_channel(r, g, b);
#else
    gpio_set_level(LED_R, r);
    gpio_set_level(LED_G, g);
    gpio_set_level(LED_B, b);
#endif
}

static void board_led_init(void)
{
#ifdef BLE_MESH_LED_STRIP_IO
    lightbulb_config_t config = {
        .type = DRIVER_WS2812,
        .driver_conf.ws2812.led_num = 3,
        .driver_conf.ws2812.ctrl_io = 8,
        .capability.enable_fades = true,
        .capability.fades_ms = 800,
        .capability.enable_status_storage = false,
        .capability.mode_mask = COLOR_MODE,
        .capability.storage_cb = NULL,
        .external_limit = NULL,
        .gamma_conf = NULL,
        .init_status.mode = WORK_COLOR,
        .init_status.on = false,
        .init_status.hue = 0,
        .init_status.saturation = 100,
        .init_status.value = 100,
    };
    lightbulb_init(&config);
    ws2812_set_rgb_channel(LED_OFF, LED_OFF, LED_OFF);
#else
    gpio_set_level(LED_R, LED_OFF);
    gpio_set_level(LED_G, LED_OFF);
    gpio_set_level(LED_B, LED_OFF);
#endif
}

void board_init(void)
{
    board_led_init();
}
