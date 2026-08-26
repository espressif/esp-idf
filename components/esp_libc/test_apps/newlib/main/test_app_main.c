/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "esp_task_wdt.h"

#define TEST_MEMORY_LEAK_THRESHOLD (-350)

static size_t before_free_8bit;
static size_t before_free_32bit;

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n", type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= TEST_MEMORY_LEAK_THRESHOLD, "memory leak");
}

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);

#if SOC_WDT_SUPPORTED
    esp_task_wdt_add(NULL);
    esp_task_wdt_reset();
#endif

}

void tearDown(void)
{
#if SOC_WDT_SUPPORTED
    esp_task_wdt_reset();
    esp_task_wdt_delete(NULL);
#endif

    // Add a short delay of 10ms to allow the idle task to free an remaining memory
    vTaskDelay(pdMS_TO_TICKS(10));
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");
}

void app_main(void)
{
#if SOC_WDT_SUPPORTED
    // Configure the task watchdog timer to catch any tests that hang
    esp_task_wdt_config_t config = {
        .timeout_ms = 25 * 1000, // 25 seconds, smaller than the default pytest timeout
        .idle_core_mask = 0,
        .trigger_panic = true,
    };

    esp_task_wdt_init(&config);
#endif // SOC_WDT_SUPPORTED

    // Raise priority to main task as some tests depend on the test task being at priority UNITY_FREERTOS_PRIORITY
    vTaskPrioritySet(NULL, UNITY_FREERTOS_PRIORITY);
    unity_run_menu();
}
