/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "string.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "test_utils.h"
#include "esp_newlib.h"
#include "memory_checks.h"

#ifdef CONFIG_HEAP_TRACING
#include "esp_heap_trace.h"
#endif

static void unity_task(void *pvParameters)
{
    // Delay a bit to let the main task and any other startup tasks be deleted
    vTaskDelay(pdMS_TO_TICKS(50));
    // Start running unity (prints test menu and doesn't return)
    unity_run_menu();
}

void test_main(void)
{
    // Note: if unpinning this task, change the way run times are calculated in
    // unity_port_esp32.c
    xTaskCreatePinnedToCore(unity_task, "unityTask", UNITY_FREERTOS_STACK_SIZE, NULL,
                            UNITY_FREERTOS_PRIORITY, NULL, UNITY_FREERTOS_CPU);
}

/* setUp runs before every test */
void setUp(void)
{
// If heap tracing is enabled in kconfig, leak trace the test
#ifdef CONFIG_HEAP_TRACING
    setup_heap_record();
#endif

    printf("%s", ""); /* sneakily lazy-allocate the reent structure for this test task */

#ifdef CONFIG_APP_BUILD_USE_FLASH_SECTIONS
    /* TODO: add sufficient startup code in case of building an ELF file, so that
     * flash cache is initialized and can work in such mode.
     * For now this is disabled to allow running unit tests which don't require
     * flash cache related operations.
     */
    get_test_data_partition();  /* allocate persistent partition table structures */
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS

#ifdef CONFIG_HEAP_TRACING
    heap_trace_start(HEAP_TRACE_LEAKS);
#endif
    test_utils_record_free_mem();
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_WARN_LEAK_LEVEL_GENERAL, ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_LWIP);
}

typedef enum {
    NO_LEAK_CHECK,
    DEFAULT_LEAK_CHECK,
    SPECIAL_LEAK_CHECK
} leak_check_type_t;

/**
 * It is possible to specify the maximum allowed memory leak level directly in the test case
 * or disable leak checking for a test case.
 * This function checks if this is the case and return the appropriate return value.
 * If a custom leak level has been specified, that custom threshold is written to the value pointed by threshold.
 */
static leak_check_type_t leak_check_required(size_t *threshold)
{
    if (Unity.CurrentDetail1 != NULL) {
        const char *leaks = "[leaks";
        const int len_leaks = strlen(leaks);
        const char *sub_leaks = strstr(Unity.CurrentDetail1, leaks);
        if (sub_leaks != NULL) {
            if (sub_leaks[len_leaks] == ']') {
                return NO_LEAK_CHECK;
            } else if (sub_leaks[len_leaks] == '=') {
                *threshold = strtol(&sub_leaks[len_leaks + 1], NULL, 10);
                return SPECIAL_LEAK_CHECK;
            }
        }
    }
    return DEFAULT_LEAK_CHECK;
}

/* tearDown runs after every test */
void tearDown(void)
{
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* clean up some of the newlib's lazy allocations */
    esp_reent_cleanup();

    /* We want the teardown to have this file in the printout if TEST_ASSERT fails */
    const char *real_testfile = Unity.TestFile;
    Unity.TestFile = __FILE__;

    /* check if unit test has caused heap corruption in any heap */
    TEST_ASSERT_MESSAGE( heap_caps_check_integrity(MALLOC_CAP_INVALID, true), "The test has corrupted the heap");

    /* check for leaks */
#ifdef CONFIG_HEAP_TRACING
    heap_trace_stop();
    heap_trace_dump();
#endif

    size_t leak_threshold_critical = 0;
    size_t leak_threshold_warning = 0;
    leak_check_type_t check_type = leak_check_required(&leak_threshold_critical);

    // In the "special case", only one level can be passed directly from the test case.
    // Hence, we set both warning and critical leak levels to that same value here
    leak_threshold_warning = leak_threshold_critical;

    if (check_type == NO_LEAK_CHECK) {
        // do not check
    } else if (check_type == SPECIAL_LEAK_CHECK) {
        test_utils_finish_and_evaluate_leaks(leak_threshold_warning, leak_threshold_critical);
    } else if (check_type == DEFAULT_LEAK_CHECK) {
        test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
    } else {
        assert(false); // coding error
    }

    Unity.TestFile = real_testfile; // go back to the real filename
}
