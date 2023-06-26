/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in flash encryption, the threadhold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (-400)

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

    //     ________    ___   _____ __  __   _______   ______________  ______  ______________  _   __
    //    / ____/ /   /   | / ___// / / /  / ____/ | / / ____/ __ \ \/ / __ \/_  __/  _/ __ \/ | / /
    //   / /_  / /   / /| | \__ \/ /_/ /  / __/ /  |/ / /   / /_/ /\  / /_/ / / /  / // / / /  |/ /
    //  / __/ / /___/ ___ |___/ / __  /  / /___/ /|  / /___/ _, _/ / / ____/ / / _/ // /_/ / /|  /
    // /_/   /_____/_/  |_/____/_/ /_/  /_____/_/ |_/\____/_/ |_| /_/_/     /_/ /___/\____/_/ |_/

    printf("    ________    ___   _____ __  __   _______   ______________  ______  ______________  _   __ \n");
    printf("   / ____/ /   /   | / ___// / / /  / ____/ | / / ____/ __ \\ \\/ / __ \\/_  __/  _/ __ \\/ | / / \n");
    printf("  / /_  / /   / /| | \\__ \\/ /_/ /  / __/ /  |/ / /   / /_/ /\\  / /_/ / / /  / // / / /  |/ /  \n");
    printf(" / __/ / /___/ ___ |___/ / __  /  / /___/ /|  / /___/ _, _/ / / ____/ / / _/ // /_/ / /|  /   \n");
    printf("/_/   /_____/_/  |_/____/_/ /_/  /_____/_/ |_/\\____/_/ |_| /_/_/     /_/ /___/\\____/_/ |_/    \n");

    unity_run_menu();
}
