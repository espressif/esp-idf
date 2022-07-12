/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "esp_newlib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Some resources are lazy allocated in pulse_cnt driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (-300)

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
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* clean up some of the newlib's lazy allocations */
    esp_reent_cleanup();

    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");
}

void app_main(void)
{
    //  _____ _____ __  __   _____         _
    // | ____|_   _|  \/  | |_   _|__  ___| |_
    // |  _|   | | | |\/| |   | |/ _ \/ __| __|
    // | |___  | | | |  | |   | |  __/\__ \ |_
    // |_____| |_| |_|  |_|   |_|\___||___/\__|
    printf(" _____ _____ __  __   _____         _\r\n");
    printf("| ____|_   _|  \\/  | |_   _|__  ___| |_\r\n");
    printf("|  _|   | | | |\\/| |   | |/ _ \\/ __| __|\r\n");
    printf("| |___  | | | |  | |   | |  __/\\__ \\ |_\r\n");
    printf("|_____| |_| |_|  |_|   |_|\\___||___/\\__|\r\n");
    unity_run_menu();
}
