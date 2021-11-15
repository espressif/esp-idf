/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "freertos/FreeRTOS.h"

/**
 * @brief For the USB PHY into the connected or disconnected state
 *
 * @param connected For into connected state if true, disconnected if false
 * @param delay_ticks Delay in ticks before forcing state
 */
void test_usb_force_conn_state(bool connected, TickType_t delay_ticks);
