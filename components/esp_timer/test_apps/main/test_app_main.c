/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

// On C2 due to only have a single HW WDT we use the timer
// so some tests will interfere with the WDT
#if !CONFIG_IDF_TARGET_ESP32C2
#define WDT_MONITOR_UNITY_TASK
#endif

void setUp(void)
{
#ifdef WDT_MONITOR_UNITY_TASK
    esp_task_wdt_add(NULL);
    esp_task_wdt_reset();
#endif
}

void tearDown(void)
{
#ifdef WDT_MONITOR_UNITY_TASK
    esp_task_wdt_reset();
    esp_task_wdt_delete(NULL);
#endif
}

void app_main(void)
{
    // Configure the task watchdog timer to catch any tests that hang
#ifdef WDT_MONITOR_UNITY_TASK
    esp_task_wdt_config_t config = {
        .timeout_ms = 25 * 1000, // 25 seconds, smaller than the default pytest timeout
        .idle_core_mask = 0,
        .trigger_panic = true,
    };

    esp_task_wdt_init(&config);
#endif

    vTaskPrioritySet(NULL, CONFIG_UNITY_FREERTOS_PRIORITY);
    unity_run_menu();
}
