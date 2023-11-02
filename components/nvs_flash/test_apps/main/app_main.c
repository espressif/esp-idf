/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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

#include "esp_heap_caps.h"
#include "time.h"

// recorded heap free sizes (MALLOC_CAP_INTERNAL and MALLOC_CAP_SPIRAM)
static size_t recorded_internal_heap_free_size = 0;
static size_t recorded_spiram_heap_free_size = 0;

// stores heap free sizes for internal and spiram pools
void record_heap_free_sizes(void)
{
    recorded_internal_heap_free_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    recorded_spiram_heap_free_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
}

// returns difference between actual heap free size and recorded heap free size
// parameter nvs_active_pool controls whether active or inactive heap will be examined
// if CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM is not set, active pool is MALLOC_CAP_INTERNAL and inactive is MALLOC_CAP_SPIRAM
// if CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM is set, active pool is MALLOC_CAP_SPIRAM and inactive is MALLOC_CAP_INTERNAL
int32_t get_heap_free_difference(const bool nvs_active_pool)
{
    int32_t recorded_heap_free_size = 0;
    int32_t actual_heap_free_size = 0;

    bool evaluate_spiram = false;
#ifdef CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM
    // here active means spiram
    evaluate_spiram = nvs_active_pool;
#else
    // here active means internal
    evaluate_spiram = !nvs_active_pool;
#endif

    if(evaluate_spiram) {
        recorded_heap_free_size = recorded_spiram_heap_free_size;
        actual_heap_free_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    }
    else {
        recorded_heap_free_size = recorded_internal_heap_free_size;
        actual_heap_free_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    }

    return actual_heap_free_size - recorded_heap_free_size;
}

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
