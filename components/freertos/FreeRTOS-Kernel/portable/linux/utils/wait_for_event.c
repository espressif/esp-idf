/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "wait_for_event.h"

/*-----------------------------------------------------------*/
/* Create a new event */
event_t *event_create(void)
{
    event_t * ev = malloc(sizeof(event_t));
    assert(ev != NULL);

    ev->event_triggered = false;
    pthread_mutex_init(&ev->mutex, NULL);
    pthread_cond_init(&ev->cond, NULL);

    return ev;
}

/*-----------------------------------------------------------*/
/* Delete an event */
void event_delete(event_t *ev)
{
    pthread_mutex_destroy(&ev->mutex);
    pthread_cond_destroy(&ev->cond);
    free(ev);
}

/*-----------------------------------------------------------*/
/* Cleanup handler for event_wait*(): a cancelled waiter exits with the mutex
 * re-locked by the cond wait. Unlock it, or event_clear() deadlocks. */
static void event_unlock_on_cancel(void *mutex)
{
    pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

/*-----------------------------------------------------------*/
/* Wait for event indefinitely (cooperative blocking) */
bool event_wait(event_t *ev)
{
    pthread_mutex_lock(&ev->mutex);
    pthread_cleanup_push(event_unlock_on_cancel, &ev->mutex);

    while (!ev->event_triggered)
    {
        pthread_cond_wait(&ev->cond, &ev->mutex);
    }

    ev->event_triggered = false;
    pthread_cleanup_pop(1);
    return true;
}

/*-----------------------------------------------------------*/
/* Wait for event with timeout (milliseconds) */
bool event_wait_timed(event_t *ev, time_t ms)
{
    struct timespec ts;
    /* volatile: glibc's pthread_cleanup_push() uses setjmp */
    volatile int ret = 0;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec  += ms / 1000;
    ts.tv_nsec += (ms % 1000) * 1000000;

    /* Normalize tv_nsec in case it exceeds 1,000,000,000 */
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec += ts.tv_nsec / 1000000000L;
        ts.tv_nsec = ts.tv_nsec % 1000000000L;
    }

    pthread_mutex_lock(&ev->mutex);
    pthread_cleanup_push(event_unlock_on_cancel, &ev->mutex);

    while (!ev->event_triggered && ret == 0)
    {
        ret = pthread_cond_timedwait(&ev->cond, &ev->mutex, &ts);
    }

    ev->event_triggered = false;
    pthread_cleanup_pop(1);
    return ret != ETIMEDOUT;
}

/*-----------------------------------------------------------*/
/* Signal / resume an event */
void event_signal(event_t *ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->event_triggered = true;
    pthread_cond_signal(&ev->cond);
    pthread_mutex_unlock(&ev->mutex);
}

/*-----------------------------------------------------------*/
/* Discard a pending signal that was never consumed by a wait */
void event_clear(event_t *ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->event_triggered = false;
    pthread_mutex_unlock(&ev->mutex);
}
