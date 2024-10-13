/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdatomic.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "sys/queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/soc_memory_layout.h"

#include "pthread_internal.h"
#include "esp_pthread.h"

#include "esp_log.h"
const static char *TAG = "pthread_rw_lock";

/** pthread rw_mutex FreeRTOS wrapper */
typedef struct {
    /**
     *
     */
    pthread_cond_t cv;

    pthread_mutex_t resource_mutex;

    /**
     * Number of current readers holding this lock, negative number means waiting readers
     */
    int8_t active_readers;

    uint8_t active_writers;
    uint8_t waiting_writers;

} esp_pthread_rwlock_t;

#define WRITER_QUEUE_SIZE 4
#define READER_QUEUE_SIZE 4

int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr)
{
    int result;
    if (!rwlock) {
        return EINVAL;
    }

    if (attr) {
        // TODO: implement attributes in IDF-4284
        return ENOSYS;
    }

    esp_pthread_rwlock_t *esp_rwlock = (esp_pthread_rwlock_t*) calloc(1, sizeof(esp_pthread_rwlock_t));
    if (esp_rwlock == NULL) {
        return ENOMEM;
    }

    result = pthread_mutex_init(&esp_rwlock->resource_mutex, NULL);
    if (result != 0) {
        free(esp_rwlock);
        return ENOMEM;
    }

    result = pthread_cond_init(&esp_rwlock->cv, NULL);
    if (result != 0) {
        pthread_mutex_destroy(&esp_rwlock->resource_mutex);
        free(esp_rwlock);
        return ENOMEM;
    }

    esp_rwlock->active_readers = 0;
    esp_rwlock->active_writers = 0;
    esp_rwlock->waiting_writers = 0;

    *rwlock = (pthread_rwlock_t) esp_rwlock;

    return 0;
}

static int pthread_rwlock_init_if_static(pthread_rwlock_t *rwlock)
{
    int res = 0;
    if ((intptr_t) *rwlock == PTHREAD_RWLOCK_INITIALIZER) {
        portENTER_CRITICAL(&pthread_lazy_init_lock);
        if ((intptr_t) *rwlock == PTHREAD_RWLOCK_INITIALIZER) {
            res = pthread_rwlock_init(rwlock, NULL);
        }
        portEXIT_CRITICAL(&pthread_lazy_init_lock);
    }
    return res;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;

    ESP_LOGV(TAG, "%s %p", __FUNCTION__, rwlock);

    if (!rwlock) {
        return EINVAL;
    }

    if ((intptr_t) *rwlock == PTHREAD_RWLOCK_INITIALIZER) {
        return 0; // Static rwlock was never initialized
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    if (esp_rwlock == NULL) {
        return EINVAL;
    }

    // TODO: necessary?
    pthread_mutex_lock(&esp_rwlock->resource_mutex);

    if (esp_rwlock->active_readers != 0 || esp_rwlock->active_writers > 0 || esp_rwlock->waiting_writers > 0) {
        pthread_mutex_unlock(&esp_rwlock->resource_mutex);
        return EBUSY;
    }

    // delete whole lock
    pthread_cond_destroy(&esp_rwlock->cv);
    pthread_mutex_unlock(&esp_rwlock->resource_mutex);
    pthread_mutex_destroy(&esp_rwlock->resource_mutex);

    free(esp_rwlock);

    return 0;
}

static int checkrw_lock(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;
    int res;

    if (rwlock == NULL) {
        return EINVAL;
    }

    res = pthread_rwlock_init_if_static(rwlock);
    if (res != 0) {
        return res;
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    if (esp_rwlock == NULL) {
        return EINVAL;
    }

    return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;
    int res;

    res = checkrw_lock(rwlock);
    if (res != 0) {
        return res;
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    res = pthread_mutex_lock(&esp_rwlock->resource_mutex);
    if (res != 0) {
        return res;
    }

    if (esp_rwlock->active_writers == 0) {
        esp_rwlock->active_readers++;
    } else {
        while (true) {
            pthread_cond_wait(&esp_rwlock->cv, &esp_rwlock->resource_mutex);
            if (esp_rwlock->active_writers == 0 && esp_rwlock->waiting_writers == 0) {
                esp_rwlock->active_readers++;
                break;
            }
        }
    }

    pthread_mutex_unlock(&esp_rwlock->resource_mutex);

    return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;
    int res;

    res = checkrw_lock(rwlock);
    if (res != 0) {
        return res;
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    res = pthread_mutex_trylock(&esp_rwlock->resource_mutex);
    if (res != 0) {
        return res;
    }

    if (esp_rwlock->active_writers == 0) {
        esp_rwlock->active_readers++;
        res = 0;
    } else {
        res = EBUSY;
    }

    pthread_mutex_unlock(&esp_rwlock->resource_mutex);

    return res;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;
    int res;

    res = checkrw_lock(rwlock);
    if (res != 0) {
        return res;
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    res = pthread_mutex_lock(&esp_rwlock->resource_mutex);
    if (res != 0) {
        return res;
    }

    esp_rwlock->waiting_writers++;
    while (esp_rwlock->active_readers > 0 || esp_rwlock->active_writers > 0) {
        pthread_cond_wait(&esp_rwlock->cv, &esp_rwlock->resource_mutex);
    }
    esp_rwlock->waiting_writers--;
    esp_rwlock->active_writers++;

    pthread_mutex_unlock(&esp_rwlock->resource_mutex);

    return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;
    int res;

    res = checkrw_lock(rwlock);
    if (res != 0) {
        return res;
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    res = pthread_mutex_trylock(&esp_rwlock->resource_mutex);
    if (res != 0) {
        return res;
    }

    if (esp_rwlock->active_readers > 0 ||
            esp_rwlock->active_writers > 0 ||
            esp_rwlock->waiting_writers > 0) { // the last check for waiting_writers is to avoid skipping the queue
        res = EBUSY;
    } else {
        esp_rwlock->active_writers++;
        res = 0;
    }

    pthread_mutex_unlock(&esp_rwlock->resource_mutex);

    return res;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    esp_pthread_rwlock_t *esp_rwlock;
    int res;

    res = checkrw_lock(rwlock);
    if (res != 0) {
        return res;
    }

    esp_rwlock = (esp_pthread_rwlock_t *)*rwlock;
    res = pthread_mutex_lock(&esp_rwlock->resource_mutex);
    if (res != 0) {
        return res;
    }

    assert(!(esp_rwlock->active_readers > 0 && esp_rwlock->active_writers > 0));

    if (esp_rwlock->active_readers > 0) {
        // we are a reader
        esp_rwlock->active_readers--;
        if (esp_rwlock->active_readers == 0) {
            pthread_cond_broadcast(&esp_rwlock->cv);
        }
    } else {
        // we are a writer
        esp_rwlock->active_writers = 0;
        pthread_cond_broadcast(&esp_rwlock->cv);
    }

    pthread_mutex_unlock(&esp_rwlock->resource_mutex);

    return 0;
}

/* Hook function to force linking this file */
void pthread_include_pthread_rwlock_impl(void)
{
}
