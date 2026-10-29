/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "osi/alarm.h"
#include "osi/allocator.h"
#include "osi/slist.h"
#include "esp_timer.h"
#include "btc/btc_task.h"
#include "btc/btc_alarm.h"
#include "osi/mutex.h"
#include "osi/atomic.h"
#include "osi/thread.h"
#include "bt_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* User callback has started. Cancel returns FAIL and lets it finish. */
#define ALARM_FLAG_RUNNING       (1u << 0)
/* Quiesce owns the alarm until stop_blocking returns. A tick must not post. */
#define ALARM_FLAG_CANCELING     (1u << 1)
/* Mirrors armed_list membership. alarm_list_of() uses it to pick one list. */
#define ALARM_FLAG_ARMED         (1u << 2)
/* Periodic esp_timer stopped after a failed post. The next success resumes it. */
#define ALARM_FLAG_DEGRADED      (1u << 3)
/* osi_alarm_free ran while a callback still holds a ref. Released at the last unref. */
#define ALARM_FLAG_FREE_DEFERRED (1u << 4)
/* alarm_release_locked is tearing the object down. A second free must not follow. */
#define ALARM_FLAG_RELEASING     (1u << 5)
/* Union word is the one-shot retry grade. set_deadline_us must not write it. */
#define ALARM_FLAG_ONE_SHOT      (1u << 6)

/* High bit of handler_state. Low bits count ticks inside alarm_cb_handler. */
#define ALARM_HANDLER_CLOSING    (1u << 31)

typedef struct alarm_t {
    /* List membership is the logical arm state. */
    slist_node_t node;
    /* Callback arg is dispatch_id, not this handle. */
    esp_timer_handle_t alarm_hdl;
    osi_alarm_callback_t cb;
    void *cb_data;
    /* NULL after detach. That is not the BTC thread, even when BTC is also NULL. */
    osi_thread_t *post_task;
    /* Unique among live alarms. 0 is reserved so a stale timer arg cannot match. */
    uint32_t dispatch_id;
    /* Bumped on cancel/free. 0 is reserved so a zeroed post cannot match. */
    uint8_t generation;
    /* Posted callbacks that have not reached the end of alarm_invoke. */
    uint8_t refs;
    uint8_t flags;
    /* Periodic phase, or the one-shot timeout used to grade retries. */
    union {
        uint64_t deadline_us;
        uint64_t timeout_us;
    };
    /* Periodic period in microseconds. 0 is a one-shot. */
    uint64_t period_us;
} osi_alarm_t;

typedef struct {
    struct alarm_t *alarm;
    uint8_t generation;
} alarm_post_args_t;

/* Empty lists stolen for the last-ref teardown. task commits them. */
typedef struct {
    slist_t *idle_list;
    slist_t *armed_list;
    TaskHandle_t task;
} alarm_deferred_close_t;

/* Tracks every alarm created by osi_alarm_new(). An alarm is on exactly one list. */
typedef struct {
    osi_mutex_t mutex;
    /* Task holding mutex. A nested discard unrefs without locking again. */
    TaskHandle_t mutex_owner;
    /* CLOSING | depth. CLOSING before create_mux publishes the mutex, so a tick
     * that races an early deinit drops out instead of taking a NULL semaphore.
     * A deinit that still has queued callbacks sets CLOSING and leaves the mutex
     * up; the task that drops the last ref frees it. */
    osi_atomic_u32_t handler_state;
    /* Not armed. Non-NULL means the subsystem is open. */
    slist_t *idle_list;
    slist_t *armed_list;
    alarm_deferred_close_t deferred;
    /* esp_timer task inside alarm_cb_handler. NULL outside that callback. */
    TaskHandle_t timer_task;
    /* Never reset, so ids stay unique across init/deinit cycles. */
    uint32_t next_dispatch_id;
    /* Tasks polling a canceling alarm. Free waits for this to hit zero. */
    uint8_t cancel_waits;
} osi_alarm_cb_t;

#if defined(BT_BLUEDROID_UNIT_TEST)
static void (*s_invoke_hook)(void);
static void (*s_cancel_hook)(void);
static void (*s_timer_hook)(void);
#endif
static osi_alarm_cb_t s_osi_alarm_cb = {
    .handler_state = OSI_ATOMIC_U32_INITIALIZER(ALARM_HANDLER_CLOSING),
};

static osi_alarm_err_t alarm_set(osi_alarm_t *alarm, period_ms_t timeout, bool is_periodic);
static uint64_t alarm_retry_delay_us(const struct alarm_t *alarm);
typedef enum {
    /* Queued. The ref stays until invoke or the queue free_cb. */
    ALARM_POST_OK = 0,
    /* Not queued. The caller arms a short retry. */
    ALARM_POST_RETRY,
    /* Target thread is gone. Stop the timer and move to idle. */
    ALARM_POST_DROPPED,
} alarm_post_result_t;

static alarm_post_result_t alarm_post(struct alarm_t *alarm);
static bool alarm_arm_once(struct alarm_t *alarm, uint64_t delay_us);
static bool alarm_arm_periodic_once(struct alarm_t *alarm);
static bool alarm_restore_periodic(struct alarm_t *alarm);
static bool alarm_schedule_retry(struct alarm_t *alarm);
static void alarm_to_idle(struct alarm_t *alarm);
static void alarm_to_armed(struct alarm_t *alarm);
/* true when |alarm| was freed and must not be touched. */
static bool alarm_release_locked(struct alarm_t *alarm);
static void alarm_claim_deferred_close_locked(void);
static void alarm_commit_deferred_close(void);

#if defined(BT_BLUEDROID_UNIT_TEST)
uint32_t osi_alarm_test_dispatch_id(const osi_alarm_t *alarm);
void osi_alarm_test_dispatch(uint32_t dispatch_id);
#endif

/* Record the owner so alarm_btc_discard can unref without locking twice. */
static void alarm_lock(void)
{
    osi_mutex_lock(&s_osi_alarm_cb.mutex, OSI_MUTEX_MAX_TIMEOUT);
    s_osi_alarm_cb.mutex_owner = xTaskGetCurrentTaskHandle();
}

static void alarm_unlock(void)
{
    s_osi_alarm_cb.mutex_owner = NULL;
    osi_mutex_unlock(&s_osi_alarm_cb.mutex);
}

/* One CAS claims a tick or observes that delete_mux has started. */
static bool alarm_handler_enter(void)
{
    uint32_t state = osi_atomic_load_n(&s_osi_alarm_cb.handler_state, __ATOMIC_ACQUIRE);

    for (;;) {
        if ((state & ALARM_HANDLER_CLOSING) != 0) {
            return false;
        }
        if (osi_atomic_compare_exchange_n(&s_osi_alarm_cb.handler_state, &state, state + 1,
                                          false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            return true;
        }
    }
}

static void alarm_handler_leave(void)
{
    osi_atomic_fetch_sub(&s_osi_alarm_cb.handler_state, 1, __ATOMIC_ACQ_REL);
}

static bool alarm_handler_is_closing(void)
{
    return (osi_atomic_load_n(&s_osi_alarm_cb.handler_state, __ATOMIC_ACQUIRE) & ALARM_HANDLER_CLOSING) != 0;
}

static void alarm_handler_set_closing(void)
{
    uint32_t state = osi_atomic_load_n(&s_osi_alarm_cb.handler_state, __ATOMIC_ACQUIRE);

    for (;;) {
        if (osi_atomic_compare_exchange_n(&s_osi_alarm_cb.handler_state, &state,
                                          state | ALARM_HANDLER_CLOSING, false,
                                          __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            return;
        }
    }
}

/* Depth is the low bits. CLOSING may already be set, so mask it off. */
static void alarm_handler_wait_idle(void)
{
    while ((osi_atomic_load_n(&s_osi_alarm_cb.handler_state, __ATOMIC_ACQUIRE) & ~ALARM_HANDLER_CLOSING) != 0) {
        vTaskDelay(1);
    }
}

/* Both lists are created and freed together. A non-NULL idle list means open. */
static bool alarm_is_open(void)
{
    return s_osi_alarm_cb.idle_list != NULL;
}

static bool alarm_ref_locked(struct alarm_t *alarm)
{
    if (alarm->refs == UINT8_MAX) {
        return false;
    }

    alarm->refs++;
    return true;
}

/* true when this unref freed the object. The caller must not touch |alarm|. */
static bool alarm_unref_locked(struct alarm_t *alarm)
{
    assert(alarm->refs > 0);
    alarm->refs--;
    if (alarm->refs == 0 && (alarm->flags & ALARM_FLAG_FREE_DEFERRED) != 0) {
        return alarm_release_locked(alarm);
    }
    return false;
}

static void alarm_retire(struct alarm_t *alarm)
{
    alarm->generation++;
    if (alarm->generation == 0) {
        alarm->generation = 1;
    }
}

static struct alarm_t *alarm_find(slist_t *list, uint32_t dispatch_id, slist_node_t **prev_out)
{
    slist_node_t *prev = NULL;
    slist_node_t *node;

    if (prev_out != NULL) {
        *prev_out = NULL;
    }
    if (list == NULL || dispatch_id == 0) {
        return NULL;
    }

    for (node = slist_begin(list); node != NULL; node = slist_next(node)) {
        struct alarm_t *alarm = (struct alarm_t *)node;
        if (alarm->dispatch_id == dispatch_id) {
            if (prev_out != NULL) {
                *prev_out = prev;
            }
            return alarm;
        }
        prev = node;
    }
    return NULL;
}

/* Caller holds the alarm mutex. ARMED selects the list; the walk confirms membership.
 * |prev_out| receives the predecessor. NULL means the node is the head. */
static slist_t *alarm_list_of(const struct alarm_t *alarm, slist_node_t **prev_out)
{
    slist_t *list;
    slist_node_t *prev = NULL;
    slist_node_t *node;

    if (prev_out != NULL) {
        *prev_out = NULL;
    }
    if (alarm == NULL) {
        return NULL;
    }

    list = (alarm->flags & ALARM_FLAG_ARMED) != 0 ? s_osi_alarm_cb.armed_list : s_osi_alarm_cb.idle_list;
    if (list == NULL) {
        return NULL;
    }

    for (node = slist_begin(list); node != NULL; node = slist_next(node)) {
        if (node == &alarm->node) {
            if (prev_out != NULL) {
                *prev_out = prev;
            }
            return list;
        }
        prev = node;
    }
    return NULL;
}

/* Caller holds the alarm mutex. Validates open state and alarm membership.
 * Returns the owning list via |list_out| and its predecessor via |prev_out|. */
static osi_alarm_err_t alarm_validate_locked(struct alarm_t *alarm, slist_t **list_out,
                                             slist_node_t **prev_out)
{
    if (!alarm_is_open()) {
        OSI_TRACE_ERROR("%s alarm is closed\n", __func__);
        return OSI_ALARM_ERR_INVALID_STATE;
    }

    slist_t *list = alarm_list_of(alarm, prev_out);
    if (list == NULL || alarm->alarm_hdl == NULL) {
        OSI_TRACE_ERROR("%s null\n", __func__);
        return OSI_ALARM_ERR_INVALID_ARG;
    }

    if (list_out != NULL) {
        *list_out = list;
    }
    return OSI_ALARM_ERR_PASS;
}

static size_t alarm_live_count(void)
{
    return slist_length(s_osi_alarm_cb.idle_list) + slist_length(s_osi_alarm_cb.armed_list);
}

static uint32_t alarm_alloc_dispatch_id(void)
{
    uint32_t dispatch_id;

    /* Skip 0 and any id still owned by a live alarm. Idle alarms own ids too:
     * their esp_timer callback can still be queued. */
    do {
        dispatch_id = ++s_osi_alarm_cb.next_dispatch_id;
    } while (dispatch_id == 0 ||
             alarm_find(s_osi_alarm_cb.idle_list, dispatch_id, NULL) != NULL ||
             alarm_find(s_osi_alarm_cb.armed_list, dispatch_id, NULL) != NULL);

    return dispatch_id;
}

static void alarm_clear_period(struct alarm_t *alarm)
{
    alarm->period_us = 0;
    alarm->flags &= ~ALARM_FLAG_DEGRADED;
}

/* |prev| is the armed-list predecessor. NULL means |alarm| is the head. */
static void alarm_idle_at(struct alarm_t *alarm, slist_node_t *prev)
{
    if ((alarm->flags & ALARM_FLAG_ARMED) == 0) {
        return;
    }

    slist_delete_after(s_osi_alarm_cb.armed_list, prev, &alarm->node);
    slist_append(s_osi_alarm_cb.idle_list, &alarm->node);
    alarm->flags &= ~ALARM_FLAG_ARMED;
}

/* No predecessor. Walk the armed list to unlink. */
static void alarm_to_idle(struct alarm_t *alarm)
{
    if ((alarm->flags & ALARM_FLAG_ARMED) == 0) {
        return;
    }

    slist_delete(s_osi_alarm_cb.armed_list, &alarm->node);
    slist_append(s_osi_alarm_cb.idle_list, &alarm->node);
    alarm->flags &= ~ALARM_FLAG_ARMED;
}

static void alarm_to_armed(struct alarm_t *alarm)
{
    if ((alarm->flags & ALARM_FLAG_ARMED) != 0) {
        return;
    }

    slist_delete(s_osi_alarm_cb.idle_list, &alarm->node);
    slist_append(s_osi_alarm_cb.armed_list, &alarm->node);
    alarm->flags |= ALARM_FLAG_ARMED;
}

static bool alarm_lists_create(void)
{
    s_osi_alarm_cb.idle_list = slist_new(NULL);
    s_osi_alarm_cb.armed_list = slist_new(NULL);
    if (s_osi_alarm_cb.idle_list == NULL || s_osi_alarm_cb.armed_list == NULL) {
        slist_free(s_osi_alarm_cb.idle_list);
        slist_free(s_osi_alarm_cb.armed_list);
        s_osi_alarm_cb.idle_list = NULL;
        s_osi_alarm_cb.armed_list = NULL;
        return false;
    }
    return true;
}

/* ESP_OK or already stopped. Other errors are logged. */
static esp_err_t alarm_stop(esp_timer_handle_t hdl)
{
    esp_err_t err = esp_timer_stop(hdl);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        OSI_TRACE_WARNING("%s stop failed 0x%x\n", __func__, err);
    }
    return err;
}

/* refs != 0. Deinit cannot wait. The queued callback unrefs. */
static void alarm_retire_posted(struct alarm_t *alarm)
{
    if (alarm->alarm_hdl != NULL) {
        alarm_stop(alarm->alarm_hdl);
    }
    alarm_retire(alarm);
    alarm->flags |= ALARM_FLAG_CANCELING | ALARM_FLAG_FREE_DEFERRED;
    OSI_TRACE_ERROR("%s callbacks still posted\n", __func__);
}

/* refs == 0. Stop is not logged; a delete failure leaves the node. */
static void alarm_delete_zero_ref(struct alarm_t *alarm, slist_t *list)
{
    esp_err_t err;

    if (alarm->alarm_hdl != NULL) {
        esp_timer_stop(alarm->alarm_hdl);
        err = esp_timer_delete(alarm->alarm_hdl);
        if (err != ESP_OK) {
            OSI_TRACE_ERROR("%s failed to delete timer, err 0x%x\n", __func__, err);
            return;
        }
    }

    slist_delete(list, &alarm->node);
    osi_free(alarm);
}

/* true keeps slist_foreach walking. false would leave the rest of the list alive. */
static bool alarm_destroy_node(slist_node_t *node, void *context)
{
    struct alarm_t *alarm = (struct alarm_t *)node;
    slist_t *list = (slist_t *)context;

    if (alarm->refs != 0) {
        alarm_retire_posted(alarm);
    } else {
        alarm_delete_zero_ref(alarm, list);
    }
    return true;
}

/* false: a posted callback still holds a ref. Lists and the listener stay. */
static bool alarm_close_lists(void)
{
    if (s_osi_alarm_cb.armed_list != NULL) {
        slist_foreach(s_osi_alarm_cb.armed_list, alarm_destroy_node, s_osi_alarm_cb.armed_list);
    }
    if (s_osi_alarm_cb.idle_list != NULL) {
        slist_foreach(s_osi_alarm_cb.idle_list, alarm_destroy_node, s_osi_alarm_cb.idle_list);
    }

    if ((s_osi_alarm_cb.armed_list != NULL && !slist_is_empty(s_osi_alarm_cb.armed_list)) ||
            (s_osi_alarm_cb.idle_list != NULL && !slist_is_empty(s_osi_alarm_cb.idle_list))) {
        return false;
    }

    slist_free(s_osi_alarm_cb.idle_list);
    slist_free(s_osi_alarm_cb.armed_list);
    s_osi_alarm_cb.idle_list = NULL;
    s_osi_alarm_cb.armed_list = NULL;
    return true;
}

int osi_alarm_create_mux(void)
{
    if (alarm_is_open()) {
        OSI_TRACE_WARNING("%s alarm is open\n", __func__);
        return -1;
    }
    /* Probe native on the first create; later creates keep it. */
    osi_atomic_u32_init(&s_osi_alarm_cb.handler_state, ALARM_HANDLER_CLOSING);
    if (osi_mutex_new(&s_osi_alarm_cb.mutex) != 0) {
        return -1;
    }
    /* Mutex is visible before a tick is allowed to take it. */
    osi_atomic_store_n(&s_osi_alarm_cb.handler_state, 0, __ATOMIC_RELEASE);
    return 0;
}

int osi_alarm_delete_mux(void)
{
    if (s_osi_alarm_cb.mutex == NULL) {
        /* Idempotent: the mesh adapter frees the mux before btc_deinit. */
        return 0;
    }

    if (alarm_is_open()) {
        OSI_TRACE_WARNING("%s alarm is open\n", __func__);
        return -1;
    }

    alarm_handler_set_closing();
    alarm_handler_wait_idle();
    osi_mutex_free(&s_osi_alarm_cb.mutex);
    /* Keep CLOSING set so ticks after a delete drop out until the next create. */
    return 0;
}

static bool alarm_detach_node(slist_node_t *node, void *context)
{
    struct alarm_t *alarm = (struct alarm_t *)node;
    osi_thread_t *thread = (osi_thread_t *)context;
    esp_err_t err;

    if (alarm->post_task != thread) {
        return true;
    }

    alarm->post_task = NULL;
    if ((alarm->flags & ALARM_FLAG_ARMED) == 0) {
        return true;
    }

    alarm_clear_period(alarm);
    err = alarm_stop(alarm->alarm_hdl);
    /* An expiry already in flight keeps the armed id so the handler can drop it. */
    if (err != ESP_OK) {
        return true;
    }

    alarm_to_idle(alarm);
    return true;
}

void osi_alarm_detach_post_task(osi_thread_t *thread)
{
    if (thread == NULL || s_osi_alarm_cb.mutex == NULL) {
        return;
    }

    alarm_lock();
    if (!alarm_is_open()) {
        alarm_unlock();
        return;
    }

    slist_foreach(s_osi_alarm_cb.idle_list, alarm_detach_node, thread);
    slist_foreach(s_osi_alarm_cb.armed_list, alarm_detach_node, thread);
    alarm_unlock();
}

static void alarm_thread_listener(osi_thread_t *thread, osi_thread_evt_t evt)
{
    if (evt == OSI_THREAD_EVT_STOPPED) {
        osi_alarm_detach_post_task(thread);
    }
}

static struct osi_thread_listener_node s_alarm_listener_node = {
    .listener = alarm_thread_listener,
};

static bool alarm_lists_idle(void)
{
    return s_osi_alarm_cb.idle_list != NULL &&
           s_osi_alarm_cb.armed_list != NULL &&
           slist_is_empty(s_osi_alarm_cb.idle_list) &&
           slist_is_empty(s_osi_alarm_cb.armed_list);
}

/* Caller holds the alarm mutex. Pair with alarm_commit_deferred_close after unlock. */
static void alarm_claim_deferred_close_locked(void)
{
    alarm_deferred_close_t *deferred = &s_osi_alarm_cb.deferred;

    if (deferred->idle_list != NULL || !alarm_handler_is_closing() || !alarm_lists_idle()) {
        return;
    }

    deferred->idle_list = s_osi_alarm_cb.idle_list;
    deferred->armed_list = s_osi_alarm_cb.armed_list;
    s_osi_alarm_cb.idle_list = NULL;
    s_osi_alarm_cb.armed_list = NULL;
    deferred->task = xTaskGetCurrentTaskHandle();
}

/* deferred->task commits after it has dropped the alarm mutex and left the timer callback. */
static void alarm_commit_deferred_close(void)
{
    alarm_deferred_close_t *deferred = &s_osi_alarm_cb.deferred;
    slist_t *idle;
    slist_t *armed;

    if (deferred->task != xTaskGetCurrentTaskHandle()) {
        return;
    }

    deferred->task = NULL;
    idle = deferred->idle_list;
    armed = deferred->armed_list;
    deferred->idle_list = NULL;
    deferred->armed_list = NULL;
    if (idle == NULL) {
        return;
    }

    alarm_handler_wait_idle();
    osi_thread_listener_unregister(&s_alarm_listener_node);
    slist_free(idle);
    slist_free(armed);
    osi_mutex_free(&s_osi_alarm_cb.mutex);
}

void osi_alarm_init(void)
{
    bool opened = false;

    assert(s_osi_alarm_cb.mutex != NULL);

    alarm_lock();
    if (alarm_is_open()) {
        OSI_TRACE_WARNING("%s alarm is open\n", __func__);
        goto end;
    }

    if (!alarm_lists_create()) {
        OSI_TRACE_ERROR("%s, malloc failed\n", __func__);
        goto end;
    }

    opened = true;

end:
    alarm_unlock();
    if (!opened) {
        return;
    }
    if (!osi_thread_listener_register(&s_alarm_listener_node)) {
        OSI_TRACE_ERROR("%s listener register failed\n", __func__);
        osi_alarm_deinit();
    }
}

void osi_alarm_deinit(void)
{
    bool closed = false;

    if (s_osi_alarm_cb.mutex == NULL) {
        /* No mux: e.g. the NimBLE blufi path never creates one. */
        osi_thread_listener_unregister(&s_alarm_listener_node);
        return;
    }

    alarm_lock();
    if (!alarm_is_open()) {
        OSI_TRACE_WARNING("%s alarm is closed\n", __func__);
        closed = true;
        goto end;
    }

    if (!alarm_close_lists()) {
        /* Still open. The last queued unref unregisters the listener. */
        alarm_handler_set_closing();
        OSI_TRACE_ERROR("%s timers still live\n", __func__);
        goto end;
    }

    closed = true;

end:
    alarm_unlock();
    if (closed) {
        osi_thread_listener_unregister(&s_alarm_listener_node);
    }
}

#define ALARM_RETRY_SHORT_PERIOD_US     (10000ULL)
#define ALARM_RETRY_MID_PERIOD_US       (100000ULL)
#define ALARM_RETRY_SHORT_DELAY_US      (1000ULL)
#define ALARM_RETRY_MID_DELAY_US        (5000ULL)
#define ALARM_RETRY_LONG_DELAY_US       (10000ULL)

static uint64_t alarm_retry_delay_us(const struct alarm_t *alarm)
{
    uint64_t base_us = alarm->period_us != 0 ? alarm->period_us : alarm->timeout_us;

    /* Retry delay graded by the timer period. */
    if (base_us <= ALARM_RETRY_SHORT_PERIOD_US) {
        return ALARM_RETRY_SHORT_DELAY_US;
    } else if (base_us <= ALARM_RETRY_MID_PERIOD_US) {
        return ALARM_RETRY_MID_DELAY_US;
    } else {
        return ALARM_RETRY_LONG_DELAY_US;
    }
}

static void alarm_invoke_common(struct alarm_t *alarm, uint8_t generation)
{
    osi_alarm_callback_t cb = NULL;
    void *cb_data = NULL;
    bool run = false;

#if defined(BT_BLUEDROID_UNIT_TEST)
    /* Before the lock, so cancel can catch a callback that is dequeued but not started. */
    if (s_invoke_hook != NULL) {
        s_invoke_hook();
    }
#endif

    alarm_lock();
    /* The post that reached here holds a ref, so free and deinit cannot release it. */
    if (alarm->generation == generation &&
            (alarm->flags & ALARM_FLAG_CANCELING) == 0) {
        alarm->flags |= ALARM_FLAG_RUNNING;
        cb = alarm->cb;
        cb_data = alarm->cb_data;
        run = true;
    } else {
        /* Stale generation or a cancel already in progress. Skip the user callback. */
        alarm_unref_locked(alarm);
    }
    alarm_unlock();

    if (!run) {
        alarm_commit_deferred_close();
        return;
    }

    /* The callback may free or set this alarm. The mutex is not held. */
    if (cb != NULL) {
        cb(cb_data);
    }

    alarm_lock();
    alarm->flags &= ~ALARM_FLAG_RUNNING;
    alarm_unref_locked(alarm);
    alarm_unlock();
    alarm_commit_deferred_close();
}

void alarm_invoke(osi_alarm_t *alarm, uint8_t generation)
{
    alarm_invoke_common(alarm, generation);
}

static void alarm_post_handler(void *arg)
{
    alarm_post_args_t *args = (alarm_post_args_t *)arg;

    alarm_invoke_common(args->alarm, args->generation);
    osi_free(args);
}

static void alarm_dispatch_free(void *context)
{
    alarm_post_args_t *args = (alarm_post_args_t *)context;

    alarm_lock();
    if (args != NULL && args->alarm != NULL) {
        alarm_unref_locked(args->alarm);
    }
    alarm_unlock();
    alarm_commit_deferred_close();
    osi_free(args);
}

/* No-op. A non-NULL copy_func is what makes BTC keep alarm_btc_discard. */
static void alarm_btc_copy(btc_msg_t *msg, void *dst, void *src)
{
    (void)msg;
    (void)dst;
    (void)src;
}

/* Post failure runs on the task that already holds the alarm mutex. */
static void alarm_btc_discard(btc_msg_t *msg)
{
    btc_alarm_args_t *arg = (btc_alarm_args_t *)msg->arg;
    bool held;

    if (arg == NULL) {
        return;
    }

    held = s_osi_alarm_cb.mutex_owner == xTaskGetCurrentTaskHandle();
    if (!held) {
        alarm_lock();
    }
    if (arg->alarm != NULL) {
        alarm_unref_locked(arg->alarm);
    }
    /* The lock holder commits the deferred close after it unlocks. */
    if (!held) {
        alarm_unlock();
        alarm_commit_deferred_close();
    }
}

static alarm_post_result_t alarm_post_btc(struct alarm_t *alarm)
{
    btc_msg_t msg = {0};
    btc_alarm_args_t arg;
    bt_status_t status;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ALARM;
    arg.alarm = alarm;
    arg.generation = alarm->generation;

    status = btc_transfer_context_nb(&msg, &arg, sizeof(arg), alarm_btc_copy, alarm_btc_discard);
    if (status != BT_STATUS_SUCCESS) {
        /* BUSY already ran alarm_btc_discard. The other failures do not. */
        if (status != BT_STATUS_BUSY) {
            alarm_unref_locked(alarm);
        }
        return ALARM_POST_RETRY;
    }

    return ALARM_POST_OK;
}

static alarm_post_result_t alarm_post_thread(struct alarm_t *alarm)
{
    alarm_post_args_t *args = (alarm_post_args_t *)osi_malloc(sizeof(*args));

    if (args == NULL) {
        alarm_unref_locked(alarm);
        OSI_TRACE_WARNING("%s no memory\n", __func__);
        return ALARM_POST_RETRY;
    }

    args->alarm = alarm;
    args->generation = alarm->generation;
    if (!osi_thread_post(alarm->post_task, alarm_post_handler, args,
                         (osi_thread_free_cb_t)alarm_dispatch_free, 0, 0)) {
        alarm_unref_locked(alarm);
        osi_free(args);
        return ALARM_POST_RETRY;
    }

    return ALARM_POST_OK;
}

static alarm_post_result_t alarm_post(struct alarm_t *alarm)
{
    /* A detached target is NULL. That must not fall through to the BTC path,
     * which is also NULL once the BTC thread itself has been freed. */
    if (alarm->post_task == NULL) {
        return ALARM_POST_DROPPED;
    }
    if (!alarm_ref_locked(alarm)) {
        return ALARM_POST_RETRY;
    }

    if (alarm->post_task == btc_get_current_thread()) {
        return alarm_post_btc(alarm);
    }

    return alarm_post_thread(alarm);
}

static bool alarm_arm_once(struct alarm_t *alarm, uint64_t delay_us)
{
    esp_err_t err = esp_timer_start_once(alarm->alarm_hdl, delay_us);

    if (err != ESP_OK) {
        OSI_TRACE_WARNING("%s start_once failed 0x%x\n", __func__, err);
        return false;
    }

    return true;
}

/* start_*_at rejects a deadline already in the past. Each miss steps one
 * period later; after this many, give up on the original phase. */
#define ALARM_PERIODIC_PHASE_CATCHUP    3

/* Next phase that start_*_at will accept. A deadline already due steps forward. */
static uint64_t alarm_future_deadline(uint64_t deadline, uint64_t period)
{
    int64_t now = esp_timer_get_time();

    if (period != 0 && (int64_t)deadline <= now) {
        uint64_t steps = ((uint64_t)now - deadline) / period + 1;

        deadline += steps * period;
    }
    return deadline;
}

/* Step the phase until start_*_at accepts it. period 0 leaves |err_out| unset. */
static bool alarm_arm_at_phase(struct alarm_t *alarm, bool periodic, uint64_t *deadline_out,
                               esp_err_t *err_out)
{
    uint64_t period = alarm->period_us;
    uint64_t deadline;
    esp_err_t err = ESP_FAIL;

    if (period == 0) {
        return false;
    }

    deadline = alarm_future_deadline(alarm->deadline_us, period);
    for (uint8_t attempt = 0; attempt < ALARM_PERIODIC_PHASE_CATCHUP && err != ESP_OK; attempt++) {
        if (periodic) {
            err = esp_timer_start_periodic_at(alarm->alarm_hdl, period, deadline);
        } else {
            err = esp_timer_start_once_at(alarm->alarm_hdl, deadline);
        }
        if (err != ESP_OK) {
            deadline += period;
        }
    }
    if (err_out != NULL) {
        *err_out = err;
    }
    if (err != ESP_OK) {
        return false;
    }

    *deadline_out = deadline;
    return true;
}

/* Keep the original phase with one-shot deadlines while periodic mode is stopped. */
static bool alarm_arm_periodic_once(struct alarm_t *alarm)
{
    uint64_t deadline;
    esp_err_t err = ESP_FAIL;

    if (alarm_arm_at_phase(alarm, false, &deadline, &err)) {
        alarm->deadline_us = deadline;
        return true;
    }
    if (alarm->period_us == 0) {
        return false;
    }

    OSI_TRACE_WARNING("%s start_once_at failed 0x%x\n", __func__, err);
    err = esp_timer_start_once(alarm->alarm_hdl, alarm->period_us);
    if (err != ESP_OK) {
        OSI_TRACE_ERROR("%s failed to arm periodic one-shot, err 0x%x\n", __func__, err);
        return false;
    }

    alarm->deadline_us = (uint64_t)esp_timer_get_time() + alarm->period_us;
    return true;
}

/* Post succeeded. Resume the periodic esp_timer on the same phase. */
static bool alarm_restore_periodic(struct alarm_t *alarm)
{
    uint64_t deadline;
    esp_err_t err = ESP_FAIL;

    if (!alarm_arm_at_phase(alarm, true, &deadline, &err)) {
        if (alarm->period_us != 0) {
            OSI_TRACE_WARNING("%s start_periodic_at failed 0x%x\n", __func__, err);
        }
        return false;
    }

    alarm->deadline_us = deadline;
    alarm->flags &= ~ALARM_FLAG_DEGRADED;
    return true;
}

static bool alarm_schedule_retry(struct alarm_t *alarm)
{
    uint64_t delay_us = alarm_retry_delay_us(alarm);

    if (alarm->period_us != 0 && (alarm->flags & ALARM_FLAG_DEGRADED) == 0) {
        /* Leave periodic mode until the next post succeeds. */
        alarm->flags |= ALARM_FLAG_DEGRADED;
        alarm->deadline_us += alarm->period_us;
        alarm_stop(alarm->alarm_hdl);
    }

    if (alarm_arm_once(alarm, delay_us)) {
        return true;
    }
    if ((alarm->flags & ALARM_FLAG_DEGRADED) != 0 && alarm_arm_periodic_once(alarm)) {
        return true;
    }

    /* Leave the alarm armed when the underlying timer is still running. */
    return esp_timer_is_active(alarm->alarm_hdl);
}

/* Hard failure leaves the alarm armed. Already-stopped still moves it to idle. */
static bool alarm_stop_to_idle(struct alarm_t *alarm, slist_node_t *prev)
{
    esp_err_t err = alarm_stop(alarm->alarm_hdl);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return false;
    }

    alarm_clear_period(alarm);
    alarm_idle_at(alarm, prev);
    return true;
}

/* Degraded resumes periodic mode. A live period only steps the phase. */
static void alarm_handle_post_ok(struct alarm_t *alarm, slist_node_t *prev)
{
    if ((alarm->flags & ALARM_FLAG_DEGRADED) != 0) {
        if (!alarm_restore_periodic(alarm) && !alarm_arm_periodic_once(alarm)) {
            alarm_clear_period(alarm);
            alarm_idle_at(alarm, prev);
        }
    } else if (alarm->period_us != 0) {
        alarm->deadline_us += alarm->period_us;
    } else {
        alarm_idle_at(alarm, prev);
    }
}

static void alarm_handle_post_retry(struct alarm_t *alarm, slist_node_t *prev)
{
    if (!alarm_schedule_retry(alarm)) {
        alarm_clear_period(alarm);
        alarm_idle_at(alarm, prev);
    }
}

/* Caller holds the alarm mutex. */
static void alarm_service_tick_locked(struct alarm_t *alarm, slist_node_t *prev)
{
    if ((alarm->flags & ALARM_FLAG_CANCELING) != 0) {
        /* This tick belongs to a cancel already in progress. Do not post or re-arm. */
        alarm_stop_to_idle(alarm, prev);
        return;
    }

    switch (alarm_post(alarm)) {
    case ALARM_POST_OK:
        alarm_handle_post_ok(alarm, prev);
        break;
    case ALARM_POST_RETRY:
        alarm_handle_post_retry(alarm, prev);
        break;
    case ALARM_POST_DROPPED:
        alarm_stop_to_idle(alarm, prev);
        break;
    }
}

static void alarm_cb_handler(void *param)
{
    slist_node_t *prev = NULL;
    uint32_t dispatch_id = (uint32_t)(uintptr_t)param;
    struct alarm_t *alarm;

#if defined(BT_BLUEDROID_UNIT_TEST)
    /* Before the lock, so cancel's stop_blocking can see this callback still running. */
    if (s_timer_hook != NULL) {
        s_timer_hook();
    }
#endif

    OSI_TRACE_DEBUG("TimerID %u\n", (unsigned)dispatch_id);

    /* delete_mux may already be freeing the alarm mutex. Drop the tick instead. */
    if (!alarm_handler_enter()) {
        return;
    }
    s_osi_alarm_cb.timer_task = xTaskGetCurrentTaskHandle();

    /* The id is resolved under the same lock as cancel/free. A callback that
     * was already queued cannot arm an alarm its owner has dropped, and it
     * cannot follow a freed object into a reused address. */
    alarm_lock();
    if (!alarm_is_open()) {
        goto end;
    }
    alarm = alarm_find(s_osi_alarm_cb.armed_list, dispatch_id, &prev);
    if (alarm == NULL || alarm->alarm_hdl == NULL) {
        goto end;
    }

    alarm_service_tick_locked(alarm, prev);

end:
    alarm_unlock();
    s_osi_alarm_cb.timer_task = NULL;
    alarm_handler_leave();
    alarm_commit_deferred_close();
}

osi_alarm_t *osi_alarm_new(const char *alarm_name, osi_alarm_callback_t callback, void *data, osi_thread_t *post_task)
{
    assert(s_osi_alarm_cb.mutex != NULL);

    struct alarm_t *alarm = NULL;

    alarm_lock();
    if (!alarm_is_open() || alarm_handler_is_closing()) {
        OSI_TRACE_ERROR("%s alarm is closed\n", __func__);
        goto end;
    }

    if (alarm_live_count() >= ALARM_CBS_NUM) {
        OSI_TRACE_ERROR("%s alarm_cbs exhausted\n", __func__);
        goto end;
    }

    alarm = (struct alarm_t *)osi_calloc(sizeof(*alarm));
    if (alarm == NULL) {
        OSI_TRACE_ERROR("%s malloc failed\n", __func__);
        goto end;
    }

    alarm->dispatch_id = alarm_alloc_dispatch_id();
    alarm->generation = 1;
    alarm->cb = callback;
    alarm->cb_data = data;
    alarm->post_task = (post_task != NULL) ? post_task : btc_get_current_thread();
    if (alarm->post_task == NULL) {
        OSI_TRACE_ERROR("%s post task is null\n", __func__);
        osi_free(alarm);
        alarm = NULL;
        goto end;
    }
    alarm->period_us = 0;
    alarm->deadline_us = 0;

    esp_timer_create_args_t tca = {0};
    tca.callback = alarm_cb_handler;
    tca.arg = (void *)(uintptr_t)alarm->dispatch_id;
    tca.dispatch_method = ESP_TIMER_TASK;
    tca.name = alarm_name;

    esp_err_t stat = esp_timer_create(&tca, &alarm->alarm_hdl);
    if (stat != ESP_OK) {
        OSI_TRACE_ERROR("%s failed to create timer, err 0x%x\n", __func__, stat);
        osi_free(alarm);
        alarm = NULL;
        goto end;
    }

    slist_append(s_osi_alarm_cb.idle_list, &alarm->node);

end:
    alarm_unlock();
    return alarm;
}

/* Caller holds the alarm mutex. Another cancel/free owns this alarm until CANCELING clears.
 * |prev_out| is refreshed after each unlock, because the node may have moved. */
static bool alarm_wait_canceling(struct alarm_t *alarm, slist_t *list, slist_node_t **prev_out)
{
    while (list != NULL && (alarm->flags & ALARM_FLAG_CANCELING) != 0) {
        if (alarm_handler_is_closing() || !alarm_is_open()) {
            return false;
        }
        s_osi_alarm_cb.cancel_waits++;
        alarm_unlock();
        vTaskDelay(1);
        alarm_lock();
        if (s_osi_alarm_cb.cancel_waits > 0) {
            s_osi_alarm_cb.cancel_waits--;
        }
        if (alarm_handler_is_closing() || !alarm_is_open()) {
            return false;
        }
        list = alarm_list_of(alarm, prev_out);
    }

    return list != NULL;
}

/* Lock is held by the caller and held again on return. |list| and |prev| are the caller's lookup.
 * Queued work stays. Generation makes invoke drop it; thread teardown runs free_cb. */
static slist_t *alarm_quiesce(struct alarm_t *alarm, slist_t *list, slist_node_t *prev,
                              bool *was_running, bool run_hook)
{
    esp_timer_handle_t hdl;
    bool pinned;

    if (!alarm_wait_canceling(alarm, list, &prev)) {
        return NULL;
    }

    /* Sample RUNNING before retiring, so cancel can tell a started callback from a queued one. */
    if (was_running != NULL) {
        *was_running = (alarm->flags & ALARM_FLAG_RUNNING) != 0;
    }
    alarm->flags |= ALARM_FLAG_CANCELING;
    alarm_retire(alarm);
    alarm_clear_period(alarm);
    alarm_idle_at(alarm, prev);
    hdl = alarm->alarm_hdl;
    /* This ref keeps free/deinit from releasing the object while the lock is dropped. */
    pinned = alarm_ref_locked(alarm);

    /* alarm_cb_handler takes this mutex, so stop_blocking must run without it. */
    alarm_unlock();

#if defined(BT_BLUEDROID_UNIT_TEST)
    /* Mutex is not held here, so the hook may call osi_alarm_set. */
    if (run_hook && s_cancel_hook != NULL) {
        s_cancel_hook();
    }
#else
    (void)run_hook;
#endif

    if (hdl != NULL) {
        esp_err_t err = esp_timer_stop_blocking(hdl, portMAX_DELAY);

        if (err != ESP_OK) {
            OSI_TRACE_WARNING("%s stop_blocking failed 0x%x\n", __func__, err);
        }
    }

    alarm_lock();
    if (!alarm_is_open()) {
        return NULL;
    }
    if (pinned) {
        if (alarm_unref_locked(alarm)) {
            return NULL;
        }
        /* ARMED is updated under this lock, so membership does not need another walk. */
        list = (alarm->flags & ALARM_FLAG_ARMED) != 0 ? s_osi_alarm_cb.armed_list : s_osi_alarm_cb.idle_list;
    } else {
        list = alarm_list_of(alarm, NULL);
        if (list == NULL) {
            return NULL;
        }
    }

    alarm->flags &= ~ALARM_FLAG_CANCELING;
    return list;
}

/* Lock is held. May drop it while waiting for the timer task, then take it again. */
static bool alarm_delete_timer_locked(struct alarm_t *alarm)
{
    esp_timer_handle_t hdl = alarm->alarm_hdl;
    esp_err_t stat;

    if (hdl == NULL) {
        return true;
    }

    stat = esp_timer_delete(hdl);
    /* Delete can fail while the timer task is still inside alarm_cb_handler. */
    if (stat != ESP_OK && alarm_handler_is_closing() &&
            s_osi_alarm_cb.timer_task != xTaskGetCurrentTaskHandle()) {
        alarm_unlock();
        alarm_handler_wait_idle();
        alarm_lock();
        stat = esp_timer_delete(hdl);
    }
    if (stat != ESP_OK) {
        /* Keep the object so a later free can retry. */
        OSI_TRACE_ERROR("%s failed to delete timer, err 0x%x\n", __func__, stat);
        return false;
    }

    alarm->alarm_hdl = NULL;
    return true;
}

/* Lock is held. |alarm| has no refs. On success the object is freed. */
static bool alarm_release_locked(struct alarm_t *alarm)
{
    slist_t *list;

    if ((alarm->flags & ALARM_FLAG_RELEASING) != 0) {
        return true;
    }
    alarm->flags |= ALARM_FLAG_RELEASING;

    while (s_osi_alarm_cb.cancel_waits != 0) {
        alarm_unlock();
        vTaskDelay(1);
        alarm_lock();
    }

    if (!alarm_delete_timer_locked(alarm)) {
        alarm->flags &= ~ALARM_FLAG_RELEASING;
        return false;
    }

    list = alarm_list_of(alarm, NULL);
    if (list != NULL) {
        slist_delete(list, &alarm->node);
    }
    osi_free(alarm);
    alarm_claim_deferred_close_locked();
    return true;
}

osi_alarm_err_t osi_alarm_free(osi_alarm_t *alarm)
{
    slist_t *list;
    slist_node_t *prev = NULL;
    osi_alarm_err_t ret = OSI_ALARM_ERR_PASS;

    assert(s_osi_alarm_cb.mutex != NULL);

    alarm_lock();
    ret = alarm_validate_locked(alarm, &list, &prev);
    if (ret != OSI_ALARM_ERR_PASS) {
        goto end;
    }
    if ((alarm->flags & ALARM_FLAG_RELEASING) != 0) {
        /* alarm_release_locked owns it across the cancel-wait unlock. */
        goto end;
    }
    if ((alarm->flags & ALARM_FLAG_FREE_DEFERRED) != 0) {
        if (alarm->refs == 0 && !alarm_release_locked(alarm)) {
            ret = OSI_ALARM_ERR_FAIL;
        }
        goto end;
    }
    list = alarm_quiesce(alarm, list, prev, NULL, false);
    if (list == NULL) {
        ret = OSI_ALARM_ERR_INVALID_ARG;
        goto end;
    }

    /* A queued post or the running callback still holds a ref. Drop the
     * caller's pointer now; the last unref releases the slot. */
    if (alarm->refs != 0) {
        alarm->flags |= ALARM_FLAG_FREE_DEFERRED;
        goto end;
    }

    if (!alarm_release_locked(alarm)) {
        ret = OSI_ALARM_ERR_FAIL;
    }

end:
    alarm_unlock();
    alarm_commit_deferred_close();
    return ret;
}

/* Caller holds the alarm mutex. */
static osi_alarm_err_t alarm_start_locked(struct alarm_t *alarm, period_ms_t timeout, bool is_periodic)
{
    int64_t timeout_us = 1000 * (int64_t)timeout;
    int64_t now_us = esp_timer_get_time();
    esp_err_t stat;

    if (is_periodic) {
        stat = esp_timer_start_periodic(alarm->alarm_hdl, (uint64_t)timeout_us);
    } else {
        stat = esp_timer_start_once(alarm->alarm_hdl, (uint64_t)timeout_us);
    }
    if (stat != ESP_OK) {
        OSI_TRACE_ERROR("%s failed to start timer, err 0x%x\n", __func__, stat);
        return OSI_ALARM_ERR_FAIL;
    }

    alarm->flags &= ~ALARM_FLAG_DEGRADED;
    if (is_periodic) {
        alarm->flags &= ~ALARM_FLAG_ONE_SHOT;
        alarm->period_us = (uint64_t)timeout_us;
        alarm->deadline_us = (uint64_t)now_us + (uint64_t)timeout_us;
    } else {
        alarm->flags |= ALARM_FLAG_ONE_SHOT;
        alarm->period_us = 0;
        alarm->timeout_us = (uint64_t)timeout_us;
    }
    alarm_to_armed(alarm);
    return OSI_ALARM_ERR_PASS;
}

static osi_alarm_err_t alarm_set(osi_alarm_t *alarm, period_ms_t timeout, bool is_periodic)
{
    assert(s_osi_alarm_cb.mutex != NULL);

    osi_alarm_err_t ret = OSI_ALARM_ERR_PASS;
    slist_t *list;
    slist_node_t *prev = NULL;

    alarm_lock();
    ret = alarm_validate_locked(alarm, &list, &prev);
    if (ret != OSI_ALARM_ERR_PASS) {
        goto end;
    }
    /* Cancel owns the alarm across its unlocked wait. Its hook calls set. */
    if ((alarm->flags & ALARM_FLAG_CANCELING) != 0) {
        ret = OSI_ALARM_ERR_FAIL;
        goto end;
    }

    /* Stop a live arm, including a post-failure retry, and drop a queued callback. */
    list = alarm_quiesce(alarm, list, prev, NULL, false);
    if (list == NULL) {
        ret = OSI_ALARM_ERR_INVALID_ARG;
        goto end;
    }

    ret = alarm_start_locked(alarm, timeout, is_periodic);

end:
    alarm_unlock();
    alarm_commit_deferred_close();
    return ret;
}

osi_alarm_err_t osi_alarm_set(osi_alarm_t *alarm, period_ms_t timeout)
{
    return alarm_set(alarm, timeout, FALSE);
}

osi_alarm_err_t osi_alarm_set_periodic(osi_alarm_t *alarm, period_ms_t period)
{
    return alarm_set(alarm, period, TRUE);
}

/* Caller holds the alarm mutex. FAIL means the user callback had already started. */
static osi_alarm_err_t alarm_cancel_locked(struct alarm_t *alarm, slist_t *list, slist_node_t *prev)
{
    bool was_running = false;

    if (alarm_quiesce(alarm, list, prev, &was_running, true) == NULL) {
        return OSI_ALARM_ERR_INVALID_ARG;
    }
    if (was_running) {
        return OSI_ALARM_ERR_FAIL;
    }
    return OSI_ALARM_ERR_PASS;
}

osi_alarm_err_t osi_alarm_cancel(osi_alarm_t *alarm)
{
    osi_alarm_err_t ret = OSI_ALARM_ERR_PASS;
    slist_t *list;
    slist_node_t *prev = NULL;

    alarm_lock();
    ret = alarm_validate_locked(alarm, &list, &prev);
    if (ret != OSI_ALARM_ERR_PASS) {
        goto end;
    }

    ret = alarm_cancel_locked(alarm, list, prev);

end:
    alarm_unlock();
    alarm_commit_deferred_close();
    return ret;
}

static period_ms_t alarm_remaining_ms(esp_timer_handle_t hdl)
{
    uint64_t expiry = 0;
    int64_t dt_us;

    if (esp_timer_get_expiry_time(hdl, &expiry) != ESP_OK) {
        return 0;
    }
    dt_us = (int64_t)expiry - esp_timer_get_time();
    return (dt_us > 0) ? (period_ms_t)(dt_us / 1000) : 0;
}

osi_alarm_err_t osi_alarm_cancel_active(osi_alarm_t *alarm, const period_ms_t *sooner_than_ms)
{
    osi_alarm_err_t ret = OSI_ALARM_ERR_PASS;
    slist_t *list;
    slist_node_t *prev = NULL;

    alarm_lock();
    ret = alarm_validate_locked(alarm, &list, &prev);
    if (ret != OSI_ALARM_ERR_PASS) {
        goto end;
    }

    if (!esp_timer_is_active(alarm->alarm_hdl)) {
        ret = OSI_ALARM_ERR_INVALID_STATE;
        goto end;
    }
    if (sooner_than_ms != NULL && alarm_remaining_ms(alarm->alarm_hdl) <= *sooner_than_ms) {
        ret = OSI_ALARM_ERR_FAIL;
        goto end;
    }

    ret = alarm_cancel_locked(alarm, list, prev);

end:
    alarm_unlock();
    alarm_commit_deferred_close();
    return ret;
}

osi_alarm_err_t osi_alarm_set_deadline_us(osi_alarm_t *alarm, uint64_t deadline_us)
{
    assert(s_osi_alarm_cb.mutex != NULL);

    osi_alarm_err_t ret = OSI_ALARM_ERR_PASS;
    alarm_lock();
    ret = alarm_validate_locked(alarm, NULL, NULL);
    if (ret != OSI_ALARM_ERR_PASS) {
        goto end;
    }

    /* One-shot keeps timeout_us. Cancel has already cleared period_us, so the
     * flag is what still says this word grades retries. */
    if ((alarm->flags & ALARM_FLAG_ONE_SHOT) == 0) {
        alarm->deadline_us = deadline_us;
    }

end:
    alarm_unlock();
    return ret;
}

period_ms_t osi_alarm_get_remaining_ms(const osi_alarm_t *alarm)
{
    period_ms_t remaining = 0;

    assert(s_osi_alarm_cb.mutex != NULL);

    if (alarm == NULL) {
        return 0;
    }

    alarm_lock();
    if (alarm_validate_locked((struct alarm_t *)alarm, NULL, NULL) == OSI_ALARM_ERR_PASS &&
            alarm->period_us == 0) {
        /* period_us stays set while a periodic alarm retries on a one-shot.
         * Keep the one-shot-only result: every periodic alarm reports 0. */
        remaining = alarm_remaining_ms(alarm->alarm_hdl);
    }
    alarm_unlock();

    return remaining;
}

uint32_t osi_time_get_os_boottime_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

bool osi_alarm_is_active(osi_alarm_t *alarm)
{
    assert(alarm != NULL);

    assert(s_osi_alarm_cb.mutex != NULL);

    bool active = false;
    alarm_lock();
    if (!alarm_is_open()) {
        active = false;
        goto end;
    }

    if (alarm->alarm_hdl != NULL) {
        active = esp_timer_is_active(alarm->alarm_hdl);
    }

end:
    alarm_unlock();
    return active;
}

#if defined(BT_BLUEDROID_UNIT_TEST)
void osi_alarm_test_set_invoke_hook(void (*hook)(void))
{
    s_invoke_hook = hook;
}

void osi_alarm_test_set_cancel_hook(void (*hook)(void))
{
    s_cancel_hook = hook;
}

void osi_alarm_test_set_timer_hook(void (*hook)(void))
{
    s_timer_hook = hook;
}

uint32_t osi_alarm_test_dispatch_id(const osi_alarm_t *alarm)
{
    uint32_t dispatch_id = 0;

    if (alarm == NULL || s_osi_alarm_cb.mutex == NULL) {
        return 0;
    }

    alarm_lock();
    dispatch_id = alarm->dispatch_id;
    alarm_unlock();
    return dispatch_id;
}

void osi_alarm_test_dispatch(uint32_t dispatch_id)
{
    alarm_cb_handler((void *)(uintptr_t)dispatch_id);
}
#endif
