/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/list.h"
#include "pthread_internal.h"

#include <sys/queue.h>
#include <sys/time.h>

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

static int s_check_and_init_if_static(pthread_cond_t *cv)
{
    int res = 0;

    if (cv == NULL || *cv == (pthread_cond_t) 0) {
        return EINVAL;
    }

    if (*cv == PTHREAD_COND_INITIALIZER) {
        portENTER_CRITICAL(&pthread_lazy_init_lock);
        if (*cv == PTHREAD_COND_INITIALIZER) {
            res = pthread_cond_init(cv, NULL);
        }
        portEXIT_CRITICAL(&pthread_lazy_init_lock);
    }

    return res;
}

int pthread_cond_signal(pthread_cond_t *cv)
{
    int res = s_check_and_init_if_static(cv);
    if (res) {
        return res;
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
    int res = s_check_and_init_if_static(cv);
    if (res) {
        return res;
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

    int res = s_check_and_init_if_static(cv);
    if (res) {
        return res;
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) *cv;

    if (to == NULL) {
        timeout_ticks = portMAX_DELAY;
    } else {
        struct timeval abs_time, cur_time, diff_time;
        long timeout_msec;

        gettimeofday(&cur_time, NULL);

        abs_time.tv_sec = to->tv_sec;
        // Round up nanoseconds to the next microsecond
        abs_time.tv_usec = (to->tv_nsec + 1000 - 1) / 1000;

        if (timercmp(&abs_time, &cur_time, <)) {
            /* As per the pthread spec, if the time has already
             * passed, no sleep is required.
             */
            timeout_msec = 0;
        } else {
            timersub(&abs_time, &cur_time, &diff_time);
            // Round up timeout microseconds to the next millisecond
            timeout_msec = (diff_time.tv_sec * 1000) +
                ((diff_time.tv_usec + 1000 - 1) / 1000);
        }

        if (timeout_msec <= 0) {
            return ETIMEDOUT;
        }

        // Round up milliseconds to the next tick
        timeout_ticks = (timeout_msec + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;

        /* We have to add 1 more tick of delay

           The reason for this is that vTaskDelay(1) will sleep until the start of the next tick,
           which can be any amount of time up to one tick period. So if we don't add one more tick,
           we're likely to timeout a small time (< 1 tick period) before the requested timeout.
           If we add 1 tick then we will timeout a  small time (< 1 tick period) after the
           requested timeout.
         */
        timeout_ticks += 1;
    }

    esp_pthread_cond_waiter_t w;

    // Around 80 bytes
    StaticSemaphore_t sem_buffer;
    // Create semaphore: first take will block
    w.wait_sem = xSemaphoreCreateCountingStatic(1, 0, &sem_buffer);

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

// The following pthread_condattr_* function definitions are placed here to enable builds of code
// that references these functions but does not actively use them.

int pthread_condattr_init(pthread_condattr_t *attr)
{
    ESP_LOGW(TAG, "%s not yet implemented (%p)", __FUNCTION__, attr);
    return ENOSYS;
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    ESP_LOGW(TAG, "%s not yet implemented (%p)", __FUNCTION__, attr);
    return ENOSYS;
}

int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr, int *restrict pshared)
{
    ESP_LOGW(TAG, "%s not yet implemented (%p)", __FUNCTION__, attr);
    return ENOSYS;
}

int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared)
{
    ESP_LOGW(TAG, "%s not yet implemented (%p)", __FUNCTION__, attr);
    return ENOSYS;
}

int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t *restrict clock_id)
{
    ESP_LOGW(TAG, "%s not yet implemented (%p)", __FUNCTION__, attr);
    return ENOSYS;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
    ESP_LOGW(TAG, "%s: not yet supported!", __func__);
    return 0; // moved here from newlib, where it was 0 instead of ENOSYS
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
    if (*cv == PTHREAD_COND_INITIALIZER) {
        return 0; // never initialized
    }

    esp_pthread_cond_t *cond = (esp_pthread_cond_t *) *cv;
    if (!cond) {
        return EINVAL;
    }

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

/* Hook function to force linking this file */
void pthread_include_pthread_cond_var_impl(void)
{
}
