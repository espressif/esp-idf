/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <pthread.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct {
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    unsigned delay_ms;
} thread_args_t;

static void *thread_signals(void *arg)
{
    const thread_args_t *targs = (thread_args_t *)arg;
    int r;

    r = pthread_mutex_lock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_cond_signal(targs->cond);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_mutex_unlock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    usleep(targs->delay_ms * 1000);

    r = pthread_mutex_lock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_cond_broadcast(targs->cond);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_mutex_unlock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    return NULL;
}

static void *thread_waits(void *arg)
{
    const thread_args_t *targs = (thread_args_t *)arg;
    int r;

    r = pthread_mutex_lock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_cond_wait(targs->cond, targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_mutex_unlock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    usleep(targs->delay_ms * 1000);

    r = pthread_mutex_lock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    struct timespec two_seconds;
    clock_gettime(CLOCK_REALTIME, &two_seconds);
    two_seconds.tv_sec += 2;
    r = pthread_cond_timedwait(targs->cond, targs->mutex, &two_seconds);
    TEST_ASSERT_EQUAL_INT(0, r);

    r = pthread_mutex_unlock(targs->mutex);
    TEST_ASSERT_EQUAL_INT(0, r);

    return NULL;
}

#define NUM_THREADS 3

TEST_CASE("pthread cond wait", "[pthread]")
{
    int r;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    struct {
        thread_args_t args;
        pthread_t thread;
    } wait[NUM_THREADS];
    struct {
        thread_args_t args;
        pthread_t thread;
    } signal[NUM_THREADS];

    wait[0].args.delay_ms = 50;
    wait[1].args.delay_ms = 100;
    wait[2].args.delay_ms = 200;

    signal[0].args.delay_ms = 30;
    signal[1].args.delay_ms = 150;
    signal[2].args.delay_ms = 500; // highest delay, ensure that broadcast will be received by all waiter threads

    for (int i = 0; i < NUM_THREADS; i++) {
        wait[i].args.cond = &cond;
        wait[i].args.mutex = &mutex;
        signal[i].args.cond = &cond;
        signal[i].args.mutex = &mutex;

        r = pthread_create(&signal[i].thread, NULL, thread_signals, &signal[i].args);
        TEST_ASSERT_EQUAL_INT(0, r);
        r = pthread_create(&wait[i].thread, NULL, thread_waits, &wait[i].args);
        TEST_ASSERT_EQUAL_INT(0, r);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        r = pthread_join(signal[i].thread, NULL);
        TEST_ASSERT_EQUAL_INT(0, r);
        pthread_join(wait[i].thread, NULL);
        TEST_ASSERT_EQUAL_INT(0, r);
    }

    TEST_ASSERT_EQUAL_INT(ESP_OK, pthread_cond_destroy(&cond));
    TEST_ASSERT_EQUAL_INT(ESP_OK, pthread_mutex_destroy(&mutex));

    // Wait a few ticks to allow freertos idle task to free up memory
    vTaskDelay(10);
}
