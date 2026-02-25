/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "component2";

void component2_print_hello(void)
{
    ESP_LOGI(TAG, "Hello from component2!");
}
