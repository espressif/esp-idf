/*
 * SPDX-FileCopyrightText: 2016 Wind River Systems, Inc.
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#include <zephyr/sys/slist.h>
#include <zephyr/logging/log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "toolchain.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Mutex */

#define K_MUTEX_FOREVER     portMAX_DELAY

struct k_mutex {
    SemaphoreHandle_t handle;
};

static inline void k_mutex_create(struct k_mutex *mutex)
{
    assert(mutex);
    assert(mutex->handle == NULL);

    mutex->handle = xSemaphoreCreateRecursiveMutex();
    assert(mutex->handle);
}

static inline void k_mutex_delete(struct k_mutex *mutex)
{
    assert(mutex);
    assert(mutex->handle);

    vSemaphoreDelete(mutex->handle);
    mutex->handle = NULL;
}

static inline int k_mutex_lock(struct k_mutex *mutex, uint32_t timeout)
{
    assert(mutex);
    assert(mutex->handle);

    if (xSemaphoreTakeRecursive(mutex->handle, timeout) != pdTRUE) {
        LOG_ERR("KMutexLockFail");
        return -EIO;
    }

    return 0;
}

static inline int k_mutex_unlock(struct k_mutex *mutex)
{
    assert(mutex);
    assert(mutex->handle);

    if (xSemaphoreGiveRecursive(mutex->handle) != pdTRUE) {
        LOG_ERR("KMutexUnlockFail");
        return -EIO;
    }

    return 0;
}

/* Timer */

typedef uint32_t k_timeout_t;

#define K_NO_WAIT       0
#define K_FOREVER       (-1)

#define MSEC_PER_SEC    1000
#define K_USEC(t)       (t)
#define K_MSEC(ms)      (ms)
#define K_SECONDS(s)    K_MSEC((s) * MSEC_PER_SEC)
#define K_MINUTES(m)    K_SECONDS((m) * 60)
#define K_HOURS(h)      K_MINUTES((h) * 60)

struct k_work;

typedef void (*k_work_handler_t)(struct k_work *work);

struct k_work {
    void *timer;
    k_work_handler_t handler;
    int64_t timeout_us;
    void *user_data;
};

struct k_work_sync {
    struct k_work work;
};

struct k_work_delayable {
    struct k_work work;
};

#define K_WORK_DEFINE(work, work_handler) \
        struct k_work work = { \
            .handler = work_handler, \
        }

#define K_WORK_DELAYABLE_DEFINE(dwork, work_handler) \
        struct k_work_delayable dwork = { \
            .work.handler = work_handler, \
        }

#define K_TIMEOUT_EQ(a, b) ((a) == (b))

typedef void (*k_work_handler_t)(struct k_work *work);

int k_work_submit(struct k_work *work);

bool k_work_is_pending(struct k_work *work);

void k_work_init(struct k_work *work, k_work_handler_t handler);

struct k_work_delayable *k_work_delayable_from_work(struct k_work *work);

void k_work_init_delayable(struct k_work_delayable *dwork,
                           k_work_handler_t handler);

void k_work_deinit_delayable(struct k_work_delayable *dwork);

int k_work_cancel_delayable(struct k_work_delayable *dwork);

bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork,
                                  struct k_work_sync *sync);

int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t delay);

int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t delay);

int k_work_schedule_periodic(struct k_work_delayable *dwork, k_timeout_t period_ms);

k_timeout_t k_work_delayable_remaining_get(struct k_work_delayable *dwork);

/* Slist */

struct k_fifo {
    sys_slist_t slist;
};

#ifdef __cplusplus
}
#endif
