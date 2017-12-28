// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This is a simple implementation of pthread condition variables. In essence,
// the waiter creates its own semaphore to wait on and pushes it in the cond var
// specific list. Upon notify and broadcast, all the waiters for the given cond
// var are woken up.

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/list.h"

#include <rom/queue.h>
#include <sys/time.h>

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_pthread";

typedef struct esp_pthread_cond_waiter {
    SemaphoreHandle_t   wait_sem;           ///< task specific semaphore to wait on
    TAILQ_ENTRY(esp_pthread_cond_waiter) link;  ///< stash on the list of semaphores to be notified
} esp_pthread_cond_waiter_t;

typedef struct esp_pthread_cond {
    _lock_t lock;                      ///< lock that protects the list of semaphores
    TAILQ_HEAD(, esp_pthread_cond_waiter) waiter_list;  ///< head of the list of semaphores
} esp_pthread_cond_t;

int pthread_cond_signal(pthread_cond_t *cv)
{
    if (cv == NULL || *cv == (pthread_cond_t) 0) {
        return EINVAL;
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) *cv;

    _lock_acquire_recursive(&cond->lock);
    esp_pthread_cond_waiter_t *entry;
    entry = TAILQ_FIRST(&cond->waiter_list);
    if (entry) {
        xSemaphoreGive(entry->wait_sem);
    }
    _lock_release_recursive(&cond->lock);

    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cv)
{
    if (cv == NULL || *cv == (pthread_cond_t) 0) {
        return EINVAL;
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) *cv;

    _lock_acquire_recursive(&cond->lock);
    esp_pthread_cond_waiter_t *entry;
    TAILQ_FOREACH(entry, &cond->waiter_list, link) {
        xSemaphoreGive(entry->wait_sem);
    }
    _lock_release_recursive(&cond->lock);

    return 0;
}

int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mut)
{
    return pthread_cond_timedwait(cv, mut, NULL);
}

int pthread_cond_timedwait(pthread_cond_t *cv, pthread_mutex_t *mut, const struct timespec *to)
{
    int ret;
    TickType_t timeout_ticks;

    if (cv == NULL || *cv == (pthread_cond_t) 0) {
        return EINVAL;
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) *cv;

    if (to == NULL) {
        timeout_ticks = portMAX_DELAY;
    } else {
        struct timeval abs_time, cur_time, diff_time;
        long timeout_msec;

        gettimeofday(&cur_time, NULL);

        abs_time.tv_sec = to->tv_sec;
        abs_time.tv_usec = to->tv_nsec / 1000;

        if (timercmp(&abs_time, &cur_time, <)) {
            /* As per the pthread spec, if the time has already
             * passed, no sleep is required.
             */
            timeout_msec = 0;
        } else {
            timersub(&abs_time, &cur_time, &diff_time);
            timeout_msec = (diff_time.tv_sec * 1000) + (diff_time.tv_usec / 1000);
        }

        if (timeout_msec <= 0) {
            return ETIMEDOUT;
        }

        timeout_ticks = timeout_msec / portTICK_PERIOD_MS;
    }

    esp_pthread_cond_waiter_t w;
    w.wait_sem = xSemaphoreCreateCounting(1, 0); /* First get will block */

    _lock_acquire_recursive(&cond->lock);
    TAILQ_INSERT_TAIL(&cond->waiter_list, &w, link);
    _lock_release_recursive(&cond->lock);
    pthread_mutex_unlock(mut);

    if (xSemaphoreTake(w.wait_sem, timeout_ticks) == pdTRUE) {
        ret = 0;
    } else {
        ret = ETIMEDOUT;
    }

    _lock_acquire_recursive(&cond->lock);
    TAILQ_REMOVE(&cond->waiter_list, &w, link);
    _lock_release_recursive(&cond->lock);
    vSemaphoreDelete(w.wait_sem);

    pthread_mutex_lock(mut);
    return ret;
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    ESP_LOGV(TAG, "%s not yet implemented (%p)", __FUNCTION__, attr);
    return ENOSYS;
}

int pthread_cond_init(pthread_cond_t *cv, const pthread_condattr_t *att)
{
    (void) att; /* Unused argument as of now */

    if (cv == NULL) {
        return EINVAL;
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) calloc(1, sizeof(esp_pthread_cond_t));
    if (cond == NULL) {
        return ENOMEM;
    }

    _lock_init_recursive(&cond->lock);
    TAILQ_INIT(&cond->waiter_list);

    *cv = (pthread_cond_t) cond;
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cv)
{
    int ret = 0;

    if (cv == NULL || *cv == (pthread_cond_t) 0) {
        return EINVAL;
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) *cv;

    _lock_acquire_recursive(&cond->lock);
    if (!TAILQ_EMPTY(&cond->waiter_list)) {
        ret = EBUSY;
    }
    _lock_release_recursive(&cond->lock);

    if (ret == 0) {
        *cv = (pthread_cond_t) 0;
        _lock_close_recursive(&cond->lock);
        free(cond);
    }

    return ret;
}
