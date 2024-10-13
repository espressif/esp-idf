/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <cxxabi.h>
#include <stdint.h>
#include <limits.h>
#include <algorithm>
#include <sys/lock.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

using __cxxabiv1::__guard;

static SemaphoreHandle_t s_static_init_mutex = NULL;        //!< lock used for the critical section
static SemaphoreHandle_t s_static_init_wait_sem = NULL;     //!< counting semaphore used by the waiting tasks
static portMUX_TYPE s_init_spinlock = portMUX_INITIALIZER_UNLOCKED;   //!< spinlock used to guard initialization of the above two primitives
static size_t s_static_init_waiting_count = 0;              //!< number of tasks which are waiting for static init guards
#ifndef _NDEBUG
static size_t s_static_init_max_waiting_count = 0;          //!< maximum ever value of the above; can be inspected using GDB for debugging purposes
#endif

extern "C" int __cxa_guard_acquire(__guard* pg);
extern "C" void __cxa_guard_release(__guard* pg) throw();
extern "C" void __cxa_guard_abort(__guard* pg) throw();
extern "C" void __cxa_guard_dummy(void);

/**
 * Layout of the guard object (defined by the ABI).
 *
 * Compiler will check lower byte before calling guard functions.
 */
typedef struct {
    uint8_t ready;      //!< nonzero if initialization is done
    uint8_t pending;    //!< nonzero if initialization is in progress
} guard_t;

static void static_init_prepare()
{
    portENTER_CRITICAL(&s_init_spinlock);
    if (s_static_init_mutex == NULL) {
        s_static_init_mutex = xSemaphoreCreateMutex();
        s_static_init_wait_sem = xSemaphoreCreateCounting(INT_MAX, 0);
        if (s_static_init_mutex == NULL || s_static_init_wait_sem == NULL) {
            // no way to bail out of static initialization without these
            abort();
        }
    }
    portEXIT_CRITICAL(&s_init_spinlock);
}

/**
 * Use s_static_init_wait_sem to wait until guard->pending == 0.
 * Preconditions:
 * - s_static_init_mutex taken
 * - guard.pending == 1
 * Postconditions:
 * - s_static_init_mutex taken
 * - guard.pending == 0
 */
static void wait_for_guard_obj(guard_t* g)
{
    s_static_init_waiting_count++;
#ifndef _NDEBUG
    s_static_init_max_waiting_count = std::max(s_static_init_waiting_count,
                                               s_static_init_max_waiting_count);
#endif

    do {
        auto result = xSemaphoreGive(s_static_init_mutex);
        assert(result);
        static_cast<void>(result);
        /* Task may be preempted here, but this isn't a problem,
         * as the semaphore will be given exactly the s_static_init_waiting_count
         * number of times; eventually the current task will execute next statement,
         * which will immediately succeed.
         */
        result = xSemaphoreTake(s_static_init_wait_sem, portMAX_DELAY);
        assert(result);
        /* At this point the semaphore was given, so all waiting tasks have woken up.
         * We take s_static_init_mutex before accessing the state of the guard
         * object again.
         */
        result = xSemaphoreTake(s_static_init_mutex, portMAX_DELAY);
        assert(result);
        /* Semaphore may have been given because some other guard object became ready.
         * Check the guard object we need and wait again if it is still pending.
         */
    } while (g->pending);
    s_static_init_waiting_count--;
}

/**
 * Unblock tasks waiting for static initialization to complete.
 * Preconditions:
 * - s_static_init_mutex taken
 * Postconditions:
 * - s_static_init_mutex taken
 */
static void signal_waiting_tasks()
{
    auto count = s_static_init_waiting_count;
    while (count--) {
        xSemaphoreGive(s_static_init_wait_sem);
    }
}

extern "C" int __cxa_guard_acquire(__guard* pg)
{
    guard_t* g = reinterpret_cast<guard_t*>(pg);
    const auto scheduler_started = xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
    if (!scheduler_started) {
        if (g->pending) {
            /* Before the scheduler has started, there we don't support simultaneous
             * static initialization. This may be implemented using a spinlock and a
             * s32c1i instruction, though.
             */
            abort();
        }
    } else {
        if (s_static_init_mutex == NULL) {
            static_init_prepare();
        }

        /* We don't need to use double-checked locking pattern here, as the compiler
         * must generate code to check if the first byte of *pg is non-zero, before
         * calling __cxa_guard_acquire.
         */
        auto result = xSemaphoreTake(s_static_init_mutex, portMAX_DELAY);
        assert(result);
        static_cast<void>(result);
        if (g->pending) {
            /* Another task is doing initialization at the moment; wait until it calls
             * __cxa_guard_release or __cxa_guard_abort
             */
            wait_for_guard_obj(g);
            /* At this point there are two scenarios:
             * - the task which was doing static initialization has called __cxa_guard_release,
             *   which means that g->ready is set. We need to return 0.
             * - the task which was doing static initialization has called __cxa_guard_abort,
             *   which means that g->ready is not set; we should acquire the guard and return 1,
             *   same as for the case if we didn't have to wait.
             * Note: actually the second scenario is unlikely to occur in the current
             * configuration because exception support is disabled.
             */
        }
    }
    int ret;
    if (g->ready) {
        /* Static initialization has been done by another task; nothing to do here */
        ret = 0;
    } else {
        /* Current task can start doing static initialization */
        g->pending = 1;
        ret = 1;
    }
    if (scheduler_started) {
        auto result = xSemaphoreGive(s_static_init_mutex);
        assert(result);
        static_cast<void>(result);
    }
    return ret;
}

extern "C" void __cxa_guard_release(__guard* pg) throw()
{
    guard_t* g = reinterpret_cast<guard_t*>(pg);
    const auto scheduler_started = xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
    if (scheduler_started) {
        auto result = xSemaphoreTake(s_static_init_mutex, portMAX_DELAY);
        assert(result);
        static_cast<void>(result);
    }
    assert(g->pending && "tried to release a guard which wasn't acquired");
    g->pending = 0;
    /* Initialization was successful */
    g->ready = 1;
    if (scheduler_started) {
        /* Unblock the tasks waiting for static initialization to complete */
        signal_waiting_tasks();
        auto result = xSemaphoreGive(s_static_init_mutex);
        assert(result);
        static_cast<void>(result);
    }
}

extern "C" void __cxa_guard_abort(__guard* pg) throw()
{
    guard_t* g = reinterpret_cast<guard_t*>(pg);
    const auto scheduler_started = xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
    if (scheduler_started) {
        auto result = xSemaphoreTake(s_static_init_mutex, portMAX_DELAY);
        assert(result);
        static_cast<void>(result);
    }
    assert(!g->ready && "tried to abort a guard which is ready");
    assert(g->pending && "tried to release a guard which is not acquired");
    g->pending = 0;
    if (scheduler_started) {
        /* Unblock the tasks waiting for static initialization to complete */
        signal_waiting_tasks();
        auto result = xSemaphoreGive(s_static_init_mutex);
        assert(result);
        static_cast<void>(result);
    }
}

/**
 * Dummy function used to force linking this file instead of the same one in libstdc++.
 * This works via -u __cxa_guard_dummy flag in component.mk
 */
extern "C" void __cxa_guard_dummy(void)
{
}
