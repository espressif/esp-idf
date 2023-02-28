/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include "semaphore.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define BLN 1000000000
#define MIO 1000000

// sem_t is used to store SemaphoreHandle_t, so make sure there is enough space
static_assert(sizeof(sem_t) == sizeof(SemaphoreHandle_t));

int sem_destroy(sem_t * semaphore)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    SemaphoreHandle_t freertos_semaphore = (SemaphoreHandle_t) *semaphore;
    vSemaphoreDelete(freertos_semaphore);
    return 0;
}

int sem_init(sem_t * semaphore, int pshared, unsigned value)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (value > SEM_VALUE_MAX) {
        errno = EINVAL;
        return -1;
    }

    SemaphoreHandle_t freertos_semaphore;
    freertos_semaphore = xSemaphoreCreateCounting(SEM_VALUE_MAX, value);

    if (freertos_semaphore == NULL) {
        errno = ENOSPC;
        return -1;
    }

    *semaphore = (sem_t) freertos_semaphore;
    return 0;
}

int sem_post(sem_t * semaphore)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    SemaphoreHandle_t freertos_semaphore = (SemaphoreHandle_t) *semaphore;
    BaseType_t ret = xSemaphoreGive(freertos_semaphore);

    if (ret == pdFALSE) {
        errno = EAGAIN;
        return -1;
    }

    return 0;
}

int sem_timedwait(sem_t * restrict semaphore, const struct timespec *restrict abstime)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (abstime == NULL || abstime->tv_nsec >= (1 * BLN) || abstime->tv_nsec < 0) {
        errno = EINVAL;
        return -1;
    }

    TickType_t timeout_ticks;
    struct timespec cur_time;
    clock_gettime(CLOCK_REALTIME, &cur_time);

    if (timespeccmp(abstime, &cur_time, <)) {
        timeout_ticks = 0;
    } else {
        struct timespec diff_time;
        timespecsub(abstime, &cur_time, &diff_time);

        long timeout_msec;
        // Round up timeout nanoseconds to the next millisecond
        timeout_msec = (diff_time.tv_sec * 1000) +
            ((diff_time.tv_nsec + (1 * MIO) - 1) / (1 * MIO));

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

    SemaphoreHandle_t freertos_semaphore = (SemaphoreHandle_t) *semaphore;
    BaseType_t sem_take_result;
    sem_take_result = xSemaphoreTake(freertos_semaphore, timeout_ticks);
    if (sem_take_result == pdFALSE) {
        errno = ETIMEDOUT;
        return -1;
    }

    return 0;
}

int sem_trywait(sem_t * semaphore)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    SemaphoreHandle_t freertos_semaphore = (SemaphoreHandle_t) *semaphore;

    BaseType_t ret = xSemaphoreTake(freertos_semaphore, 0);

    if (ret == pdFALSE) {
        errno = EAGAIN;
        return -1;
    }

    return 0;
}

int sem_wait(sem_t * semaphore)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    SemaphoreHandle_t freertos_semaphore = (SemaphoreHandle_t) *semaphore;

    // Only returns failure if block time expires, but we block indefinitely, hence not return code check
    xSemaphoreTake(freertos_semaphore, portMAX_DELAY);
    return 0;
}

int sem_getvalue(sem_t *restrict semaphore, int *restrict sval)
{
    if (semaphore == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (sval == NULL) {
        errno = EINVAL;
        return -1;
    }

    SemaphoreHandle_t freertos_semaphore = (SemaphoreHandle_t) *semaphore;
    *sval = uxSemaphoreGetCount(freertos_semaphore);
    return 0;
}

/* Hook function to force linking this file */
void pthread_include_pthread_semaphore_impl(void) { }
