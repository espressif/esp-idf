/* board.c - Board-specific hooks */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "board.h"

#define TAG "BOARD"

void board_led_operation(uint8_t r, uint8_t g, uint8_t b)
{
#ifdef BLE_MESH_LED_STRIP_IO
    rmt_led_set(r, g, b);
#else
    gpio_set_level(LED_R, r);
    gpio_set_level(LED_G, g);
    gpio_set_level(LED_B, b);
#endif
}

static void board_led_init(void)
{
#ifdef BLE_MESH_LED_STRIP_IO
    rmt_encoder_init();
    rmt_led_set(LED_OFF,LED_OFF,LED_OFF);
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
