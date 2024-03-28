/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_partition.h"
#ifdef CONFIG_NVS_ENCRYPTION
#include "mbedtls/aes.h"
#endif
#include "memory_checks.h"

/* setUp runs before every test */
void setUp(void)
{
    // Execute mbedtls_aes_init operation to allocate AES interrupt
    // allocation memory which is considered as memory leak otherwise
#if defined(CONFIG_NVS_ENCRYPTION) && defined(SOC_AES_SUPPORTED)
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
#endif

    // Calling esp_partition_find_first ensures that the partitions have been loaded
    // and subsequent calls to esp_partition_find_first from the tests would not
    // load partitions which otherwise gets considered as a memory leak.
    esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);

    test_utils_record_free_mem();
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_WARN_LEAK_LEVEL_GENERAL, ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_LWIP);
}

/* tearDown runs after every test */
void tearDown(void)
{
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* clean up some of the newlib's lazy allocations */
    esp_reent_cleanup();

    /* check if unit test has caused heap corruption in any heap */
    TEST_ASSERT_MESSAGE( heap_caps_check_integrity(MALLOC_CAP_INVALID, true), "The test has corrupted the heap");

    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
            test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));

}

static void test_task(void *pvParameters)
{
    vTaskDelay(2); /* Delay a bit to let the main task be deleted */
    unity_run_menu();
}

void app_main(void)
{
    xTaskCreatePinnedToCore(test_task, "testTask", CONFIG_UNITY_FREERTOS_STACK_SIZE, NULL, CONFIG_UNITY_FREERTOS_PRIORITY, NULL, CONFIG_UNITY_FREERTOS_CPU);
}
