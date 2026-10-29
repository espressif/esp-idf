/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * osi_alarm posts its callback without blocking the esp_timer task.
 * A full target queue re-arms the same timer with a tiered delay. A periodic
 * alarm keeps that phase and returns to a periodic esp_timer after a post succeeds.
 */

#include <string.h>

#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "btc/btc_task.h"
#include "osi/alarm.h"
#include "osi/allocator.h"
#include "osi/thread.h"

extern void test_case_cleanup_set(void (*cleanup)(void));

uint32_t osi_alarm_test_dispatch_id(const osi_alarm_t *alarm);
void osi_alarm_test_dispatch(uint32_t dispatch_id);
void osi_alarm_test_set_invoke_hook(void (*hook)(void));
void osi_alarm_test_set_cancel_hook(void (*hook)(void));
void osi_alarm_test_set_timer_hook(void (*hook)(void));

#define TEST_WORKER_STACK   3072
#define TEST_WORKER_PRIO    2
#define TEST_WAIT_MS        1000

#define PHASE_STAMP_MAX 8
#define PHASE_PERIOD_US (50 * 1000)
#define PHASE_WINDOW_US (20 * 1000)

static SemaphoreHandle_t s_block;
static SemaphoreHandle_t s_done;
static SemaphoreHandle_t s_hit;
static SemaphoreHandle_t s_go;
static volatile uint32_t s_count;
static volatile TaskHandle_t s_cb_task;
static volatile bool s_worker_timed_out;
static osi_thread_t *s_worker;
static int64_t s_stamps[PHASE_STAMP_MAX];
static volatile uint32_t s_stamp_n;

static void alarm_test_end(void);
static void alarm_btc_end(void);
static void gates_begin(void);
static void gates_end(void);
static void stall_timer(void);
static void give_block_soon(void *arg);

static void alarm_cb(void *arg)
{
    uint32_t n;

    (void)arg;
    s_cb_task = xTaskGetCurrentTaskHandle();
    n = s_stamp_n;
    if (n < PHASE_STAMP_MAX) {
        s_stamps[n] = esp_timer_get_time();
        __sync_synchronize();
        s_stamp_n = n + 1;
    }
    s_count++;
    xSemaphoreGive(s_done);
}

/* Record a timeout for the test task. Unity asserts belong on that task. */
static void worker_take(SemaphoreHandle_t sem, uint32_t wait_ms)
{
    if (sem == NULL || xSemaphoreTake(sem, pdMS_TO_TICKS(wait_ms)) != pdTRUE) {
        s_worker_timed_out = true;
    }
}

static void blocker(void *arg)
{
    (void)arg;
    worker_take(s_block, 3000);
}

static void dummy_work(void *arg)
{
    (void)arg;
}

static void alarm_sems_begin(void)
{
    osi_alarm_test_set_invoke_hook(NULL);
    osi_alarm_test_set_cancel_hook(NULL);
    osi_alarm_test_set_timer_hook(NULL);
    s_count = 0;
    s_stamp_n = 0;
    s_cb_task = NULL;
    s_worker_timed_out = false;
    s_block = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_block);
    s_done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_done);
}

static void alarm_sems_end(void)
{
    if (s_block != NULL) {
        vSemaphoreDelete(s_block);
        s_block = NULL;
    }
    if (s_done != NULL) {
        vSemaphoreDelete(s_done);
        s_done = NULL;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
}

/* Cancel can lose the race with an in-flight retry. After the worker is
 * released, that callback signals s_done; drop the token with the count. */
static void alarm_count_clear(void)
{
    s_count = 0;
    s_stamp_n = 0;
    if (s_done != NULL) {
        while (xSemaphoreTake(s_done, 0) == pdTRUE) {
        }
    }
}

static void alarm_thread_free(osi_thread_t *thread)
{
    if (thread == NULL) {
        return;
    }
    if (s_worker == thread) {
        s_worker = NULL;
    }
    osi_thread_free(thread);
}

static void alarm_release_workers(void)
{
    int released = 0;

    if (s_block != NULL) {
        xSemaphoreGive(s_block);
        released = 1;
    }
    if (s_go != NULL) {
        xSemaphoreGive(s_go);
        released = 1;
    }
    /* A failed case may still be blocked in the worker. Let that callback finish. */
    if (released && Unity.CurrentTestFailed != 0) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void alarm_test_end(void)
{
    int mux;

    test_case_cleanup_set(NULL);
    osi_alarm_test_set_invoke_hook(NULL);
    osi_alarm_test_set_cancel_hook(NULL);
    osi_alarm_test_set_timer_hook(NULL);
    alarm_release_workers();
    alarm_thread_free(s_worker);
    osi_alarm_deinit();
    mux = osi_alarm_delete_mux();
    osi_thread_event_deinit();
    if (s_hit != NULL) {
        gates_end();
    }
    alarm_sems_end();
    if (Unity.CurrentTestFailed == 0) {
        TEST_ASSERT_FALSE(s_worker_timed_out);
        TEST_ASSERT_EQUAL(0, mux);
    }
}

static void alarm_test_begin(void)
{
    test_case_cleanup_set(alarm_test_end);
    s_worker = NULL;
    alarm_sems_begin();
    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();
}

static osi_thread_t *test_thread_create(size_t queue_len)
{
    const size_t workqueue_len[] = {queue_len};
    osi_thread_t *thread = osi_thread_create("alarm_test", TEST_WORKER_STACK, TEST_WORKER_PRIO,
                                             OSI_THREAD_CORE_0, 1, workqueue_len, false);
    TEST_ASSERT_NOT_NULL(thread);
    s_worker = thread;
    return thread;
}

/* Block the worker, then fill every queue slot so the next post fails. */
static void fill_queue(osi_thread_t *thread)
{
    size_t posted = 0;

    TEST_ASSERT_TRUE(osi_thread_post(thread, blocker, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(30));
    while (osi_thread_post(thread, dummy_work, NULL, NULL, 0, 0)) {
        posted++;
        TEST_ASSERT_LESS_THAN(200, posted);
    }
    TEST_ASSERT_GREATER_THAN(0, posted);
}

static void release_queue(void)
{
    xSemaphoreGive(s_block);
}

static uint32_t max_remaining_after(osi_alarm_t *alarm, uint32_t timeout_ms, uint32_t window_ms)
{
    int64_t start = esp_timer_get_time();
    int64_t ready = start + (int64_t)timeout_ms * 1000 + 2000;
    int64_t end = ready + (int64_t)window_ms * 1000;
    uint32_t max_left = 0;

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, timeout_ms));
    while (esp_timer_get_time() < end) {
        if (esp_timer_get_time() >= ready) {
            uint32_t left = (uint32_t)osi_alarm_get_remaining_ms(alarm);
            if (left > max_left) {
                max_left = left;
            }
        }
    }
    return max_left;
}

/* This image builds Unity without 64-bit asserts. */
static void assert_us_near(int64_t expected, int64_t actual)
{
    int64_t delta = actual - expected;

    if (delta < 0) {
        delta = -delta;
    }
    if (delta > 0x7fffffffLL) {
        delta = 0x7fffffffLL;
    }
    TEST_ASSERT_LESS_OR_EQUAL_INT32(PHASE_WINDOW_US, (int32_t)delta);
}

/* Later callbacks stay on the phase captured at arm time. The first one is
 * the retry that ran once the queue opened, so it is not on that grid. */
static void assert_kept_phase(int64_t origin)
{
    uint32_t n = s_stamp_n;
    uint32_t i;
    int64_t next;

    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(3, n);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(PHASE_STAMP_MAX, n);
    TEST_ASSERT_TRUE(s_stamps[0] > origin + PHASE_PERIOD_US);

    next = origin + PHASE_PERIOD_US;
    while (next <= s_stamps[0]) {
        next += PHASE_PERIOD_US;
    }
    /* A phase reset arms the next tick one period after the retry. That
     * prediction has to sit outside the kept-phase window. */
    {
        int64_t reset_next = s_stamps[0] + PHASE_PERIOD_US;
        int64_t apart = reset_next > next ? (reset_next - next) : (next - reset_next);

        TEST_ASSERT_TRUE(apart > PHASE_WINDOW_US);
    }
    for (i = 1; i < n; i++) {
        assert_us_near(next, s_stamps[i]);
        next += PHASE_PERIOD_US;
    }
}

static void expect_callback_once(void)
{
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL(1, s_count);
    TEST_ASSERT_NOT_NULL(s_cb_task);
    TEST_ASSERT_NULL(strstr(pcTaskGetName(s_cb_task), "esp_timer"));
}

TEST_CASE("osi_alarm one-shot retries with a tiered delay on a full task queue", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    uint32_t left;

    alarm_test_begin();
    thread = test_thread_create(4);
    fill_queue(thread);
    alarm = osi_alarm_new("tier", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);

    left = max_remaining_after(alarm, 10, 20);
    TEST_ASSERT_LESS_OR_EQUAL(1, left);
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));

    osi_alarm_cancel(alarm);
    release_queue();
    vTaskDelay(pdMS_TO_TICKS(30));
    alarm_count_clear();

    fill_queue(thread);
    left = max_remaining_after(alarm, 50, 30);
    TEST_ASSERT_GREATER_OR_EQUAL(2, left);
    TEST_ASSERT_LESS_OR_EQUAL(5, left);

    osi_alarm_cancel(alarm);
    release_queue();
    vTaskDelay(pdMS_TO_TICKS(30));
    alarm_count_clear();

    fill_queue(thread);
    left = max_remaining_after(alarm, 200, 40);
    TEST_ASSERT_GREATER_OR_EQUAL(6, left);
    TEST_ASSERT_LESS_OR_EQUAL(10, left);
    TEST_ASSERT_EQUAL(0, s_count);

    release_queue();
    expect_callback_once();
    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_EQUAL(1, s_count);

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm set_deadline_us keeps a one-shot retry grade", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    int64_t start;
    int64_t ready;
    int64_t end;
    uint32_t max_left = 0;

    alarm_test_begin();
    thread = test_thread_create(4);
    fill_queue(thread);
    alarm = osi_alarm_new("deadline", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);

    /* 200 ms grades a 10 ms retry. Clearing the phase must not turn that into 1 ms. */
    start = esp_timer_get_time();
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 200));
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set_deadline_us(alarm, 0));

    ready = start + 200 * 1000 + 2000;
    end = ready + 20 * 1000;
    while (esp_timer_get_time() < end) {
        if (esp_timer_get_time() >= ready) {
            uint32_t left = (uint32_t)osi_alarm_get_remaining_ms(alarm);

            if (left > max_left) {
                max_left = left;
            }
        }
    }
    TEST_ASSERT_GREATER_OR_EQUAL(6, max_left);
    TEST_ASSERT_LESS_OR_EQUAL(10, max_left);
    TEST_ASSERT_EQUAL(0, s_count);

    release_queue();
    expect_callback_once();
    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

/* NULL stops any active timer. A sooner one-shot stays armed. A periodic
 * timer has no remaining time, so a threshold leaves it armed. */
TEST_CASE("osi_alarm cancel_active stops only a later timer", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    period_ms_t sooner;

    alarm_test_begin();
    thread = test_thread_create(4);
    alarm = osi_alarm_new("cancel-active", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_INVALID_STATE, osi_alarm_cancel_active(alarm, NULL));
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 200));
    sooner = 100;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel_active(alarm, &sooner));
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 200));
    sooner = 300;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_FAIL, osi_alarm_cancel_active(alarm, &sooner));
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set_periodic(alarm, 1000));
    sooner = 100000;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_FAIL, osi_alarm_cancel_active(alarm, &sooner));
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel_active(alarm, NULL));
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));
    TEST_ASSERT_EQUAL(0, s_count);

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm periodic post retries once then resumes the period", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    uint32_t count_at_first;
    int64_t origin;

    alarm_test_begin();
    thread = test_thread_create(4);
    fill_queue(thread);
    alarm = osi_alarm_new("period", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    origin = esp_timer_get_time();
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set_periodic(alarm, 50));

    vTaskDelay(pdMS_TO_TICKS(80));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));
    /* The retry is a one-shot, but this alarm is still periodic. */
    TEST_ASSERT_EQUAL(0, (uint32_t)osi_alarm_get_remaining_ms(alarm));

    release_queue();
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    count_at_first = s_count;
    TEST_ASSERT_GREATER_OR_EQUAL(1, count_at_first);
    TEST_ASSERT_LESS_OR_EQUAL(2, count_at_first);
    TEST_ASSERT_NULL(strstr(pcTaskGetName(s_cb_task), "esp_timer"));

    vTaskDelay(pdMS_TO_TICKS(120));
    TEST_ASSERT_LESS_OR_EQUAL(6, s_count);
    assert_kept_phase(origin);
    TEST_ASSERT_EQUAL(0, (uint32_t)osi_alarm_get_remaining_ms(alarm));

    count_at_first = s_count;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    vTaskDelay(pdMS_TO_TICKS(80));
    TEST_ASSERT_EQUAL(count_at_first, s_count);

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm NULL post_task fails when the BTC task is absent", "[osi_alarm]")
{
    alarm_test_begin();
    TEST_ASSERT_NULL(btc_get_current_thread());
    TEST_ASSERT_NULL(osi_alarm_new("no-btc", alarm_cb, NULL, NULL));
    alarm_test_end();
}

TEST_CASE("osi_alarm NULL post_task retries on the BTC task and runs the callback once", "[osi_alarm]")
{
    osi_alarm_t *alarm;
    osi_thread_t *btc;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();
    btc = btc_get_current_thread();
    TEST_ASSERT_NOT_NULL(btc);
    fill_queue(btc);

    alarm = osi_alarm_new("btc", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));

    vTaskDelay(pdMS_TO_TICKS(40));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));
    TEST_ASSERT_LESS_OR_EQUAL(1, (uint32_t)osi_alarm_get_remaining_ms(alarm));

    release_queue();
    expect_callback_once();
    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_EQUAL(1, s_count);

    osi_alarm_free(alarm);
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    btc_deinit();
    alarm_sems_end();
}

/*
 * A one-shot alarm runs on the BTC task, and mux teardown is idempotent.
 *
 *   alarm_set(1ms) --> esp_timer --> BTC --> alarm_cb
 *   delete_mux #2 --> 0 (the mesh adapter frees the mux first)
 *   deinit without a mux --> no-op (the NimBLE blufi path)
 */
TEST_CASE("osi_alarm alarm cycle and idempotent teardown", "[osi_alarm]")
{
    osi_alarm_t *alarm;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    alarm = osi_alarm_new("ut_alarm", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 1));
    expect_callback_once();

    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_alarm_deinit();

    btc_deinit();
    alarm_sems_end();
}

/* No queued post: deinit still drops the listener and delete_mux frees the mutex. */
TEST_CASE("osi_alarm deinit without a queued post frees the mux", "[osi_alarm]")
{
    osi_alarm_t *alarm;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    alarm = osi_alarm_new("btc-idle", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 1000));

    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    btc_deinit();
    TEST_ASSERT_NULL(btc_get_current_thread());
    TEST_ASSERT_EQUAL(0, s_count);
    alarm_sems_end();
}

/* DEINIT has already run. One BTC post is still queued behind a blocked worker.
 * Drain must drop that ref and unregister the listener before event teardown.
 * delete_mux returning -1 is the mesh host path: btc_deinit still runs.
 */
TEST_CASE("osi_alarm deinit drains a queued BTC post before event teardown", "[osi_alarm]")
{
    osi_alarm_t *alarm;
    osi_thread_t *btc;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();
    btc = btc_get_current_thread();
    TEST_ASSERT_NOT_NULL(btc);
    TEST_ASSERT_TRUE(osi_thread_post(btc, blocker, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(30));

    alarm = osi_alarm_new("btc-queued", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 1000));
    osi_alarm_test_dispatch(osi_alarm_test_dispatch_id(alarm));
    TEST_ASSERT_EQUAL(0, s_count);

    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(-1, osi_alarm_delete_mux());
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(give_block_soon, "give_blk", 2048, NULL, 3, NULL));
    btc_deinit();
    TEST_ASSERT_NULL(btc_get_current_thread());
    TEST_ASSERT_EQUAL(0, s_count);
    alarm_sems_end();
}

/* The worker is freed while the alarm is still armed. The callback must not run. */
TEST_CASE("osi_alarm drops a one-shot when its thread is freed first", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    thread = test_thread_create(4);
    alarm = osi_alarm_new("drop-once", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 30));

    alarm_thread_free(thread);
    vTaskDelay(pdMS_TO_TICKS(80));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_free(alarm);
    alarm_test_end();
}

/* Stall the tick inside the timer hook, then free the thread. detach cannot
 * stop an expiry that already entered the callback, so the handler must drop it. */
TEST_CASE("osi_alarm drops an in-flight tick when its thread is freed", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    gates_begin();
    thread = test_thread_create(4);
    osi_alarm_test_set_timer_hook(stall_timer);
    alarm = osi_alarm_new("inflight", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_hit, pdMS_TO_TICKS(TEST_WAIT_MS)));

    alarm_thread_free(thread);
    xSemaphoreGive(s_go);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_test_set_timer_hook(NULL);
    osi_alarm_free(alarm);
    alarm_test_end();
}

TEST_CASE("osi_alarm drops a periodic timer when its thread is freed", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    uint32_t count;

    alarm_test_begin();
    thread = test_thread_create(4);
    alarm = osi_alarm_new("drop-period", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set_periodic(alarm, 20));

    vTaskDelay(pdMS_TO_TICKS(30));
    alarm_thread_free(thread);
    count = s_count;
    vTaskDelay(pdMS_TO_TICKS(80));
    TEST_ASSERT_EQUAL(count, s_count);
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_free(alarm);
    alarm_test_end();
}

/* osi_thread_free joins the worker. Give s_block only after that join is
 * waiting, so stop is already set and the queued retry is discarded. */
static void give_block_soon(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(20));
    if (s_block != NULL) {
        xSemaphoreGive(s_block);
    }
    vTaskDelete(NULL);
}

TEST_CASE("osi_alarm stops retrying after its thread is freed", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    thread = test_thread_create(4);
    fill_queue(thread);
    alarm = osi_alarm_new("drop-retry", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));

    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(give_block_soon, "give_blk", 2048, NULL, 3, NULL));
    alarm_thread_free(thread);
    release_queue();
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);

    osi_alarm_free(alarm);
    alarm_test_end();
}

TEST_CASE("osi_alarm one-shot can be armed again after it fires", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    thread = test_thread_create(4);
    alarm = osi_alarm_new("rearm", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 1));
    expect_callback_once();
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    s_count = 0;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 1));
    expect_callback_once();
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm rejects creation once ALARM_CBS_NUM alarms exist", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t **alarms;
    osi_alarm_t *extra;

    alarm_test_begin();
    thread = test_thread_create(4);
    alarms = osi_calloc(ALARM_CBS_NUM * sizeof(*alarms));
    TEST_ASSERT_NOT_NULL(alarms);

    for (int i = 0; i < ALARM_CBS_NUM; i++) {
        alarms[i] = osi_alarm_new("pool", alarm_cb, NULL, thread);
        TEST_ASSERT_NOT_NULL(alarms[i]);
    }
    extra = osi_alarm_new("pool-full", alarm_cb, NULL, thread);
    TEST_ASSERT_NULL(extra);

    osi_alarm_free(alarms[0]);
    alarms[0] = osi_alarm_new("pool-reuse", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarms[0]);
    extra = osi_alarm_new("pool-full-again", alarm_cb, NULL, thread);
    TEST_ASSERT_NULL(extra);

    for (int i = 0; i < ALARM_CBS_NUM; i++) {
        osi_alarm_free(alarms[i]);
    }
    osi_free(alarms);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm cancel stops a retry and a late dispatch", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    uint32_t dispatch_id;

    alarm_test_begin();
    thread = test_thread_create(4);
    fill_queue(thread);
    alarm = osi_alarm_new("cancel-retry", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    dispatch_id = osi_alarm_test_dispatch_id(alarm);
    TEST_ASSERT_NOT_EQUAL(0, dispatch_id);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));

    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    release_queue();
    osi_alarm_test_dispatch(dispatch_id);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);

    s_count = 0;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 1));
    expect_callback_once();

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm set replaces a post-failure retry", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    thread = test_thread_create(4);
    fill_queue(thread);
    alarm = osi_alarm_new("replace-retry", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));

    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(alarm));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 200));
    release_queue();
    vTaskDelay(pdMS_TO_TICKS(40));
    TEST_ASSERT_EQUAL(0, s_count);

    expect_callback_once();
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm drops a stale dispatch id after free and reuse", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *first;
    osi_alarm_t *second;
    uint32_t stale_id;
    int marker_a = 1;
    int marker_b = 2;

    alarm_test_begin();
    thread = test_thread_create(4);
    first = osi_alarm_new("stale-a", alarm_cb, &marker_a, thread);
    TEST_ASSERT_NOT_NULL(first);
    stale_id = osi_alarm_test_dispatch_id(first);
    TEST_ASSERT_NOT_EQUAL(0, stale_id);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(first, 1000));
    osi_alarm_free(first);

    second = osi_alarm_new("stale-b", alarm_cb, &marker_b, thread);
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_NOT_EQUAL(stale_id, osi_alarm_test_dispatch_id(second));
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(second, 1000));

    osi_alarm_test_dispatch(stale_id);
    vTaskDelay(pdMS_TO_TICKS(40));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_TRUE(osi_alarm_is_active(second));

    osi_alarm_free(second);
    alarm_thread_free(thread);
    alarm_test_end();
}

static osi_alarm_t *s_hook_alarm;
static volatile osi_alarm_err_t s_hook_set;

static void gates_begin(void)
{
    s_hit = xSemaphoreCreateBinary();
    s_go = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_hit);
    TEST_ASSERT_NOT_NULL(s_go);
}

static void gates_end(void)
{
    if (s_hit != NULL) {
        vSemaphoreDelete(s_hit);
        s_hit = NULL;
    }
    if (s_go != NULL) {
        vSemaphoreDelete(s_go);
        s_go = NULL;
    }
}

static void alarm_btc_end(void)
{
    int mux;

    test_case_cleanup_set(NULL);
    osi_alarm_test_set_invoke_hook(NULL);
    osi_alarm_test_set_cancel_hook(NULL);
    osi_alarm_test_set_timer_hook(NULL);
    alarm_release_workers();
    osi_alarm_deinit();
    mux = osi_alarm_delete_mux();
    btc_deinit();
    if (s_hit != NULL) {
        gates_end();
    }
    alarm_sems_end();
    if (Unity.CurrentTestFailed == 0) {
        TEST_ASSERT_FALSE(s_worker_timed_out);
        TEST_ASSERT_EQUAL(0, mux);
    }
}

static void block_worker(osi_thread_t *thread)
{
    TEST_ASSERT_TRUE(osi_thread_post(thread, blocker, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(30));
}

static void wait_until_idle(osi_alarm_t *alarm)
{
    int64_t end = esp_timer_get_time() + 500 * 1000;

    while (osi_alarm_is_active(alarm) && esp_timer_get_time() < end) {
        vTaskDelay(1);
    }
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));
    /* The one-shot is disarmed before its post returns. Wait that post out. */
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_EQUAL(0, s_count);
}

static void pause_invoke(void)
{
    xSemaphoreGive(s_hit);
    worker_take(s_go, 3000);
}

static void hold_cb(void *arg)
{
    (void)arg;
    s_count++;
    xSemaphoreGive(s_hit);
    worker_take(s_go, 3000);
}

static void set_during_cancel(void)
{
    s_hook_set = osi_alarm_set(s_hook_alarm, 1000);
}

static void stall_timer(void)
{
    xSemaphoreGive(s_hit);
    worker_take(s_go, 3000);
}

static void cancel_reached(void)
{
    xSemaphoreGive(s_done);
}

static void release_stalled_timer(void *arg)
{
    (void)arg;
    if (s_done != NULL && xSemaphoreTake(s_done, pdMS_TO_TICKS(2000)) == pdTRUE) {
        xSemaphoreGive(s_go);
    } else {
        s_worker_timed_out = true;
    }
    vTaskDelete(NULL);
}

TEST_CASE("osi_alarm cancel drops a queued callback", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    thread = test_thread_create(4);
    block_worker(thread);
    alarm = osi_alarm_new("queued", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    wait_until_idle(alarm);
    TEST_ASSERT_EQUAL(0, s_count);

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    release_queue();
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm cancel drops a dequeued callback before it runs", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    gates_begin();
    thread = test_thread_create(4);
    osi_alarm_test_set_invoke_hook(pause_invoke);
    alarm = osi_alarm_new("dequeued", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_hit, pdMS_TO_TICKS(TEST_WAIT_MS)));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    TEST_ASSERT_EQUAL(0, s_count);
    xSemaphoreGive(s_go);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);

    osi_alarm_test_set_invoke_hook(NULL);
    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    gates_end();
    alarm_test_end();
}

TEST_CASE("osi_alarm cancel returns fail once the callback has started", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    uint32_t count;

    alarm_test_begin();
    gates_begin();
    thread = test_thread_create(4);
    alarm = osi_alarm_new("running", hold_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set_periodic(alarm, 30));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_hit, pdMS_TO_TICKS(TEST_WAIT_MS)));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_FAIL, osi_alarm_cancel(alarm));
    count = s_count;
    xSemaphoreGive(s_go);
    vTaskDelay(pdMS_TO_TICKS(80));
    TEST_ASSERT_EQUAL(count, s_count);
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    gates_end();
    alarm_test_end();
}

TEST_CASE("osi_alarm cancel drops a queued BTC callback", "[osi_alarm]")
{
    osi_alarm_t *alarm;
    osi_thread_t *btc;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    gates_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();
    btc = btc_get_current_thread();
    TEST_ASSERT_NOT_NULL(btc);
    block_worker(btc);

    alarm = osi_alarm_new("btc-queued", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    wait_until_idle(alarm);

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    release_queue();
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_free(alarm));
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    btc_deinit();
    gates_end();
    alarm_sems_end();
}

TEST_CASE("osi_alarm cancel drops a dequeued BTC callback before it runs", "[osi_alarm]")
{
    osi_alarm_t *alarm;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    gates_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();
    osi_alarm_test_set_invoke_hook(pause_invoke);

    alarm = osi_alarm_new("btc-dequeued", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_hit, pdMS_TO_TICKS(TEST_WAIT_MS)));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    TEST_ASSERT_EQUAL(0, s_count);
    xSemaphoreGive(s_go);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(0, s_count);

    osi_alarm_test_set_invoke_hook(NULL);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_free(alarm));
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    btc_deinit();
    gates_end();
    alarm_sems_end();
}

TEST_CASE("osi_alarm cancel stops a full BTC queue from retrying", "[osi_alarm]")
{
    osi_alarm_t *alarm;
    osi_thread_t *btc;

    test_case_cleanup_set(alarm_btc_end);
    alarm_sems_begin();
    gates_begin();
    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();
    btc = btc_get_current_thread();
    TEST_ASSERT_NOT_NULL(btc);
    fill_queue(btc);

    osi_alarm_test_set_timer_hook(stall_timer);
    osi_alarm_test_set_cancel_hook(cancel_reached);
    alarm = osi_alarm_new("btc-full", alarm_cb, NULL, NULL);
    TEST_ASSERT_NOT_NULL(alarm);
    /* The helper is already waiting. cancel signals it, then stop_blocking waits for the handler. */
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(release_stalled_timer, "alarm_rel", 2048, NULL, 3, NULL));
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_hit, pdMS_TO_TICKS(TEST_WAIT_MS)));

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    TEST_ASSERT_EQUAL(0, s_count);
    TEST_ASSERT_FALSE(osi_alarm_is_active(alarm));

    osi_alarm_test_set_timer_hook(NULL);
    osi_alarm_test_set_cancel_hook(NULL);
    release_queue();
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_free(alarm));
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    btc_deinit();
    gates_end();
    alarm_sems_end();
}

TEST_CASE("osi_alarm rejects set while cancel is waiting and the next set runs once", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;

    alarm_test_begin();
    thread = test_thread_create(4);
    block_worker(thread);
    alarm = osi_alarm_new("rearm-race", alarm_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    s_hook_alarm = alarm;
    s_hook_set = OSI_ALARM_ERR_PASS;
    osi_alarm_test_set_cancel_hook(set_during_cancel);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    wait_until_idle(alarm);

    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_cancel(alarm));
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_FAIL, s_hook_set);
    osi_alarm_test_set_cancel_hook(NULL);

    s_count = 0;
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    release_queue();
    expect_callback_once();
    vTaskDelay(pdMS_TO_TICKS(40));
    TEST_ASSERT_EQUAL(1, s_count);

    osi_alarm_free(alarm);
    alarm_thread_free(thread);
    alarm_test_end();
}

TEST_CASE("osi_alarm free from a running callback releases the slot when it returns", "[osi_alarm]")
{
    osi_thread_t *thread;
    osi_alarm_t *alarm;
    osi_alarm_t *slots[ALARM_CBS_NUM];
    size_t i;

    alarm_test_begin();
    gates_begin();
    thread = test_thread_create(4);
    alarm = osi_alarm_new("free-running", hold_cb, NULL, thread);
    TEST_ASSERT_NOT_NULL(alarm);
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_set(alarm, 10));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_hit, pdMS_TO_TICKS(TEST_WAIT_MS)));

    /* Callback still on the stack. Ownership moves to the last unref. */
    TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_free(alarm));
    xSemaphoreGive(s_go);
    vTaskDelay(pdMS_TO_TICKS(50));

    for (i = 0; i < ALARM_CBS_NUM; i++) {
        slots[i] = osi_alarm_new("slot", NULL, NULL, thread);
        TEST_ASSERT_NOT_NULL(slots[i]);
    }
    for (i = 0; i < ALARM_CBS_NUM; i++) {
        TEST_ASSERT_EQUAL(OSI_ALARM_ERR_PASS, osi_alarm_free(slots[i]));
    }

    alarm_thread_free(thread);
    gates_end();
    alarm_test_end();
}
