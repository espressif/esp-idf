/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sdkconfig.h"

#include <errno.h>
#include <stdatomic.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "esp_pthread.h"
#include <pthread.h>

#include "unity.h"

TEST_CASE("pthread_rwlock_init invalid arg", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(NULL, NULL), EINVAL);
}

TEST_CASE("pthread_rwlock_destroy invalid arg", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(NULL), EINVAL);

    pthread_rwlock_t rwlock = 0;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), EINVAL);
}

TEST_CASE("create and destroy rwlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("pthread_rwlock_destroy encounters static initializer", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("rdlock invalid param", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(NULL), EINVAL);

    pthread_rwlock_t rwlock = 0;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), EINVAL);
}

TEST_CASE("unlock invalid param", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(NULL), EINVAL);

    pthread_rwlock_t rwlock = 0;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), EINVAL);
}

TEST_CASE("wrlock lock invalid param", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(NULL), EINVAL);

    pthread_rwlock_t rwlock = 0;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(&rwlock), EINVAL);
}

TEST_CASE("rdlock lock statically initialized lock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("rdlock unlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("multiple read locks", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("wrlock lock-unlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

struct ReaderWriterArgs {
    QueueHandle_t *wait_queue;
    size_t sem_wait_release_num;
    pthread_rwlock_t *rwlock;
    volatile bool reading;
    volatile bool writing;
};

static void *reader(void *arg)
{
    uint8_t dummy_message;
    struct ReaderWriterArgs *rw_args = (struct ReaderWriterArgs*) arg;

    TEST_ASSERT_EQUAL(xQueueReceive(*(rw_args->wait_queue), &dummy_message, portMAX_DELAY), pdTRUE);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(rw_args->rwlock), 0);
    rw_args->reading = true;

    TEST_ASSERT_FALSE(rw_args->writing);

    rw_args->reading = false;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(rw_args->rwlock), 0);

    return NULL;
}

static void *writer(void *arg)
{
    uint8_t dummy_msg;
    struct ReaderWriterArgs *rw_args = (struct ReaderWriterArgs*) arg;

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(rw_args->rwlock), 0);
    rw_args->writing = true;

    for (size_t i = 0; i < rw_args->sem_wait_release_num; i++) {
        TEST_ASSERT_EQUAL(xQueueSendToBack(*(rw_args->wait_queue), &dummy_msg, portMAX_DELAY), pdTRUE);
    }

    TEST_ASSERT_FALSE(rw_args->reading);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    TEST_ASSERT_FALSE(rw_args->reading);

    rw_args->writing = false;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(rw_args->rwlock), 0);
    return NULL;
}

TEST_CASE("wrlock reader waits", "[pthread][rwlock]")
{
    QueueHandle_t wait_queue;
    pthread_rwlock_t rwlock;
    pthread_t reader_thread;
    pthread_t writer_thread;
    struct ReaderWriterArgs rw_args;

    wait_queue = xQueueCreate(1, 1);
    TEST_ASSERT(wait_queue);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);

    rw_args.wait_queue   = &wait_queue;
    rw_args.sem_wait_release_num = 1;
    rw_args.rwlock     = &rwlock;
    rw_args.writing = false;
    rw_args.reading = false;

    TEST_ASSERT_EQUAL(pthread_create(&reader_thread, NULL, reader, &rw_args), 0);
    TEST_ASSERT_EQUAL(pthread_create(&writer_thread, NULL, writer, &rw_args), 0);

    TEST_ASSERT_EQUAL(pthread_join(writer_thread, NULL), 0);
    TEST_ASSERT_EQUAL(pthread_join(reader_thread, NULL), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
    vQueueDelete(wait_queue);

    // Wait a few ticks to allow freertos idle task to free up memory
    vTaskDelay(10);
}

TEST_CASE("wrlock multiple readers wait", "[pthread][rwlock]")
{
    static const size_t THREAD_NUM = 4;
    QueueHandle_t wait_queue;
    pthread_rwlock_t rwlock;
    pthread_t reader_thread[THREAD_NUM];
    pthread_t writer_thread;
    struct ReaderWriterArgs rw_args;

    wait_queue = xQueueCreate(THREAD_NUM, 1);
    TEST_ASSERT(wait_queue);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);

    rw_args.wait_queue   = &wait_queue;
    rw_args.sem_wait_release_num = THREAD_NUM;
    rw_args.rwlock     = &rwlock;
    rw_args.writing = false;
    rw_args.reading = false;

    for (size_t i = 0; i < THREAD_NUM; i++) {
        TEST_ASSERT_EQUAL(pthread_create(&(reader_thread[i]), NULL, reader, &rw_args), 0);
    }
    TEST_ASSERT_EQUAL(pthread_create(&writer_thread, NULL, writer, &rw_args), 0);

    TEST_ASSERT_EQUAL(pthread_join(writer_thread, NULL), 0);
    for (size_t i = 0; i < THREAD_NUM; i++) {
        TEST_ASSERT_EQUAL(pthread_join(reader_thread[i], NULL), 0);
    }

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
    vQueueDelete(wait_queue);
}

static void *writer2(void *arg)
{
    uint8_t dummy_msg;
    struct ReaderWriterArgs *rw_args = (struct ReaderWriterArgs*) arg;

    TEST_ASSERT_EQUAL(xQueueReceive(*(rw_args->wait_queue), &dummy_msg, portMAX_DELAY), pdTRUE);
    TEST_ASSERT_TRUE(rw_args->writing);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(rw_args->rwlock), 0);
    TEST_ASSERT_FALSE(rw_args->writing);
    rw_args->writing = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);

    rw_args->writing = false;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(rw_args->rwlock), 0);

    return NULL;
}

TEST_CASE("wrlock writer waits", "[pthread][rwlock]")
{
    QueueHandle_t wait_queue;
    pthread_rwlock_t rwlock;
    pthread_t writer_thread;
    pthread_t writer_2_thread;
    struct ReaderWriterArgs rw_args;

    wait_queue = xQueueCreate(1, 1);
    TEST_ASSERT(wait_queue);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);

    rw_args.wait_queue   = &wait_queue;
    rw_args.sem_wait_release_num = 1;
    rw_args.rwlock  = &rwlock;
    rw_args.writing = false;
    rw_args.reading = false;

    TEST_ASSERT_EQUAL(pthread_create(&writer_2_thread, NULL, writer2, &rw_args), 0);
    TEST_ASSERT_EQUAL(pthread_create(&writer_thread, NULL, writer, &rw_args), 0);

    TEST_ASSERT_EQUAL(pthread_join(writer_thread, NULL), 0);
    TEST_ASSERT_EQUAL(pthread_join(writer_2_thread, NULL), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
    vQueueDelete(wait_queue);
}

TEST_CASE("wrlock multiple writers wait", "[pthread][rwlock]")
{
    static const size_t THREAD_NUM = 4;
    QueueHandle_t wait_queue;
    pthread_rwlock_t rwlock;
    pthread_t writer_thread;
    pthread_t writer_2_thread[THREAD_NUM];
    struct ReaderWriterArgs rw_args;

    wait_queue = xQueueCreate(THREAD_NUM, 1);
    TEST_ASSERT(wait_queue);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);

    rw_args.wait_queue   = &wait_queue;
    rw_args.sem_wait_release_num = THREAD_NUM;
    rw_args.rwlock  = &rwlock;
    rw_args.writing = false;
    rw_args.reading = false;

    for (size_t i = 0; i < THREAD_NUM; i++) {
        TEST_ASSERT_EQUAL(pthread_create(&writer_2_thread[i], NULL, writer2, &rw_args), 0);
    }
    TEST_ASSERT_EQUAL(pthread_create(&writer_thread, NULL, writer, &rw_args), 0);

    TEST_ASSERT_EQUAL(pthread_join(writer_thread, NULL), 0);
    for (size_t i = 0; i < THREAD_NUM; i++) {
        TEST_ASSERT_EQUAL(pthread_join(writer_2_thread[i], NULL), 0);
    }

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
    vQueueDelete(wait_queue);
}

TEST_CASE("tryrdlock invalid param", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_tryrdlock(NULL), EINVAL);

    pthread_rwlock_t rwlock = 0;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_tryrdlock(&rwlock), EINVAL);
}

TEST_CASE("tryrdlock fails on write-locked rwlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_tryrdlock(&rwlock), EBUSY);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("tryrdlock succeeds on read-locked rwlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_tryrdlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("tryrdlock lock statically initialized lock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_tryrdlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("trywrlock invalid param", "[pthread][rwlock]")
{
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_trywrlock(NULL), EINVAL);

    pthread_rwlock_t rwlock = 0;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_trywrlock(&rwlock), EINVAL);
}

TEST_CASE("trywrlock fails on write-locked rwlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_wrlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_trywrlock(&rwlock), EBUSY);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("trywrlock fails on read-locked rwlock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock;
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_init(&rwlock, NULL), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_rdlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_trywrlock(&rwlock), EBUSY);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}

TEST_CASE("trywrlock lock statically initialized lock", "[pthread][rwlock]")
{
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_trywrlock(&rwlock), 0);

    TEST_ASSERT_EQUAL_INT(pthread_rwlock_unlock(&rwlock), 0);
    TEST_ASSERT_EQUAL_INT(pthread_rwlock_destroy(&rwlock), 0);
}
