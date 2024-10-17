/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <time.h>
#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_log.h"

static const char *TAG = "Linux_sim";

void setUp(void)
{
    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks();
}

void app_main(void)
{
    // The following code avoids memory leaks from initializing libc structures in clock_gettime() calls.
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    (void) tp;

    ESP_LOGI(TAG, "Running FreeRTOS Linux test app");
    unity_run_menu();
}
