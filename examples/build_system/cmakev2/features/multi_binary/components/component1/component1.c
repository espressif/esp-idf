/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "component1";

void component1_print_hello(void)
{
    ESP_LOGI(TAG, "Hello from component1!");
}
