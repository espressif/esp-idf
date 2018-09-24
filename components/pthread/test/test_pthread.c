#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_pthread.h"
#include <pthread.h>

#include "unity.h"

static void *compute_square(void *arg)
{
    int *num = (int *) arg;
    *num = (*num) * (*num);
    pthread_exit((void *) num);
    return NULL;
}

TEST_CASE("pthread create join", "[pthread]")
{
    int res = 0;
    volatile int num = 7;
    volatile bool attr_init = false;
    void *thread_rval = NULL;
    pthread_t new_thread = NULL;
    pthread_attr_t attr;

    if (TEST_PROTECT()) {
        res = pthread_attr_init(&attr);
        TEST_ASSERT_EQUAL_INT(0, res);
        attr_init = true;

        res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_create(&new_thread, &attr, compute_square, (void *) &num);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_join(new_thread, &thread_rval);
        TEST_ASSERT_EQUAL_INT(EDEADLK, res);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        TEST_ASSERT_EQUAL_INT(49, num);

        res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_create(&new_thread, &attr, compute_square, (void *) &num);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_join(new_thread, &thread_rval);
        TEST_ASSERT_EQUAL_INT(0, res);

        TEST_ASSERT_EQUAL_INT(2401, num);
        TEST_ASSERT_EQUAL_PTR(&num, thread_rval);
    }

    if (attr_init) {
        pthread_attr_destroy(&attr);
    }
}

TEST_CASE("pthread attr init destroy", "[pthread]")
{
    int res = 0;
    size_t stack_size_1 = 0, stack_size_2 = 0;
    volatile bool attr_init = pdFALSE;
    pthread_attr_t attr;

    if (TEST_PROTECT()) {
        res = pthread_attr_init(&attr);
        TEST_ASSERT_EQUAL_INT(0, res);
        attr_init = true;

        res = pthread_attr_getstacksize(&attr, &stack_size_1);
        TEST_ASSERT_EQUAL_INT(0, res);
        res = pthread_attr_setstacksize(&attr, stack_size_1);
        TEST_ASSERT_EQUAL_INT(0, res);
        res = pthread_attr_getstacksize(&attr, &stack_size_2);
        TEST_ASSERT_EQUAL_INT(0, res);
        TEST_ASSERT_EQUAL_INT(stack_size_2, stack_size_1);

        stack_size_1 = PTHREAD_STACK_MIN - 1;
        res = pthread_attr_setstacksize(&attr, stack_size_1);
        TEST_ASSERT_EQUAL_INT(EINVAL, res);
    }

    if (attr_init) {
        TEST_ASSERT_EQUAL_INT(0, pthread_attr_destroy(&attr));
    }
}

static void *unlock_mutex(void *arg)
{
    pthread_mutex_t *mutex = (pthread_mutex_t *) arg;
    intptr_t res = (intptr_t) pthread_mutex_unlock(mutex);
    pthread_exit((void *) res);
    return NULL;
}

static void test_mutex_lock_unlock(int mutex_type)
{
    int res = 0;
    int set_type = -1;
    volatile bool attr_created = false;
    volatile bool mutex_created = false;
    volatile intptr_t thread_rval = 0;
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    pthread_t new_thread;

    if (TEST_PROTECT()) {
        res = pthread_mutexattr_init(&attr);
        TEST_ASSERT_EQUAL_INT(0, res);
        attr_created = true;

        res = pthread_mutexattr_settype(&attr, mutex_type);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_mutexattr_gettype(&attr, &set_type);
        TEST_ASSERT_EQUAL_INT(0, res);
        TEST_ASSERT_EQUAL_INT(mutex_type, set_type);

        res = pthread_mutex_init(&mutex, &attr);
        TEST_ASSERT_EQUAL_INT(0, res);
        mutex_created = true;

        res = pthread_mutex_lock(&mutex);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_mutex_lock(&mutex);

        if(mutex_type == PTHREAD_MUTEX_ERRORCHECK) {
            TEST_ASSERT_EQUAL_INT(EDEADLK, res);
        } else {
            TEST_ASSERT_EQUAL_INT(0, res);

            res = pthread_mutex_unlock(&mutex);
            TEST_ASSERT_EQUAL_INT(0, res);
        }

        pthread_create(&new_thread, NULL, unlock_mutex, &mutex);

        pthread_join(new_thread, (void **) &thread_rval);
        TEST_ASSERT_EQUAL_INT(EPERM, (int) thread_rval);

        res = pthread_mutex_unlock(&mutex);
        TEST_ASSERT_EQUAL_INT(0, res);
    }

    if (attr_created) {
        pthread_mutexattr_destroy(&attr);
    }

    if (mutex_created) {
        pthread_mutex_destroy(&mutex);
    }
}

TEST_CASE("pthread mutex lock unlock", "[pthread]")
{
    int res = 0;

    /* Present behavior of mutex initializer is unlike what is
     * defined in Posix standard, ie. calling pthread_mutex_lock
     * on such a mutex would internally cause dynamic allocation.
     * Therefore pthread_mutex_destroy needs to be called in
     * order to avoid memory leak. */
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    res = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL_INT(0, res);

    /* This deviates from the Posix standard static mutex behavior.
     * This needs to be removed in the future when standard mutex
     * initializer is supported */
    pthread_mutex_destroy(&mutex);

    test_mutex_lock_unlock(PTHREAD_MUTEX_ERRORCHECK);
    test_mutex_lock_unlock(PTHREAD_MUTEX_RECURSIVE);
}

static void timespec_add_nano(struct timespec * out, struct timespec * in, long val)
{
    out->tv_nsec = val + in->tv_nsec;
    if (out->tv_nsec < (in->tv_nsec)) {
        out->tv_sec += 1;
    }
}

TEST_CASE("pthread mutex trylock timedlock", "[pthread]")
{
    int res = 0;
    volatile bool mutex_created = false;
    pthread_mutex_t mutex;
    struct timespec abs_timeout;

    if (TEST_PROTECT()) {
        res = pthread_mutex_init(&mutex, NULL);
        TEST_ASSERT_EQUAL_INT(0, res);
        mutex_created = true;

        res = pthread_mutex_trylock(&mutex);
        TEST_ASSERT_EQUAL_INT(0, res);

        res = pthread_mutex_trylock(&mutex);
        TEST_ASSERT_EQUAL_INT(EBUSY, res);

        clock_gettime(CLOCK_REALTIME, &abs_timeout);
        timespec_add_nano(&abs_timeout, &abs_timeout, 100000000LL);

        res = pthread_mutex_timedlock(&mutex, &abs_timeout);
        TEST_ASSERT_EQUAL_INT(ETIMEDOUT, res);

        res = pthread_mutex_unlock(&mutex);
        TEST_ASSERT_EQUAL_INT(0, res);
    }

    if (mutex_created) {
        pthread_mutex_destroy(&mutex);
    }
}
