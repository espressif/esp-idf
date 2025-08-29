/* board.c - Board-specific hooks */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "iot_button.h"
#include "sdkconfig.h"

#define TAG "BOARD"

#define BUTTON_IO_NUM           CONFIG_BUTTON_IO_NUM
#define BUTTON_ACTIVE_LEVEL     CONFIG_BUTTON_ACTIVE_LEVEL

extern void example_ble_mesh_send_vendor_message(bool resend);

static void button_tap_cb(void* arg)
{
    example_ble_mesh_send_vendor_message(false);
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
