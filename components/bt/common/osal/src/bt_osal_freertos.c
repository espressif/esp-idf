/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "bt_osal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/portable.h"
#include "bt_osal_freertos.h"

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"

static portMUX_TYPE s_bt_osal_port_mutex = portMUX_INITIALIZER_UNLOCKED;
static uint8_t s_bt_osal_in_critical[portNUM_PROCESSORS];

#if BT_OSAL_USE_ESP_TIMER
static const char *TAG = "Timer";
#endif

/*
 * One-shot work item posted with bt_osal_freertos_eventq_post_func(). The embedded
 * event (which must stay the first member so it can be cast back to the item)
 * is queued to the eventq; its backing storage is embedded too so a single
 * allocation/free covers the whole item.
 */
struct bt_osal_freertos_work_item {
    struct bt_osal_event ev;                /*!< Event queued to the eventq (must be first) */
    struct bt_osal_event_freertos storage;  /*!< Backing storage for @ref ev */
    bt_osal_event_fn *fn;                    /*!< User event handler to run */
};

static void bt_osal_freertos_work_item_run(struct bt_osal_event *ev);

/*
 * All OSAL objects (events, event queues, callouts, mutexes, semaphores) are
 * touched from ISR context, so their backing storage must come from internal
 * RAM: an object placed in PSRAM is unreachable while the flash cache is
 * disabled. Allocate it here rather than through bt_osi_mem, which lives in the
 * NimBLE controller porting layer and is not built on every target/host combo.
 */
static void *bt_osal_malloc(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}

static void bt_osal_free(void *ptr)
{
    if (ptr) {
        heap_caps_free(ptr);
    }
}

void bt_osal_freertos_event_run(struct bt_osal_event *ev);

/*
 * Free @p ev's backing work item iff it is an OSAL-owned one-shot item. Returns
 * true if reclaimed, false for the stop sentinel and caller-owned events (which
 * are left untouched). Safe to call with a NULL/uninitialized event.
 */
static bool bt_osal_freertos_reclaim_work_item(struct bt_osal_event *ev)
{
    struct bt_osal_event_freertos *event;

    if (ev == NULL || ev->event == NULL) {
        return false;
    }
    event = (struct bt_osal_event_freertos *)ev->event;
    if (event->fn != bt_osal_freertos_work_item_run) {
        return false;
    }
    /* ev is the first member of the work item, so it aliases the item base. */
    bt_osal_free((struct bt_osal_freertos_work_item *)ev);
    return true;
}

bool IRAM_ATTR bt_osal_freertos_os_started(void)
{
    return xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
}

void * IRAM_ATTR bt_osal_freertos_get_current_task_id(void)
{
    return xTaskGetCurrentTaskHandle();
}

void IRAM_ATTR bt_osal_freertos_event_init(struct bt_osal_event *ev, bt_osal_event_fn *fn,
                    void *arg)
{
    struct bt_osal_event_freertos *event = NULL;
    if(!ev->event) {
        ev->event = bt_osal_malloc(sizeof(struct bt_osal_event_freertos));
    }
    event = (struct bt_osal_event_freertos *)ev->event;
    BT_OSAL_ASSERT(event);

    memset(event, 0, sizeof(*event));
    event->fn = fn;
    event->arg = arg;
}

void IRAM_ATTR bt_osal_freertos_event_deinit(struct bt_osal_event *ev)
{
    if (!ev->event) {
        return;
    }
    bt_osal_free(ev->event);
    ev->event = NULL;
}

void IRAM_ATTR bt_osal_freertos_event_reset(struct bt_osal_event *ev)
{
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    BT_OSAL_ASSERT(event);
    // Resetting a still-queued event is a caller error
    BT_OSAL_ASSERT(!event->queued);
    event->queued = 0;
}

/*
 * Intrusive singly-linked list helpers for the event queue. The list threads
 * through each event's backing storage (struct bt_osal_event_freertos::next),
 * so no extra allocation is needed. Every helper below (except the flush) assumes
 * the caller already holds eventq->lock (entered via portENTER_CRITICAL[_ISR]).
 */
static struct bt_osal_event_freertos *IRAM_ATTR bt_osal_event_priv(struct bt_osal_event *ev)
{
    return (struct bt_osal_event_freertos *)ev->event;
}

/* Append @p ev to the tail of the list. */
static void IRAM_ATTR bt_osal_list_put_tail(struct bt_osal_eventq_freertos *eventq,
                                               struct bt_osal_event *ev)
{
    bt_osal_event_priv(ev)->next = NULL;
    if (eventq->tail != NULL) {
        bt_osal_event_priv(eventq->tail)->next = ev;
    } else {
        eventq->head = ev;
    }
    eventq->tail = ev;
}

/* Insert @p ev at the head of the list. */
static void IRAM_ATTR bt_osal_list_put_head(struct bt_osal_eventq_freertos *eventq,
                                               struct bt_osal_event *ev)
{
    bt_osal_event_priv(ev)->next = eventq->head;
    eventq->head = ev;
    if (eventq->tail == NULL) {
        eventq->tail = ev;
    }
}

/* Pop and return the head event, or NULL if the list is empty. */
static struct bt_osal_event *IRAM_ATTR bt_osal_list_pop_head(struct bt_osal_eventq_freertos *eventq)
{
    struct bt_osal_event *ev = eventq->head;

    if (ev != NULL) {
        struct bt_osal_event_freertos *event = bt_osal_event_priv(ev);
        eventq->head = event->next;
        if (eventq->head == NULL) {
            eventq->tail = NULL;
        }
        event->next = NULL;
        event->queued = false;
    }

    return ev;
}

/* Unlink @p ev from the list if present. Caller clears ev's queued flag. */
static void IRAM_ATTR bt_osal_list_unlink(struct bt_osal_eventq_freertos *eventq,
                                             struct bt_osal_event *ev)
{
    struct bt_osal_event *cur = eventq->head;
    struct bt_osal_event *prev = NULL;

    while (cur != NULL) {
        struct bt_osal_event_freertos *cur_priv = bt_osal_event_priv(cur);

        if (cur == ev) {
            if (prev != NULL) {
                bt_osal_event_priv(prev)->next = cur_priv->next;
            } else {
                eventq->head = cur_priv->next;
            }
            if (cur_priv->next == NULL) {
                eventq->tail = prev;
            }
            cur_priv->next = NULL;
            return;
        }
        prev = cur;
        cur = cur_priv->next;
    }
}

/*
 * Detach every event and clear the wakeup semaphore so the queue matches a
 * freshly initialized (empty) state. Task context only (takes the semaphore).
 *
 * OSAL-owned one-shot items (post_func) can only be freed on dispatch or by
 * reclaim; once detached here they can never be dispatched, so they must be
 * reclaimed to avoid a heap leak. Caller-owned events are left untouched
 * except for having their queued/next flags cleared so they stay re-postable.
 */
static void bt_osal_freertos_eventq_flush(struct bt_osal_eventq_freertos *eventq)
{
    struct bt_osal_event *ev;

    /* Detach the whole list under the lock, then process it outside. */
    portENTER_CRITICAL(&eventq->lock);
    ev = eventq->head;
    eventq->head = NULL;
    eventq->tail = NULL;
    portEXIT_CRITICAL(&eventq->lock);

    while (ev != NULL) {
        struct bt_osal_event_freertos *event = bt_osal_event_priv(ev);
        /* Save next first: reclaim may free the node backing this event. */
        struct bt_osal_event *next = event->next;
        event->next = NULL;
        event->queued = false;
        /* Frees OSAL-owned one-shot items; no-op for caller-owned events. */
        bt_osal_freertos_reclaim_work_item(ev);
        ev = next;
    }

    /* Binary semaphore: a single take clears any pending wakeup. */
    xSemaphoreTake(eventq->sem, 0);
}

void bt_osal_freertos_eventq_init(struct bt_osal_eventq *evq)
{
    struct bt_osal_eventq_freertos *eventq = NULL;
    if(!evq->eventq) {
        evq->eventq = bt_osal_malloc(sizeof(struct bt_osal_eventq_freertos));
        eventq = (struct bt_osal_eventq_freertos*)evq->eventq;
        BT_OSAL_ASSERT(eventq);
        memset(eventq, 0, sizeof(*eventq));
        eventq->head = NULL;
        eventq->tail = NULL;
        eventq->lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        /*
         * Binary "work pending" flag, not a per-event token count. The list is
         * the single source of truth for what is queued.
         */
        eventq->sem = xSemaphoreCreateBinary();
        BT_OSAL_ASSERT(eventq->sem);
    } else {
        eventq = (struct bt_osal_eventq_freertos*)evq->eventq;
        bt_osal_freertos_eventq_flush(eventq);
    }
}

/*
 * Ask the event queue's worker task to exit, and block until it has. Kept in one
 * place so the stop sequence has a single home (and is ready to expose as a
 * public stop API should the need ever arise). No-op if no worker was started.
 *
 * vTaskDelete() on the worker would strike it at an arbitrary point (possibly
 * mid-callback, holding a mutex or in a critical section). Instead we ask it to
 * exit itself: set `stop`, then post a sentinel event to the front of the queue
 * to wake it out of eventq_get(). Front-posting guarantees the worker's next
 * dequeue is the sentinel, so no still-queued event is popped and dropped on the
 * way out; whatever remains is reclaimed by the teardown sweep in
 * bt_osal_freertos_eventq_deinit(). The worker observes `stop` at a safe point
 * between events, signals `done` and self-deletes; we wait on `done` here.
 *
 * Caller must have validated that evq->eventq is non-NULL.
 */
static void bt_osal_freertos_eventq_stop_worker(struct bt_osal_eventq *evq)
{
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    struct bt_osal_event stop_ev = {0};

    if (eventq->task == NULL) {
        return;
    }

    eventq->stop = true;
    bt_osal_freertos_event_init(&stop_ev, NULL, NULL);
    /* Front-posted so the worker's next dequeue is the sentinel (see above). */
    bt_osal_freertos_eventq_put_to_front(evq, &stop_ev);

    xSemaphoreTake(eventq->done, portMAX_DELAY);

    /* Worker has exited and no longer touches the queue or the sentinel. */
    bt_osal_freertos_eventq_remove(evq, &stop_ev);
    bt_osal_freertos_event_deinit(&stop_ev);
    vSemaphoreDelete(eventq->done);
    eventq->done = NULL;
    eventq->task = NULL;
}

/*
 * Reclaim OSAL-owned one-shot work items still queued at teardown so they do not
 * leak; caller-owned events are simply dropped (the caller frees their storage).
 *
 * Surviving caller-owned events have their queued/next flags cleared so they
 * uphold the module-wide "detached => queued=false" invariant (mirroring
 * pop_head, eventq_remove and flush). This keeps such an event re-postable and
 * lets the documented bt_osal_event_reset / bt_osal_callout_mem_reset recovery
 * path run without tripping its BT_OSAL_ASSERT(!event->queued).
 */
static void bt_osal_freertos_eventq_reclaim_pending(struct bt_osal_eventq_freertos *eventq)
{
    struct bt_osal_event *ev = eventq->head;

    while (ev != NULL) {
        struct bt_osal_event_freertos *event = bt_osal_event_priv(ev);
        /* Save next first: reclaim may free the node backing this event. */
        struct bt_osal_event *next = event->next;
        /* Frees OSAL-owned one-shot items; returns false for caller-owned. */
        if (!bt_osal_freertos_reclaim_work_item(ev)) {
            event->next = NULL;
            event->queued = false;
        }
        ev = next;
    }
    eventq->head = NULL;
    eventq->tail = NULL;
}

void bt_osal_freertos_eventq_deinit(struct bt_osal_eventq *evq)
{
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;

    /* Deinit can be invoked twice without init . Handle this case */
    if (eventq == NULL) {
        return;
    }
    BT_OSAL_ASSERT(eventq);

    /* Gracefully stop the worker (if any) before deleting the queue it waits on. */
    bt_osal_freertos_eventq_stop_worker(evq);

    /* Worker is gone: reclaim any OSAL-owned one-shot items left queued. */
    bt_osal_freertos_eventq_reclaim_pending(eventq);

    vSemaphoreDelete(eventq->sem);
    bt_osal_free((void *)eventq);
    evq->eventq = NULL;
}

void IRAM_ATTR bt_osal_freertos_callout_mem_reset(struct bt_osal_callout *co)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;

    BT_OSAL_ASSERT(callout);
    BT_OSAL_ASSERT(callout->handle);

    bt_osal_event_reset(&callout->ev);
}

static inline bool IRAM_ATTR in_isr(void)
{
    /* XXX hw specific! */
    return xPortInIsrContext() != 0;
}

struct bt_osal_event *IRAM_ATTR bt_osal_freertos_eventq_get(struct bt_osal_eventq *evq, bt_osal_time_t tmo)
{
    struct bt_osal_event *ev = NULL;
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    if (in_isr()) {
        BT_OSAL_ASSERT(tmo == 0);
        /*
         * The list is the source of truth, so pop directly. Leaving the wakeup
         * semaphore untouched only ever over-signals a blocked consumer (safe);
         * taking it here could instead clear a signal owned by another
         * still-queued event and strand it.
         */
        portENTER_CRITICAL_ISR(&eventq->lock);
        ev = bt_osal_list_pop_head(eventq);
        portEXIT_CRITICAL_ISR(&eventq->lock);
        return ev;
    }

    /*
     * Pop first; only block on the wakeup semaphore when the list is empty.
     * A binary semaphore coalesces signals, so a token count cannot mirror the
     * list length; instead put() links then gives, and here we pop then take.
     * That ordering never loses a wakeup, while a coalesced (extra) signal or a
     * concurrent eventq_remove() emptying the list merely costs one spurious
     * re-check before we block again.
     *
     * A finite timeout must stay an upper bound across those retries, so the
     * remaining budget is recomputed from the entry tick before waiting again.
     * An infinite wait needs no bookkeeping and skips the tick read entirely.
     */
    TickType_t start_tick = (tmo != portMAX_DELAY) ? xTaskGetTickCount() : 0;
    bt_osal_time_t remaining = tmo;

    while (1) {
        portENTER_CRITICAL(&eventq->lock);
        ev = bt_osal_list_pop_head(eventq);
        portEXIT_CRITICAL(&eventq->lock);

        if (ev != NULL) {
            return ev;
        }

        if (tmo != portMAX_DELAY) {
            /* Modular arithmetic keeps this correct across tick counter wrap. */
            TickType_t elapsed = xTaskGetTickCount() - start_tick;
            if (elapsed >= tmo) {
                return NULL;
            }
            remaining = tmo - elapsed;
        }

        if (xSemaphoreTake(eventq->sem, remaining) != pdTRUE) {
            return NULL;
        }
    }
}

void IRAM_ATTR bt_osal_freertos_eventq_put(struct bt_osal_eventq *evq, struct bt_osal_event *ev)
{
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    BaseType_t woken;

    if (in_isr()) {
        portENTER_CRITICAL_ISR(&eventq->lock);
        if (event->queued) {
            portEXIT_CRITICAL_ISR(&eventq->lock);
            return;
        }
        event->queued = true;
        bt_osal_list_put_tail(eventq, ev);
        portEXIT_CRITICAL_ISR(&eventq->lock);

        woken = pdFALSE;
        /* Wake a blocked consumer. The semaphore is a binary "work pending"
         * flag, so a give that finds it already set fails harmlessly (a wakeup
         * is already pending) — the linked event is the source of truth. */
        xSemaphoreGiveFromISR(eventq->sem, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        portENTER_CRITICAL(&eventq->lock);
        if (event->queued) {
            portEXIT_CRITICAL(&eventq->lock);
            return;
        }
        event->queued = true;
        bt_osal_list_put_tail(eventq, ev);
        portEXIT_CRITICAL(&eventq->lock);

        /* Signal availability outside the spinlock (FreeRTOS API is not
         * callable inside a critical section). Binary semaphore: a give that
         * finds it already set fails harmlessly, so the result is ignored. */
        (void)xSemaphoreGive(eventq->sem);
    }
}

void IRAM_ATTR bt_osal_freertos_eventq_put_to_front(struct bt_osal_eventq *evq, struct bt_osal_event *ev)
{
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    BaseType_t woken;

    if (in_isr()) {
        portENTER_CRITICAL_ISR(&eventq->lock);
        if (event->queued) {
            portEXIT_CRITICAL_ISR(&eventq->lock);
            return;
        }
        event->queued = true;
        bt_osal_list_put_head(eventq, ev);
        portEXIT_CRITICAL_ISR(&eventq->lock);

        woken = pdFALSE;
        /* Best-effort wakeup; see bt_osal_freertos_eventq_put(). */
        xSemaphoreGiveFromISR(eventq->sem, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        portENTER_CRITICAL(&eventq->lock);
        if (event->queued) {
            portEXIT_CRITICAL(&eventq->lock);
            return;
        }
        event->queued = true;
        bt_osal_list_put_head(eventq, ev);
        portEXIT_CRITICAL(&eventq->lock);

        /* Best-effort wakeup; see bt_osal_freertos_eventq_put(). */
        (void)xSemaphoreGive(eventq->sem);
    }
}

void IRAM_ATTR bt_osal_freertos_eventq_remove(struct bt_osal_eventq *evq,
                       struct bt_osal_event *ev)
{
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    /*
     * Unlink from the list only; leave the wakeup semaphore alone. The list is
     * the source of truth, so a now-stale "work pending" signal just makes a
     * blocked consumer re-check and find nothing (safe over-signal). Taking the
     * semaphore here could instead clear a signal owned by another still-queued
     * event and strand it.
     */
    if (in_isr()) {
        portENTER_CRITICAL_ISR(&eventq->lock);
        if (event->queued) {
            bt_osal_list_unlink(eventq, ev);
            event->queued = 0;
        }
        portEXIT_CRITICAL_ISR(&eventq->lock);
    } else {
        portENTER_CRITICAL(&eventq->lock);
        if (event->queued) {
            bt_osal_list_unlink(eventq, ev);
            event->queued = 0;
        }
        portEXIT_CRITICAL(&eventq->lock);
    }
}

bool IRAM_ATTR bt_osal_freertos_eventq_is_empty(struct bt_osal_eventq *evq)
{
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    bool empty;

    if (in_isr()) {
        portENTER_CRITICAL_ISR(&eventq->lock);
        empty = (eventq->head == NULL);
        portEXIT_CRITICAL_ISR(&eventq->lock);
    } else {
        portENTER_CRITICAL(&eventq->lock);
        empty = (eventq->head == NULL);
        portEXIT_CRITICAL(&eventq->lock);
    }

    return empty;
}

static void bt_osal_freertos_eventq_task(void *arg)
{
    struct bt_osal_eventq *evq = (struct bt_osal_eventq *)arg;
    struct bt_osal_eventq_freertos *eventq = (struct bt_osal_eventq_freertos *)evq->eventq;
    struct bt_osal_event *ev;

    while (1) {
        /* Block forever until an event is posted, then dispatch it */
        ev = bt_osal_freertos_eventq_get(evq, portMAX_DELAY);

        /* Graceful stop: stop_worker() set `stop` and woke us with a sentinel
         * front-posted to the queue, so `ev` is normally that sentinel. Acknowledge
         * and self-delete here, at a safe point between events, so the task is
         * never torn down mid-callback. If a race dequeued a real item instead,
         * it is dropped unrun; reclaim it when the OSAL owns it (the sentinel and
         * caller-owned events are left alone) so it does not leak. */
        if (eventq->stop) {
            bt_osal_freertos_reclaim_work_item(ev);
            xSemaphoreGive(eventq->done);
            vTaskDelete(NULL);
        }

        if (ev != NULL) {
            bt_osal_freertos_event_run(ev);
        }
    }
}

bt_osal_error_t bt_osal_freertos_eventq_start(struct bt_osal_eventq *evq,
                                          const struct bt_osal_task_info *info)
{
    struct bt_osal_eventq_freertos *eventq;
    BaseType_t ret;

    if (evq == NULL || evq->eventq == NULL || info == NULL || info->name == NULL) {
        return BT_OSAL_EINVAL;
    }

    /* xTaskCreatePinnedToCore() asserts on a bad core, so reject it here and let
     * the caller handle it instead of aborting the system. */
    if (info->core_id != BT_OSAL_TASK_NO_AFFINITY &&
        (info->core_id < 0 || info->core_id >= portNUM_PROCESSORS)) {
        return BT_OSAL_EINVAL;
    }

    eventq = (struct bt_osal_eventq_freertos *)evq->eventq;

    /* A task is already processing this queue */
    if (eventq->task != NULL) {
        return BT_OSAL_EINVAL;
    }

    /* Completion signal the worker raises when it exits; deinit waits on it. */
    eventq->stop = false;
    eventq->done = xSemaphoreCreateBinary();
    if (eventq->done == NULL) {
        return BT_OSAL_ENOMEM;
    }

    ret = xTaskCreatePinnedToCore(bt_osal_freertos_eventq_task, info->name,
                                  info->stack_size, evq, info->prio,
                                  &eventq->task, info->core_id);
    if (ret != pdPASS) {
        vSemaphoreDelete(eventq->done);
        eventq->done = NULL;
        eventq->task = NULL;
        return BT_OSAL_ENOMEM;
    }

    return BT_OSAL_OK;
}

static void bt_osal_freertos_work_item_run(struct bt_osal_event *ev)
{
    /* ev is the first member of the item, so it aliases the item itself. */
    struct bt_osal_freertos_work_item *item = (struct bt_osal_freertos_work_item *)ev;

    /* Run the user handler, then release the one-shot work item. */
    item->fn(ev);
    bt_osal_free(item);
}

bt_osal_error_t bt_osal_freertos_eventq_post_func(struct bt_osal_eventq *evq,
                                              bt_osal_event_fn *fn, void *arg)
{
    struct bt_osal_freertos_work_item *item;

    if (evq == NULL || evq->eventq == NULL || fn == NULL) {
        return BT_OSAL_EINVAL;
    }

    /* Memory allocation is not ISR-safe; reject calls from interrupt context. */
    if (in_isr()) {
        return BT_OSAL_ERR_IN_ISR;
    }

    item = bt_osal_malloc(sizeof(*item));
    if (item == NULL) {
        return BT_OSAL_ENOMEM;
    }

    item->fn = fn;

    /*
     * Point the event at the embedded storage so bt_osal_freertos_event_init()
     * skips its internal allocation. The wrapper handler receives the event
     * and forwards it to the user handler, whose argument (@p arg) is
     * retrievable via bt_osal_event_get_arg().
     */
    item->ev.event = &item->storage;
    bt_osal_freertos_event_init(&item->ev, bt_osal_freertos_work_item_run, arg);

    bt_osal_freertos_eventq_put(evq, &item->ev);

    return BT_OSAL_OK;
}

bt_osal_error_t bt_osal_freertos_mutex_init(struct bt_osal_mutex *mu)
{
    struct bt_osal_mutex_freertos *mutex = NULL;

    if(!mu->mutex) {
        mu->mutex = bt_osal_malloc(sizeof(struct bt_osal_mutex_freertos));
        mutex = (struct bt_osal_mutex_freertos *)mu->mutex;

        if (!mutex) {
            return BT_OSAL_ENOMEM;
        }

        memset(mutex, 0, sizeof(*mutex));
        mutex->handle = xSemaphoreCreateRecursiveMutex();
        if (!mutex->handle) {
            bt_osal_free((void *)mutex);
            mu->mutex = NULL;
            return BT_OSAL_ENOMEM;
        }
    }

    return BT_OSAL_OK;
}

bt_osal_error_t bt_osal_freertos_mutex_deinit(struct bt_osal_mutex *mu)
{
    struct bt_osal_mutex_freertos *mutex = (struct bt_osal_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BT_OSAL_INVALID_PARAM;
    }

    BT_OSAL_ASSERT(mutex->handle);
    vSemaphoreDelete(mutex->handle);

    bt_osal_free((void *)mutex);
    mu->mutex = NULL;

    return BT_OSAL_OK;
}

void IRAM_ATTR bt_osal_freertos_event_run(struct bt_osal_event *ev)
{
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    event->fn(ev);
}

bool IRAM_ATTR bt_osal_freertos_event_is_queued(struct bt_osal_event *ev)
{
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    return event->queued;
}

void * IRAM_ATTR bt_osal_freertos_event_get_arg(struct bt_osal_event *ev)
{
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    return event->arg;
}

void IRAM_ATTR bt_osal_freertos_event_set_arg(struct bt_osal_event *ev, void *arg)
{
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)ev->event;
    event->arg = arg;
}


bt_osal_error_t IRAM_ATTR bt_osal_freertos_mutex_pend(struct bt_osal_mutex *mu, bt_osal_time_t timeout)
{
    BaseType_t ret;
    struct bt_osal_mutex_freertos *mutex = (struct bt_osal_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BT_OSAL_INVALID_PARAM;
    }

    BT_OSAL_ASSERT(mutex->handle);

    if (in_isr()) {
        ret = pdFAIL;
        BT_OSAL_ASSERT(0);
    } else {
        ret = xSemaphoreTakeRecursive(mutex->handle, timeout);
    }

    return ret == pdPASS ? BT_OSAL_OK : BT_OSAL_TIMEOUT;
}

bt_osal_error_t IRAM_ATTR bt_osal_freertos_mutex_release(struct bt_osal_mutex *mu)
{
    struct bt_osal_mutex_freertos *mutex = (struct bt_osal_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BT_OSAL_INVALID_PARAM;
    }

    BT_OSAL_ASSERT(mutex->handle);

    if (in_isr()) {
        BT_OSAL_ASSERT(0);
    } else {
        if (xSemaphoreGiveRecursive(mutex->handle) != pdPASS) {
            return BT_OSAL_BAD_MUTEX;
        }
    }

    return BT_OSAL_OK;
}

bt_osal_error_t bt_osal_freertos_sem_init(struct bt_osal_sem *sem, uint16_t tokens)
{
    struct bt_osal_sem_freertos *semaphore = NULL;

    if(!sem->sem) {
        sem->sem = bt_osal_malloc(sizeof(struct bt_osal_sem_freertos));
        semaphore = (struct bt_osal_sem_freertos *)sem->sem;

        if (!semaphore) {
            return BT_OSAL_ENOMEM;
        }

        memset(semaphore, 0, sizeof(*semaphore));
        semaphore->handle = xSemaphoreCreateCounting(128, tokens);
        if (!semaphore->handle) {
            bt_osal_free((void *)semaphore);
            sem->sem = NULL;
            return BT_OSAL_ENOMEM;
        }
    }

    return BT_OSAL_OK;
}

bt_osal_error_t bt_osal_freertos_sem_deinit(struct bt_osal_sem *sem)
{
    struct bt_osal_sem_freertos *semaphore = (struct bt_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BT_OSAL_INVALID_PARAM;
    }

    BT_OSAL_ASSERT(semaphore->handle);
    vSemaphoreDelete(semaphore->handle);

    bt_osal_free((void *)semaphore);
    sem->sem = NULL;

    return BT_OSAL_OK;
}

bt_osal_error_t IRAM_ATTR bt_osal_freertos_sem_pend(struct bt_osal_sem *sem, bt_osal_time_t timeout)
{
    BaseType_t woken = pdFALSE;
    BaseType_t ret;
    struct bt_osal_sem_freertos *semaphore = (struct bt_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BT_OSAL_INVALID_PARAM;
    }

    BT_OSAL_ASSERT(semaphore->handle);

    if (in_isr()) {
        BT_OSAL_ASSERT(timeout == 0);
        ret = xSemaphoreTakeFromISR(semaphore->handle, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xSemaphoreTake(semaphore->handle, timeout);
    }

    return ret == pdPASS ? BT_OSAL_OK : BT_OSAL_TIMEOUT;
}

bt_osal_error_t IRAM_ATTR bt_osal_freertos_sem_release(struct bt_osal_sem *sem)
{
    BaseType_t ret;
    BaseType_t woken = pdFALSE;
    struct bt_osal_sem_freertos *semaphore = (struct bt_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BT_OSAL_INVALID_PARAM;
    }

    BT_OSAL_ASSERT(semaphore->handle);

    if (in_isr()) {
        ret = xSemaphoreGiveFromISR(semaphore->handle, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xSemaphoreGive(semaphore->handle);
    }

    /* A give fails only when the counting semaphore is already at its maximum
     * count; report it as a runtime error instead of silently swallowing it in
     * assertions-disabled builds. */
    if (ret != pdPASS) {
        return BT_OSAL_ERROR;
    }

    return BT_OSAL_OK;
}

#if BT_OSAL_USE_ESP_TIMER
static void IRAM_ATTR bt_osal_event_fn_wrapper(void *arg)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)arg;
    BT_OSAL_ASSERT(callout);

    if (callout->evq) {
        bt_osal_eventq_put(callout->evq, &callout->ev);
    } else {
        struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)callout->ev.event;
        event->fn(&callout->ev);
    }
}

static IRAM_ATTR bt_osal_error_t esp_err_to_bt_osal_error(esp_err_t err)
{
    switch(err) {
    case ESP_ERR_INVALID_ARG:
        return BT_OSAL_INVALID_PARAM;

    case ESP_ERR_INVALID_STATE:
        return BT_OSAL_EINVAL;

    case ESP_OK:
        return BT_OSAL_OK;

   default:
        return BT_OSAL_ERROR;
    }
}
#else

static void IRAM_ATTR bt_osal_callout_timer_cb(TimerHandle_t timer)
{
    struct bt_osal_callout_freertos *callout;

    callout = pvTimerGetTimerID(timer);
    BT_OSAL_ASSERT(callout);

    if (callout->evq) {
        bt_osal_eventq_put(callout->evq, &callout->ev);
    } else {
        struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)callout->ev.event;
        event->fn(&callout->ev);
    }
}
#endif

bt_osal_error_t bt_osal_freertos_callout_init(struct bt_osal_callout *co, struct bt_osal_eventq *evq,
                      bt_osal_event_fn *ev_cb, void *ev_arg)
{
    struct bt_osal_callout_freertos *callout = NULL;

    if(!co->co) {
        co->co = bt_osal_malloc(sizeof(struct bt_osal_callout_freertos));
        callout = (struct bt_osal_callout_freertos *)co->co;
        if (!callout) {
            return BT_OSAL_ENOMEM;
        }

        memset(callout, 0, sizeof(*callout));
        bt_osal_event_init(&callout->ev, ev_cb, ev_arg);
        /* Set for both timer backends: with a NULL evq the expiry callback runs
         * the event inline in timer context instead of queueing it. */
        callout->evq = evq;

#if BT_OSAL_USE_ESP_TIMER
        esp_timer_create_args_t create_args = {
            .callback = bt_osal_event_fn_wrapper,
            .arg = callout,
            .name = "bt_osal_co"
        };

        if (esp_timer_create(&create_args, &callout->handle) != ESP_OK) {
            bt_osal_event_deinit(&callout->ev);
            bt_osal_free((void *)callout);
            co->co = NULL;
            return BT_OSAL_ENOMEM;
        }
#else
        callout->handle = xTimerCreate("co", 1, pdFALSE, callout, bt_osal_callout_timer_cb);

        if (!callout->handle) {
            bt_osal_event_deinit(&callout->ev);
            bt_osal_free((void *)callout);
            co->co = NULL;
            return BT_OSAL_ENOMEM;
        }
#endif // BT_OSAL_USE_ESP_TIMER
    }
    else {
        callout = (struct bt_osal_callout_freertos *)co->co;
        BT_OSAL_ASSERT(callout);
        callout->evq = evq;
        bt_osal_event_init(&callout->ev, ev_cb, ev_arg);
    }

    return BT_OSAL_OK;
}

void bt_osal_freertos_callout_deinit(struct bt_osal_callout *co)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;

    /* Since we dynamically deinit timers, function can be called for NULL timers. Return for such scenarios */
    if (!callout) {
        return;
    }

    if (!callout->handle) {
        return;
    }

    /* Stop and delete the timer BEFORE deinitializing the event. Otherwise a
     * timer callback firing on the esp_timer/timer-daemon task could dereference
     * callout->ev.event after it has been freed and set to NULL. */
#if BT_OSAL_USE_ESP_TIMER
    esp_err_t err = esp_timer_stop(callout->handle);
    if(err != ESP_OK) {
        if (err != ESP_ERR_INVALID_STATE) { // ESP_ERR_INVALID_STATE is expected when timer is already stopped
            ESP_LOGD(TAG, "Timer not stopped");
        }
    }
    err = esp_timer_delete(callout->handle);
    if(err != ESP_OK) {
        ESP_LOGW(TAG, "Timer not deleted");
    }
#else
    xTimerStop(callout->handle, portMAX_DELAY);
    xTimerDelete(callout->handle, portMAX_DELAY);
#endif // BT_OSAL_USE_ESP_TIMER

    /* Unlink the event from the queue's intrusive list before freeing its
     * backing memory. A one-shot timer that already fired may have posted
     * &callout->ev to callout->evq, leaving a dangling pointer otherwise. */
    if (callout->evq) {
        bt_osal_freertos_eventq_remove(callout->evq, &callout->ev);
    }

    bt_osal_event_deinit(&callout->ev);

    bt_osal_free((void *)callout);
    co->co = NULL;
    memset(co, 0, sizeof(struct bt_osal_callout));
}

uint16_t IRAM_ATTR bt_osal_freertos_sem_get_count(struct bt_osal_sem *sem)
{
    struct bt_osal_sem_freertos *semaphore = (struct bt_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return 0;
    }

    return uxSemaphoreGetCount(semaphore->handle);
}


bt_osal_error_t IRAM_ATTR bt_osal_freertos_callout_reset(struct bt_osal_callout *co, bt_osal_time_t ticks)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;
#if BT_OSAL_USE_ESP_TIMER
    esp_timer_stop(callout->handle);
    if (callout->evq) {
        bt_osal_freertos_eventq_remove(callout->evq, &callout->ev);
    }

    return esp_err_to_bt_osal_error(esp_timer_start_once(callout->handle, (uint64_t)ticks * 1000));
#else

    BaseType_t woken1 = pdFALSE, woken2 = pdFALSE, woken3 = pdFALSE;

    if (ticks == 0) {
        ticks = 1;
    }
    if (in_isr()) {
        xTimerStopFromISR(callout->handle, &woken1);
        if (callout->evq) {
            bt_osal_freertos_eventq_remove(callout->evq, &callout->ev);
        }
        xTimerChangePeriodFromISR(callout->handle, ticks, &woken2);
        xTimerResetFromISR(callout->handle, &woken3);

        if( woken1 == pdTRUE || woken2 == pdTRUE || woken3 == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        xTimerStop(callout->handle, portMAX_DELAY);
        if (callout->evq) {
            bt_osal_freertos_eventq_remove(callout->evq, &callout->ev);
        }
        xTimerChangePeriod(callout->handle, ticks, portMAX_DELAY);
        xTimerReset(callout->handle, portMAX_DELAY);
    }

    return BT_OSAL_OK;
#endif
}

void IRAM_ATTR bt_osal_freertos_callout_stop(struct bt_osal_callout *co)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;

    if (!callout) {
        return;
    }

#if BT_OSAL_USE_ESP_TIMER
    /* esp_timer_stop is ISR-safe (uses portENTER_CRITICAL_SAFE internally),
     * so call it unconditionally to guarantee the timer is disarmed. */
    esp_timer_stop(callout->handle);
#else
    if (in_isr()) {
        BaseType_t woken = pdFALSE;
        xTimerStopFromISR(callout->handle, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        xTimerStop(callout->handle, portMAX_DELAY);
    }
#endif

    if (callout->evq) {
        bt_osal_freertos_eventq_remove(callout->evq, &callout->ev);
    }
}

bool IRAM_ATTR bt_osal_freertos_callout_is_active(struct bt_osal_callout *co)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;
    if (!callout) {
        return false;
    }
#if BT_OSAL_USE_ESP_TIMER
    return esp_timer_is_active(callout->handle);
#else
    return xTimerIsTimerActive(callout->handle) == pdTRUE;
#endif
}

bt_osal_time_t IRAM_ATTR bt_osal_freertos_callout_get_ticks(struct bt_osal_callout *co)
{
#if BT_OSAL_USE_ESP_TIMER

    uint32_t exp = 0;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    uint64_t expiry = 0;
    esp_err_t err;

    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;

    //Fetch expiry time in microseconds
    err = esp_timer_get_expiry_time((esp_timer_handle_t)(callout->handle), &expiry);
    if (err != ESP_OK) {
        //Error. Could not fetch the expiry time
        return 0;
    }

    //Convert microseconds to ticks
    bt_osal_freertos_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
#else
    //esp_timer_get_expiry_time() is only available from IDF 5.0 onwards
    /* Returning 0 from here should not cause any effect.
     * Drawback of this approach is that existing code to reset timer would be called
     * more often (since the if condition to invoke reset timer would always succeed if
     * timer is active).
     */
    exp = 0;
#endif //ESP_IDF_VERSION

    return exp;
#else
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;
    return xTimerGetExpiryTime(callout->handle);
#endif
}

bt_osal_time_t IRAM_ATTR bt_osal_freertos_callout_remaining_ticks(struct bt_osal_callout *co,
                                     bt_osal_time_t now)
{
    bt_osal_time_t rt;
    uint32_t exp = 0;

    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;

#if BT_OSAL_USE_ESP_TIMER
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    uint64_t expiry = 0;
    esp_err_t err;

    //Fetch expiry time in microseconds
    err = esp_timer_get_expiry_time((esp_timer_handle_t)(callout->handle), &expiry);
    if (err != ESP_OK) {
        //Error. Could not fetch the expiry time
        return 0;
    }

    //Convert microseconds to ticks
    bt_osal_freertos_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
#else
    //esp_timer_get_expiry_time() is only available from IDF 5.0 onwards
    //Set expiry to 0
    exp = 0;
#endif //ESP_IDF_VERSION
#else
    exp = xTimerGetExpiryTime(callout->handle);
#endif

    //Use signed modular subtraction so the comparison stays correct even when
    //the 32-bit tick/millisecond counter wraps around (valid for periods up to
    //half the uint32_t range).
    if ((bt_osal_stime_t)(exp - now) > 0) {
        rt = exp - now;
    } else {
        rt = 0;
    }

    return rt;
}

void IRAM_ATTR bt_osal_freertos_callout_set_arg(struct bt_osal_callout *co, void *arg)
{
    struct bt_osal_callout_freertos *callout = (struct bt_osal_callout_freertos *)co->co;
    struct bt_osal_event_freertos *event = (struct bt_osal_event_freertos *)callout->ev.event;
    event->arg = arg;
}

uint32_t IRAM_ATTR bt_osal_freertos_time_get(void)
{
#if BT_OSAL_USE_ESP_TIMER
    return esp_timer_get_time() / 1000;
#else
    return xTaskGetTickCountFromISR();
#endif
}

bt_osal_error_t IRAM_ATTR bt_osal_freertos_time_ms_to_ticks(uint32_t ms, bt_osal_time_t *out_ticks)
{
    uint64_t ticks;
#if BT_OSAL_USE_ESP_TIMER
    ticks = (uint64_t)ms;
#else
    ticks = ((uint64_t)ms * configTICK_RATE_HZ) / 1000;
#endif
    if (ticks > UINT32_MAX) {
        return BT_OSAL_EINVAL;
    }

    *out_ticks = ticks;

    return 0;
}

bt_osal_error_t IRAM_ATTR bt_osal_freertos_time_ticks_to_ms(bt_osal_time_t ticks, uint32_t *out_ms)
{
    uint64_t ms;
#if BT_OSAL_USE_ESP_TIMER
    ms = ((uint64_t)ticks);
#else
    ms = ((uint64_t)ticks * 1000) / configTICK_RATE_HZ;
#endif
    if (ms > UINT32_MAX) {
        return BT_OSAL_EINVAL;
     }

    *out_ms = ms;

    return 0;
}

bt_osal_time_t IRAM_ATTR bt_osal_freertos_time_ms_to_ticks32(uint32_t ms)
{
#if BT_OSAL_USE_ESP_TIMER
    return ms;
#else
    return (bt_osal_time_t)((uint64_t)ms * configTICK_RATE_HZ / 1000);
#endif
}

uint32_t IRAM_ATTR bt_osal_freertos_time_ticks_to_ms32(bt_osal_time_t ticks)
{
#if BT_OSAL_USE_ESP_TIMER
    return ticks;
#else
    return (uint32_t)((uint64_t)ticks * 1000 / configTICK_RATE_HZ);
#endif
}

void IRAM_ATTR bt_osal_freertos_time_delay(bt_osal_time_t ticks)
{
#if BT_OSAL_USE_ESP_TIMER
    vTaskDelay(ticks / portTICK_PERIOD_MS);
#else
    vTaskDelay(ticks);
#endif
}

uint32_t IRAM_ATTR bt_osal_freertos_hw_enter_critical(void)
{
    BaseType_t core;

    portENTER_CRITICAL(&s_bt_osal_port_mutex);
    core = xPortGetCoreID();
    if (core < portNUM_PROCESSORS) {
        ++s_bt_osal_in_critical[core];
    }
    return 0;
}

uint8_t IRAM_ATTR bt_osal_freertos_hw_is_in_critical(void)
{
    BaseType_t core;

    core = xPortGetCoreID();
    if (core >= portNUM_PROCESSORS) {
        return 0;
    }
    return s_bt_osal_in_critical[core];
}

void IRAM_ATTR bt_osal_freertos_hw_exit_critical(uint32_t ctx)
{
    BaseType_t core;

    core = xPortGetCoreID();
    if (core < portNUM_PROCESSORS && s_bt_osal_in_critical[core] > 0) {
        --s_bt_osal_in_critical[core];
    }
    portEXIT_CRITICAL(&s_bt_osal_port_mutex);

}

uint32_t IRAM_ATTR bt_osal_freertos_get_time_forever(void)
{
    return portMAX_DELAY;
}

const struct bt_osal_funcs_t bt_osal_funcs_ro = {
    .p_bt_osal_os_started = bt_osal_freertos_os_started,
    .p_bt_osal_get_current_task_id = bt_osal_freertos_get_current_task_id,
    .p_bt_osal_eventq_init = bt_osal_freertos_eventq_init,
    .p_bt_osal_eventq_deinit = bt_osal_freertos_eventq_deinit,
    .p_bt_osal_eventq_get = bt_osal_freertos_eventq_get,
    .p_bt_osal_eventq_put = bt_osal_freertos_eventq_put,
    .p_bt_osal_eventq_put_to_front = bt_osal_freertos_eventq_put_to_front,
    .p_bt_osal_eventq_remove = bt_osal_freertos_eventq_remove,
    .p_bt_osal_event_run = bt_osal_freertos_event_run,
    .p_bt_osal_eventq_is_empty = bt_osal_freertos_eventq_is_empty,
    .p_bt_osal_event_init = bt_osal_freertos_event_init,
    .p_bt_osal_event_deinit = bt_osal_freertos_event_deinit,
    .p_bt_osal_event_reset = bt_osal_freertos_event_reset,
    .p_bt_osal_event_is_queued = bt_osal_freertos_event_is_queued,
    .p_bt_osal_event_get_arg = bt_osal_freertos_event_get_arg,
    .p_bt_osal_event_set_arg = bt_osal_freertos_event_set_arg,
    .p_bt_osal_mutex_init = bt_osal_freertos_mutex_init,
    .p_bt_osal_mutex_deinit = bt_osal_freertos_mutex_deinit,
    .p_bt_osal_mutex_pend = bt_osal_freertos_mutex_pend,
    .p_bt_osal_mutex_release = bt_osal_freertos_mutex_release,
    .p_bt_osal_sem_init = bt_osal_freertos_sem_init,
    .p_bt_osal_sem_deinit = bt_osal_freertos_sem_deinit,
    .p_bt_osal_sem_pend = bt_osal_freertos_sem_pend,
    .p_bt_osal_sem_release = bt_osal_freertos_sem_release,
    .p_bt_osal_sem_get_count = bt_osal_freertos_sem_get_count,
    .p_bt_osal_callout_init = bt_osal_freertos_callout_init,
    .p_bt_osal_callout_reset = bt_osal_freertos_callout_reset,
    .p_bt_osal_callout_stop = bt_osal_freertos_callout_stop,
    .p_bt_osal_callout_deinit = bt_osal_freertos_callout_deinit,
    .p_bt_osal_callout_mem_reset = bt_osal_freertos_callout_mem_reset,
    .p_bt_osal_callout_is_active = bt_osal_freertos_callout_is_active,
    .p_bt_osal_callout_get_ticks = bt_osal_freertos_callout_get_ticks,
    .p_bt_osal_callout_remaining_ticks = bt_osal_freertos_callout_remaining_ticks,
    .p_bt_osal_callout_set_arg = bt_osal_freertos_callout_set_arg,
    .p_bt_osal_time_get = bt_osal_freertos_time_get,
    .p_bt_osal_time_ms_to_ticks = bt_osal_freertos_time_ms_to_ticks,
    .p_bt_osal_time_ticks_to_ms = bt_osal_freertos_time_ticks_to_ms,
    .p_bt_osal_time_ms_to_ticks32 = bt_osal_freertos_time_ms_to_ticks32,
    .p_bt_osal_time_ticks_to_ms32 = bt_osal_freertos_time_ticks_to_ms32,
    .p_bt_osal_time_delay = bt_osal_freertos_time_delay,
#if NIMBLE_CFG_CONTROLLER || CONFIG_NIMBLE_CONTROLLER_MODE
    .p_bt_osal_hw_set_isr = NULL,
#endif
    .p_bt_osal_hw_enter_critical = bt_osal_freertos_hw_enter_critical,
    .p_bt_osal_hw_exit_critical = bt_osal_freertos_hw_exit_critical,
    .p_bt_osal_get_time_forever = bt_osal_freertos_get_time_forever,
    .p_bt_osal_hw_is_in_critical = bt_osal_freertos_hw_is_in_critical,
    // Added
    .p_bt_osal_eventq_start = bt_osal_freertos_eventq_start,
    .p_bt_osal_eventq_post_func = bt_osal_freertos_eventq_post_func
};

struct bt_osal_funcs_t *bt_osal_funcs = NULL;

struct bt_osal_funcs_t * bt_osal_freertos_funcs_get(void)
{
    return bt_osal_funcs;
}

void bt_osal_freertos_funcs_init(void)
{
    bt_osal_funcs = (struct bt_osal_funcs_t *)bt_osal_malloc(sizeof(struct bt_osal_funcs_t));
    if(!bt_osal_funcs) {
        printf("OSAL funcs init failed\n");
        assert(0);
        return;
    }
    memcpy(bt_osal_funcs, &bt_osal_funcs_ro, sizeof(struct bt_osal_funcs_t));
}

void bt_osal_freertos_funcs_deinit(void)
{
    if (bt_osal_funcs) {
        bt_osal_free(bt_osal_funcs);
    }
    bt_osal_funcs = NULL;
}

void bt_osal_func_init(void)
{
    return bt_osal_freertos_funcs_init();
}

void bt_osal_func_deinit(void)
{
    return bt_osal_freertos_funcs_deinit();
}

struct bt_osal_funcs_t* bt_osal_funcs_get(void)
{
    return bt_osal_freertos_funcs_get();
}
