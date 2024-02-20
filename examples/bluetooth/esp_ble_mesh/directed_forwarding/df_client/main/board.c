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

#include "iot_button.h"
#include "board.h"
#include "esp_timer.h"
#include "lightbulb.h"

#define TAG "BOARD"

#define BUTTON_ACTIVE_LEVEL     0

extern void example_ble_mesh_start_example_configuration(void);
extern void example_ble_mesh_send_gen_onoff_set(bool by_df);
extern uint8_t in_configuration_phase;
esp_timer_handle_t led_timer_hdl;

void board_led_operation(uint8_t r, uint8_t g, uint8_t b)
{
    ws2812_set_rgb_channel(r, g, b);
}

static void led_timer_callback(void* arg)
{
    board_led_operation(0,0,0);
}

static void esp_led_timer_init(void)
{
    const esp_timer_create_args_t led_timer_args = {
        .callback = &led_timer_callback,
        .arg = NULL,
        .name = "led timer",
    };

    ESP_ERROR_CHECK(esp_timer_create(&led_timer_args, &led_timer_hdl));
}

void board_led_operation_auto_close(uint8_t r, uint8_t g, uint8_t b, uint32_t ms)
{
    esp_timer_stop(led_timer_hdl);
    board_led_operation(r,g,b);
    esp_timer_start_once(led_timer_hdl, ms * 1000);
}


static void board_led_init(void)
{
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
    esp_led_timer_init();
}

static void button_tap_cb(void* arg)
{
    ESP_LOGI(TAG, "tap cb (%s)", (char *)arg);
    static bool use_df = true;
    if (in_configuration_phase) {
        example_ble_mesh_start_example_configuration();
    } else {
        example_ble_mesh_send_gen_onoff_set(use_df);
        use_df = !use_df;
    }
}

static void board_button_init(void)
{
    button_handle_t btn_handle = iot_button_create(BUTTON_IO_NUM, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_RELEASE, button_tap_cb, "RELEASE");
    }
}

void board_init(void)
{
    board_led_init();
    board_button_init();
}
