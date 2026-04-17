/* board.c - Board-specific hooks */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "iot_button.h"
#include "esp_log.h"
#include "board.h"

#define TAG "BOARD"

#define BUTTON_IO_NUM           CONFIG_BUTTON_IO_NUM
#define BUTTON_ACTIVE_LEVEL     0

extern uint8_t click_to_send_onoff_set;
extern void example_ble_mesh_send_gen_onoff_set(uint8_t onoff);
extern void example_ble_mesh_send_remote_provisioning_scan_start(void);

static void button_tap_cb(void* arg)
{
    static uint8_t onoff = 1;
    ESP_LOGI(TAG, "tap cb (%s)", (char *)arg);
    if (click_to_send_onoff_set) {
        example_ble_mesh_send_gen_onoff_set(onoff);
        onoff = !onoff;
    } else {
        example_ble_mesh_send_remote_provisioning_scan_start();
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
    board_button_init();
}
