/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#ifdef CONFIG_HEAP_TRACING
#include "memory_checks.h"
#include "esp_heap_trace.h"
#endif

static const char* TAG = "event_test_app";

void setUp(void)
{
    // If heap tracing is enabled in kconfig, leak trace the test
#ifdef CONFIG_HEAP_TRACING
    setup_heap_record();
    heap_trace_start(HEAP_TRACE_LEAKS);
#endif

    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();

    esp_task_wdt_add(NULL);
    esp_task_wdt_reset();

}

void tearDown(void)
{
#ifdef CONFIG_HEAP_TRACING
    heap_trace_stop();
    heap_trace_dump();
#endif

    unity_utils_evaluate_leaks();

    esp_task_wdt_reset();
    esp_task_wdt_delete(NULL);
}

void app_main(void)
{
    // Configure the task watchdog timer to catch any tests that hang
    esp_task_wdt_config_t config = {
        .timeout_ms = 25 * 1000, // 25 seconds, smaller than the default pytest timeout
        .idle_core_mask = 0,
        .trigger_panic = true,
    };

    esp_task_wdt_init(&config);

    ESP_LOGI(TAG, "Running esp-event test app");
    // rand() seems to do a one-time allocation. Call it here so that the memory it allocates
    // is not counted as a leak.
    unsigned int _rand __attribute__((unused)) = rand();

    unity_run_menu();
}
