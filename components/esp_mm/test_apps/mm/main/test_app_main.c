/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

/**
 * Hardware related tests, e.g.
 * - traversing all vaddr range to check their attributes
 *
 * These tests need certain number of internal resources (heap memory), as they uses up the vaddr ranges
 * On ESP32, it should be around 450
 * On ESP32S2, it should be around 600
 * On other chips, it should be around 400
 */
#define TEST_MEMORY_LEAK_THRESHOLD (-650)

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
    /*
         _____ ___________  ___  ______  ___  ___  ______   _____ ___
        |  ___/  ___| ___ \ |  \/  ||  \/  ||_   _|  ___/  ___|_   _|
        | |__ \ `--.| |_/ / | .  . || .  . |  | | | |__ \ `--.  | |
        |  __| `--. \  __/  | |\/| || |\/| |  | | |  __| `--. \ | |
        | |___/\__/ / |     | |  | || |  | |  | | | |___/\__/ / | |
        \____/\____/\_|     \_|  |_/\_|  |_/  \_/ \____/\____/  \_/
    */

    printf(" _____ ___________  ___  ______  ___  ___  ______   _____ ___\r\n");
    printf("|  ___/  ___| ___ \\ |  \\/  ||  \\/  ||_   _|  ___/  ___|_   _|\r\n");
    printf("| |__ \\ `--.| |_/ / | .  . || .  . |  | | | |__ \\ `--.  | |\r\n");
    printf("|  __| `--. \\  __/  | |\\/| || |\\/| |  | | |  __| `--. \\ | |\r\n");
    printf("| |___/\\__/ / |     | |  | || |  | |  | | | |___/\\__/ / | |\r\n");
    printf("\\____/\\____/\\_|     \\_|  |_/\\_|  |_/  \\_/ \\____/\\____/  \\_/\r\n");

    unity_run_menu();
}
