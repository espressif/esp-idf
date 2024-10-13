/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "nimble/nimble_npl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/portable.h"
#include "nimble/npl_freertos.h"

#include "os/os_mempool.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "esp_bt.h"
#include "bt_osi_mem.h"

portMUX_TYPE ble_port_mutex = portMUX_INITIALIZER_UNLOCKED;

#if BLE_NPL_USE_ESP_TIMER
static const char *TAG = "Timer";
#endif

#define OS_MEM_ALLOC (1)

#if (!defined(SOC_ESP_NIMBLE_CONTROLLER) || !SOC_ESP_NIMBLE_CONTROLLER)
#error "not defined SOC_ESP_NIMBLE_CONTROLLER or SOC_ESP_NIMBLE_CONTROLLER is zero"
#endif

#if CONFIG_BT_NIMBLE_ENABLED
#define BLE_HOST_CO_COUNT    (8)
#define BLE_HOST_EV_COUNT    (11 + BLE_HOST_CO_COUNT)
#define BLE_HOST_EVQ_COUNT   (3)
#define BLE_HOST_SEM_COUNT   (10)
#define BLE_HOST_MUTEX_COUNT (4)
#else
#define BLE_HOST_CO_COUNT    (0)
#define BLE_HOST_EV_COUNT    (0)
#define BLE_HOST_EVQ_COUNT   (0)
#define BLE_HOST_SEM_COUNT   (0)
#define BLE_HOST_MUTEX_COUNT (0)
#endif

struct os_mempool ble_freertos_ev_pool;
static os_membuf_t *ble_freertos_ev_buf = NULL;

struct os_mempool ble_freertos_evq_pool;
static os_membuf_t *ble_freertos_evq_buf = NULL;

struct os_mempool ble_freertos_co_pool;
static os_membuf_t *ble_freertos_co_buf = NULL;

struct os_mempool ble_freertos_sem_pool;
static os_membuf_t *ble_freertos_sem_buf = NULL;

struct os_mempool ble_freertos_mutex_pool;
static os_membuf_t *ble_freertos_mutex_buf = NULL;

static uint16_t ble_freertos_total_event_cnt = 0;
static ble_npl_count_info_t g_ctrl_npl_info = {
    .co_count = 0,
    .evt_count = 0,
    .evtq_count = 0,
    .mutex_count = 0,
    .sem_count = 0,
};

bool
IRAM_ATTR npl_freertos_os_started(void)
{
    return xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
}

void *
IRAM_ATTR npl_freertos_get_current_task_id(void)
{
    return xTaskGetCurrentTaskHandle();
}

void
IRAM_ATTR npl_freertos_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                    void *arg)
{
    struct ble_npl_event_freertos *event = NULL;
#if OS_MEM_ALLOC
    if (!os_memblock_from(&ble_freertos_ev_pool,ev->event)) {
        ev->event = os_memblock_get(&ble_freertos_ev_pool);
    }
#else
    if(!ev->event) {
        ev->event = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_event_freertos));
    }
#endif
    event = (struct ble_npl_event_freertos *)ev->event;
    BLE_LL_ASSERT(event);

    memset(event, 0, sizeof(*event));
    event->fn = fn;
    event->arg = arg;
}

void
IRAM_ATTR npl_freertos_event_deinit(struct ble_npl_event *ev)
{
    BLE_LL_ASSERT(ev->event);
#if OS_MEM_ALLOC
    os_memblock_put(&ble_freertos_ev_pool,ev->event);
#else
    bt_osi_mem_free(ev->event);
#endif
    ev->event = NULL;
}

void
IRAM_ATTR npl_freertos_event_reset(struct ble_npl_event *ev)
{
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;
    BLE_LL_ASSERT(event);
    event->queued = 0;
}

void
npl_freertos_eventq_init(struct ble_npl_eventq *evq)
{
    struct ble_npl_eventq_freertos *eventq = NULL;
#if OS_MEM_ALLOC
    if (!os_memblock_from(&ble_freertos_evq_pool,evq->eventq)) {
        evq->eventq = os_memblock_get(&ble_freertos_evq_pool);
        eventq = (struct ble_npl_eventq_freertos*)evq->eventq;
        BLE_LL_ASSERT(eventq);
        memset(eventq, 0, sizeof(*eventq));
        eventq->q = xQueueCreate(ble_freertos_total_event_cnt, sizeof(struct ble_npl_eventq *));
        BLE_LL_ASSERT(eventq->q);
    } else {
        eventq = (struct ble_npl_eventq_freertos*)evq->eventq;
        xQueueReset(eventq->q);
    }
#else
    if(!evq->eventq) {
        evq->eventq = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_eventq_freertos));
        eventq = (struct ble_npl_eventq_freertos*)evq->eventq;
        BLE_LL_ASSERT(eventq);
        memset(eventq, 0, sizeof(*eventq));
        eventq->q = xQueueCreate(ble_freertos_total_event_cnt, sizeof(struct ble_npl_eventq *));
        BLE_LL_ASSERT(eventq->q);
    } else {
        eventq = (struct ble_npl_eventq_freertos*)evq->eventq;
        xQueueReset(eventq->q);
    }
#endif
}

void
npl_freertos_eventq_deinit(struct ble_npl_eventq *evq)
{
    struct ble_npl_eventq_freertos *eventq = (struct ble_npl_eventq_freertos *)evq->eventq;

    BLE_LL_ASSERT(eventq);
    vQueueDelete(eventq->q);
#if OS_MEM_ALLOC
    os_memblock_put(&ble_freertos_evq_pool,eventq);
#else
    bt_osi_mem_free((void *)eventq);
#endif
    evq->eventq = NULL;
}

void
IRAM_ATTR npl_freertos_callout_mem_reset(struct ble_npl_callout *co)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;

    BLE_LL_ASSERT(callout);
    BLE_LL_ASSERT(callout->handle);

    ble_npl_event_reset(&callout->ev);
}

static inline bool
IRAM_ATTR in_isr(void)
{
    /* XXX hw specific! */
    return xPortInIsrContext() != 0;
}

struct ble_npl_event *
IRAM_ATTR npl_freertos_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    struct ble_npl_event *ev = NULL;
    struct ble_npl_eventq_freertos *eventq = (struct ble_npl_eventq_freertos *)evq->eventq;
    BaseType_t woken;
    BaseType_t ret;

    if (in_isr()) {
        BLE_LL_ASSERT(tmo == 0);
        ret = xQueueReceiveFromISR(eventq->q, &ev, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xQueueReceive(eventq->q, &ev, tmo);
    }
    BLE_LL_ASSERT(ret == pdPASS || ret == errQUEUE_EMPTY);

    if (ev) {
	struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;
	if (event) {
            event->queued = false;
	}
    }

    return ev;
}

void
IRAM_ATTR npl_freertos_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    BaseType_t woken;
    BaseType_t ret;
    struct ble_npl_eventq_freertos *eventq = (struct ble_npl_eventq_freertos *)evq->eventq;
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;

    if (event->queued) {
        return;
    }

    event->queued = true;

    if (in_isr()) {
        ret = xQueueSendToBackFromISR(eventq->q, &ev, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xQueueSendToBack(eventq->q, &ev, portMAX_DELAY);
    }

    BLE_LL_ASSERT(ret == pdPASS);
}

void
IRAM_ATTR npl_freertos_eventq_put_to_front(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    BaseType_t woken;
    BaseType_t ret;
    struct ble_npl_eventq_freertos *eventq = (struct ble_npl_eventq_freertos *)evq->eventq;
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;

    if (event->queued) {
        return;
    }

    event->queued = true;

    if (in_isr()) {
        ret = xQueueSendToFrontFromISR(eventq->q, &ev, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xQueueSendToFront(eventq->q, &ev, portMAX_DELAY);
    }

    BLE_LL_ASSERT(ret == pdPASS);
}

void
IRAM_ATTR npl_freertos_eventq_remove(struct ble_npl_eventq *evq,
                       struct ble_npl_event *ev)
{
    struct ble_npl_event *tmp_ev;
    BaseType_t ret;
    int i;
    int count;
    BaseType_t woken, woken2;
    struct ble_npl_eventq_freertos *eventq = (struct ble_npl_eventq_freertos *)evq->eventq;
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;

    if (!event->queued) {
        return;
    }

    /*
     * XXX We cannot extract element from inside FreeRTOS queue so as a quick
     * workaround we'll just remove all elements and add them back except the
     * one we need to remove. This is silly, but works for now - we probably
     * better use counting semaphore with os_queue to handle this in future.
     */

    if (in_isr()) {
        woken = pdFALSE;

        count = uxQueueMessagesWaitingFromISR(eventq->q);
        for (i = 0; i < count; i++) {
            ret = xQueueReceiveFromISR(eventq->q, &tmp_ev, &woken2);
            BLE_LL_ASSERT(ret == pdPASS);
            woken |= woken2;

            if (tmp_ev == ev) {
                continue;
            }

            ret = xQueueSendToBackFromISR(eventq->q, &tmp_ev, &woken2);
            BLE_LL_ASSERT(ret == pdPASS);
            woken |= woken2;
        }

        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        portMUX_TYPE ble_npl_mut = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL(&ble_npl_mut);

        count = uxQueueMessagesWaiting(eventq->q);
        for (i = 0; i < count; i++) {
            ret = xQueueReceive(eventq->q, &tmp_ev, 0);
            BLE_LL_ASSERT(ret == pdPASS);

            if (tmp_ev == ev) {
                continue;
            }

            ret = xQueueSendToBack(eventq->q, &tmp_ev, 0);
            BLE_LL_ASSERT(ret == pdPASS);
        }

        portEXIT_CRITICAL(&ble_npl_mut);
    }

    event->queued = 0;
}

ble_npl_error_t
npl_freertos_mutex_init(struct ble_npl_mutex *mu)
{
    struct ble_npl_mutex_freertos *mutex = NULL;
#if OS_MEM_ALLOC
    if (!os_memblock_from(&ble_freertos_mutex_pool,mu->mutex)) {
        mu->mutex = os_memblock_get(&ble_freertos_mutex_pool);
        mutex = (struct ble_npl_mutex_freertos *)mu->mutex;

        if (!mutex) {
            return BLE_NPL_INVALID_PARAM;
        }

        memset(mutex, 0, sizeof(*mutex));
        mutex->handle = xSemaphoreCreateRecursiveMutex();
        BLE_LL_ASSERT(mutex->handle);
    }
#else
    if(!mu->mutex) {
        mu->mutex = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_mutex_freertos));
        mutex = (struct ble_npl_mutex_freertos *)mu->mutex;

        if (!mutex) {
            return BLE_NPL_INVALID_PARAM;
        }

        memset(mutex, 0, sizeof(*mutex));
        mutex->handle = xSemaphoreCreateRecursiveMutex();
        BLE_LL_ASSERT(mutex->handle);
    }
#endif

    return BLE_NPL_OK;
}

ble_npl_error_t
npl_freertos_mutex_deinit(struct ble_npl_mutex *mu)
{
    struct ble_npl_mutex_freertos *mutex = (struct ble_npl_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BLE_NPL_INVALID_PARAM;
    }

    BLE_LL_ASSERT(mutex->handle);
    vSemaphoreDelete(mutex->handle);

#if OS_MEM_ALLOC
    os_memblock_put(&ble_freertos_mutex_pool,mutex);
#else
    bt_osi_mem_free((void *)mutex);
#endif
    mu->mutex = NULL;

    return BLE_NPL_OK;
}

void
IRAM_ATTR npl_freertos_event_run(struct ble_npl_event *ev)
{
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;
    event->fn(ev);
}

bool
IRAM_ATTR npl_freertos_eventq_is_empty(struct ble_npl_eventq *evq)
{
    struct ble_npl_eventq_freertos *eventq = (struct ble_npl_eventq_freertos *)evq->eventq;
    return xQueueIsQueueEmptyFromISR(eventq->q);
}

bool
IRAM_ATTR npl_freertos_event_is_queued(struct ble_npl_event *ev)
{
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;
    return event->queued;
}

void *
IRAM_ATTR npl_freertos_event_get_arg(struct ble_npl_event *ev)
{
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;
    return event->arg;
}

void
IRAM_ATTR npl_freertos_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)ev->event;
    event->arg = arg;
}


ble_npl_error_t
IRAM_ATTR npl_freertos_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    BaseType_t ret;
    struct ble_npl_mutex_freertos *mutex = (struct ble_npl_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BLE_NPL_INVALID_PARAM;
    }

    BLE_LL_ASSERT(mutex->handle);

    if (in_isr()) {
        ret = pdFAIL;
        BLE_LL_ASSERT(0);
    } else {
        ret = xSemaphoreTakeRecursive(mutex->handle, timeout);
    }

    return ret == pdPASS ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t
IRAM_ATTR npl_freertos_mutex_release(struct ble_npl_mutex *mu)
{
    struct ble_npl_mutex_freertos *mutex = (struct ble_npl_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BLE_NPL_INVALID_PARAM;
    }

    BLE_LL_ASSERT(mutex->handle);

    if (in_isr()) {
        BLE_LL_ASSERT(0);
    } else {
        if (xSemaphoreGiveRecursive(mutex->handle) != pdPASS) {
            return BLE_NPL_BAD_MUTEX;
        }
    }

    return BLE_NPL_OK;
}

ble_npl_error_t
npl_freertos_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    struct ble_npl_sem_freertos *semaphore = NULL;
#if OS_MEM_ALLOC
    if (!os_memblock_from(&ble_freertos_sem_pool,sem->sem)) {
        sem->sem = os_memblock_get(&ble_freertos_sem_pool);
        semaphore = (struct ble_npl_sem_freertos *)sem->sem;

        if (!semaphore) {
            return BLE_NPL_INVALID_PARAM;
        }

        memset(semaphore, 0, sizeof(*semaphore));
        semaphore->handle = xSemaphoreCreateCounting(128, tokens);
        BLE_LL_ASSERT(semaphore->handle);
    }
#else
    if(!sem->sem) {
        sem->sem = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_sem_freertos));
        semaphore = (struct ble_npl_sem_freertos *)sem->sem;

        if (!semaphore) {
            return BLE_NPL_INVALID_PARAM;
        }

        memset(semaphore, 0, sizeof(*semaphore));
        semaphore->handle = xSemaphoreCreateCounting(128, tokens);
        BLE_LL_ASSERT(semaphore->handle);
    }
#endif

    return BLE_NPL_OK;
}

ble_npl_error_t
npl_freertos_sem_deinit(struct ble_npl_sem *sem)
{
    struct ble_npl_sem_freertos *semaphore = (struct ble_npl_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BLE_NPL_INVALID_PARAM;
    }

    BLE_LL_ASSERT(semaphore->handle);
    vSemaphoreDelete(semaphore->handle);

#if OS_MEM_ALLOC
    os_memblock_put(&ble_freertos_sem_pool,semaphore);
#else
    bt_osi_mem_free((void *)semaphore);
#endif
    sem->sem = NULL;

    return BLE_NPL_OK;
}

ble_npl_error_t
IRAM_ATTR npl_freertos_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    BaseType_t woken;
    BaseType_t ret;
    struct ble_npl_sem_freertos *semaphore = (struct ble_npl_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BLE_NPL_INVALID_PARAM;
    }

    BLE_LL_ASSERT(semaphore->handle);

    if (in_isr()) {
        BLE_LL_ASSERT(timeout == 0);
        ret = xSemaphoreTakeFromISR(semaphore->handle, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xSemaphoreTake(semaphore->handle, timeout);
    }

    return ret == pdPASS ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t
IRAM_ATTR npl_freertos_sem_release(struct ble_npl_sem *sem)
{
    BaseType_t ret;
    BaseType_t woken;
    struct ble_npl_sem_freertos *semaphore = (struct ble_npl_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BLE_NPL_INVALID_PARAM;
    }

    BLE_LL_ASSERT(semaphore->handle);

    if (in_isr()) {
        ret = xSemaphoreGiveFromISR(semaphore->handle, &woken);
        if( woken == pdTRUE ) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xSemaphoreGive(semaphore->handle);
    }

    BLE_LL_ASSERT(ret == pdPASS);
    return BLE_NPL_OK;
}

#if BLE_NPL_USE_ESP_TIMER
static void
IRAM_ATTR ble_npl_event_fn_wrapper(void *arg)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)arg;
    BLE_LL_ASSERT(callout);

    if (callout->evq) {
        ble_npl_eventq_put(callout->evq, &callout->ev);
    } else {
        struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)callout->ev.event;
        event->fn(&callout->ev);
    }
}

static
IRAM_ATTR ble_npl_error_t esp_err_to_npl_error(esp_err_t err)
{
    switch(err) {
    case ESP_ERR_INVALID_ARG:
        return BLE_NPL_INVALID_PARAM;

    case ESP_ERR_INVALID_STATE:
        return BLE_NPL_EINVAL;

    case ESP_OK:
        return BLE_NPL_OK;

   default:
        return BLE_NPL_ERROR;
    }
}
#else

static void
IRAM_ATTR os_callout_timer_cb(TimerHandle_t timer)
{
    struct ble_npl_callout_freertos *callout;

    callout = pvTimerGetTimerID(timer);
    BLE_LL_ASSERT(callout);

    if (callout->evq) {
        ble_npl_eventq_put(callout->evq, &callout->ev);
    } else {
        struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)callout->ev.event;
        event->fn(&callout->ev);
    }
}
#endif

int
npl_freertos_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                      ble_npl_event_fn *ev_cb, void *ev_arg)
{
    struct ble_npl_callout_freertos *callout = NULL;

#if OS_MEM_ALLOC
    if (!os_memblock_from(&ble_freertos_co_pool, co->co)) {
        co->co = os_memblock_get(&ble_freertos_co_pool);
        callout = (struct ble_npl_callout_freertos *)co->co;
        BLE_LL_ASSERT(callout);

        memset(callout, 0, sizeof(*callout));
        ble_npl_event_init(&callout->ev, ev_cb, ev_arg);

#if BLE_NPL_USE_ESP_TIMER
        callout->evq = evq;

        esp_timer_create_args_t create_args = {
            .callback = ble_npl_event_fn_wrapper,
            .arg = callout,
            .name = "nimble_timer"
        };

        if (esp_timer_create(&create_args, &callout->handle) != ESP_OK) {
            ble_npl_event_deinit(&callout->ev);
            os_memblock_put(&ble_freertos_co_pool,callout);
            co->co = NULL;
            return -1;
        }
#else
        callout->handle = xTimerCreate("co", 1, pdFALSE, callout, os_callout_timer_cb);

        if (!callout->handle) {
            ble_npl_event_deinit(&callout->ev);
            os_memblock_put(&ble_freertos_co_pool,callout);
            co->co = NULL;
            return -1;
        }
#endif // BLE_NPL_USE_ESP_TIMER
    } else {
        callout = (struct ble_npl_callout_freertos *)co->co;
        BLE_LL_ASSERT(callout);
        callout->evq = evq;
        ble_npl_event_init(&callout->ev, ev_cb, ev_arg);
    }
#else

    if(!co->co) {
        co->co = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_callout_freertos));
        callout = (struct ble_npl_callout_freertos *)co->co;
        if (!callout) {
            return -1;
        }

        memset(callout, 0, sizeof(*callout));
        ble_npl_event_init(&callout->ev, ev_cb, ev_arg);

#if BLE_NPL_USE_ESP_TIMER
        callout->evq = evq;

        esp_timer_create_args_t create_args = {
            .callback = ble_npl_event_fn_wrapper,
            .arg = callout,
            .name = "nimble_timer"
        };

        if (esp_timer_create(&create_args, &callout->handle) != ESP_OK) {
            ble_npl_event_deinit(&callout->ev);
            bt_osi_mem_free((void *)callout);
            co->co = NULL;
            return -1;
        }
#else
        callout->handle = xTimerCreate("co", 1, pdFALSE, callout, os_callout_timer_cb);

        if (!callout->handle) {
            ble_npl_event_deinit(&callout->ev);
            bt_osi_mem_free((void *)callout);
            co->co = NULL;
            return -1;
        }
#endif // BLE_NPL_USE_ESP_TIMER
    }
    else {
        callout = (struct ble_npl_callout_freertos *)co->co;
        BLE_LL_ASSERT(callout);
        callout->evq = evq;
        ble_npl_event_init(&callout->ev, ev_cb, ev_arg);
    }
#endif // OS_MEM_ALLOC
    return 0;
}

void
npl_freertos_callout_deinit(struct ble_npl_callout *co)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;

    /* Since we dynamically deinit timers, function can be called for NULL timers. Return for such scenarios */
    if (!callout) {
        return;
    }

    if (!callout->handle) {
        return;
    }

    ble_npl_event_deinit(&callout->ev);
#if BLE_NPL_USE_ESP_TIMER
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
    xTimerDelete(callout->handle, portMAX_DELAY);
#endif // BLE_NPL_USE_ESP_TIMER
#if OS_MEM_ALLOC
    os_memblock_put(&ble_freertos_co_pool,callout);
#else
    bt_osi_mem_free((void *)callout);
#endif // OS_MEM_ALLOC
    co->co = NULL;
    memset(co, 0, sizeof(struct ble_npl_callout));
}

uint16_t
IRAM_ATTR npl_freertos_sem_get_count(struct ble_npl_sem *sem)
{
    struct ble_npl_sem_freertos *semaphore = (struct ble_npl_sem_freertos *)sem->sem;
    return uxSemaphoreGetCount(semaphore->handle);
}


ble_npl_error_t
IRAM_ATTR npl_freertos_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;
#if BLE_NPL_USE_ESP_TIMER
    esp_timer_stop(callout->handle);

    return esp_err_to_npl_error(esp_timer_start_once(callout->handle, ticks*1000));
#else

    BaseType_t woken1, woken2, woken3;

    if (ticks == 0) {
        ticks = 1;
    }
    if (in_isr()) {
        xTimerStopFromISR(callout->handle, &woken1);
        xTimerChangePeriodFromISR(callout->handle, ticks, &woken2);
        xTimerResetFromISR(callout->handle, &woken3);

        if( woken1 == pdTRUE || woken2 == pdTRUE || woken3 == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        xTimerStop(callout->handle, portMAX_DELAY);
        xTimerChangePeriod(callout->handle, ticks, portMAX_DELAY);
        xTimerReset(callout->handle, portMAX_DELAY);
    }

    return BLE_NPL_OK;
#endif
}

void
IRAM_ATTR npl_freertos_callout_stop(struct ble_npl_callout *co)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;

    if (!callout) {
	return;
    }

#if BLE_NPL_USE_ESP_TIMER
    esp_timer_stop(callout->handle);
#else
    xTimerStop(callout->handle, portMAX_DELAY);
#endif
}

bool
IRAM_ATTR npl_freertos_callout_is_active(struct ble_npl_callout *co)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;
#if BLE_NPL_USE_ESP_TIMER
    return esp_timer_is_active(callout->handle);
#else
    return xTimerIsTimerActive(callout->handle) == pdTRUE;
#endif
}

ble_npl_time_t
IRAM_ATTR npl_freertos_callout_get_ticks(struct ble_npl_callout *co)
{
#if BLE_NPL_USE_ESP_TIMER

     uint32_t exp = 0;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
     uint64_t expiry = 0;
     esp_err_t err;

     struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;

     //Fetch expiry time in microseconds
     err = esp_timer_get_expiry_time((esp_timer_handle_t)(callout->handle), &expiry);
     if (err != ESP_OK) {
         //Error. Could not fetch the expiry time
         return 0;
     }

     //Convert microseconds to ticks
     npl_freertos_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
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
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;
    return xTimerGetExpiryTime(callout->handle);
#endif
}

ble_npl_time_t
IRAM_ATTR npl_freertos_callout_remaining_ticks(struct ble_npl_callout *co,
                                     ble_npl_time_t now)
{
    ble_npl_time_t rt;
    uint32_t exp = 0;

    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;

#if BLE_NPL_USE_ESP_TIMER
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
    npl_freertos_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
#else
    //esp_timer_get_expiry_time() is only available from IDF 5.0 onwards
    //Set expiry to 0
    exp = 0;
#endif //ESP_IDF_VERSION
#else
    exp = xTimerGetExpiryTime(callout->handle);
#endif

    if (exp > now) {
        rt = exp - now;
    } else {
        rt = 0;
    }

    return rt;
}

void
IRAM_ATTR npl_freertos_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
    struct ble_npl_callout_freertos *callout = (struct ble_npl_callout_freertos *)co->co;
    struct ble_npl_event_freertos *event = (struct ble_npl_event_freertos *)callout->ev.event;
    event->arg = arg;
}

uint32_t
IRAM_ATTR npl_freertos_time_get(void)
{
#if BLE_NPL_USE_ESP_TIMER
    return esp_timer_get_time() / 1000;
#else
    return xTaskGetTickCountFromISR();
#endif
}

ble_npl_error_t
IRAM_ATTR npl_freertos_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    uint64_t ticks;
#if BLE_NPL_USE_ESP_TIMER
    ticks = (uint64_t)ms;
#else
    ticks = ((uint64_t)ms * configTICK_RATE_HZ) / 1000;
#endif
    if (ticks > UINT32_MAX) {
        return BLE_NPL_EINVAL;
    }

    *out_ticks = ticks;

    return 0;
}

ble_npl_error_t
IRAM_ATTR npl_freertos_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    uint64_t ms;
#if BLE_NPL_USE_ESP_TIMER
    ms = ((uint64_t)ticks);
#else
    ms = ((uint64_t)ticks * 1000) / configTICK_RATE_HZ;
#endif
    if (ms > UINT32_MAX) {
        return BLE_NPL_EINVAL;
     }

    *out_ms = ms;

    return 0;
}

ble_npl_time_t
IRAM_ATTR npl_freertos_time_ms_to_ticks32(uint32_t ms)
{
#if BLE_NPL_USE_ESP_TIMER
    return ms;
#else
    return ms * configTICK_RATE_HZ / 1000;
#endif
}

uint32_t
IRAM_ATTR npl_freertos_time_ticks_to_ms32(ble_npl_time_t ticks)
{
#if BLE_NPL_USE_ESP_TIMER
    return ticks;
#else
    return ticks * 1000 / configTICK_RATE_HZ;
#endif
}

void
IRAM_ATTR npl_freertos_time_delay(ble_npl_time_t ticks)
{
#if BLE_NPL_USE_ESP_TIMER
    vTaskDelay(ticks / portTICK_PERIOD_MS);
#else
    vTaskDelay(ticks);
#endif
}


uint8_t hw_critical_state_status = 0;

uint32_t
IRAM_ATTR npl_freertos_hw_enter_critical(void)
{
    ++hw_critical_state_status;
    portENTER_CRITICAL(&ble_port_mutex);
    return 0;
}

uint8_t
IRAM_ATTR npl_freertos_hw_is_in_critical(void)
{
    return hw_critical_state_status;
}

void
IRAM_ATTR npl_freertos_hw_exit_critical(uint32_t ctx)
{
    --hw_critical_state_status;
    portEXIT_CRITICAL(&ble_port_mutex);

}

uint32_t
IRAM_ATTR npl_freertos_get_time_forever(void)
{
    return portMAX_DELAY;
}

const struct npl_funcs_t npl_funcs_ro = {
    .p_ble_npl_os_started = npl_freertos_os_started,
    .p_ble_npl_get_current_task_id = npl_freertos_get_current_task_id,
    .p_ble_npl_eventq_init = npl_freertos_eventq_init,
    .p_ble_npl_eventq_deinit = npl_freertos_eventq_deinit,
    .p_ble_npl_eventq_get = npl_freertos_eventq_get,
    .p_ble_npl_eventq_put = npl_freertos_eventq_put,
    .p_ble_npl_eventq_put_to_front = npl_freertos_eventq_put_to_front,
    .p_ble_npl_eventq_remove = npl_freertos_eventq_remove,
    .p_ble_npl_event_run = npl_freertos_event_run,
    .p_ble_npl_eventq_is_empty = npl_freertos_eventq_is_empty,
    .p_ble_npl_event_init = npl_freertos_event_init,
    .p_ble_npl_event_deinit = npl_freertos_event_deinit,
    .p_ble_npl_event_reset = npl_freertos_event_reset,
    .p_ble_npl_event_is_queued = npl_freertos_event_is_queued,
    .p_ble_npl_event_get_arg = npl_freertos_event_get_arg,
    .p_ble_npl_event_set_arg = npl_freertos_event_set_arg,
    .p_ble_npl_mutex_init = npl_freertos_mutex_init,
    .p_ble_npl_mutex_deinit = npl_freertos_mutex_deinit,
    .p_ble_npl_mutex_pend = npl_freertos_mutex_pend,
    .p_ble_npl_mutex_release = npl_freertos_mutex_release,
    .p_ble_npl_sem_init = npl_freertos_sem_init,
    .p_ble_npl_sem_deinit = npl_freertos_sem_deinit,
    .p_ble_npl_sem_pend = npl_freertos_sem_pend,
    .p_ble_npl_sem_release = npl_freertos_sem_release,
    .p_ble_npl_sem_get_count = npl_freertos_sem_get_count,
    .p_ble_npl_callout_init = npl_freertos_callout_init,
    .p_ble_npl_callout_reset = npl_freertos_callout_reset,
    .p_ble_npl_callout_stop = npl_freertos_callout_stop,
    .p_ble_npl_callout_deinit = npl_freertos_callout_deinit,
    .p_ble_npl_callout_mem_reset = npl_freertos_callout_mem_reset,
    .p_ble_npl_callout_is_active = npl_freertos_callout_is_active,
    .p_ble_npl_callout_get_ticks = npl_freertos_callout_get_ticks,
    .p_ble_npl_callout_remaining_ticks = npl_freertos_callout_remaining_ticks,
    .p_ble_npl_callout_set_arg = npl_freertos_callout_set_arg,
    .p_ble_npl_time_get = npl_freertos_time_get,
    .p_ble_npl_time_ms_to_ticks = npl_freertos_time_ms_to_ticks,
    .p_ble_npl_time_ticks_to_ms = npl_freertos_time_ticks_to_ms,
    .p_ble_npl_time_ms_to_ticks32 = npl_freertos_time_ms_to_ticks32,
    .p_ble_npl_time_ticks_to_ms32 = npl_freertos_time_ticks_to_ms32,
    .p_ble_npl_time_delay = npl_freertos_time_delay,
#if NIMBLE_CFG_CONTROLLER || CONFIG_NIMBLE_CONTROLLER_MODE
    .p_ble_npl_hw_set_isr = NULL,
#endif
    .p_ble_npl_hw_enter_critical = npl_freertos_hw_enter_critical,
    .p_ble_npl_hw_exit_critical = npl_freertos_hw_exit_critical,
    .p_ble_npl_get_time_forever = npl_freertos_get_time_forever,
    .p_ble_npl_hw_is_in_critical = npl_freertos_hw_is_in_critical
};

struct npl_funcs_t *npl_funcs = NULL;

struct npl_funcs_t * npl_freertos_funcs_get(void)
{
    return npl_funcs;
}

void npl_freertos_funcs_init(void)
{
    npl_funcs = (struct npl_funcs_t *)bt_osi_mem_malloc_internal(sizeof(struct npl_funcs_t));
    if(!npl_funcs) {
        printf("npl funcs init failed\n");
        assert(0);
    }
    memcpy(npl_funcs, &npl_funcs_ro, sizeof(struct npl_funcs_t));
}

int npl_freertos_set_controller_npl_info(ble_npl_count_info_t *ctrl_npl_info)
{
    if (!ctrl_npl_info) {
        return -1;
    }

    memcpy(&g_ctrl_npl_info, ctrl_npl_info, sizeof(ble_npl_count_info_t));
    return 0;
}

int npl_freertos_mempool_init(void)
{
    int rc = -1;
    uint16_t ble_total_evt_count = 0;
    uint16_t ble_total_co_count = 0;
    uint16_t ble_total_evtq_count = 0;
    uint16_t ble_total_sem_count = 0;
    uint16_t ble_total_mutex_count = 0;
    ble_total_evt_count = g_ctrl_npl_info.evt_count + BLE_HOST_EV_COUNT;
    ble_total_evtq_count = g_ctrl_npl_info.evtq_count + BLE_HOST_EVQ_COUNT;
    ble_total_co_count = g_ctrl_npl_info.co_count + BLE_HOST_CO_COUNT;
    ble_total_sem_count = g_ctrl_npl_info.sem_count + BLE_HOST_SEM_COUNT;
    ble_total_mutex_count = g_ctrl_npl_info.mutex_count + BLE_HOST_MUTEX_COUNT;
    ble_freertos_total_event_cnt = ble_total_evt_count;

    if (ble_total_evt_count) {
        ble_freertos_ev_buf  = bt_osi_mem_malloc_internal(OS_MEMPOOL_SIZE(ble_total_evt_count,
                                      sizeof (struct ble_npl_event_freertos)) *
                                      sizeof(os_membuf_t));
        if (!ble_freertos_ev_buf) {
            goto _error;
        }
        rc = os_mempool_init(&ble_freertos_ev_pool, ble_total_evt_count,
                             sizeof (struct ble_npl_event_freertos), ble_freertos_ev_buf,
                             "ble_freertos_ev_pool");
        if (rc) {
            goto _error;
        }
    }

    if (ble_total_evtq_count) {
        ble_freertos_evq_buf  = bt_osi_mem_malloc_internal(OS_MEMPOOL_SIZE(ble_total_evtq_count,
                                       sizeof (struct ble_npl_eventq_freertos)) *
                                       sizeof(os_membuf_t));
        if (!ble_freertos_evq_buf) {
            goto _error;
        }
        rc = os_mempool_init(&ble_freertos_evq_pool, ble_total_evtq_count,
                             sizeof (struct ble_npl_eventq_freertos), ble_freertos_evq_buf,
                             "ble_freertos_evq_pool");
        if (rc) {
            goto _error;
        }
    }

    if (ble_total_co_count) {
        ble_freertos_co_buf  = bt_osi_mem_malloc_internal(OS_MEMPOOL_SIZE(ble_total_co_count,
                                      sizeof (struct ble_npl_callout_freertos)) *
                                      sizeof(os_membuf_t));
        if (!ble_freertos_co_buf) {
            goto _error;
        }
        rc = os_mempool_init(&ble_freertos_co_pool, ble_total_co_count,
                             sizeof (struct ble_npl_callout_freertos), ble_freertos_co_buf,
                             "ble_freertos_co_pool");
        if (rc) {
            goto _error;
        }
    }

    if (ble_total_sem_count) {
        ble_freertos_sem_buf  = bt_osi_mem_malloc_internal(OS_MEMPOOL_SIZE(ble_total_sem_count,
                                       sizeof (struct ble_npl_sem_freertos)) *
                                       sizeof(os_membuf_t));
        if (!ble_freertos_sem_buf) {
            goto _error;
        }
        rc = os_mempool_init(&ble_freertos_sem_pool, ble_total_sem_count,
                             sizeof (struct ble_npl_sem_freertos), ble_freertos_sem_buf,
                             "ble_freertos_sem_pool");
        if (rc) {
            goto _error;
        }
    }

    if (ble_total_mutex_count) {
        ble_freertos_mutex_buf  = bt_osi_mem_malloc_internal(OS_MEMPOOL_SIZE(ble_total_mutex_count,
                                         sizeof (struct ble_npl_mutex_freertos)) *
                                         sizeof(os_membuf_t));
        if (!ble_freertos_mutex_buf) {
            goto _error;
        }
        rc = os_mempool_init(&ble_freertos_mutex_pool, ble_total_mutex_count,
                             sizeof (struct ble_npl_mutex_freertos), ble_freertos_mutex_buf,
                             "ble_freertos_mutex_pool");
        if (rc) {
            goto _error;
        }
    }

    return 0;
_error:
    if (ble_freertos_ev_buf) {
        bt_osi_mem_free(ble_freertos_ev_buf);
        ble_freertos_ev_buf = NULL;
    }

    if (ble_freertos_evq_buf) {
        bt_osi_mem_free(ble_freertos_evq_buf);
        ble_freertos_evq_buf = NULL;
    }

    if (ble_freertos_co_buf) {
        bt_osi_mem_free(ble_freertos_co_buf);
        ble_freertos_co_buf = NULL;
    }

    if (ble_freertos_sem_buf) {
        bt_osi_mem_free(ble_freertos_sem_buf);
        ble_freertos_sem_buf = NULL;
    }

    if (ble_freertos_mutex_buf) {
        bt_osi_mem_free(ble_freertos_mutex_buf);
        ble_freertos_mutex_buf = NULL;
    }
    return -1;
}

void npl_freertos_mempool_deinit(void)
{
    if (ble_freertos_ev_buf) {
        bt_osi_mem_free(ble_freertos_ev_buf);
        ble_freertos_ev_buf = NULL;
    }
    if (ble_freertos_evq_buf) {
        bt_osi_mem_free(ble_freertos_evq_buf);
        ble_freertos_evq_buf = NULL;
    }
    if (ble_freertos_co_buf) {
        bt_osi_mem_free(ble_freertos_co_buf);
        ble_freertos_co_buf = NULL;
    }
    if (ble_freertos_sem_buf) {
        bt_osi_mem_free(ble_freertos_sem_buf);
        ble_freertos_sem_buf = NULL;
    }
    if (ble_freertos_mutex_buf) {
        bt_osi_mem_free(ble_freertos_mutex_buf);
        ble_freertos_mutex_buf = NULL;
    }
}

void npl_freertos_funcs_deinit(void)
{
    if (npl_funcs) {
        bt_osi_mem_free(npl_funcs);
    }
    npl_funcs = NULL;
}
