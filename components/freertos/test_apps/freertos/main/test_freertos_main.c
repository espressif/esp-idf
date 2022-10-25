/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "test_utils.h"

#define TEST_MEMORY_LEAK_THRESHOLD (-512)

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
}

void tearDown(void)
{
    // Add a short delay of 10ms to allow the idle task to free an remaining memory
    vTaskDelay(pdMS_TO_TICKS(10));
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");
}

void app_main(void)
{
    /*
    Some FreeRTOS tests are reliant on the main task being at priority UNITY_FREERTOS_PRIORITY to test scheduling
    behavior. Thus, we raise the main task's priority before any tasks are run. See IDF-6088
    */
    vTaskPrioritySet(NULL, UNITY_FREERTOS_PRIORITY);
    printf(" ______             _____ _______ ____   _____\n");
    printf("|  ____|           |  __ \\__   __/ __ \\ / ____|\n");
    printf("| |__ _ __ ___  ___| |__) | | | | |  | | (___\n");
    printf("|  __| '__/ _ \\/ _ \\  _  /  | | | |  | |\\___ \\\n");
    printf("| |  | | |  __/  __/ | \\ \\  | | | |__| |____) |\n");
    printf("|_|  |_|  \\___|\\___|_|  \\_\\ |_|  \\____/|_____/\n");
    unity_run_menu();
}
