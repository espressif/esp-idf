/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Unit tests for the shared BLE profile task (components/bt/ble_profiles/common):
 * lifecycle, the running-state guard on the shared queue, and the caller-owned
 * (zero-allocation) event path.
 *
 * Each test brings the OSAL function table up (bt_prf_task_init() dispatches
 * through it) and tears it down again, so the per-test heap check in tearDown()
 * also covers the profile task's own allocations.
 */

#include <stdbool.h>
#include <stdint.h>

#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "bt_osal.h"
#include "bt_osal_freertos.h"
#include "bt_prf_task.h"

/* Long enough for the worker to run the handler, short enough to stay quick. */
#define TEST_WAIT_MS      1000
/* Waited on when a handler is expected NOT to run. */
#define TEST_NO_RUN_MS    50

/* Shared state between the test task and the posted handlers. */
static SemaphoreHandle_t s_done;
static volatile uint32_t s_run_count;
static void *s_last_arg;
static TaskHandle_t s_run_task;

static void prf_test_begin(void)
{
    s_run_count = 0;
    s_last_arg = NULL;
    s_run_task = NULL;

    s_done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_done);

    bt_osal_freertos_funcs_init();
    TEST_ASSERT_NOT_NULL(bt_osal_freertos_funcs_get());
}

static void prf_test_end(void)
{
    /* Safe even if a test already deinitialized it. */
    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_deinit());

    bt_osal_freertos_funcs_deinit();
    TEST_ASSERT_NULL(bt_osal_freertos_funcs_get());

    vSemaphoreDelete(s_done);
    s_done = NULL;

    /* The worker task is deleted by bt_prf_task_deinit(); its TCB/stack are
     * reclaimed asynchronously by the idle task, so give it a moment before the
     * heap check in tearDown() runs. */
    vTaskDelay(pdMS_TO_TICKS(20));
}

/* Fire-and-forget handler: records the argument and the task it ran on. */
static void handler_record(struct bt_osal_event *ev)
{
    s_run_count++;
    s_last_arg = bt_osal_event_get_arg(ev);
    s_run_task = xTaskGetCurrentTaskHandle();
    xSemaphoreGive(s_done);
}

TEST_CASE("prf_task init/deinit lifecycle", "[prf_task]")
{
    prf_test_begin();

    TEST_ASSERT_FALSE(bt_prf_task_is_running());
    TEST_ASSERT_NULL(bt_prf_task_eventq());

    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_init());
    TEST_ASSERT_TRUE(bt_prf_task_is_running());
    TEST_ASSERT_NOT_NULL(bt_prf_task_eventq());

    /* Second init while already running is an idempotent no-op. */
    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_init());
    TEST_ASSERT_TRUE(bt_prf_task_is_running());

    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_deinit());
    TEST_ASSERT_FALSE(bt_prf_task_is_running());
    TEST_ASSERT_NULL(bt_prf_task_eventq());

    /* Deinit when not running is a no-op. */
    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_deinit());

    prf_test_end();
}

TEST_CASE("prf_task eventq post runs handler on the worker task", "[prf_task]")
{
    struct bt_osal_event ev = {0};
    struct bt_osal_eventq *evq;

    prf_test_begin();

    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_init());
    evq = bt_prf_task_eventq();
    TEST_ASSERT_NOT_NULL(evq);

    /* Post a caller-owned event straight onto the shared queue. */
    bt_osal_event_init(&ev, handler_record, (void *)0xABCD);
    bt_osal_eventq_put(evq, &ev);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));

    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL_PTR((void *)0xABCD, s_last_arg);
    /* The handler must run on the worker task, not the caller's context. */
    TEST_ASSERT_NOT_NULL(s_run_task);
    TEST_ASSERT_NOT_EQUAL(xTaskGetCurrentTaskHandle(), s_run_task);

    bt_osal_event_deinit(&ev);

    prf_test_end();
}

TEST_CASE("prf_task eventq is available only while running", "[prf_task]")
{
    prf_test_begin();

    /* No queue to post to before the task is running. */
    TEST_ASSERT_NULL(bt_prf_task_eventq());

    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_init());
    TEST_ASSERT_NOT_NULL(bt_prf_task_eventq());

    /* The queue is gone again after teardown. */
    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_deinit());
    TEST_ASSERT_NULL(bt_prf_task_eventq());

    prf_test_end();
}

TEST_CASE("prf_task drives a caller-owned event (zero allocation)", "[prf_task]")
{
    struct bt_osal_event ev = {0};
    struct bt_osal_eventq *evq;

    prf_test_begin();

    TEST_ASSERT_EQUAL(ESP_OK, bt_prf_task_init());
    evq = bt_prf_task_eventq();
    TEST_ASSERT_NOT_NULL(evq);

    /* Post a caller-owned event straight onto the shared queue. */
    bt_osal_event_init(&ev, handler_record, (void *)0x1234);
    bt_osal_eventq_put(evq, &ev);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done, pdMS_TO_TICKS(TEST_WAIT_MS)));
    TEST_ASSERT_EQUAL_UINT32(1, s_run_count);
    TEST_ASSERT_EQUAL_PTR((void *)0x1234, s_last_arg);

    bt_osal_event_deinit(&ev);

    prf_test_end();
}
