/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <errno.h>

#define SHARED 0

#include <limits.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>

#include "unity.h"

// This test is actually not mentioned in the standard, but it's good IDF practice
TEST_CASE("sem_init nullptr", "[semaphore]")
{
    TEST_ASSERT_EQUAL_INT(-1, sem_init(NULL, SHARED, 0));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

TEST_CASE("sem_init semaphore value exceeded", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(-1, sem_init(&semaphore, SHARED, SEM_VALUE_MAX + 1));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// This test is actually not mentioned in the standard, but it's good IDF practice
TEST_CASE("sem_destroy nullptr", "[semaphore]")
{
    TEST_ASSERT_EQUAL_INT(-1, sem_destroy(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

TEST_CASE("sem_init and destroy work correctly", "[semaphore]")
{
    sem_t semaphore;
    memset(&semaphore, 0, sizeof(semaphore));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));
    TEST_ASSERT(semaphore);
    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_wait nulltpr", "[semaphore]")
{
    TEST_ASSERT_EQUAL_INT(-1, sem_wait(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

TEST_CASE("sem_post nulltpr", "[semaphore]")
{
    TEST_ASSERT_EQUAL_INT(-1, sem_post(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

TEST_CASE("lock and unlock semaphore", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));

    TEST_ASSERT_EQUAL_INT(0, sem_wait(&semaphore));
    TEST_ASSERT_EQUAL_INT(0, sem_post(&semaphore));

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_post posts up to SEM_VALUE_MAX times", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, SEM_VALUE_MAX - 1));

    TEST_ASSERT_EQUAL_INT(0, sem_post(&semaphore));

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

// This test is actually not mentioned in the standard
TEST_CASE("sem_post fails on semaphore with SEM_VALUE_MAX", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, SEM_VALUE_MAX));

    TEST_ASSERT_EQUAL_INT(-1, sem_post(&semaphore));
    TEST_ASSERT_EQUAL_INT(EAGAIN, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_wait waits on semaphore with SEM_VALUE_MAX", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, SEM_VALUE_MAX));

    TEST_ASSERT_EQUAL_INT(0, sem_wait(&semaphore));

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_trywait nulltpr", "[semaphore]")
{
    TEST_ASSERT_EQUAL_INT(-1, sem_post(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

TEST_CASE("sem_trywait on semaphore initialized in locked state", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));

    TEST_ASSERT_EQUAL_INT(-1, sem_trywait(&semaphore));
    TEST_ASSERT_EQUAL_INT(EAGAIN, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_trywait on locked semaphore", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));
    TEST_ASSERT_EQUAL_INT(0, sem_wait(&semaphore));

    TEST_ASSERT_EQUAL_INT(-1, sem_trywait(&semaphore));
    TEST_ASSERT_EQUAL_INT(EAGAIN, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_post(&semaphore));
    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait fails with semaphore null", "[semaphore]")
{
    struct timespec abstime = { .tv_sec = 0, .tv_nsec = 0 };
    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(NULL, &abstime));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// This test is actually not mentioned in the standard, but it's good IDF practice
TEST_CASE("sem_timedwait fails when abstime null, locked semaphore", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

// This test is actually not necessary but OK according to the standard, it simplifies implementation
TEST_CASE("sem_timedwait fails when abstime null, unlocked semaphore", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait fails with tv_nsec >= 1 bln, locked semaphore", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));
    abstime.tv_sec = abstime.tv_sec + 1; // set the time to the future
    abstime.tv_nsec = 1000000000; // make tv_nsec invalid

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

// This test is actually not mentioned but OK according to the standard, it simplifies implementation
TEST_CASE("sem_timedwait fails with tv_nsec >= 1 bln, unlocked semaphore", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));
    abstime.tv_sec = abstime.tv_sec + 1; // set the time to the future
    abstime.tv_nsec = 1000000000; // make tv_nsec invalid

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait fails with tv_nsec < 0, locked semaphore", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));
    abstime.tv_sec = abstime.tv_sec + 2; // set the time to the future
    abstime.tv_nsec = -1; // make tv_nsec invalid

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

// This test is actually not mentioned but OK according to the standard, it simplifies implementation
TEST_CASE("sem_timedwait fails with tv_nsec < 0, unlocked semaphore", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));
    abstime.tv_sec = abstime.tv_sec + 2; // set the time to the future
    abstime.tv_nsec = -1; // make tv_nsec invalid

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait lock locked semaphore with tv_nsec 0", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));
    if (abstime.tv_nsec > 900000000) { // ~ ten ticks if tick rate is 100Hz, reduce test time to max ~1.1s
        abstime.tv_sec = abstime.tv_sec + 2;
    } else {
        abstime.tv_sec = abstime.tv_sec + 1;
    }
    abstime.tv_nsec = 0;

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(ETIMEDOUT, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait lock locked semaphore with tv_nsec 999999999", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));
    if (abstime.tv_nsec > 900000000) { // ~ ten ticks if tick rate is 100Hz, reduce test time to max ~1.1s
        abstime.tv_sec = abstime.tv_sec + 1;
    }
    abstime.tv_nsec = 999999999;

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(ETIMEDOUT, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

// POSIX explicitly requires this
TEST_CASE("sem_timedwait still locks unlocked semaphore even if abstime expired ", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));
    abstime.tv_sec = abstime.tv_sec - 1;

    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&semaphore, &abstime));

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait too old time", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));
    abstime.tv_sec = abstime.tv_sec - 1;

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(ETIMEDOUT, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait lock unlocked semaphore", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));
    abstime.tv_sec = abstime.tv_sec + 1;

    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_post(&semaphore));

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_timedwait wait on locked semaphore (timeout)", "[semaphore]")
{
    sem_t semaphore;
    struct timespec abstime;
    TEST_ASSERT_EQUAL_INT(0, clock_gettime(CLOCK_REALTIME, &abstime));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));
    abstime.tv_nsec = abstime.tv_nsec + 20000000;
    if (abstime.tv_nsec >= 1000000000) {
        abstime.tv_sec = abstime.tv_sec + 1;
        abstime.tv_sec = abstime.tv_nsec % 1000000000;
    }

    TEST_ASSERT_EQUAL_INT(-1, sem_timedwait(&semaphore, &abstime));
    TEST_ASSERT_EQUAL_INT(ETIMEDOUT, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_getvalue semaphore NULL", "[semaphore]")
{
    int semaphore_value;
    TEST_ASSERT_EQUAL_INT(-1, sem_getvalue(NULL, &semaphore_value));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// This test is actually not mentioned in the standard, but it's good IDF practice
TEST_CASE("sem_getvalue value ptr NULL", "[semaphore]")
{
    sem_t semaphore;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));

    TEST_ASSERT_EQUAL_INT(-1, sem_getvalue(&semaphore, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_getvalue locked semaphore", "[semaphore]")
{
    sem_t semaphore;
    int semaphore_value;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 0));

    TEST_ASSERT_EQUAL_INT(0, sem_getvalue(&semaphore, &semaphore_value));
    TEST_ASSERT_EQUAL_INT(0, semaphore_value);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_getvalue unlocked semaphore", "[semaphore]")
{
    sem_t semaphore;
    int semaphore_value;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 1));

    TEST_ASSERT_EQUAL_INT(0, sem_getvalue(&semaphore, &semaphore_value));
    TEST_ASSERT_EQUAL_INT(1, semaphore_value);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

TEST_CASE("sem_getvalue changes after waiting and posting", "[semaphore]")
{
    sem_t semaphore;
    int semaphore_value;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&semaphore, SHARED, 2));

    TEST_ASSERT_EQUAL_INT(0, sem_getvalue(&semaphore, &semaphore_value));
    TEST_ASSERT_EQUAL_INT(2, semaphore_value);

    TEST_ASSERT_EQUAL_INT(0, sem_wait(&semaphore));

    TEST_ASSERT_EQUAL_INT(0, sem_getvalue(&semaphore, &semaphore_value));
    TEST_ASSERT_EQUAL_INT(1, semaphore_value);

    TEST_ASSERT_EQUAL_INT(0, sem_post(&semaphore));

    TEST_ASSERT_EQUAL_INT(0, sem_getvalue(&semaphore, &semaphore_value));
    TEST_ASSERT_EQUAL_INT(2, semaphore_value);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&semaphore));
}

static bool finished_wait;
static sem_t g_thread_waiter;
static sem_t g_synchronizer;
static void *do_something(void *arg)
{

    TEST_ASSERT_EQUAL_INT(0, sem_wait(&g_thread_waiter));
    finished_wait = true;
    TEST_ASSERT_EQUAL_INT(0, sem_post(&g_synchronizer));
    return NULL;
}

TEST_CASE("thread is waiting on semaphore", "[semaphore]")
{
    pthread_t thr;
    finished_wait = false;

    TEST_ASSERT_EQUAL_INT(0, sem_init(&g_thread_waiter, SHARED, 0));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&g_synchronizer, SHARED, 0));
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&thr, NULL, do_something, NULL));

    TEST_ASSERT_FALSE(finished_wait);
    TEST_ASSERT_EQUAL_INT(0, sem_post(&g_thread_waiter));
    TEST_ASSERT_EQUAL_INT(0, sem_wait(&g_synchronizer));
    TEST_ASSERT_TRUE(finished_wait);

    pthread_join(thr, NULL);
    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&g_synchronizer));
    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&g_thread_waiter));
}
