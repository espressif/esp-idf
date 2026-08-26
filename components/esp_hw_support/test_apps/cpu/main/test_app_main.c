/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_log.h"

static const char *TAG = "cpu_test_app";

void app_main(void)
{
    ESP_LOGI(TAG, "Running esp_cpu test app");
    unity_run_menu();
}
