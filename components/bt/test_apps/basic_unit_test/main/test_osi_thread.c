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
#include "freertos/semphr.h"
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

static volatile uint32_t s_ran;
static volatile uint32_t s_freed;
static volatile int s_queued_at_stop;
static volatile bool s_post_ok;
static SemaphoreHandle_t s_block;

static void counted_work(void *arg)
{
    (void)arg;
    s_ran++;
}

static void counted_free(void *arg)
{
    (void)arg;
    s_freed++;
}

static void blocker(void *arg)
{
    (void)arg;
    if (s_block != NULL) {
        xSemaphoreTake(s_block, pdMS_TO_TICKS(3000));
    }
}

/* osi_thread_free joins the worker. Release the blocker only after that join waits. */
static void give_block_soon(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(20));
    if (s_block != NULL) {
        xSemaphoreGive(s_block);
    }
    vTaskDelete(NULL);
}

/* STOPPED is after the worker has stopped and before drain. A post from here
 * stays queued, so drain reclaims it and the handler never runs. */
static void stopped_listener(osi_thread_t *thread, osi_thread_evt_t evt)
{
    if (evt != OSI_THREAD_EVT_STOPPED) {
        return;
    }
    s_post_ok = osi_thread_post(thread, counted_work, NULL, counted_free, 0, 0);
    s_queued_at_stop = osi_thread_queue_wait_size(thread, 0);
}

static struct osi_thread_listener_node s_stopped_node = {
    .listener = stopped_listener,
};

TEST_CASE("osi_thread_free drains a post made at STOPPED without running it", "[osi_thread]")
{
    osi_thread_t *thread;
    bool post_ok;
    int queued;
    uint32_t ran;
    uint32_t freed;

    s_ran = 0;
    s_freed = 0;
    s_queued_at_stop = -1;
    s_post_ok = false;
    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_TRUE(osi_thread_listener_register(&s_stopped_node));

    thread = test_thread_create();
    osi_thread_free(thread);
    post_ok = s_post_ok;
    queued = s_queued_at_stop;
    ran = s_ran;
    freed = s_freed;
    osi_thread_listener_unregister(&s_stopped_node);
    osi_thread_event_deinit();

    TEST_ASSERT_TRUE(post_ok);
    TEST_ASSERT_GREATER_THAN(0, queued);
    TEST_ASSERT_EQUAL(0, ran);
    TEST_ASSERT_EQUAL(1, freed);
}

TEST_CASE("osi_thread free_cb runs on drain and not on dispatch", "[osi_thread]")
{
    osi_thread_t *thread;

    s_ran = 0;
    s_freed = 0;
    thread = test_thread_create();
    TEST_ASSERT_TRUE(osi_thread_post(thread, counted_work, NULL, counted_free, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(1, s_ran);
    TEST_ASSERT_EQUAL(0, s_freed);
    osi_thread_free(thread);

    s_ran = 0;
    s_freed = 0;
    s_block = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_block);
    thread = test_thread_create();
    TEST_ASSERT_TRUE(osi_thread_post(thread, blocker, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_TRUE(osi_thread_post(thread, counted_work, NULL, counted_free, 0, 0));
    TEST_ASSERT_GREATER_THAN(0, osi_thread_queue_wait_size(thread, 0));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(give_block_soon, "give_blk", 2048, NULL, 3, NULL));
    osi_thread_free(thread);
    TEST_ASSERT_EQUAL(0, s_ran);
    TEST_ASSERT_EQUAL(1, s_freed);

    vSemaphoreDelete(s_block);
    s_block = NULL;
    vTaskDelay(pdMS_TO_TICKS(20));
}
