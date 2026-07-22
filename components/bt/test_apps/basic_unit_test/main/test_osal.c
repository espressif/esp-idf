/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Unit tests for the BT OSAL porting layer (components/bt/common/osal): events and
 * the list-backed event queue, the queue worker task, mutexes, semaphores,
 * callouts, time helpers and critical sections.
 *
 * Every test brings the OSAL function table up and tears it down again, so the
 * per-test heap check in tearDown() also covers the layer's own allocations.
 */

#include <stdbool.h>
#include <stdint.h>

#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "bt_osal.h"
#include "bt_osal_freertos.h"

#define TEST_EVQ_STACK_SIZE   3072
#define TEST_EVQ_PRIO         (configMAX_PRIORITIES - 3)
#define TEST_HELPER_STACK     2560

/* Long enough for a blocking call to succeed, short enough to keep the suite quick. */
#define TEST_WAIT_MS          1000
/* Waited on when an operation is expected NOT to happen. */
#define TEST_NO_EVENT_MS      50

/* Shared state between the test task and the event/timer callbacks. */
static SemaphoreHandle_t s_done;
static volatile uint32_t s_run_count;
static struct bt_osal_event *s_last_ev;
static void *s_last_arg;

/* Attributes shared by the worker tasks started across these tests. */
static const struct bt_osal_task_info s_test_task_info = {
    .name = "osal_test_evq",
    .prio = TEST_EVQ_PRIO,
    .stack_size = TEST_EVQ_STACK_SIZE,
    .core_id = BT_OSAL_TASK_NO_AFFINITY,
};

/* Core the worker task dispatched the last event on; recorded by ev_cb_core(). */
static volatile BaseType_t s_last_core;

static void ev_cb_count(struct bt_osal_event *ev)
{
    s_run_count++;
    s_last_ev = ev;
    s_last_arg = bt_osal_event_get_arg(ev);
    xSemaphoreGive(s_done);
}

static void ev_cb_core(struct bt_osal_event *ev)
{
    s_last_core = xPortGetCoreID();
    ev_cb_count(ev);
}

/* Bring up the OSAL function table; every bt_osal_* call dispatches through it. */
static void osal_test_begin(void)
{
    s_run_count = 0;
    s_last_ev = NULL;
    s_last_arg = NULL;

    s_done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_done);

    bt_osal_freertos_funcs_init();
    TEST_ASSERT_NOT_NULL(bt_osal_freertos_funcs_get());
}

static void osal_test_end(void)
{
    bt_osal_freertos_funcs_deinit();
    TEST_ASSERT_NULL(bt_osal_freertos_funcs_get());

    vSemaphoreDelete(s_done);
    s_done = NULL;

    /* Tasks deleted during the test (queue workers, helpers) and timers deleted
     * by callout_deinit() are freed asynchronously by the idle and timer tasks;
     * give them a moment so the heap check in tearDown() is not tripped.
     */
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("osal event init, run, arg accessors and deinit", "[osal]")
{
    struct bt_osal_event ev = {0};

    osal_test_begin();

    bt_osal_event_init(&ev, ev_cb_count, (void *)0x1234);
    TEST_ASSERT_NOT_NULL(ev.event);
    TEST_ASSERT_FALSE(bt_osal_event_is_queued(&ev));
    TEST_ASSERT_EQUAL_PTR((void *)0x1234, bt_osal_event_get_arg(&ev));

    bt_osal_event_set_arg(&ev, (void *)0x5678);
    TEST_ASSERT_EQUAL_PTR((void *)0x5678, bt_osal_event_get_arg(&ev));

    /* Running an event outside a queue calls the handler in the caller's context. */
    bt_osal_event_run(&ev);
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL_PTR(&ev, s_last_ev);
    TEST_ASSERT_EQUAL_PTR((void *)0x5678, s_last_arg);

    bt_osal_event_deinit(&ev);
    TEST_ASSERT_NULL(ev.event);

    /* Deinit of an already released event is a no-op. */
    bt_osal_event_deinit(&ev);

    osal_test_end();
}

TEST_CASE("osal eventq keeps FIFO order and ignores a duplicate put", "[osal]")
{
    struct bt_osal_eventq evq = {0};
    struct bt_osal_event ev1 = {0};
    struct bt_osal_event ev2 = {0};
    struct bt_osal_event ev3 = {0};

    osal_test_begin();

    bt_osal_eventq_init(&evq);
    TEST_ASSERT_NOT_NULL(evq.eventq);
    TEST_ASSERT_TRUE(bt_osal_eventq_is_empty(&evq));
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, 0));

    bt_osal_event_init(&ev1, ev_cb_count, NULL);
    bt_osal_event_init(&ev2, ev_cb_count, NULL);
    bt_osal_event_init(&ev3, ev_cb_count, NULL);

    bt_osal_eventq_put(&evq, &ev1);
    bt_osal_eventq_put(&evq, &ev2);
    /* An event that is already queued must not be linked (or counted) twice. */
    bt_osal_eventq_put(&evq, &ev1);
    TEST_ASSERT_TRUE(bt_osal_event_is_queued(&ev1));
    TEST_ASSERT_FALSE(bt_osal_eventq_is_empty(&evq));

    /* put_to_front() jumps the queue. */
    bt_osal_eventq_put_to_front(&evq, &ev3);

    TEST_ASSERT_EQUAL_PTR(&ev3, bt_osal_eventq_get(&evq, 0));
    TEST_ASSERT_EQUAL_PTR(&ev1, bt_osal_eventq_get(&evq, 0));
    TEST_ASSERT_EQUAL_PTR(&ev2, bt_osal_eventq_get(&evq, 0));

    TEST_ASSERT_TRUE(bt_osal_eventq_is_empty(&evq));
    TEST_ASSERT_FALSE(bt_osal_event_is_queued(&ev1));
    /* The duplicate put left no extra token behind: the queue is really empty. */
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    bt_osal_event_deinit(&ev1);
    bt_osal_event_deinit(&ev2);
    bt_osal_event_deinit(&ev3);
    bt_osal_eventq_deinit(&evq);
    TEST_ASSERT_NULL(evq.eventq);

    osal_test_end();
}

TEST_CASE("osal eventq_remove unlinks head, middle and tail events", "[osal]")
{
    struct bt_osal_eventq evq = {0};
    struct bt_osal_event ev1 = {0};
    struct bt_osal_event ev2 = {0};
    struct bt_osal_event ev3 = {0};

    osal_test_begin();

    bt_osal_eventq_init(&evq);
    bt_osal_event_init(&ev1, ev_cb_count, NULL);
    bt_osal_event_init(&ev2, ev_cb_count, NULL);
    bt_osal_event_init(&ev3, ev_cb_count, NULL);

    /* Remove from the middle. */
    bt_osal_eventq_put(&evq, &ev1);
    bt_osal_eventq_put(&evq, &ev2);
    bt_osal_eventq_put(&evq, &ev3);
    bt_osal_eventq_remove(&evq, &ev2);
    TEST_ASSERT_FALSE(bt_osal_event_is_queued(&ev2));
    TEST_ASSERT_EQUAL_PTR(&ev1, bt_osal_eventq_get(&evq, 0));
    TEST_ASSERT_EQUAL_PTR(&ev3, bt_osal_eventq_get(&evq, 0));
    TEST_ASSERT_TRUE(bt_osal_eventq_is_empty(&evq));
    /* The removed event's token was reclaimed, so a get() must block and time out. */
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    /* Remove the head. */
    bt_osal_eventq_put(&evq, &ev1);
    bt_osal_eventq_put(&evq, &ev2);
    bt_osal_eventq_remove(&evq, &ev1);
    TEST_ASSERT_EQUAL_PTR(&ev2, bt_osal_eventq_get(&evq, 0));
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    /* Remove the tail. */
    bt_osal_eventq_put(&evq, &ev1);
    bt_osal_eventq_put(&evq, &ev2);
    bt_osal_eventq_remove(&evq, &ev2);
    TEST_ASSERT_EQUAL_PTR(&ev1, bt_osal_eventq_get(&evq, 0));
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    /* Removing an event that is not queued is a no-op. */
    bt_osal_eventq_remove(&evq, &ev3);
    TEST_ASSERT_TRUE(bt_osal_eventq_is_empty(&evq));

    /* The queue is reusable after all that shuffling. */
    bt_osal_eventq_put(&evq, &ev3);
    TEST_ASSERT_EQUAL_PTR(&ev3, bt_osal_eventq_get(&evq, 0));

    bt_osal_event_deinit(&ev1);
    bt_osal_event_deinit(&ev2);
    bt_osal_event_deinit(&ev3);
    bt_osal_eventq_deinit(&evq);

    osal_test_end();
}

TEST_CASE("osal eventq worker task dispatches queued events", "[osal]")
{
    struct bt_osal_eventq evq = {0};
    struct bt_osal_eventq not_inited = {0};
    struct bt_osal_event ev = {0};

    osal_test_begin();

    /* A queue that was never initialized has nothing to service. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL,
                          bt_osal_eventq_start(&not_inited, &s_test_task_info));

    bt_osal_eventq_init(&evq);
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK,
                          bt_osal_eventq_start(&evq, &s_test_task_info));
    /* Only one task may service a queue. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL,
                          bt_osal_eventq_start(&evq, &s_test_task_info));

    bt_osal_event_init(&ev, ev_cb_count, (void *)0xC0FFEE);
    bt_osal_eventq_put(&evq, &ev);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL_PTR(&ev, s_last_ev);
    TEST_ASSERT_EQUAL_PTR((void *)0xC0FFEE, s_last_arg);
    /* The worker cleared the queued flag before running the handler. */
    TEST_ASSERT_FALSE(bt_osal_event_is_queued(&ev));

    bt_osal_event_deinit(&ev);
    /* deinit() stops the worker task before releasing the queue it waits on. */
    bt_osal_eventq_deinit(&evq);

    osal_test_end();
}

TEST_CASE("osal eventq worker honours the requested core affinity", "[osal]")
{
    struct bt_osal_eventq evq = {0};
    struct bt_osal_event ev = {0};
    struct bt_osal_task_info info = s_test_task_info;

    osal_test_begin();

    bt_osal_eventq_init(&evq);

    /* A core that does not exist is rejected instead of asserting inside FreeRTOS. */
    info.core_id = portNUM_PROCESSORS;
    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL, bt_osal_eventq_start(&evq, &info));
    info.core_id = -2;
    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL, bt_osal_eventq_start(&evq, &info));

    /* Pinned to CPU0, the worker must dispatch from CPU0. */
    info.core_id = 0;
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_eventq_start(&evq, &info));

    s_last_core = -1;
    bt_osal_event_init(&ev, ev_cb_core, NULL);
    bt_osal_eventq_put(&evq, &ev);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_INT(0, s_last_core);

    bt_osal_event_deinit(&ev);
    bt_osal_eventq_deinit(&evq);

    osal_test_end();
}

TEST_CASE("osal eventq_post_func runs a one-shot handler", "[osal]")
{
    struct bt_osal_eventq evq = {0};

    osal_test_begin();

    bt_osal_eventq_init(&evq);
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK,
                          bt_osal_eventq_start(&evq, &s_test_task_info));

    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL, bt_osal_eventq_post_func(&evq, NULL, NULL));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL, bt_osal_eventq_post_func(NULL, ev_cb_count, NULL));

    /* The event is allocated by the layer and freed after the handler returns;
     * a leak would be caught by the heap check in tearDown().
     */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK,
                          bt_osal_eventq_post_func(&evq, ev_cb_count, NULL));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK,
                          bt_osal_eventq_post_func(&evq, ev_cb_count, (void *)0xABCD));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(2, s_run_count);
    TEST_ASSERT_EQUAL_PTR((void *)0xABCD, s_last_arg);

    bt_osal_eventq_deinit(&evq);

    osal_test_end();
}

TEST_CASE("osal eventq_deinit reclaims still-queued post_func items", "[osal]")
{
    struct bt_osal_eventq evq = {0};

    osal_test_begin();

    bt_osal_eventq_init(&evq);

    /* Post one-shot items but never start a worker, so they stay queued and are
     * never dispatched. Their backing storage is owned by the layer, so
     * eventq_deinit() must reclaim it; otherwise the heap check in tearDown()
     * would report a leak.
     */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_eventq_post_func(&evq, ev_cb_count, NULL));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_eventq_post_func(&evq, ev_cb_count, (void *)0x1));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_eventq_post_func(&evq, ev_cb_count, (void *)0x2));
    TEST_ASSERT_FALSE(bt_osal_eventq_is_empty(&evq));

    bt_osal_eventq_deinit(&evq);
    TEST_ASSERT_NULL(evq.eventq);
    /* The items were reclaimed on teardown, not dispatched. */
    TEST_ASSERT_EQUAL_UINT32(0, s_run_count);

    osal_test_end();
}

/* Producer for the concurrency test: posts every event of the array to the queue. */
static void eventq_producer_task(void *arg)
{
    struct bt_osal_eventq *evq = (struct bt_osal_eventq *)arg;
    static struct bt_osal_event evs[8];

    for (int i = 0; i < 8; i++) {
        evs[i] = (struct bt_osal_event){0};
        bt_osal_event_init(&evs[i], ev_cb_count, (void *)(uintptr_t)i);
        bt_osal_eventq_put(evq, &evs[i]);
    }

    /* Hand the events back to the test task so it can release them. */
    s_last_arg = evs;
    xSemaphoreGive(s_done);
    vTaskDelete(NULL);
}

TEST_CASE("osal eventq delivers every event under a concurrent producer", "[osal]")
{
    struct bt_osal_eventq evq = {0};
    struct bt_osal_event *evs;
    bool seen[8] = {false};

    osal_test_begin();

    bt_osal_eventq_init(&evq);

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(eventq_producer_task, "osal_prod", TEST_HELPER_STACK,
                                          &evq, uxTaskPriorityGet(NULL), NULL));

    /* Drain the queue: each event must come out exactly once. */
    for (int i = 0; i < 8; i++) {
        struct bt_osal_event *ev = bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_WAIT_MS));
        TEST_ASSERT_NOT_NULL(ev);

        uintptr_t idx = (uintptr_t)bt_osal_event_get_arg(ev);
        TEST_ASSERT_LESS_THAN_UINT32(8, idx);
        TEST_ASSERT_FALSE(seen[idx]);
        seen[idx] = true;
    }
    TEST_ASSERT_TRUE(bt_osal_eventq_is_empty(&evq));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    evs = (struct bt_osal_event *)s_last_arg;
    for (int i = 0; i < 8; i++) {
        bt_osal_event_deinit(&evs[i]);
    }

    bt_osal_eventq_deinit(&evq);

    osal_test_end();
}

/* Holds the mutex until the test task tells it to let go. */
static SemaphoreHandle_t s_holder_ready;

static void mutex_holder_task(void *arg)
{
    struct bt_osal_mutex *mu = (struct bt_osal_mutex *)arg;

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_pend(mu, BT_OSAL_TIME_FOREVER));
    xSemaphoreGive(s_holder_ready);

    /* Wait for the test task to finish its contended pend, then hand the mutex back. */
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_release(mu));

    xSemaphoreGive(s_done);
    vTaskDelete(NULL);
}

TEST_CASE("osal mutex is recursive and pend times out while held", "[osal]")
{
    struct bt_osal_mutex mu = {0};
    struct bt_osal_mutex not_inited = {0};

    osal_test_begin();

    /* Operations on a mutex that was never initialized are rejected. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_INVALID_PARAM, bt_osal_mutex_pend(&not_inited, 0));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_INVALID_PARAM, bt_osal_mutex_release(&not_inited));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_INVALID_PARAM, bt_osal_mutex_deinit(&not_inited));

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_init(&mu));
    TEST_ASSERT_NOT_NULL(mu.mutex);

    /* The owner may take it again without deadlocking. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_pend(&mu, 0));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_pend(&mu, 0));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_release(&mu));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_release(&mu));

    s_holder_ready = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_holder_ready);

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(mutex_holder_task, "osal_hold", TEST_HELPER_STACK,
                                          &mu, uxTaskPriorityGet(NULL), NULL));

    /* Another task owns the mutex now: a bounded pend must give up. */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_holder_ready, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_TIMEOUT, bt_osal_mutex_pend(&mu, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    /* Once the holder releases it, the same pend succeeds. */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_pend(&mu, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_release(&mu));

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_mutex_deinit(&mu));
    TEST_ASSERT_NULL(mu.mutex);

    vSemaphoreDelete(s_holder_ready);
    s_holder_ready = NULL;

    osal_test_end();
}

static void sem_releaser_task(void *arg)
{
    struct bt_osal_sem *sem = (struct bt_osal_sem *)arg;

    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_release(sem));

    vTaskDelete(NULL);
}

TEST_CASE("osal semaphore counts tokens and blocks until released", "[osal]")
{
    struct bt_osal_sem sem = {0};
    struct bt_osal_sem not_inited = {0};

    osal_test_begin();

    TEST_ASSERT_EQUAL_INT(BT_OSAL_INVALID_PARAM, bt_osal_sem_pend(&not_inited, 0));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_INVALID_PARAM, bt_osal_sem_release(&not_inited));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_INVALID_PARAM, bt_osal_sem_deinit(&not_inited));

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_init(&sem, 2));
    TEST_ASSERT_NOT_NULL(sem.sem);
    TEST_ASSERT_EQUAL_UINT16(2, bt_osal_sem_get_count(&sem));

    /* Drain the initial tokens. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_pend(&sem, 0));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_pend(&sem, 0));
    TEST_ASSERT_EQUAL_UINT16(0, bt_osal_sem_get_count(&sem));

    /* No token left: a bounded pend gives up. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_TIMEOUT, bt_osal_sem_pend(&sem, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_release(&sem));
    TEST_ASSERT_EQUAL_UINT16(1, bt_osal_sem_get_count(&sem));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_pend(&sem, 0));

    /* A release from another task wakes a blocked pend. */
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(sem_releaser_task, "osal_sem", TEST_HELPER_STACK,
                                          &sem, uxTaskPriorityGet(NULL), NULL));
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_pend(&sem, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT16(0, bt_osal_sem_get_count(&sem));

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_sem_deinit(&sem));
    TEST_ASSERT_NULL(sem.sem);

    osal_test_end();
}

TEST_CASE("osal callout posts its event to the event queue on expiry", "[osal]")
{
    struct bt_osal_eventq evq = {0};
    struct bt_osal_callout co = {0};
    struct bt_osal_event *ev;
    bt_osal_time_t ticks;
    bt_osal_time_t remaining;

    osal_test_begin();

    bt_osal_eventq_init(&evq);
    TEST_ASSERT_EQUAL_INT(0, bt_osal_callout_init(&co, &evq, ev_cb_count, (void *)0x99));
    TEST_ASSERT_NOT_NULL(co.co);
    TEST_ASSERT_FALSE(bt_osal_callout_is_active(&co));

    ticks = bt_osal_time_ms_to_ticks32(200);
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_callout_reset(&co, ticks));

    /* The FreeRTOS timer daemon arms the timer asynchronously, so let it run
     * before inspecting the callout's state.
     */
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_TRUE(bt_osal_callout_is_active(&co));

    /* An armed callout expires in the future and has time left to run. */
    TEST_ASSERT_GREATER_THAN_UINT32(bt_osal_time_get(), bt_osal_callout_get_ticks(&co));
    remaining = bt_osal_callout_remaining_ticks(&co, bt_osal_time_get());
    TEST_ASSERT_GREATER_THAN_UINT32(0, remaining);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(ticks, remaining);

    /* Nothing is delivered before the callout expires. */
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_NO_EVENT_MS)));

    /* On expiry the callout's event lands in the queue, ready to be run. */
    ev = bt_osal_eventq_get(&evq, pdMS_TO_TICKS(TEST_WAIT_MS));
    TEST_ASSERT_NOT_NULL(ev);
    bt_osal_event_run(ev);
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL_PTR((void *)0x99, s_last_arg);
    /* A callout is one-shot. */
    TEST_ASSERT_FALSE(bt_osal_callout_is_active(&co));

    /* Stopping an armed callout keeps its event out of the queue. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_callout_reset(&co, bt_osal_time_ms_to_ticks32(100)));
    bt_osal_callout_stop(&co);
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_FALSE(bt_osal_callout_is_active(&co));
    TEST_ASSERT_NULL(bt_osal_eventq_get(&evq, pdMS_TO_TICKS(200)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);

    bt_osal_callout_deinit(&co);
    TEST_ASSERT_NULL(co.co);
    /* Deinit of an already released callout is a no-op. */
    bt_osal_callout_deinit(&co);

    bt_osal_eventq_deinit(&evq);

    osal_test_end();
}

TEST_CASE("osal callout without an event queue runs its handler inline", "[osal]")
{
    struct bt_osal_callout co = {0};

    osal_test_begin();

    /* With no event queue the handler is called straight from timer context. */
    TEST_ASSERT_EQUAL_INT(0, bt_osal_callout_init(&co, NULL, ev_cb_count, (void *)0x77));

    bt_osal_callout_set_arg(&co, (void *)0x88);
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_callout_reset(&co, bt_osal_time_ms_to_ticks32(50)));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL_PTR((void *)0x88, s_last_arg);

    /* The callout can be re-armed after firing. */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_callout_reset(&co, bt_osal_time_ms_to_ticks32(50)));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(2, s_run_count);

    bt_osal_callout_deinit(&co);

    osal_test_end();
}

TEST_CASE("osal time helpers round-trip between ms and ticks", "[osal]")
{
    bt_osal_time_t ticks = 0;
    uint32_t ms = 0;
    uint32_t elapsed;
    uint32_t start;

    osal_test_begin();

    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_time_ms_to_ticks(1000, &ticks));
    TEST_ASSERT_GREATER_THAN_UINT32(0, ticks);
    TEST_ASSERT_EQUAL_INT(BT_OSAL_OK, bt_osal_time_ticks_to_ms(ticks, &ms));
    TEST_ASSERT_EQUAL_UINT32(1000, ms);

    /* The 32-bit variants must agree with the checked ones. */
    TEST_ASSERT_EQUAL_UINT32(ticks, bt_osal_time_ms_to_ticks32(1000));
    TEST_ASSERT_EQUAL_UINT32(1000, bt_osal_time_ticks_to_ms32(ticks));

#if !BT_OSAL_USE_ESP_TIMER && (configTICK_RATE_HZ < 1000)
    /* One tick is more than one millisecond here, so a full-range tick count
     * cannot be expressed in milliseconds.
     */
    TEST_ASSERT_EQUAL_INT(BT_OSAL_EINVAL, bt_osal_time_ticks_to_ms(UINT32_MAX, &ms));
#endif

    /* The clock advances by at least the requested delay. */
    start = bt_osal_time_get();
    bt_osal_time_delay(bt_osal_time_ms_to_ticks32(50));
    elapsed = bt_osal_time_get() - start;
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(bt_osal_time_ms_to_ticks32(40), elapsed);

    osal_test_end();
}

TEST_CASE("osal reports OS state and nests critical sections", "[osal]")
{
    uint32_t outer;
    uint32_t inner;
    bool in_outer;
    bool in_inner;
    bool after_inner;
    bool after_outer;

    osal_test_begin();

    TEST_ASSERT_TRUE(bt_osal_os_started());
    TEST_ASSERT_EQUAL_PTR(xTaskGetCurrentTaskHandle(), bt_osal_get_current_task_id());
    TEST_ASSERT_EQUAL_UINT32(portMAX_DELAY, BT_OSAL_TIME_FOREVER);
    TEST_ASSERT_FALSE(bt_osal_hw_is_in_critical());

    /* Sample the state inside the critical section but assert outside it: a failing
     * assertion longjmps out of the test and would leave the spinlock held.
     */
    outer = bt_osal_hw_enter_critical();
    in_outer = bt_osal_hw_is_in_critical();
    inner = bt_osal_hw_enter_critical();
    in_inner = bt_osal_hw_is_in_critical();
    bt_osal_hw_exit_critical(inner);
    after_inner = bt_osal_hw_is_in_critical();
    bt_osal_hw_exit_critical(outer);
    after_outer = bt_osal_hw_is_in_critical();

    TEST_ASSERT_TRUE(in_outer);
    TEST_ASSERT_TRUE(in_inner);
    /* Leaving the inner section must not clear the outer one. */
    TEST_ASSERT_TRUE(after_inner);
    TEST_ASSERT_FALSE(after_outer);

    osal_test_end();
}
