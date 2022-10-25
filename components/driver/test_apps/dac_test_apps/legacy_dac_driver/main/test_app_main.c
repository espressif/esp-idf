/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in dac driver, the threshold is left for that case
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
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");
}

void app_main(void)
{
    //  ____    _    ____   _____         _
    // |  _ \  / \  / ___| |_   _|__  ___| |_
    // | | | |/ _ \| |       | |/ _ \/ __| __|
    // | |_| / ___ \ |___    | |  __/\__ \ |_
    // |____/_/   \_\____|   |_|\___||___/\__|

    printf("  ____    _    ____   _____         _   \n");
    printf(" |  _ \\  / \\  / ___| |_   _|__  ___| |_ \n");
    printf(" | | | |/ _ \\| |       | |/ _ \\/ __| __|\n");
    printf(" | |_| / ___ \\ |___    | |  __/\\__ \\ |_ \n");
    printf(" |____/_/   \\_\\____|   |_|\\___||___/\\__| (legacy)\n");

    unity_run_menu();
}
