/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "osi/alarm.h"
#include "osi/thread.h"

#define TEST_MEMORY_LEAK_THRESHOLD_DEFAULT -150
static int leak_threshold = TEST_MEMORY_LEAK_THRESHOLD_DEFAULT;
void set_leak_threshold(int threshold)
{
    leak_threshold = threshold;
}

static size_t before_free_8bit;
static size_t before_free_32bit;

/* Set by a case that owns process-wide BT state. The runner longjmps out of
 * the case on a failed assertion, so tearDown is what still runs this. */
static void (*s_case_cleanup)(void);

void test_case_cleanup_set(void (*cleanup)(void))
{
    s_case_cleanup = cleanup;
}

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n", type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= leak_threshold, "memory leak");
}

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit;
    size_t after_free_32bit;

    /* The case body is gone after a failed assertion. Run its own end first
     * so threads and c2h objects are released before the alarm mux is dropped. */
    if (s_case_cleanup != NULL) {
        void (*cleanup)(void) = s_case_cleanup;

        s_case_cleanup = NULL;
        if (TEST_PROTECT()) {
            cleanup();
        }
    }

    after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    if (TEST_PROTECT()) {
        check_leak(before_free_8bit, after_free_8bit, "8BIT");
        check_leak(before_free_32bit, after_free_32bit, "32BIT");
    }

    /* Cases that never registered a cleanup still have to leave the mux free. */
    if (TEST_PROTECT()) {
        osi_alarm_deinit();
        (void)osi_alarm_delete_mux();
        osi_thread_event_deinit();
    }

    leak_threshold = TEST_MEMORY_LEAK_THRESHOLD_DEFAULT;
}

void app_main(void)
{
    printf("Running bt component tests\n");
    unity_run_menu();
}
