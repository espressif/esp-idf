/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"

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
    // _     ______   _____ _____  _____   _____ _____ _____ _____
    // | |    | ___ \ |_   _/ __  \/  ___| |_   _|  ___/  ___|_   _|
    // | |    | |_/ /   | | `' / /'\ `--.    | | | |__ \ `--.  | |
    // | |    |  __/    | |   / /   `--. \   | | |  __| `--. \ | |
    // | |____| |      _| |_./ /___/\__/ /   | | | |___/\__/ / | |
    // \_____/\_|      \___/\_____/\____/    \_/ \____/\____/  \_/

    printf(" _     ______   _____ _____  _____   _____ _____ _____ _____\r\n");
    printf("| |    | ___ \\ |_   _/ __  \\/  ___| |_   _|  ___/  ___|_   _|\r\n");
    printf("| |    | |_/ /   | | `' / /'\\ `--.    | | | |__ \\ `--.  | |\r\n");
    printf("| |    |  __/    | |   / /   `--. \\   | | |  __| `--. \\ | |\r\n");
    printf("| |____| |      _| |_./ /___/\\__/ /   | | | |___/\\__/ / | |\r\n");
    printf("\\_____/\\_|      \\___/\\_____/\\____/    \\_/ \\____/\\____/  \\_/\r\n");

    unity_run_menu();
}
