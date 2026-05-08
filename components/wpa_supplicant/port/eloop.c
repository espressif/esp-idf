/*
 * Event loop based on select() loop
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "includes.h"

#include "common.h"
#include "list.h"
#include "eloop.h"
#include "esp_wifi_driver.h"
#include "rom/ets_sys.h"
#include <stdatomic.h>
#include <stdint.h>

bool current_task_is_wifi_task(void);

enum eloop_state {
    ELOOP_STATE_STOPPED = 0,
    ELOOP_STATE_RUNNING,
    ELOOP_STATE_DESTROYING,
};

struct eloop_timeout {
    struct dl_list list;
    struct os_reltime time;
    void *eloop_data;
    void *user_data;
    eloop_timeout_handler handler;
    eloop_timeout_handler destroy_handler;
    eloop_blocking_timeout_handler blocking_handler;
    void *sync_semph;
    atomic_uint ref_count;
    int ret;
    bool blocking;
#ifdef ELOOP_DEBUG
    char func_name[100];
    int line;
#endif
};

struct eloop_data {
    struct dl_list timeout;
    ETSTimer eloop_timer;
    atomic_int state;
    atomic_uint dispatch_count;
};

#define ELOOP_LOCK() os_mutex_lock(eloop_data_lock)
#define ELOOP_UNLOCK() os_mutex_unlock(eloop_data_lock)

static void *eloop_data_lock = NULL;
static struct eloop_data eloop;
static atomic_bool eloop_lifecycle_busy = false;

static void eloop_lifecycle_lock(void)
{
    bool expected = false;

    while (!atomic_compare_exchange_weak(&eloop_lifecycle_busy, &expected, true)) {
        expected = false;
        vTaskDelay(1);
    }
}

static void eloop_lifecycle_unlock(void)
{
    atomic_store(&eloop_lifecycle_busy, false);
}

#ifdef ELOOP_DEBUG
static void eloop_debug_log_timeout(const char *action,
                                    const struct eloop_timeout *timeout,
                                    const struct os_reltime *remaining)
{
    if (timeout->blocking) {
        if (remaining) {
            wpa_printf(MSG_DEBUG,
                       "ELOOP: %s blocking timer fn:%p scheduled by %s:%d "
                       "eloop_data=%p user_data=%p in %llu.%06d",
                       action, timeout->blocking_handler, timeout->func_name,
                       timeout->line, timeout->eloop_data, timeout->user_data,
                       (unsigned long long) remaining->sec,
                       (int) remaining->usec);
        } else {
            wpa_printf(MSG_DEBUG,
                       "ELOOP: %s blocking timer fn:%p scheduled by %s:%d "
                       "eloop_data=%p user_data=%p",
                       action, timeout->blocking_handler, timeout->func_name,
                       timeout->line, timeout->eloop_data, timeout->user_data);
        }
    } else {
        if (remaining) {
            wpa_printf(MSG_DEBUG,
                       "ELOOP: %s timer fn:%p scheduled by %s:%d "
                       "eloop_data=%p user_data=%p in %llu.%06d",
                       action, timeout->handler, timeout->func_name,
                       timeout->line, timeout->eloop_data, timeout->user_data,
                       (unsigned long long) remaining->sec,
                       (int) remaining->usec);
        } else {
            wpa_printf(MSG_DEBUG,
                       "ELOOP: %s timer fn:%p scheduled by %s:%d "
                       "eloop_data=%p user_data=%p",
                       action, timeout->handler, timeout->func_name,
                       timeout->line, timeout->eloop_data, timeout->user_data);
        }
    }
}
#endif

static bool eloop_is_running(void)
{
    return atomic_load(&eloop.state) == ELOOP_STATE_RUNNING;
}

static int eloop_get_reltime_now(struct os_reltime *time)
{
    return os_get_reltime(time);
}

static void *eloop_alloc(size_t len)
{
    return os_zalloc(len);
}

static void *eloop_sync_sem_create(void)
{
    return os_semphr_create(1, 0);
}

static void eloop_timeout_get(struct eloop_timeout *timeout)
{
    atomic_fetch_add(&timeout->ref_count, 1);
}

static void eloop_timeout_put(struct eloop_timeout *timeout)
{
    if (atomic_fetch_sub(&timeout->ref_count, 1) != 1) {
        return;
    }

    if (timeout->sync_semph) {
        os_semphr_delete(timeout->sync_semph);
    }
    os_free(timeout);
}

static void eloop_remaining(const struct os_reltime *deadline,
                            const struct os_reltime *now,
                            struct os_reltime *remaining)
{
    if (os_reltime_before(now, deadline)) {
        os_reltime_sub(deadline, now, remaining);
    } else {
        remaining->sec = 0;
        remaining->usec = 0;
    }
}

static uint32_t eloop_reltime_to_ms_ceil(const struct os_reltime *remaining)
{
    uint64_t ms = remaining->sec * 1000;

    ms += ((uint32_t)remaining->usec + 999U) / 1000U;
    if (ms > UINT32_MAX) {
        return UINT32_MAX;
    }

    return (uint32_t)ms;
}

static int eloop_get_abs_timeout(unsigned int secs, unsigned int usecs,
                                 struct os_reltime *time)
{
    os_time_t now_sec;

    if (eloop_get_reltime_now(time) < 0) {
        return -1;
    }

    now_sec = time->sec;
    time->sec += secs;
    if (time->sec < now_sec) {
        return -1;
    }

    time->usec += usecs;
    while (time->usec >= 1000000) {
        time->sec++;
        time->usec -= 1000000;
    }

    if (time->sec < now_sec) {
        return -1;
    }

    return 0;
}

static int eloop_insert_timeout_locked(struct eloop_timeout *timeout)
{
    struct eloop_timeout *tmp;
    int count = 0;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (os_reltime_before(&timeout->time, &tmp->time)) {
            dl_list_add(tmp->list.prev, &timeout->list);
            return count;
        }
        count++;
    }

    dl_list_add_tail(&eloop.timeout, &timeout->list);
    return count;
}

static void eloop_arm_next_locked(void)
{
    struct os_reltime now, remaining;
    struct eloop_timeout *timeout;
    uint32_t ms;

    os_timer_disarm(&eloop.eloop_timer);
    if (!eloop_is_running()) {
#ifdef ELOOP_DEBUG
        wpa_printf(MSG_DEBUG, "ELOOP: timer arm skipped because eloop is not running");
#endif
        return;
    }

    if (dl_list_empty(&eloop.timeout)) {
#ifdef ELOOP_DEBUG
        wpa_printf(MSG_DEBUG, "ELOOP: timer arm skipped because timeout list is empty");
#endif
        return;
    }

    timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, list);
    if (eloop_get_reltime_now(&now) < 0) {
        os_timer_arm(&eloop.eloop_timer, 100, 0);
        return;
    }

    eloop_remaining(&timeout->time, &now, &remaining);
    ms = eloop_reltime_to_ms_ceil(&remaining);
#ifdef ELOOP_DEBUG
    eloop_debug_log_timeout("arming next", timeout, &remaining);
#endif
    os_timer_arm(&eloop.eloop_timer, ms, 0);
}

static bool eloop_timeout_matches(struct eloop_timeout *timeout,
                                  eloop_timeout_handler handler,
                                  void *eloop_data,
                                  void *user_data)
{
    if (timeout->blocking || timeout->handler != handler) {
        return false;
    }

    if (timeout->eloop_data != eloop_data && eloop_data != ELOOP_ALL_CTX) {
        return false;
    }

    if (timeout->user_data != user_data && user_data != ELOOP_ALL_CTX) {
        return false;
    }

    return true;
}

static void eloop_finish_blocking_timeout(struct eloop_timeout *timeout, int ret)
{
    timeout->ret = ret;
#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: waking blocking timer scheduled by %s:%d with ret=%d",
               timeout->func_name, timeout->line, ret);
#endif
    os_semphr_give(timeout->sync_semph);
    eloop_timeout_put(timeout);
}

static void eloop_run_async_timeout(struct eloop_timeout *timeout,
                                    eloop_timeout_handler handler)
{
    if (handler) {
        handler(timeout->eloop_data, timeout->user_data);
    }
    eloop_timeout_put(timeout);
}

static void eloop_release_timeout_list(struct dl_list *timeouts)
{
    struct eloop_timeout *timeout, *prev;

    dl_list_for_each_safe(timeout, prev, timeouts, struct eloop_timeout, list) {
        dl_list_del(&timeout->list);
#ifdef ELOOP_DEBUG
        eloop_debug_log_timeout("releasing cancelled", timeout, NULL);
#endif
        eloop_timeout_put(timeout);
    }
}

static void eloop_fail_blocking_timeout_list(struct dl_list *timeouts)
{
    struct eloop_timeout *timeout, *prev;

    dl_list_for_each_safe(timeout, prev, timeouts, struct eloop_timeout, list) {
        dl_list_del(&timeout->list);
#ifdef ELOOP_DEBUG
        eloop_debug_log_timeout("failing pending destroy-time", timeout, NULL);
#endif
        eloop_finish_blocking_timeout(timeout, -1);
    }
}

static void eloop_run_destroy_timeout_list(struct dl_list *timeouts)
{
    struct eloop_timeout *timeout, *prev;

    dl_list_for_each_safe(timeout, prev, timeouts, struct eloop_timeout, list) {
        dl_list_del(&timeout->list);
#ifdef ELOOP_DEBUG
        wpa_printf(MSG_INFO, "ELOOP: destroying timer %p scheduled by %s:%d",
                   timeout->handler, timeout->func_name, timeout->line);
#endif
        eloop_run_async_timeout(timeout, timeout->destroy_handler);
    }
}

#ifdef ELOOP_DEBUG
static struct eloop_timeout *eloop_timeout_alloc(const char *func, int line)
#else
static struct eloop_timeout *eloop_timeout_alloc(void)
#endif
{
    struct eloop_timeout *timeout = eloop_alloc(sizeof(*timeout));

    if (!timeout) {
        return NULL;
    }

    atomic_init(&timeout->ref_count, 1);
#ifdef ELOOP_DEBUG
    os_strlcpy(timeout->func_name, func, sizeof(timeout->func_name));
    timeout->line = line;
#endif
    return timeout;
}

static int eloop_run_wrapper(void *data)
{
    eloop_run();
    return 0;
}

static void eloop_run_timer(void *args)
{
    /* Execute timers in pptask context to make it thread safe */
    wifi_ipc_config_t cfg;

    cfg.fn = eloop_run_wrapper;
    cfg.arg = NULL;
    cfg.arg_size = 0;
    esp_wifi_ipc_internal(&cfg, false);
}

static void eloop_reset_state_locked(void)
{
    os_memset(&eloop, 0, sizeof(eloop));
    dl_list_init(&eloop.timeout);
    os_timer_disarm(&eloop.eloop_timer);
    os_timer_setfn(&eloop.eloop_timer, (ETSTimerFunc *)eloop_run_timer, NULL);
    atomic_store(&eloop.dispatch_count, 0);
    atomic_store(&eloop.state, ELOOP_STATE_RUNNING);
}

int eloop_init(void)
{
#ifdef ELOOP_DEBUG
    bool initialized = false;
#endif

    eloop_lifecycle_lock();
    if (eloop_data_lock) {
        int current_state = atomic_load(&eloop.state);

        if (current_state == ELOOP_STATE_RUNNING) {
            eloop_lifecycle_unlock();
            return 0;
        }
        if (current_state == ELOOP_STATE_DESTROYING) {
            eloop_lifecycle_unlock();
            return -1;
        }

        ELOOP_LOCK();
        eloop_reset_state_locked();
        ELOOP_UNLOCK();
#ifdef ELOOP_DEBUG
        initialized = true;
#endif
    } else {
        eloop_data_lock = os_recursive_mutex_create();
        if (!eloop_data_lock) {
            wpa_printf(MSG_ERROR, "failed to create eloop data lock");
            eloop_lifecycle_unlock();
            return -1;
        }

        ELOOP_LOCK();
        eloop_reset_state_locked();
        ELOOP_UNLOCK();
#ifdef ELOOP_DEBUG
        initialized = true;
#endif
    }
    eloop_lifecycle_unlock();
#ifdef ELOOP_DEBUG
    if (initialized) {
        wpa_printf(MSG_DEBUG, "ELOOP: initialized");
    }
#endif
    return 0;
}

#ifdef ELOOP_DEBUG
int eloop_register_timeout_debug(unsigned int secs, unsigned int usecs,
                                 eloop_timeout_handler handler, void *eloop_data,
                                 void *user_data, const char *func, int line)
#else
int eloop_register_timeout(unsigned int secs, unsigned int usecs,
                           eloop_timeout_handler handler,
                           void *eloop_data, void *user_data)
#endif
{
    struct eloop_timeout *timeout;
#ifdef ELOOP_DEBUG
    int count = 0;
#endif

    if (!eloop_is_running()) {
        return -1;
    }

#ifdef ELOOP_DEBUG
    timeout = eloop_timeout_alloc(func, line);
#else
    timeout = eloop_timeout_alloc();
#endif
    if (!timeout) {
        return -1;
    }

    if (eloop_get_abs_timeout(secs, usecs, &timeout->time) < 0) {
        wpa_printf(MSG_DEBUG,
                   "ELOOP: Too long timeout (secs=%u usecs=%u) to ever happen - ignore it",
                   secs, usecs);
        eloop_timeout_put(timeout);
        return -1;
    }

    timeout->eloop_data = eloop_data;
    timeout->user_data = user_data;
    timeout->handler = handler;
    /* Destroy callbacks must be explicitly opt-in to avoid side effects during teardown. */
    timeout->destroy_handler = NULL;

    ELOOP_LOCK();
    if (!eloop_is_running()) {
        ELOOP_UNLOCK();
        eloop_timeout_put(timeout);
        return -1;
    }
#ifdef ELOOP_DEBUG
    count = eloop_insert_timeout_locked(timeout);
    wpa_printf(MSG_DEBUG, "ELOOP: Added timer from %s:%d to call %p, current order=%d",
               timeout->func_name, timeout->line, timeout->handler, count);
#else
    eloop_insert_timeout_locked(timeout);
#endif
    eloop_arm_next_locked();
    ELOOP_UNLOCK();

    return 0;
}

#ifdef ELOOP_DEBUG
int eloop_register_timeout_blocking_debug(eloop_blocking_timeout_handler handler,
                                          void *eloop_data, void *user_data,
                                          const char *func, int line)
#else
int eloop_register_timeout_blocking(eloop_blocking_timeout_handler handler,
                                    void *eloop_data, void *user_data)
#endif
{
    struct eloop_timeout *timeout;
#ifdef ELOOP_DEBUG
    int count = 0;
#endif
    int ret;

    if (!eloop_is_running()) {
        return -1;
    }

    if (current_task_is_wifi_task()) {
        assert(false);
        return -1;
    }

#ifdef ELOOP_DEBUG
    timeout = eloop_timeout_alloc(func, line);
#else
    timeout = eloop_timeout_alloc();
#endif
    if (!timeout) {
        return -1;
    }

    if (eloop_get_abs_timeout(0, 0, &timeout->time) < 0) {
        eloop_timeout_put(timeout);
        return -1;
    }

    timeout->eloop_data = eloop_data;
    timeout->user_data = user_data;
    timeout->blocking_handler = handler;
    timeout->blocking = true;
    timeout->sync_semph = eloop_sync_sem_create();
    if (!timeout->sync_semph) {
        wpa_printf(MSG_INFO, "ELOOP: sync semphr not available");
        eloop_timeout_put(timeout);
        return -1;
    }

    /* Keep one ref for the waiting caller and one ref for eloop ownership. */
    eloop_timeout_get(timeout);

    ELOOP_LOCK();
    if (!eloop_is_running()) {
        ELOOP_UNLOCK();
        /* Drop both refs: caller-held ref + eloop-owned ref acquired above. */
        eloop_timeout_put(timeout);
        eloop_timeout_put(timeout);
        return -1;
    }
#ifdef ELOOP_DEBUG
    count = eloop_insert_timeout_locked(timeout);
    wpa_printf(MSG_DEBUG, "ELOOP: Added blocking timer from %s:%d to call %p, current order=%d",
               timeout->func_name, timeout->line, timeout->blocking_handler, count);
#else
    eloop_insert_timeout_locked(timeout);
#endif
    eloop_arm_next_locked();
    ELOOP_UNLOCK();

#ifdef ELOOP_DEBUG
    eloop_debug_log_timeout("waiting on", timeout, NULL);
#endif
    os_semphr_take(timeout->sync_semph, OS_BLOCK);
    ret = timeout->ret;
#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: blocking timer scheduled by %s:%d completed with ret=%d",
               timeout->func_name, timeout->line, ret);
#endif
    eloop_timeout_put(timeout);
    return ret;
}

#ifdef ELOOP_DEBUG
int eloop_cancel_timeout_debug(eloop_timeout_handler handler, void *eloop_data,
                               void *user_data, const char *func, int line)
#else
int eloop_cancel_timeout(eloop_timeout_handler handler,
                         void *eloop_data, void *user_data)
#endif
{
    struct dl_list removed;
    struct eloop_timeout *timeout, *prev;
    int removed_count = 0;

    if (!eloop_data_lock) {
        return 0;
    }

    dl_list_init(&removed);

    ELOOP_LOCK();
    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                          struct eloop_timeout, list) {
        if (!eloop_timeout_matches(timeout, handler, eloop_data, user_data)) {
            continue;
        }

        dl_list_del(&timeout->list);
        dl_list_add_tail(&removed, &timeout->list);
        removed_count++;
    }
    if (removed_count) {
        eloop_arm_next_locked();
    }
    ELOOP_UNLOCK();

#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: %s:%d removed timer handler=%p count=%d",
               func, line, handler, removed_count);
#endif

    eloop_release_timeout_list(&removed);
    return removed_count;
}

int eloop_cancel_timeout_one(eloop_timeout_handler handler,
                             void *eloop_data, void *user_data,
                             struct os_reltime *remaining)
{
    struct eloop_timeout *timeout, *prev, *removed = NULL;
    struct os_reltime now;

    if (!eloop_data_lock) {
        remaining->sec = remaining->usec = 0;
        return 0;
    }

    remaining->sec = remaining->usec = 0;
    if (eloop_get_reltime_now(&now) < 0) {
        return 0;
    }

    ELOOP_LOCK();
    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                          struct eloop_timeout, list) {
        if (!eloop_timeout_matches(timeout, handler, eloop_data, user_data)) {
            continue;
        }

        eloop_remaining(&timeout->time, &now, remaining);
        dl_list_del(&timeout->list);
        removed = timeout;
        eloop_arm_next_locked();
        break;
    }
    ELOOP_UNLOCK();

    if (removed) {
#ifdef ELOOP_DEBUG
        eloop_debug_log_timeout("removed single", removed, remaining);
#endif
        eloop_timeout_put(removed);
        return 1;
    }

    return 0;
}

int eloop_is_timeout_registered(eloop_timeout_handler handler,
                                void *eloop_data, void *user_data)
{
    struct eloop_timeout *timeout;
    int registered = 0;

    if (!eloop_data_lock) {
        return 0;
    }

    ELOOP_LOCK();
    dl_list_for_each(timeout, &eloop.timeout, struct eloop_timeout, list) {
        if (!eloop_timeout_matches(timeout, handler, eloop_data, user_data)) {
            continue;
        }

        registered = 1;
        break;
    }
    ELOOP_UNLOCK();

    return registered;
}

int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
                          eloop_timeout_handler handler, void *eloop_data,
                          void *user_data)
{
    struct os_reltime now, requested, remaining, new_time;
    struct eloop_timeout *timeout;
    int ret = -1;

    if (!eloop_data_lock) {
        return -1;
    }

    requested.sec = req_secs;
    requested.usec = req_usecs;

    ELOOP_LOCK();
    if (eloop_get_reltime_now(&now) < 0) {
        ELOOP_UNLOCK();
        return -1;
    }
    dl_list_for_each(timeout, &eloop.timeout, struct eloop_timeout, list) {
        if (!eloop_timeout_matches(timeout, handler, eloop_data, user_data)) {
            continue;
        }

        eloop_remaining(&timeout->time, &now, &remaining);
        ret = 0;
        if (os_reltime_before(&requested, &remaining)) {
            if (eloop_get_abs_timeout(requested.sec, requested.usec, &new_time) < 0) {
                ret = -1;
                break;
            }

            dl_list_del(&timeout->list);
            timeout->time = new_time;
            eloop_insert_timeout_locked(timeout);
            eloop_arm_next_locked();
            ret = 1;
#ifdef ELOOP_DEBUG
            eloop_debug_log_timeout("depleted", timeout, &requested);
#endif
        }
#ifdef ELOOP_DEBUG
        if (ret == 0) {
            eloop_debug_log_timeout("deplete left unchanged", timeout, &remaining);
        }
#endif
        break;
    }
    ELOOP_UNLOCK();

#ifdef ELOOP_DEBUG
    if (ret == -1) {
        wpa_printf(MSG_DEBUG, "ELOOP: deplete could not find timer fn:%p eloop_data=%p user_data=%p",
                   handler, eloop_data, user_data);
    }
#endif
    return ret;
}

int eloop_replenish_timeout(unsigned int req_secs, unsigned int req_usecs,
                            eloop_timeout_handler handler, void *eloop_data,
                            void *user_data)
{
    struct os_reltime now, requested, remaining, new_time;
    struct eloop_timeout *timeout;
    int ret = -1;

    if (!eloop_data_lock) {
        return -1;
    }

    requested.sec = req_secs;
    requested.usec = req_usecs;

    ELOOP_LOCK();
    if (eloop_get_reltime_now(&now) < 0) {
        ELOOP_UNLOCK();
        return -1;
    }
    dl_list_for_each(timeout, &eloop.timeout, struct eloop_timeout, list) {
        if (!eloop_timeout_matches(timeout, handler, eloop_data, user_data)) {
            continue;
        }

        eloop_remaining(&timeout->time, &now, &remaining);
        ret = 0;
        if (os_reltime_before(&remaining, &requested)) {
            if (eloop_get_abs_timeout(requested.sec, requested.usec, &new_time) < 0) {
                ret = -1;
                break;
            }

            dl_list_del(&timeout->list);
            timeout->time = new_time;
            eloop_insert_timeout_locked(timeout);
            eloop_arm_next_locked();
            ret = 1;
#ifdef ELOOP_DEBUG
            eloop_debug_log_timeout("replenished", timeout, &requested);
#endif
        }
#ifdef ELOOP_DEBUG
        if (ret == 0) {
            eloop_debug_log_timeout("replenish left unchanged", timeout, &remaining);
        }
#endif
        break;
    }
    ELOOP_UNLOCK();

#ifdef ELOOP_DEBUG
    if (ret == -1) {
        wpa_printf(MSG_DEBUG, "ELOOP: replenish could not find timer fn:%p eloop_data=%p user_data=%p",
                   handler, eloop_data, user_data);
    }
#endif
    return ret;
}

void eloop_run(void)
{
    if (!eloop_is_running()) {
        return;
    }

    while (eloop_is_running()) {
        struct eloop_timeout *timeout;
        struct os_reltime now;

        ELOOP_LOCK();
        if (!eloop_is_running()) {
            ELOOP_UNLOCK();
            break;
        }

        if (dl_list_empty(&eloop.timeout)) {
            os_timer_disarm(&eloop.eloop_timer);
            ELOOP_UNLOCK();
            break;
        }

        timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, list);
        if (eloop_get_reltime_now(&now) < 0) {
            eloop_arm_next_locked();
            ELOOP_UNLOCK();
            break;
        }
        if (os_reltime_before(&now, &timeout->time)) {
#ifdef ELOOP_DEBUG
            struct os_reltime remaining;

            eloop_remaining(&timeout->time, &now, &remaining);
            eloop_debug_log_timeout("not ready, deferring", timeout, &remaining);
#endif
            eloop_arm_next_locked();
            ELOOP_UNLOCK();
            break;
        }

        dl_list_del(&timeout->list);
        atomic_fetch_add(&eloop.dispatch_count, 1);
        ELOOP_UNLOCK();

        if (timeout->blocking) {
#ifdef ELOOP_DEBUG
            wpa_printf(MSG_DEBUG, "ELOOP: Running blocking timer fn:%p scheduled by %s:%d",
                       timeout->blocking_handler, timeout->func_name, timeout->line);
#endif
            eloop_finish_blocking_timeout(timeout,
                                          timeout->blocking_handler(timeout->eloop_data,
                                                                    timeout->user_data));
        } else {
#ifdef ELOOP_DEBUG
            wpa_printf(MSG_DEBUG, "ELOOP: Running timer fn:%p scheduled by %s:%d",
                       timeout->handler, timeout->func_name, timeout->line);
#endif
            eloop_run_async_timeout(timeout, timeout->handler);
        }

        atomic_fetch_sub(&eloop.dispatch_count, 1);
    }
}

void eloop_destroy(void)
{
    struct dl_list pending_timeouts;
    struct dl_list pending_waiters;
    struct eloop_timeout *timeout, *prev;
    uint32_t wait_loops = 0;
    const uint32_t wait_step_ms = 100;
    const uint32_t wait_warn_loops = 5000 / wait_step_ms;
    int old_state;
#ifdef ELOOP_DEBUG
    int async_count = 0;
    int blocking_count = 0;
#endif

    eloop_lifecycle_lock();
    if (!eloop_data_lock) {
        eloop_lifecycle_unlock();
        return;
    }

    old_state = atomic_exchange(&eloop.state, ELOOP_STATE_DESTROYING);
    if (old_state == ELOOP_STATE_STOPPED) {
        atomic_store(&eloop.state, ELOOP_STATE_STOPPED);
        eloop_lifecycle_unlock();
        return;
    }
    if (old_state == ELOOP_STATE_DESTROYING) {
        eloop_lifecycle_unlock();
        return;
    }

#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: destroy requested");
#endif
    dl_list_init(&pending_timeouts);
    dl_list_init(&pending_waiters);

    ELOOP_LOCK();
    os_timer_disarm(&eloop.eloop_timer);
    dl_list_for_each_safe(timeout, prev, &eloop.timeout, struct eloop_timeout, list) {
#ifdef ELOOP_DEBUG
        struct os_reltime now, remaining;

        if (eloop_get_reltime_now(&now) < 0) {
            remaining.sec = 0;
            remaining.usec = 0;
        } else {
            eloop_remaining(&timeout->time, &now, &remaining);
        }
        wpa_printf(MSG_INFO, "ELOOP: remaining timeout: %llu.%06d eloop_data=%p user_data=%p handler=%p",
                   remaining.sec, remaining.usec, timeout->eloop_data,
                   timeout->user_data, timeout->handler);
#endif
        dl_list_del(&timeout->list);
        if (timeout->blocking) {
#ifdef ELOOP_DEBUG
            blocking_count++;
            eloop_debug_log_timeout("moving pending destroy-time", timeout, NULL);
#endif
            dl_list_add_tail(&pending_waiters, &timeout->list);
        } else {
#ifdef ELOOP_DEBUG
            async_count++;
            eloop_debug_log_timeout("moving pending destroy-time", timeout, NULL);
#endif
            dl_list_add_tail(&pending_timeouts, &timeout->list);
        }
    }
    ELOOP_UNLOCK();

#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: waiting for %u in-flight dispatches before destroy",
               atomic_load(&eloop.dispatch_count));
#endif
    while (atomic_load(&eloop.dispatch_count) != 0) {
        if (++wait_loops >= wait_warn_loops) {
            wpa_printf(MSG_ERROR,
                       "ELOOP: destroy still waiting for %u in-flight dispatches after %u ms",
                       atomic_load(&eloop.dispatch_count), wait_warn_loops * wait_step_ms);
            wait_loops = 0;
        }
        vTaskDelay(wait_step_ms / portTICK_PERIOD_MS);
    }

#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: destroy draining %d async timers and %d blocking waiters",
               async_count, blocking_count);
#endif
    eloop_fail_blocking_timeout_list(&pending_waiters);
    eloop_run_destroy_timeout_list(&pending_timeouts);

    os_timer_disarm(&eloop.eloop_timer);
    os_timer_done(&eloop.eloop_timer);

    ELOOP_LOCK();
    os_memset(&eloop, 0, sizeof(eloop));
    dl_list_init(&eloop.timeout);
    ELOOP_UNLOCK();
    eloop_lifecycle_unlock();
#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: destroyed");
#endif
}
