// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include "string.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_newlib.h"

#ifdef CONFIG_HEAP_TRACING
#include "esp_heap_trace.h"
#endif

static size_t before_free_8bit;
static size_t before_free_32bit;

static size_t warn_leak_threshold;
static size_t critical_leak_threshold;

static void unity_task(void *pvParameters)
{
    vTaskDelay(2); /* Delay a bit to let the main task be deleted */
    unity_run_menu(); /* Doesn't return */
}

void test_main(void)
{
    // Note: if unpinning this task, change the way run times are calculated in
    // unity_port_esp32.c
    xTaskCreatePinnedToCore(unity_task, "unityTask", UNITY_FREERTOS_STACK_SIZE, NULL,
                            UNITY_FREERTOS_PRIORITY, NULL, UNITY_FREERTOS_CPU);
}

void unity_reset_leak_checks(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);

#ifdef CONFIG_HEAP_TRACING
    heap_trace_start(HEAP_TRACE_LEAKS);
#endif
}

/* setUp runs before every test */
void setUp(void)
{
// If heap tracing is enabled in kconfig, leak trace the test
#ifdef CONFIG_HEAP_TRACING
    const size_t num_heap_records = 80;
    static heap_trace_record_t *record_buffer;
    if (!record_buffer) {
        record_buffer = malloc(sizeof(heap_trace_record_t) * num_heap_records);
        assert(record_buffer);
        heap_trace_init_standalone(record_buffer, num_heap_records);
    }
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

    unity_reset_leak_checks();
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL, TYPE_LEAK_CRITICAL, COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_WARN_LEAK_LEVEL_GENERAL, TYPE_LEAK_WARNING, COMP_LEAK_GENERAL);
}

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    if (before_free <= after_free) {
        return;
    }
    size_t leaked = before_free - after_free;
    if (leaked <= warn_leak_threshold) {
        return;
    }

    printf("MALLOC_CAP_%s %s leak: Before %u bytes free, After %u bytes free (delta %u)\n",
           type,
           leaked <= critical_leak_threshold ? "potential" : "critical",
           before_free, after_free, leaked);
    fflush(stdout);
    TEST_ASSERT_MESSAGE(leaked <= critical_leak_threshold, "The test leaked too much memory");
}

static bool leak_check_required(void)
{
    warn_leak_threshold = test_utils_get_leak_level(TYPE_LEAK_WARNING, COMP_LEAK_ALL);
    critical_leak_threshold = test_utils_get_leak_level(TYPE_LEAK_CRITICAL, COMP_LEAK_ALL);
    if (Unity.CurrentDetail1 != NULL) {
        const char *leaks = "[leaks";
        const int len_leaks = strlen(leaks);
        const char *sub_leaks = strstr(Unity.CurrentDetail1, leaks);
        if (sub_leaks != NULL) {
            if (sub_leaks[len_leaks] == ']') {
                return true;
            } else if (sub_leaks[len_leaks] == '=') {
                critical_leak_threshold = strtol(&sub_leaks[len_leaks + 1], NULL, 10);
                warn_leak_threshold = critical_leak_threshold;
                return false;
            }
        }
    }
    return false;
}

/* tearDown runs after every test */
void tearDown(void)
{
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* clean up some of the newlib's lazy allocations */
    esp_reent_cleanup();

    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
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

    if (leak_check_required() == false) {
        check_leak(before_free_8bit, after_free_8bit, "8BIT");
        check_leak(before_free_32bit, after_free_32bit, "32BIT");
    }

    Unity.TestFile = real_testfile; // go back to the real filename
}

