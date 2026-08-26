/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* FreeRTOS timer tests
*/
#include <stdio.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

static void timer_callback(TimerHandle_t timer)
{
    volatile int *count;
    count = (volatile int *)pvTimerGetTimerID( timer );
    (*count)++;
    printf("Callback timer %p count %p = %d\n", timer, count, *count);
}

TEST_CASE("Oneshot FreeRTOS timers", "[freertos]")
{
    volatile int count = 0;
    TimerHandle_t oneshot = xTimerCreate("oneshot", 100 / portTICK_PERIOD_MS, pdFALSE,
                                         (void *)&count, timer_callback);
    TEST_ASSERT(oneshot);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(oneshot));
    TEST_ASSERT_EQUAL(0, count);

    TEST_ASSERT( xTimerStart(oneshot, 1) );
    vTaskDelay(2); /* give the timer task a chance to process the message */

    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(oneshot));
    TEST_ASSERT_EQUAL(0, count);

    vTaskDelay(250 / portTICK_PERIOD_MS); // 2.5 timer periods

    TEST_ASSERT_EQUAL(1, count);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(oneshot));

    TEST_ASSERT( xTimerDelete(oneshot, 1) );
}


TEST_CASE("Recurring FreeRTOS timers", "[freertos]")
{
    volatile int count = 0;
    TimerHandle_t recurring = xTimerCreate("oneshot", 100 / portTICK_PERIOD_MS, pdTRUE,
                                          (void *)&count, timer_callback);
    TEST_ASSERT(recurring);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(recurring));
    TEST_ASSERT_EQUAL(0, count);

    TEST_ASSERT( xTimerStart(recurring, 1) );

    vTaskDelay(2); // let timer task process the queue
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(recurring));
    TEST_ASSERT_EQUAL(0, count);

    vTaskDelay(250 / portTICK_PERIOD_MS); // 2.5 timer periods

    TEST_ASSERT_EQUAL(2, count);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(recurring));

    TEST_ASSERT( xTimerStop(recurring, 1) );

    TEST_ASSERT_EQUAL(2, count);

    vTaskDelay(100 / portTICK_PERIOD_MS); // One more timer period
    TEST_ASSERT_EQUAL(2, count); // hasn't gone up
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(recurring));

    TEST_ASSERT( xTimerDelete(recurring, 1) );
}

TEST_CASE("Static timer creation", "[freertos]")
{
    StaticTimer_t static_timer;
    TimerHandle_t created_timer;
    volatile int count = 0;

    created_timer = xTimerCreateStatic("oneshot", 100 / portTICK_PERIOD_MS,
                                    pdTRUE,
                                    (void *)&count,
                                    timer_callback,
                                    &static_timer);

    TEST_ASSERT_NOT_NULL(created_timer);
}
