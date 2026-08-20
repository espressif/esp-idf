/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Unit tests for osi_event (components/bt/common/osi/thread.c): coalesce,
 * delete-while-queued, drain without user callbacks, stale post after delete,
 * and re-post after QUEUED is cleared (lost-wakeup regression).
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "osi/thread.h"

#define TEST_WORKER_STACK       3072
#define TEST_HELPER_STACK       2560
#define TEST_WAIT_MS            1000
#define TEST_NO_RUN_MS          50
#define TEST_WORKER_PRIO_HIGH   (configMAX_PRIORITIES - 2)
#define TEST_WORKER_PRIO_LOW    2

static SemaphoreHandle_t s_done;
static SemaphoreHandle_t s_gate;
static SemaphoreHandle_t s_join;
static volatile uint32_t s_run_count;
static volatile uint32_t s_dummy_count;
static struct osi_event *s_event;
static volatile bool s_stop_posters;

static void handler_count(void *context)
{
    (void)context;
    s_run_count++;
    xSemaphoreGive(s_done);
}

static void handler_gated(void *context)
{
    (void)context;
    s_run_count++;
    xSemaphoreGive(s_done);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_gate, pdMS_TO_TICKS(TEST_WAIT_MS)));
}

static void handler_repost(void *context)
{
    (void)context;
    uint32_t n = ++s_run_count;
    if (n < 3) {
        TEST_ASSERT_TRUE(osi_thread_post_event(s_event, 0));
    }
    if (n == 3) {
        xSemaphoreGive(s_done);
    }
}

static void handler_self_delete(void *context)
{
    (void)context;
    s_run_count++;
    osi_event_delete(s_event);
    s_event = NULL;
    xSemaphoreGive(s_done);
}

static void dummy_work(void *context)
{
    (void)context;
    s_dummy_count++;
}

static osi_thread_t *test_thread_create(int priority, size_t queue_len)
{
    const size_t workqueue_len[] = {queue_len};
    /* Pin to core 0: OSI_THREAD_CORE_AFFINITY (== 2) is not a valid FreeRTOS
     * core id / tskNO_AFFINITY and trips xTaskCreatePinnedToCore on ESP32. */
    osi_thread_t *thread = osi_thread_create("osi_ev_test", TEST_WORKER_STACK, priority,
                                             OSI_THREAD_CORE_0, 1, workqueue_len, false);
    TEST_ASSERT_NOT_NULL(thread);
    return thread;
}

static struct osi_event *test_event_bind(osi_thread_t *thread, osi_thread_func_t func)
{
    struct osi_event *event = osi_event_create(func, NULL);
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_TRUE(osi_event_bind(event, thread, 0));
    return event;
}

static struct osi_dynamic_event *test_dynamic_event_bind(osi_thread_t *thread,
                                                         osi_thread_func_t func)
{
    struct osi_dynamic_event *event = osi_dynamic_event_create(func, NULL);
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_TRUE(osi_dynamic_event_bind(event, thread, 0));
    return event;
}

static void osi_event_test_begin(void)
{
    s_run_count = 0;
    s_dummy_count = 0;
    s_event = NULL;
    s_stop_posters = false;

    s_done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_done);
    s_gate = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_gate);
    s_join = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_join);

    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
}

static void osi_event_test_end(osi_thread_t *thread, struct osi_event *event)
{
    if (event != NULL) {
        osi_event_delete(event);
    }
    if (thread != NULL) {
        osi_thread_free(thread);
    }
    osi_thread_event_deinit();

    vSemaphoreDelete(s_done);
    s_done = NULL;
    vSemaphoreDelete(s_gate);
    s_gate = NULL;
    vSemaphoreDelete(s_join);
    s_join = NULL;

    /* Idle task reclaims deleted worker TCBs asynchronously. */
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("osi_event duplicate post coalesces until handler runs", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, 8);
    event = test_event_bind(thread, handler_count);

    TEST_ASSERT_TRUE(osi_thread_post_event(event, 0));
    TEST_ASSERT_FALSE(osi_thread_post_event(event, 0));
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    vTaskPrioritySet(NULL, saved_prio);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_NO_RUN_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);

    osi_event_test_end(thread, event);
}

TEST_CASE("osi_event re-post while handler is running is accepted", "[osi_event]")
{
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);
    event = test_event_bind(thread, handler_gated);

    TEST_ASSERT_TRUE(osi_thread_post_event(event, OSI_THREAD_MAX_TIMEOUT));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);

    /* QUEUED is already clear; POSTING must not reject this re-post. */
    TEST_ASSERT_TRUE(osi_thread_post_event(event, 0));
    xSemaphoreGive(s_gate);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(2, s_run_count);
    xSemaphoreGive(s_gate);

    osi_event_test_end(thread, event);
}

TEST_CASE("osi_event callback may self-repost", "[osi_event]")
{
    osi_thread_t *thread;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);
    s_event = test_event_bind(thread, handler_repost);

    TEST_ASSERT_TRUE(osi_thread_post_event(s_event, 0));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(3, s_run_count);

    osi_event_test_end(thread, s_event);
    s_event = NULL;
}

TEST_CASE("osi_event callback may self-delete", "[osi_event]")
{
    osi_thread_t *thread;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);
    s_event = test_event_bind(thread, handler_self_delete);

    TEST_ASSERT_TRUE(osi_thread_post_event(s_event, 0));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_NULL(s_event);

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_event delete then stale post is rejected", "[osi_event]")
{
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);
    event = test_event_bind(thread, handler_count);

    osi_event_delete(event);
    TEST_ASSERT_FALSE(osi_thread_post_event(event, 0));
    TEST_ASSERT_FALSE(osi_thread_post_event(NULL, 0));
    osi_event_delete(event);
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_event delete while queued skips callback and drains refs", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, 8);
    event = test_event_bind(thread, handler_count);

    TEST_ASSERT_TRUE(osi_thread_post_event(event, 0));
    osi_event_delete(event);
    vTaskPrioritySet(NULL, saved_prio);

    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_NO_RUN_MS)));
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_event thread free drains pending event without callback", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, 8);
    event = test_event_bind(thread, handler_count);

    TEST_ASSERT_TRUE(osi_thread_post_event(event, 0));
    osi_thread_free(thread);
    vTaskPrioritySet(NULL, saved_prio);

    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);
    osi_event_delete(event);
    osi_thread_event_deinit();

    vSemaphoreDelete(s_done);
    s_done = NULL;
    vSemaphoreDelete(s_gate);
    s_gate = NULL;
    vSemaphoreDelete(s_join);
    s_join = NULL;
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("osi_event drain after event subsystem deinit", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, 8);
    event = test_event_bind(thread, handler_count);

    TEST_ASSERT_TRUE(osi_thread_post_event(event, 0));
    osi_event_delete(event);
    osi_thread_event_deinit();
    osi_thread_free(thread);
    vTaskPrioritySet(NULL, saved_prio);

    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    vSemaphoreDelete(s_done);
    s_done = NULL;
    vSemaphoreDelete(s_gate);
    s_gate = NULL;
    vSemaphoreDelete(s_join);
    s_join = NULL;
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("osi_event post timeout 0 fails when work queue is full", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_event *event;
    const size_t queue_len = 2;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, queue_len);
    event = test_event_bind(thread, handler_count);

    for (size_t i = 0; i < queue_len; i++) {
        TEST_ASSERT_TRUE(osi_thread_post(thread, dummy_work, NULL, 0, 0));
    }
    TEST_ASSERT_FALSE(osi_thread_post_event(event, 0));
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    /* Queue reservation was rolled back; a later post can succeed once space exists. */
    vTaskPrioritySet(NULL, saved_prio);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_NO_RUN_MS)));

    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    TEST_ASSERT_TRUE(osi_thread_post_event(event, 0));
    vTaskPrioritySet(NULL, saved_prio);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);

    osi_event_test_end(thread, event);
}

static void poster_task(void *arg)
{
    struct osi_event *event = (struct osi_event *)arg;

    while (!s_stop_posters) {
        osi_thread_post_event(event, 0);
        /* Must block, not only yield: two ready posters can pin both CPUs and
         * starve IDLE*, which trips the task WDT. */
        vTaskDelay(1);
    }
    xSemaphoreGive(s_join);
    vTaskDelete(NULL);
}

TEST_CASE("osi_event concurrent post and delete", "[osi_event]")
{
    osi_thread_t *thread;
    struct osi_event *event;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 16);
    event = test_event_bind(thread, handler_count);

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(poster_task, "osi_ev_p1", TEST_HELPER_STACK,
                                          event, TEST_WORKER_PRIO_LOW, NULL));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(poster_task, "osi_ev_p2", TEST_HELPER_STACK,
                                          event, TEST_WORKER_PRIO_LOW, NULL));

    vTaskDelay(pdMS_TO_TICKS(30));
    osi_event_delete(event);
    s_stop_posters = true;

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_join, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_join, pdMS_TO_TICKS(TEST_WAIT_MS)));

    TEST_ASSERT_FALSE(osi_thread_post_event(event, 0));

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_event create bind post delete stop cycles", "[osi_event]")
{
    for (int cycle = 0; cycle < 8; cycle++) {
        osi_thread_t *thread;
        struct osi_event *event;

        osi_event_test_begin();
        thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);
        event = test_event_bind(thread, handler_count);

        TEST_ASSERT_TRUE(osi_thread_post_event(event, OSI_THREAD_MAX_TIMEOUT));
        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
        TEST_ASSERT_EQUAL_UINT32(1, s_run_count);

        osi_event_test_end(thread, event);
    }
}

TEST_CASE("osi_dynamic_event delete then stale post is rejected", "[osi_event]")
{
    osi_thread_t *thread;
    struct osi_dynamic_event *event;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);
    event = test_dynamic_event_bind(thread, handler_count);

    osi_dynamic_event_delete(event);
    TEST_ASSERT_FALSE(osi_dynamic_event_post(event, 0));
    TEST_ASSERT_FALSE(osi_dynamic_event_post(NULL, 0));
    osi_dynamic_event_delete(event);
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_dynamic_event delete while queued skips callback", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_dynamic_event *event;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, 8);
    event = test_dynamic_event_bind(thread, handler_count);

    TEST_ASSERT_TRUE(osi_dynamic_event_post(event, 0));
    osi_dynamic_event_delete(event);
    vTaskPrioritySet(NULL, saved_prio);

    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_NO_RUN_MS)));
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    osi_event_test_end(thread, NULL);
}

static void dynamic_poster_task(void *arg)
{
    struct osi_dynamic_event *event = (struct osi_dynamic_event *)arg;

    while (!s_stop_posters) {
        osi_dynamic_event_post(event, 0);
        vTaskDelay(1);
    }
    xSemaphoreGive(s_join);
    vTaskDelete(NULL);
}

TEST_CASE("osi_dynamic_event concurrent post and delete", "[osi_event]")
{
    osi_thread_t *thread;
    struct osi_dynamic_event *event;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 16);
    event = test_dynamic_event_bind(thread, handler_count);

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(dynamic_poster_task, "osi_dev_p1", TEST_HELPER_STACK,
                                          event, TEST_WORKER_PRIO_LOW, NULL));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(dynamic_poster_task, "osi_dev_p2", TEST_HELPER_STACK,
                                          event, TEST_WORKER_PRIO_LOW, NULL));

    vTaskDelay(pdMS_TO_TICKS(30));
    osi_dynamic_event_delete(event);
    s_stop_posters = true;

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_join, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_join, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_FALSE(osi_dynamic_event_post(event, 0));

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_dynamic_event create post delete cycles in one session", "[osi_event]")
{
    osi_thread_t *thread;

    osi_event_test_begin();
    thread = test_thread_create(TEST_WORKER_PRIO_HIGH, 8);

    for (int cycle = 0; cycle < 16; cycle++) {
        struct osi_dynamic_event *event = test_dynamic_event_bind(thread, handler_count);

        TEST_ASSERT_TRUE(osi_dynamic_event_post(event, OSI_THREAD_MAX_TIMEOUT));
        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
        osi_dynamic_event_delete(event);
    }
    TEST_ASSERT_EQUAL_UINT32(16, s_run_count);

    osi_event_test_end(thread, NULL);
}

TEST_CASE("osi_event deinit retires queued session and dynamic events", "[osi_event]")
{
    UBaseType_t saved_prio = uxTaskPriorityGet(NULL);
    osi_thread_t *thread;
    struct osi_event *session_event;
    struct osi_dynamic_event *dynamic_event;

    osi_event_test_begin();
    vTaskPrioritySet(NULL, TEST_WORKER_PRIO_HIGH);
    thread = test_thread_create(TEST_WORKER_PRIO_LOW, 8);
    session_event = test_event_bind(thread, handler_count);
    dynamic_event = test_dynamic_event_bind(thread, handler_count);

    TEST_ASSERT_TRUE(osi_thread_post_event(session_event, 0));
    TEST_ASSERT_TRUE(osi_dynamic_event_post(dynamic_event, 0));
    osi_thread_event_deinit();
    osi_thread_free(thread);
    vTaskPrioritySet(NULL, saved_prio);

    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    vSemaphoreDelete(s_done);
    s_done = NULL;
    vSemaphoreDelete(s_gate);
    s_gate = NULL;
    vSemaphoreDelete(s_join);
    s_join = NULL;
    vTaskDelay(pdMS_TO_TICKS(20));
}
