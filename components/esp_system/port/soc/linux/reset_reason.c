/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * All functions presented here are stubs for the POSIX/Linux implementation of FreeRTOS.
 * They are meant to allow to compile, but they DO NOT return any meaningful value.
 */

#include "esp_system.h"

// On Linux, this is just a meaningful value to make applications build and run
esp_reset_reason_t esp_reset_reason(void)
{
    return ESP_RST_POWERON;
}
