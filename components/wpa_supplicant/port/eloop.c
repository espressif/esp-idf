/*
 * Event loop based on select() loop
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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

bool current_task_is_wifi_task(void);

struct eloop_timeout {
    struct dl_list list;
    struct os_reltime time;
    void *eloop_data;
    void *user_data;
    eloop_timeout_handler handler;
#ifdef ELOOP_DEBUG
    char func_name[100];
    int line;
#endif
    void *sync_semph;
    int ret;
    eloop_blocking_timeout_handler blocking_handler;
};

struct eloop_data {
    struct dl_list timeout;
    ETSTimer eloop_timer;
    atomic_bool eloop_started;
    atomic_bool timeout_running;
    void *eloop_semph;
};

#define ELOOP_LOCK() os_mutex_lock(eloop_data_lock)
#define ELOOP_UNLOCK() os_mutex_unlock(eloop_data_lock)

static void *eloop_data_lock = NULL;

static struct eloop_data eloop;

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

int eloop_init(void)
{
    os_memset(&eloop, 0, sizeof(eloop));
    dl_list_init(&eloop.timeout);
    os_timer_disarm(&eloop.eloop_timer);
    os_timer_setfn(&eloop.eloop_timer, (ETSTimerFunc *)eloop_run_timer, NULL);

    eloop_data_lock = os_recursive_mutex_create();

    if (!eloop_data_lock) {
        wpa_printf(MSG_ERROR, "failed to create eloop data loop");
        return -1;
    }
    atomic_store(&eloop.eloop_started, true);
    atomic_store(&eloop.timeout_running, false);

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
    struct eloop_timeout *timeout, *tmp;
    os_time_t now_sec;
#ifdef ELOOP_DEBUG
    int count = 0;
#endif

    if (!atomic_load(&eloop.eloop_started)) {
        return -1;
    }
    timeout = os_zalloc(sizeof(*timeout));
    if (timeout == NULL) {
        return -1;
    }
    if (os_get_reltime(&timeout->time) < 0) {
        os_free(timeout);
        return -1;
    }
    now_sec = timeout->time.sec;
    timeout->time.sec += secs;
    if (timeout->time.sec < now_sec) {
        goto overflow;
    }
    timeout->time.usec += usecs;
    while (timeout->time.usec >= 1000000) {
        timeout->time.sec++;
        timeout->time.usec -= 1000000;
    }
    if (timeout->time.sec < now_sec) {
        goto overflow;
    }
    timeout->eloop_data = eloop_data;
    timeout->user_data = user_data;
    timeout->handler = handler;
#ifdef ELOOP_DEBUG
    os_strlcpy(timeout->func_name, func, 100);
    timeout->line = line;
#endif

    /* Maintain timeouts in order of increasing time */
    ELOOP_LOCK();
    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (os_reltime_before(&timeout->time, &tmp->time)) {
            dl_list_add(tmp->list.prev, &timeout->list);
            goto run;
        }
#ifdef ELOOP_DEBUG
        count++;
#endif
    }
    dl_list_add_tail(&eloop.timeout, &timeout->list);

run:
#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: Added one timer from %s:%d to call %p, current order=%d",
               timeout->func_name, line, timeout->handler, count);
#endif
    os_timer_disarm(&eloop.eloop_timer);
    os_timer_arm(&eloop.eloop_timer, 0, 0);
    ELOOP_UNLOCK();

    return 0;

overflow:
    /*
     * Integer overflow - assume long enough timeout to be assumed
     * to be infinite, i.e., the timeout would never happen.
     */
    wpa_printf(MSG_DEBUG,
               "ELOOP: Too long timeout (secs=%u usecs=%u) to ever happen - ignore it",
               secs, usecs);
    os_free(timeout);
    return -1;
}

#ifdef ELOOP_DEBUG
int eloop_register_timeout_blocking_debug(eloop_blocking_timeout_handler handler, void *eloop_data,
                                          void *user_data, const char *func, int line)
#else
int eloop_register_timeout_blocking(eloop_blocking_timeout_handler handler,
                                    void *eloop_data, void *user_data)
#endif
{
    struct eloop_timeout *timeout, *tmp;
#ifdef ELOOP_DEBUG
    int count = 0;
#endif
    int ret;

    if (current_task_is_wifi_task()) {
        assert(false);
        return -1;
    }

    if (!atomic_load(&eloop.eloop_started)) {
        return -1;
    }
    timeout = os_zalloc(sizeof(*timeout));
    if (timeout == NULL) {
        return -1;
    }
    if (os_get_reltime(&timeout->time) < 0) {
        os_free(timeout);
        return -1;
    }
    timeout->eloop_data = eloop_data;
    timeout->user_data = user_data;
    timeout->blocking_handler = handler;
#ifdef ELOOP_DEBUG
    os_strlcpy(timeout->func_name, func, 100);
    timeout->line = line;
#endif

    ELOOP_LOCK();
    if (!eloop.eloop_semph) {
        eloop.eloop_semph = os_semphr_create(1, 0);
    }
    ELOOP_UNLOCK();

    if (!eloop.eloop_semph) {
        wpa_printf(MSG_INFO, "ELOOP: sync semphr not available");
        os_free(timeout);
        return -1;
    }
    timeout->sync_semph = eloop.eloop_semph;
    /* Maintain timeouts in order of increasing time */
    ELOOP_LOCK();
    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (os_reltime_before(&timeout->time, &tmp->time)) {
            dl_list_add(tmp->list.prev, &timeout->list);
            goto run;
        }
#ifdef ELOOP_DEBUG
        count++;
#endif
    }
    dl_list_add_tail(&eloop.timeout, &timeout->list);

run:
#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: Added one blocking timer from %s:%d to call %p, current order=%d",
               timeout->func_name, line, timeout->handler, count);
#endif
    os_timer_disarm(&eloop.eloop_timer);
    os_timer_arm(&eloop.eloop_timer, 0, 0);
    ELOOP_UNLOCK();

    wpa_printf(MSG_DEBUG, "ELOOP: waiting for sync semphr");
    os_semphr_take(eloop.eloop_semph, OS_BLOCK);

    ret = timeout->ret;
    os_free(timeout);
    return ret;
}

static bool timeout_exists(struct eloop_timeout *old)
{
    struct eloop_timeout *timeout, *prev;
    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                          struct eloop_timeout, list) {
        if (old == timeout) {
            return true;
        }
    }

    return false;
}

static void eloop_remove_blocking_timeout(struct eloop_timeout *timeout)
{
    bool timeout_present = false;
    ELOOP_LOCK();
    /* Make sure timeout still exists(Another context may have deleted this) */
    timeout_present = timeout_exists(timeout);
    if (timeout_present) {
        dl_list_del(&timeout->list);
    }
    ELOOP_UNLOCK();
}

static void eloop_remove_timeout(struct eloop_timeout *timeout)
{
    bool timeout_present = false;
    ELOOP_LOCK();
    /* Make sure timeout still exists(Another context may have deleted this) */
    timeout_present = timeout_exists(timeout);
    if (timeout_present) {
        dl_list_del(&timeout->list);
    }
    ELOOP_UNLOCK();
    if (timeout_present) {
        os_free(timeout);
    }
}

#ifdef ELOOP_DEBUG
int eloop_cancel_timeout_debug(eloop_timeout_handler handler, void *eloop_data,
                               void *user_data, const char *func, int line)
#else
int eloop_cancel_timeout(eloop_timeout_handler handler,
                         void *eloop_data, void *user_data)
#endif
{
    struct eloop_timeout *timeout, *prev;
    int removed = 0;

    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                          struct eloop_timeout, list) {
        if (timeout->handler == handler &&
                (timeout->eloop_data == eloop_data ||
                 eloop_data == ELOOP_ALL_CTX) &&
                (timeout->user_data == user_data ||
                 user_data == ELOOP_ALL_CTX)) {
            eloop_remove_timeout(timeout);
            removed++;
        }
    }
#ifdef ELOOP_DEBUG
    wpa_printf(MSG_DEBUG, "ELOOP: %s:%d called to remove timer handler=%p, removed count=%d",
               func, line, handler, removed);
#endif

    return removed;
}

int eloop_cancel_timeout_one(eloop_timeout_handler handler,
                             void *eloop_data, void *user_data,
                             struct os_reltime *remaining)
{
    struct eloop_timeout *timeout, *prev;
    int removed = 0;
    struct os_reltime now;

    os_get_reltime(&now);
    remaining->sec = remaining->usec = 0;

    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                          struct eloop_timeout, list) {
        if (timeout->handler == handler &&
                (timeout->eloop_data == eloop_data) &&
                (timeout->user_data == user_data)) {
            removed = 1;
            if (os_reltime_before(&now, &timeout->time)) {
                os_reltime_sub(&timeout->time, &now, remaining);
            }
            eloop_remove_timeout(timeout);
            break;
        }
    }
    return removed;
}

int eloop_is_timeout_registered(eloop_timeout_handler handler,
                                void *eloop_data, void *user_data)
{
    struct eloop_timeout *tmp;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (tmp->handler == handler &&
                tmp->eloop_data == eloop_data &&
                tmp->user_data == user_data) {
            return 1;
        }
    }

    return 0;
}

int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
                          eloop_timeout_handler handler, void *eloop_data,
                          void *user_data)
{
    struct os_reltime now, requested, remaining;
    struct eloop_timeout *tmp;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (tmp->handler == handler &&
                tmp->eloop_data == eloop_data &&
                tmp->user_data == user_data) {
            requested.sec = req_secs;
            requested.usec = req_usecs;
            os_get_reltime(&now);
            os_reltime_sub(&tmp->time, &now, &remaining);
            if (os_reltime_before(&requested, &remaining)) {
                eloop_cancel_timeout(handler, eloop_data,
                                     user_data);
                eloop_register_timeout(requested.sec,
                                       requested.usec,
                                       handler, eloop_data,
                                       user_data);
                return 1;
            }
            return 0;
        }
    }

    return -1;
}

int eloop_replenish_timeout(unsigned int req_secs, unsigned int req_usecs,
                            eloop_timeout_handler handler, void *eloop_data,
                            void *user_data)
{
    struct os_reltime now, requested, remaining;
    struct eloop_timeout *tmp;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (tmp->handler == handler &&
                tmp->eloop_data == eloop_data &&
                tmp->user_data == user_data) {
            requested.sec = req_secs;
            requested.usec = req_usecs;
            os_get_reltime(&now);
            os_reltime_sub(&tmp->time, &now, &remaining);
            if (os_reltime_before(&remaining, &requested)) {
                eloop_cancel_timeout(handler, eloop_data,
                                     user_data);
                eloop_register_timeout(requested.sec,
                                       requested.usec,
                                       handler, eloop_data,
                                       user_data);
                return 1;
            }
            return 0;
        }
    }

    return -1;
}

void eloop_run(void)
{
    struct os_reltime tv, now;

    if (!atomic_load(&eloop.eloop_started)) {
        return;
    }
    atomic_store(&eloop.timeout_running, true);

    while (!dl_list_empty(&eloop.timeout)) {
        struct eloop_timeout *timeout;

        ELOOP_LOCK();
        timeout = dl_list_first(&eloop.timeout, struct eloop_timeout,
                                list);
        ELOOP_UNLOCK();
        if (timeout) {
            os_get_reltime(&now);
            if (os_reltime_before(&now, &timeout->time)) {
                /* we don't need to process it rn, do it later */
                uint32_t ms;
                os_reltime_sub(&timeout->time, &now, &tv);
                ms = tv.sec * 1000 + tv.usec / 1000;
                ELOOP_LOCK();
                os_timer_disarm(&eloop.eloop_timer);
                os_timer_arm(&eloop.eloop_timer, ms, 0);
                ELOOP_UNLOCK();
                goto out;
            } else {
                void *eloop_data = timeout->eloop_data;
                void *user_data = timeout->user_data;
                void *sync_semaphr = timeout->sync_semph;
                eloop_timeout_handler handler =
                    timeout->handler;
#ifdef ELOOP_DEBUG
                char fn_name[100] = {0};
                int line = timeout->line;
                os_strlcpy(fn_name, timeout->func_name, 100);
#endif
                /* will be freed in caller context in blocking call */
                if (!sync_semaphr) {
                    eloop_remove_timeout(timeout);
#ifdef ELOOP_DEBUG
                    wpa_printf(MSG_DEBUG, "ELOOP: Running timer fn:%p scheduled by %s:%d ",
                               handler, fn_name, line);
#endif
                    handler(eloop_data, user_data);
                } else {
                    eloop_remove_blocking_timeout(timeout);
                    eloop_blocking_timeout_handler handler2 =
                        timeout->blocking_handler;
#ifdef ELOOP_DEBUG
                    wpa_printf(MSG_DEBUG, "ELOOP: Running blocking timer fn:%p scheduled by %s:%d ",
                               handler2, fn_name, line);
#endif
                    timeout->ret = handler2(eloop_data, user_data);
#ifdef ELOOP_DEBUG
                    wpa_printf(MSG_DEBUG, "ELOOP: releasing sync semaphor");
#endif
                    os_semphr_give(sync_semaphr);
                }
            }
        }
    }
out:
    atomic_store(&eloop.timeout_running, false);
    return;
}

void eloop_destroy(void)
{
    struct eloop_timeout *timeout, *prev;

    if (!atomic_load(&eloop.eloop_started)) {
        return;
    }

    atomic_store(&eloop.eloop_started, false);

    while (atomic_load(&eloop.timeout_running)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);  // Yield CPU
    }
    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                          struct eloop_timeout, list) {
#ifdef ELOOP_DEBUG
        struct os_reltime now;
        os_get_reltime(&now);
        int sec, usec;
        sec = timeout->time.sec - now.sec;
        usec = timeout->time.usec - now.usec;
        if (timeout->time.usec < now.usec) {
            sec--;
            usec += 1000000;
        }
        wpa_printf(MSG_INFO, "ELOOP: remaining timeout: %d.%06d "
                   "eloop_data=%p user_data=%p handler=%p",
                   sec, usec, timeout->eloop_data, timeout->user_data,
                   timeout->handler);
#endif
        if (timeout->handler) {
            timeout->handler(timeout->eloop_data, timeout->user_data);
        }
        eloop_remove_timeout(timeout);
    }
    if (eloop_data_lock) {
        os_mutex_delete(eloop_data_lock);
        eloop_data_lock = NULL;
    }
    if (eloop.eloop_semph) {
        os_semphr_delete(eloop.eloop_semph);
        eloop.eloop_semph = NULL;
    }
    os_timer_disarm(&eloop.eloop_timer);
    os_timer_done(&eloop.eloop_timer);
    os_memset(&eloop, 0, sizeof(eloop));
}
