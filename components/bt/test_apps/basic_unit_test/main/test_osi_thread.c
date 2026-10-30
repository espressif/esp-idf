/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * osi_thread_free publishes PRE_STOP, STOPPED, then PRE_FREE. A listener
 * unregistered before the next free receives nothing.
 */

#include <stdint.h>

#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "osi/thread.h"

#define TEST_WORKER_STACK   3072
#define TEST_WORKER_PRIO    2

static osi_thread_t *s_expect;
static uint32_t s_count;
static uint32_t s_mismatch;
static osi_thread_evt_t s_evts[4];

static osi_thread_t *test_thread_create(void)
{
    const size_t workqueue_len[] = {4};
    osi_thread_t *thread = osi_thread_create("osi_th_test", TEST_WORKER_STACK, TEST_WORKER_PRIO,
                                             OSI_THREAD_CORE_0, 1, workqueue_len, false);
    TEST_ASSERT_NOT_NULL(thread);
    return thread;
}

static void lifecycle_listener(osi_thread_t *thread, osi_thread_evt_t evt)
{
    if (thread != s_expect) {
        s_mismatch++;
    }
    if (s_count < 4) {
        s_evts[s_count] = evt;
    }
    s_count++;
}

static struct osi_thread_listener_node s_lifecycle_node = {
    .listener = lifecycle_listener,
};

TEST_CASE("osi_thread_free publishes lifecycle events in order", "[osi_thread]")
{
    osi_thread_t *thread;
    uint32_t count;
    osi_thread_evt_t first;
    osi_thread_evt_t second;
    osi_thread_evt_t third;
    uint32_t after;

    s_count = 0;
    s_mismatch = 0;
    s_expect = NULL;
    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_TRUE(osi_thread_listener_register(&s_lifecycle_node));

    thread = test_thread_create();
    s_expect = thread;
    osi_thread_free(thread);
    count = s_count;
    first = s_evts[0];
    second = s_evts[1];
    third = s_evts[2];
    osi_thread_listener_unregister(&s_lifecycle_node);

    s_count = 0;
    s_expect = NULL;
    thread = test_thread_create();
    osi_thread_free(thread);
    after = s_count;
    osi_thread_event_deinit();

    TEST_ASSERT_EQUAL(0, s_mismatch);
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_EQUAL(OSI_THREAD_EVT_PRE_STOP, first);
    TEST_ASSERT_EQUAL(OSI_THREAD_EVT_STOPPED, second);
    TEST_ASSERT_EQUAL(OSI_THREAD_EVT_PRE_FREE, third);
    TEST_ASSERT_EQUAL(0, after);
}
