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
#define K_MUTEX_SHORT       (5000 / portTICK_PERIOD_MS)

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

/* Inline log helper with a fixed tag. These functions are inlined into many
 * translation units (including api/ wrappers), and not all of them call
 * LOG_MODULE_REGISTER, so we cannot reference the per-TU __iso_log_tag
 * here. Use a hardcoded "ISO_MUTEX" tag instead, keeping the same level
 * gating. */
#if CONFIG_BT_ISO_NO_LOG || (CONFIG_BT_ISO_LOG_LEVEL < BT_ISO_LOG_ERROR)
#define K_MUTEX_LOG_ERR(fmt, args...)
#else
#define K_MUTEX_LOG_ERR(fmt, args...)   BT_ISO_LOGE("ISO_MUTEX", fmt, ## args)
#endif

static inline int k_mutex_lock(struct k_mutex *mutex, uint32_t timeout)
{
    assert(mutex);
    assert(mutex->handle);

    if (xSemaphoreTakeRecursive(mutex->handle, timeout) == pdTRUE) {
        return 0;
    }

#if !CONFIG_BT_ISO_NO_LOG && (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_ERROR)
    /* On timeout, dump who timed out and who is currently holding the
     * mutex so the caller's log makes wedge diagnosis a one-look job
     * instead of a multi-task scavenger hunt.
     */
    TaskHandle_t holder = xSemaphoreGetMutexHolder(mutex->handle);
    K_MUTEX_LOG_ERR("LockFail[self=%s][holder=%s]",
                    pcTaskGetName(NULL),
                    holder ? pcTaskGetName(holder) : "<none>");
#else
    K_MUTEX_LOG_ERR("LockFail");
#endif
    return -EIO;
}

static inline int k_mutex_unlock(struct k_mutex *mutex)
{
    assert(mutex);
    assert(mutex->handle);

    if (xSemaphoreGiveRecursive(mutex->handle) != pdTRUE) {
        K_MUTEX_LOG_ERR("UnlockFail");
        return -EIO;
    }

    return 0;
}

/* Semaphore */

#define K_SEM_FOREVER       portMAX_DELAY
#define K_SEM_SHORT         (5000 / portTICK_PERIOD_MS)

struct k_sem {
    SemaphoreHandle_t handle;
    int result;
};

static inline void k_sem_create(struct k_sem *sem)
{
    assert(sem);
    assert(sem->handle == NULL);

    sem->handle = xSemaphoreCreateBinary();
    assert(sem->handle);
    sem->result = 0;
}

static inline void k_sem_delete(struct k_sem *sem)
{
    assert(sem);
    assert(sem->handle);

    vSemaphoreDelete(sem->handle);
    sem->handle = NULL;
}

/* Inline log helper with a fixed tag, mirroring K_MUTEX_LOG_ERR. The sem
 * helpers are inlined across many TUs, not all of which call
 * LOG_MODULE_REGISTER, so we cannot reference the per-TU __iso_log_tag.
 * Hardcode the "ISO_SEM" tag and keep the same level gating. */
#if CONFIG_BT_ISO_NO_LOG || (CONFIG_BT_ISO_LOG_LEVEL < BT_ISO_LOG_ERROR)
#define K_SEM_LOG_ERR(fmt, args...)
#else
#define K_SEM_LOG_ERR(fmt, args...)     BT_ISO_LOGE("ISO_SEM", fmt, ## args)
#endif

static inline int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    assert(sem);
    assert(sem->handle);

    /* Do NOT touch sem->result here. The producer may have already written
     * it and called k_sem_give before this take ran (BTU/HCI cb on a
     * higher-priority task). Clearing now would race-overwrite the
     * producer's value. Caller must k_sem_reset() before initiating the
     * async op that will produce the result. */

    if (xSemaphoreTake(sem->handle, timeout) == pdTRUE) {
        return 0;
    }

#if !CONFIG_BT_ISO_NO_LOG && (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_ERROR)
    K_SEM_LOG_ERR("TakeFail[self=%s]", pcTaskGetName(NULL));
#else
    K_SEM_LOG_ERR("TakeFail");
#endif
    return -EIO;
}

static inline int k_sem_give(struct k_sem *sem)
{
    assert(sem);
    assert(sem->handle);

    if (xSemaphoreGive(sem->handle) != pdTRUE) {
        K_SEM_LOG_ERR("GiveFail");
        return -EIO;
    }

    return 0;
}

/* Discard any pending give. Used by callers that reuse the same sem across
 * cmd/response cycles where a previous cycle timed out — without this, a
 * late give from the timed-out cycle would unblock the next take and the
 * caller would see uninitialized response data. */
static inline void k_sem_reset(struct k_sem *sem)
{
    assert(sem);
    assert(sem->handle);

    xQueueReset(sem->handle);
    sem->result = 0;
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
