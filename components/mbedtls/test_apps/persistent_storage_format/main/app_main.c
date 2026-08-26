/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_newlib.h"
#include "memory_checks.h"
#include "nvs_flash.h"
#include "unity.h"

void setUp(void)
{
    test_utils_record_free_mem();
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL,
                              ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_WARN_LEAK_LEVEL_GENERAL,
                              ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_GENERAL);
}

void tearDown(void)
{
    vTaskDelay(5);
    esp_reent_cleanup();
    TEST_ASSERT_MESSAGE(heap_caps_check_integrity(MALLOC_CAP_INVALID, true),
                        "The test has corrupted the heap");
    test_utils_finish_and_evaluate_leaks(
        test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
        test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

static void test_task(void *pvParameters)
{
    vTaskDelay(2);
    unity_run_menu();
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    xTaskCreatePinnedToCore(test_task, "testTask",
                            CONFIG_UNITY_FREERTOS_STACK_SIZE, NULL,
                            CONFIG_UNITY_FREERTOS_PRIORITY, NULL,
                            CONFIG_UNITY_FREERTOS_CPU);
}
