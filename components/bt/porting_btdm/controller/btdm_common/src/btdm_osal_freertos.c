/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "btdm_osal_freertos.h"
#include "btdm_mempool.h"

#include "esp_mac.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_ipc.h"
#include "esp_random.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

extern int esp_rom_printf(const char *fmt, ...);
#define BTDM_OSAL_ASSERT(con)                                                                      \
    do {                                                                                           \
        if (!(con)) {                                                                              \
            esp_rom_printf("assertion:%s\n", #con);                                                \
            esp_rom_printf("line:%d,function:%s\n", __LINE__, __func__);                           \
            assert(0);                                                                             \
        }                                                                                          \
    } while (0)

#define BTDM_MEMPOOL_ALLOC (1)

#if (!defined(SOC_ESP_NIMBLE_CONTROLLER) || !SOC_ESP_NIMBLE_CONTROLLER)
#error "not defined SOC_ESP_NIMBLE_CONTROLLER or SOC_ESP_NIMBLE_CONTROLLER is zero"
#endif

struct btdm_intr_alloc_params {
    union {
        struct {
            int source;
            int flags;
            intr_handler_t handler;
            void *arg;
            intr_handle_t *ret_handle_in;
        } cp;
        int rsp;
    };
};

static const char *TAG = "TBTDM OSAL";

struct btdm_mempool s_btdm_osal_ev_pool;
static btdm_membuf_t *s_btdm_osal_ev_buf = NULL;

struct btdm_mempool s_btdm_osal_evq_pool;
static btdm_membuf_t *s_btdm_osal_evq_buf = NULL;

struct btdm_mempool s_btdm_osal_co_pool;
static btdm_membuf_t *s_btdm_osal_co_buf = NULL;

struct btdm_mempool s_btdm_osal_sem_pool;
static btdm_membuf_t *s_btdm_osal_sem_buf = NULL;

struct btdm_mempool s_btdm_osal_mutex_pool;
static btdm_membuf_t *s_btdm_osal_mutex_buf = NULL;

static uint16_t s_btdm_osal_evtq_deepth = 0;
static uint8_t s_btdm_osal_intr_nest = 0;
static portMUX_TYPE s_btsm_osal_intr_mutex = portMUX_INITIALIZER_UNLOCKED;

/*
 ***************************************************************************************************
 * Static Functions
 ***************************************************************************************************
 */
static inline bool IRAM_ATTR
in_isr(void)
{
    /* XXX hw specific! */
    return xPortInIsrContext() != 0;
}

/*
 ***************************************************************************************************
 * BTDM OSAL Event Queue
 ***************************************************************************************************
 */
void
wr_btdm_osal_eventq_init(struct btdm_osal_eventq *evq)
{
    struct btdm_osal_eventq_freertos *eventq = NULL;

#if BTDM_MEMPOOL_ALLOC
    if (!btdm_memblock_from(&s_btdm_osal_evq_pool, evq->eventq)) {
        evq->eventq = btdm_memblock_get(&s_btdm_osal_evq_pool);
        eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
        BTDM_OSAL_ASSERT(eventq);
        memset(eventq, 0, sizeof(*eventq));
        eventq->q = xQueueCreate(s_btdm_osal_evtq_deepth, sizeof(struct btdm_osal_eventq *));
        BTDM_OSAL_ASSERT(eventq->q);
    } else {
        eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
        xQueueReset(eventq->q);
    }
#else
    if (!evq->eventq) {
        evq->eventq = btdm_osal_malloc(sizeof(struct btdm_osal_eventq_freertos),
                                       BTDM_OSAL_MALLOC_F_INTERNAL);
        eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
        BTDM_OSAL_ASSERT(eventq);
        memset(eventq, 0, sizeof(*eventq));
        eventq->q = xQueueCreate(s_btdm_osal_evtq_deepth, sizeof(struct btdm_osal_eventq *));
        BTDM_OSAL_ASSERT(eventq->q);
    } else {
        eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
        xQueueReset(eventq->q);
    }
#endif
}

void
wr_btdm_osal_eventq_deinit(struct btdm_osal_eventq *evq)
{
    struct btdm_osal_eventq_freertos *eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;

    BTDM_OSAL_ASSERT(eventq);
    vQueueDelete(eventq->q);
#if BTDM_MEMPOOL_ALLOC
    btdm_memblock_put(&s_btdm_osal_evq_pool, eventq);
#else
    btdm_osal_free((void *)eventq);
#endif
    evq->eventq = NULL;
}

struct btdm_osal_event *IRAM_ATTR
wr_btdm_osal_eventq_get(struct btdm_osal_eventq *evq, btdm_osal_time_t tmo)
{
    struct btdm_osal_event *ev = NULL;
    struct btdm_osal_eventq_freertos *eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
    BaseType_t woken;
    BaseType_t ret;

    if (in_isr()) {
        BTDM_OSAL_ASSERT(tmo == 0);
        ret = xQueueReceiveFromISR(eventq->q, &ev, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xQueueReceive(eventq->q, &ev, tmo);
    }
    BTDM_OSAL_ASSERT(ret == pdPASS || ret == errQUEUE_EMPTY);

    if (ev) {
        struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;
        if (event) {
            event->queued = false;
        }
    }

    return ev;
}

void IRAM_ATTR
wr_btdm_osal_eventq_put(struct btdm_osal_eventq *evq, struct btdm_osal_event *ev)
{
    BaseType_t woken;
    BaseType_t ret;
    struct btdm_osal_eventq_freertos *eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;

    if (event->queued) {
        return;
    }

    event->queued = true;

    if (in_isr()) {
        ret = xQueueSendToBackFromISR(eventq->q, &ev, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xQueueSendToBack(eventq->q, &ev, portMAX_DELAY);
    }

    BTDM_OSAL_ASSERT(ret == pdPASS);
}

void IRAM_ATTR
wr_btdm_osal_eventq_put_to_front(struct btdm_osal_eventq *evq, struct btdm_osal_event *ev)
{
    BaseType_t woken;
    BaseType_t ret;
    struct btdm_osal_eventq_freertos *eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;

    if (event->queued) {
        return;
    }

    event->queued = true;

    if (in_isr()) {
        ret = xQueueSendToFrontFromISR(eventq->q, &ev, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xQueueSendToFront(eventq->q, &ev, portMAX_DELAY);
    }

    BTDM_OSAL_ASSERT(ret == pdPASS);
}

void IRAM_ATTR
wr_btdm_osal_eventq_remove(struct btdm_osal_eventq *evq, struct btdm_osal_event *ev)
{
    struct btdm_osal_event *tmp_ev;
    BaseType_t ret;
    int i;
    int count;
    BaseType_t woken, woken2;
    struct btdm_osal_eventq_freertos *eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;

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
            BTDM_OSAL_ASSERT(ret == pdPASS);
            woken |= woken2;

            if (tmp_ev == ev) {
                continue;
            }

            ret = xQueueSendToBackFromISR(eventq->q, &tmp_ev, &woken2);
            BTDM_OSAL_ASSERT(ret == pdPASS);
            woken |= woken2;
        }

        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        /* RISK: For multi-core, this mutex can't protect the queue from being changed by other cores
         * with wr_btdm_osal_eventq_put(). The count maybe less than the actual.
         */
        portENTER_CRITICAL(&s_btsm_osal_intr_mutex);

        count = uxQueueMessagesWaiting(eventq->q);
        for (i = 0; i < count; i++) {
            ret = xQueueReceive(eventq->q, &tmp_ev, 0);
            BTDM_OSAL_ASSERT(ret == pdPASS);

            if (tmp_ev == ev) {
                continue;
            }

            ret = xQueueSendToBack(eventq->q, &tmp_ev, 0);
            BTDM_OSAL_ASSERT(ret == pdPASS);
        }

        portEXIT_CRITICAL(&s_btsm_osal_intr_mutex);
    }

    event->queued = 0;
}

bool IRAM_ATTR
wr_btdm_osal_eventq_is_empty(struct btdm_osal_eventq *evq)
{
    struct btdm_osal_eventq_freertos *eventq = (struct btdm_osal_eventq_freertos *)evq->eventq;
    return xQueueIsQueueEmptyFromISR(eventq->q);
}

/*
 ***************************************************************************************************
 * BTDM OSAL Event
 ***************************************************************************************************
 */
void IRAM_ATTR
wr_btdm_osal_event_run(struct btdm_osal_event *ev)
{
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;
    event->fn(ev);
}

void IRAM_ATTR
wr_btdm_osal_event_init(struct btdm_osal_event *ev, btdm_osal_event_fn *fn, void *arg)
{
    struct btdm_osal_event_freertos *event = NULL;

#if BTDM_MEMPOOL_ALLOC
    if (!btdm_memblock_from(&s_btdm_osal_ev_pool, ev->event)) {
        ev->event = btdm_memblock_get(&s_btdm_osal_ev_pool);
    }
#else
    if (!ev->event) {
        ev->event = btdm_osal_malloc(sizeof(struct btdm_osal_event_freertos),
                                     BTDM_OSAL_MALLOC_F_INTERNAL);
    }
#endif
    event = (struct btdm_osal_event_freertos *)ev->event;
    BTDM_OSAL_ASSERT(event);

    memset(event, 0, sizeof(*event));
    event->fn = fn;
    event->arg = arg;
}

void IRAM_ATTR
wr_btdm_osal_event_deinit(struct btdm_osal_event *ev)
{
    if (!ev->event) {
        return;
    }

#if BTDM_MEMPOOL_ALLOC
    btdm_memblock_put(&s_btdm_osal_ev_pool, ev->event);
#else
    btdm_osal_free(ev->event);
#endif
    ev->event = NULL;
}

void IRAM_ATTR
wr_btdm_osal_event_reset(struct btdm_osal_event *ev)
{
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;
    BTDM_OSAL_ASSERT(event);
    event->queued = 0;
}

bool IRAM_ATTR
wr_btdm_osal_event_is_queued(struct btdm_osal_event *ev)
{
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;
    return event->queued;
}

void *IRAM_ATTR
wr_btdm_osal_event_get_arg(struct btdm_osal_event *ev)
{
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;
    return event->arg;
}

void IRAM_ATTR
wr_btdm_osal_event_set_arg(struct btdm_osal_event *ev, void *arg)
{
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)ev->event;
    event->arg = arg;
}

/*
 ***************************************************************************************************
 * BTDM OSAL Mutex
 ***************************************************************************************************
 */
btdm_osal_error_t
wr_btdm_osal_mutex_init(struct btdm_osal_mutex *mu)
{
    struct btdm_osal_mutex_freertos *mutex = NULL;
#if BTDM_MEMPOOL_ALLOC
    if (!btdm_memblock_from(&s_btdm_osal_mutex_pool, mu->mutex)) {
        mu->mutex = btdm_memblock_get(&s_btdm_osal_mutex_pool);
        mutex = (struct btdm_osal_mutex_freertos *)mu->mutex;

        if (!mutex) {
            return BTDM_OSAL_INVALID_PARM;
        }

        memset(mutex, 0, sizeof(*mutex));
        mutex->handle = xSemaphoreCreateRecursiveMutex();
        BTDM_OSAL_ASSERT(mutex->handle);
    }
#else
    if (!mu->mutex) {
        mu->mutex = btdm_osal_malloc(sizeof(struct btdm_osal_mutex_freertos),
                                     BTDM_OSAL_MALLOC_F_INTERNAL);
        mutex = (struct btdm_osal_mutex_freertos *)mu->mutex;

        if (!mutex) {
            return BTDM_OSAL_INVALID_PARM;
        }

        memset(mutex, 0, sizeof(*mutex));
        mutex->handle = xSemaphoreCreateRecursiveMutex();
        BTDM_OSAL_ASSERT(mutex->handle);
    }
#endif

    return BTDM_OSAL_OK;
}

btdm_osal_error_t
wr_btdm_osal_mutex_deinit(struct btdm_osal_mutex *mu)
{
    struct btdm_osal_mutex_freertos *mutex = (struct btdm_osal_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BTDM_OSAL_INVALID_PARM;
    }

    BTDM_OSAL_ASSERT(mutex->handle);
    vSemaphoreDelete(mutex->handle);

#if BTDM_MEMPOOL_ALLOC
    btdm_memblock_put(&s_btdm_osal_mutex_pool, mutex);
#else
    btdm_osal_free((void *)mutex);
#endif
    mu->mutex = NULL;

    return BTDM_OSAL_OK;
}

btdm_osal_error_t IRAM_ATTR
wr_btdm_osal_mutex_pend(struct btdm_osal_mutex *mu, btdm_osal_time_t timeout)
{
    BaseType_t ret;
    struct btdm_osal_mutex_freertos *mutex = (struct btdm_osal_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BTDM_OSAL_INVALID_PARM;
    }

    BTDM_OSAL_ASSERT(mutex->handle);

    if (in_isr()) {
        ret = pdFAIL;
        BTDM_OSAL_ASSERT(0);
    } else {
        ret = xSemaphoreTakeRecursive(mutex->handle, timeout);
    }

    return ret == pdPASS ? BTDM_OSAL_OK : BTDM_OSAL_TIMEOUT;
}

btdm_osal_error_t IRAM_ATTR
wr_btdm_osal_mutex_release(struct btdm_osal_mutex *mu)
{
    struct btdm_osal_mutex_freertos *mutex = (struct btdm_osal_mutex_freertos *)mu->mutex;

    if (!mutex) {
        return BTDM_OSAL_INVALID_PARM;
    }

    BTDM_OSAL_ASSERT(mutex->handle);

    if (in_isr()) {
        BTDM_OSAL_ASSERT(0);
    } else {
        if (xSemaphoreGiveRecursive(mutex->handle) != pdPASS) {
            return BTDM_OSAL_BAD_MUTEX;
        }
    }

    return BTDM_OSAL_OK;
}

/*
 ***************************************************************************************************
 * BTDM OSAL Sem
 ***************************************************************************************************
 */
btdm_osal_error_t
wr_btdm_osal_sem_init(struct btdm_osal_sem *sem, uint16_t tokens)
{
    struct btdm_osal_sem_freertos *semaphore = NULL;

#if BTDM_MEMPOOL_ALLOC
    if (!btdm_memblock_from(&s_btdm_osal_sem_pool, sem->sem)) {
        sem->sem = btdm_memblock_get(&s_btdm_osal_sem_pool);
        semaphore = (struct btdm_osal_sem_freertos *)sem->sem;

        if (!semaphore) {
            return BTDM_OSAL_INVALID_PARM;
        }

        memset(semaphore, 0, sizeof(*semaphore));
        semaphore->handle = xSemaphoreCreateCounting(128, tokens);
        BTDM_OSAL_ASSERT(semaphore->handle);
    }
#else
    if (!sem->sem) {
        sem->sem = btdm_osal_malloc(sizeof(struct btdm_osal_sem_freertos),
                                    BTDM_OSAL_MALLOC_F_INTERNAL);
        semaphore = (struct btdm_osal_sem_freertos *)sem->sem;

        if (!semaphore) {
            return BTDM_OSAL_INVALID_PARM;
        }

        memset(semaphore, 0, sizeof(*semaphore));
        semaphore->handle = xSemaphoreCreateCounting(128, tokens);
        BTDM_OSAL_ASSERT(semaphore->handle);
    }
#endif

    return BTDM_OSAL_OK;
}

btdm_osal_error_t
wr_btdm_osal_sem_deinit(struct btdm_osal_sem *sem)
{
    struct btdm_osal_sem_freertos *semaphore = (struct btdm_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BTDM_OSAL_INVALID_PARM;
    }

    BTDM_OSAL_ASSERT(semaphore->handle);
    vSemaphoreDelete(semaphore->handle);

#if BTDM_MEMPOOL_ALLOC
    btdm_memblock_put(&s_btdm_osal_sem_pool, semaphore);
#else
    btdm_osal_free((void *)semaphore);
#endif
    sem->sem = NULL;

    return BTDM_OSAL_OK;
}

btdm_osal_error_t IRAM_ATTR
wr_btdm_osal_sem_pend(struct btdm_osal_sem *sem, btdm_osal_time_t timeout)
{
    BaseType_t woken;
    BaseType_t ret;
    struct btdm_osal_sem_freertos *semaphore = (struct btdm_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BTDM_OSAL_INVALID_PARM;
    }

    BTDM_OSAL_ASSERT(semaphore->handle);

    if (in_isr()) {
        BTDM_OSAL_ASSERT(timeout == 0);
        ret = xSemaphoreTakeFromISR(semaphore->handle, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xSemaphoreTake(semaphore->handle, timeout);
    }

    return ret == pdPASS ? BTDM_OSAL_OK : BTDM_OSAL_TIMEOUT;
}

btdm_osal_error_t IRAM_ATTR
wr_btdm_osal_sem_release(struct btdm_osal_sem *sem)
{
    BaseType_t ret;
    BaseType_t woken;
    struct btdm_osal_sem_freertos *semaphore = (struct btdm_osal_sem_freertos *)sem->sem;

    if (!semaphore) {
        return BTDM_OSAL_INVALID_PARM;
    }

    BTDM_OSAL_ASSERT(semaphore->handle);

    if (in_isr()) {
        ret = xSemaphoreGiveFromISR(semaphore->handle, &woken);
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        ret = xSemaphoreGive(semaphore->handle);
    }

    BTDM_OSAL_ASSERT(ret == pdPASS);
    return BTDM_OSAL_OK;
}

uint16_t IRAM_ATTR
wr_btdm_osal_sem_get_count(struct btdm_osal_sem *sem)
{
    struct btdm_osal_sem_freertos *semaphore = (struct btdm_osal_sem_freertos *)sem->sem;
    return uxSemaphoreGetCount(semaphore->handle);
}

/*
 ***************************************************************************************************
 * BTDM OSAL Callout
 ***************************************************************************************************
 */
#if BTDM_OSAL_USE_ESP_TIMER
static void IRAM_ATTR
btdm_osal_event_fn_wrapper(void *arg)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)arg;
    BTDM_OSAL_ASSERT(callout);

    if (callout->evq) {
        btdm_osal_eventq_put(callout->evq, &callout->ev);
    } else {
        struct btdm_osal_event_freertos *event =
            (struct btdm_osal_event_freertos *)callout->ev.event;
        event->fn(&callout->ev);
    }
}

static IRAM_ATTR btdm_osal_error_t
esp_err_to_npl_error(esp_err_t err)
{
    switch (err) {
    case ESP_ERR_INVALID_ARG:
        return BTDM_OSAL_INVALID_PARM;

    case ESP_ERR_INVALID_STATE:
        return BTDM_OSAL_EINVAL;

    case ESP_OK:
        return BTDM_OSAL_OK;

    default:
        return BTDM_OSAL_ERROR;
    }
}
#else
static void IRAM_ATTR
os_callout_timer_cb(TimerHandle_t timer)
{
    struct btdm_osal_callout_freertos *callout;

    callout = pvTimerGetTimerID(timer);
    BTDM_OSAL_ASSERT(callout);

    if (callout->evq) {
        btdm_osal_eventq_put(callout->evq, &callout->ev);
    } else {
        struct btdm_osal_event_freertos *event =
            (struct btdm_osal_event_freertos *)callout->ev.event;
        event->fn(&callout->ev);
    }
}
#endif

int
wr_btdm_osal_callout_init(struct btdm_osal_callout *co, struct btdm_osal_eventq *evq,
                          btdm_osal_event_fn *ev_cb, void *ev_arg)
{
    struct btdm_osal_callout_freertos *callout = NULL;

#if BTDM_MEMPOOL_ALLOC
    if (!btdm_memblock_from(&s_btdm_osal_co_pool, co->co)) {
        co->co = btdm_memblock_get(&s_btdm_osal_co_pool);
        callout = (struct btdm_osal_callout_freertos *)co->co;
        BTDM_OSAL_ASSERT(callout);

        memset(callout, 0, sizeof(*callout));
        btdm_osal_event_init(&callout->ev, ev_cb, ev_arg);

#if BTDM_OSAL_USE_ESP_TIMER
        callout->evq = evq;

        esp_timer_create_args_t create_args = {
            .callback = btdm_osal_event_fn_wrapper,
            .arg = callout,
            .name = "nimble_timer"
        };

        if (esp_timer_create(&create_args, &callout->handle) != ESP_OK) {
            btdm_osal_event_deinit(&callout->ev);
            btdm_memblock_put(&s_btdm_osal_co_pool, callout);
            co->co = NULL;
            return -1;
        }
#else
        callout->handle = xTimerCreate("co", 1, pdFALSE, callout, os_callout_timer_cb);

        if (!callout->handle) {
            btdm_osal_event_deinit(&callout->ev);
            btdm_memblock_put(&s_btdm_osal_co_pool, callout);
            co->co = NULL;
            return -1;
        }
#endif // BTDM_OSAL_USE_ESP_TIMER
    } else {
        callout = (struct btdm_osal_callout_freertos *)co->co;
        BTDM_OSAL_ASSERT(callout);
        callout->evq = evq;
        btdm_osal_event_init(&callout->ev, ev_cb, ev_arg);
    }
#else

    if (!co->co) {
        co->co = btdm_osal_malloc(sizeof(struct btdm_osal_callout_freertos),
                                  BTDM_OSAL_MALLOC_F_INTERNAL);
        callout = (struct btdm_osal_callout_freertos *)co->co;
        if (!callout) {
            return -1;
        }

        memset(callout, 0, sizeof(*callout));
        btdm_osal_event_init(&callout->ev, ev_cb, ev_arg);

#if BTDM_OSAL_USE_ESP_TIMER
        callout->evq = evq;

        esp_timer_create_args_t create_args = {
            .callback = btdm_osal_event_fn_wrapper, .arg = callout, .name = "nimble_timer"};

        if (esp_timer_create(&create_args, &callout->handle) != ESP_OK) {
            btdm_osal_event_deinit(&callout->ev);
            btdm_osal_free((void *)callout);
            co->co = NULL;
            return -1;
        }
#else
        callout->handle = xTimerCreate("co", 1, pdFALSE, callout, os_callout_timer_cb);

        if (!callout->handle) {
            btdm_osal_event_deinit(&callout->ev);
            btdm_osal_free((void *)callout);
            co->co = NULL;
            return -1;
        }
#endif // BTDM_OSAL_USE_ESP_TIMER
    } else {
        callout = (struct btdm_osal_callout_freertos *)co->co;
        BTDM_OSAL_ASSERT(callout);
        callout->evq = evq;
        btdm_osal_event_init(&callout->ev, ev_cb, ev_arg);
    }
#endif // BTDM_MEMPOOL_ALLOC
    return 0;
}

void
wr_btdm_osal_callout_deinit(struct btdm_osal_callout *co)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;

    /* Since we dynamically deinit timers, function can be called for NULL timers. Return for such
     * scenarios */
    if (!callout) {
        return;
    }

    if (!callout->handle) {
        return;
    }

    btdm_osal_event_deinit(&callout->ev);
#if BTDM_OSAL_USE_ESP_TIMER
    esp_err_t err = esp_timer_stop(callout->handle);

    if (err != ESP_OK) {
        /* ESP_ERR_INVALID_STATE is expected when timer is already stopped */
        if (err != ESP_ERR_INVALID_STATE) {
            ESP_LOGD(TAG, "Timer not stopped");
        }
    }
    err = esp_timer_delete(callout->handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Timer not deleted");
    }
#else
    xTimerDelete(callout->handle, portMAX_DELAY);
#endif // BTDM_OSAL_USE_ESP_TIMER
#if BTDM_MEMPOOL_ALLOC
    btdm_memblock_put(&s_btdm_osal_co_pool, callout);
#else
    btdm_osal_free((void *)callout);
#endif // BTDM_MEMPOOL_ALLOC
    co->co = NULL;
    memset(co, 0, sizeof(struct btdm_osal_callout));
}

btdm_osal_error_t IRAM_ATTR
wr_btdm_osal_callout_reset(struct btdm_osal_callout *co, btdm_osal_time_t ticks)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;
#if BTDM_OSAL_USE_ESP_TIMER
    esp_timer_stop(callout->handle);

    return esp_err_to_npl_error(esp_timer_start_once(callout->handle, ticks * 1000));
#else

    BaseType_t woken1, woken2, woken3;

    if (ticks == 0) {
        ticks = 1;
    }
    if (in_isr()) {
        xTimerStopFromISR(callout->handle, &woken1);
        xTimerChangePeriodFromISR(callout->handle, ticks, &woken2);
        xTimerResetFromISR(callout->handle, &woken3);

        if (woken1 == pdTRUE || woken2 == pdTRUE || woken3 == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        xTimerStop(callout->handle, portMAX_DELAY);
        xTimerChangePeriod(callout->handle, ticks, portMAX_DELAY);
        xTimerReset(callout->handle, portMAX_DELAY);
    }

    return BTDM_OSAL_OK;
#endif
}

void
wr_btdm_osal_callout_mem_reset(struct btdm_osal_callout *co)
{
  struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;

  btdm_osal_event_reset(&callout->ev);
}

void IRAM_ATTR
wr_btdm_osal_callout_stop(struct btdm_osal_callout *co)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;

    if (!callout) {
        return;
    }

#if BTDM_OSAL_USE_ESP_TIMER
    esp_timer_stop(callout->handle);
#else
    xTimerStop(callout->handle, portMAX_DELAY);
#endif
}

bool IRAM_ATTR
wr_btdm_osal_callout_is_active(struct btdm_osal_callout *co)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;
#if BTDM_OSAL_USE_ESP_TIMER
    return esp_timer_is_active(callout->handle);
#else
    return xTimerIsTimerActive(callout->handle) == pdTRUE;
#endif
}

btdm_osal_time_t IRAM_ATTR
wr_btdm_osal_callout_get_ticks(struct btdm_osal_callout *co)
{
#if BTDM_OSAL_USE_ESP_TIMER

    uint32_t exp = 0;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    uint64_t expiry = 0;
    esp_err_t err;

    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;

    // Fetch expiry time in microseconds
    err = esp_timer_get_expiry_time((esp_timer_handle_t)(callout->handle), &expiry);
    if (err != ESP_OK) {
        // Error. Could not fetch the expiry time
        return 0;
    }

    // Convert microseconds to ticks
    btdm_osal_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
#else
    // esp_timer_get_expiry_time() is only available from IDF 5.0 onwards
    /* Returning 0 from here should not cause any effect.
     * Drawback of this approach is that existing code to reset timer would be called
     * more often (since the if condition to invoke reset timer would always succeed if
     * timer is active).
     */
    exp = 0;
#endif // ESP_IDF_VERSION

    return exp;
#else
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;
    return xTimerGetExpiryTime(callout->handle);
#endif
}

btdm_osal_time_t IRAM_ATTR
wr_btdm_osal_callout_remaining_ticks(struct btdm_osal_callout *co, btdm_osal_time_t now)
{
    btdm_osal_time_t rt;
    uint32_t exp = 0;

    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;

#if BTDM_OSAL_USE_ESP_TIMER
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    uint64_t expiry = 0;
    esp_err_t err;

    // Fetch expiry time in microseconds
    err = esp_timer_get_expiry_time((esp_timer_handle_t)(callout->handle), &expiry);
    if (err != ESP_OK) {
        // Error. Could not fetch the expiry time
        return 0;
    }

    // Convert microseconds to ticks
    btdm_osal_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
#else
    // esp_timer_get_expiry_time() is only available from IDF 5.0 onwards
    // Set expiry to 0
    exp = 0;
#endif // ESP_IDF_VERSION
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

void IRAM_ATTR
wr_btdm_osal_callout_set_arg(struct btdm_osal_callout *co, void *arg)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;
    struct btdm_osal_event_freertos *event = (struct btdm_osal_event_freertos *)callout->ev.event;
    event->arg = arg;
}

#if 0
void IRAM_ATTR
wr_btdm_osal_callout_mem_reset(struct btdm_osal_callout *co)
{
    struct btdm_osal_callout_freertos *callout = (struct btdm_osal_callout_freertos *)co->co;

    BTDM_OSAL_ASSERT(callout);
    BTDM_OSAL_ASSERT(callout->handle);

    btdm_osal_event_reset(&callout->ev);
}
#endif

/*
 ***************************************************************************************************
 * BTDM OSAL Time
 ***************************************************************************************************
 */
uint32_t IRAM_ATTR
wr_btdm_osal_time_get(void)
{
#if BTDM_OSAL_USE_ESP_TIMER
    return esp_timer_get_time() / 1000;
#else
    return xTaskGetTickCountFromISR();
#endif
}

btdm_osal_error_t IRAM_ATTR
wr_btdm_osal_time_ms_to_ticks(uint32_t ms, btdm_osal_time_t *out_ticks)
{
    uint64_t ticks;
#if BTDM_OSAL_USE_ESP_TIMER
    ticks = (uint64_t)ms;
#else
    ticks = ((uint64_t)ms * configTICK_RATE_HZ) / 1000;
#endif
    if (ticks > UINT32_MAX) {
        return BTDM_OSAL_EINVAL;
    }

    *out_ticks = ticks;

    return 0;
}

btdm_osal_error_t IRAM_ATTR
btdm_osal_time_ticks_to_ms(btdm_osal_time_t ticks, uint32_t *out_ms)
{
    uint64_t ms;
#if BTDM_OSAL_USE_ESP_TIMER
    ms = ((uint64_t)ticks);
#else
    ms = ((uint64_t)ticks * 1000) / configTICK_RATE_HZ;
#endif
    if (ms > UINT32_MAX) {
        return BTDM_OSAL_EINVAL;
    }

    *out_ms = ms;

    return 0;
}

btdm_osal_time_t IRAM_ATTR
wr_btdm_osal_time_ms_to_ticks32(uint32_t ms)
{
#if BTDM_OSAL_USE_ESP_TIMER
    return ms;
#else
    return ms * configTICK_RATE_HZ / 1000;
#endif
}

uint32_t IRAM_ATTR
wr_btdm_osal_time_ticks_to_ms32(btdm_osal_time_t ticks)
{
#if BTDM_OSAL_USE_ESP_TIMER
    return ticks;
#else
    return ticks * 1000 / configTICK_RATE_HZ;
#endif
}

uint32_t IRAM_ATTR
wr_btdm_osal_get_time_forever(void)
{
    return portMAX_DELAY;
}

/*
 ***************************************************************************************************
 * BTDM OSAL Hw Critical
 ***************************************************************************************************
 */
uint32_t IRAM_ATTR
wr_btdm_osal_hw_enter_critical(void)
{
    portENTER_CRITICAL_SAFE(&s_btsm_osal_intr_mutex);
    ++s_btdm_osal_intr_nest;
    return 0;
}

uint8_t IRAM_ATTR
wr_btdm_osal_hw_is_in_critical(void)
{
    return s_btdm_osal_intr_nest;
}

void IRAM_ATTR
wr_btdm_osal_hw_exit_critical(uint32_t ctx)
{
    ESP_UNUSED(ctx);
    --s_btdm_osal_intr_nest;
    portEXIT_CRITICAL_SAFE(&s_btsm_osal_intr_mutex);
}

/*
 ***************************************************************************************************
 * BTDM OSAL Task
 ***************************************************************************************************
 */
int
wr_btdm_osal_task_create(btdm_osal_task_fn *fn, const char *name, uint32_t stack_size, void *arg,
                         uint32_t priority, btdm_osal_task_handle_t *task_handle, uint32_t core_id)
{
    return xTaskCreatePinnedToCore(fn, name, stack_size, arg, priority, (TaskHandle_t *)task_handle,
                                   (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY)) ==
                   pdPASS
               ? BTDM_OSAL_OK
               : BTDM_OSAL_ERROR;
}

void
wr_btdm_osal_task_delete(btdm_osal_task_handle_t task_handle)
{
    vTaskDelete(task_handle);
}

/*
 ***************************************************************************************************
 * BTDM OSAL Interrupt
 ***************************************************************************************************
 */
static void
interrupt_alloc_ipc_func(struct btdm_intr_alloc_params *params)
{
    params->rsp = esp_intr_alloc(params->cp.source, params->cp.flags, params->cp.handler,
                                 params->cp.arg, params->cp.ret_handle_in);
}

int
wr_btdm_osal_intr_alloc(int src, int flags, btdm_osal_intr_fn *fn, void *arg,
                        btdm_osal_intr_handle_t *intr_handle)
{
    struct btdm_intr_alloc_params params = {
        .cp.source = src,
        .cp.flags = ESP_INTR_FLAG_LEVEL3,
        .cp.handler = (intr_handler_t)fn,
        .cp.arg = arg,
        .cp.ret_handle_in = (intr_handle_t *)intr_handle,
    };

    if (!flags) {
        params.cp.flags |= ESP_INTR_FLAG_IRAM;
    }

#if !CONFIG_FREERTOS_UNICORE
    esp_ipc_call_blocking(CONFIG_BT_CTRL_PINNED_TO_CORE, (esp_ipc_func_t)interrupt_alloc_ipc_func,
                          &params);
#else
    interrupt_alloc_ipc_func(&params);
#endif

    return params.rsp;
}

int
wr_btdm_osal_intr_free(btdm_osal_intr_handle_t intr_handle)
{
    return esp_intr_free(intr_handle);
}

/*
 ***************************************************************************************************
 * BTDM OSAL Malloc
 ***************************************************************************************************
 */
void *
wr_btdm_osal_malloc(uint32_t size, btdm_osal_malloc_flag_t flags)
{
    if (flags == BTDM_OSAL_MALLOC_F_INTERNAL) {
        return heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    } else {
        return heap_caps_malloc(size, MALLOC_CAP_8BIT);
    }
}

void
wr_btdm_osal_free(void *ptr)
{
    heap_caps_free(ptr);
}

#if !CONFIG_BTDM_CTRL_MULTI_LINK_ENABLED
void *
wr_btdm_osal_mmgmt_block_malloc(uint32_t size)
{
    uint32_t *addr;

    // TODO: Only ble controller uses this function
    addr = btdm_osal_malloc(size + 4, BTDM_OSAL_MALLOC_F_INTERNAL);
    if (!addr) {
        return NULL;
    }

    *addr = (5 << 29) | ((size + 4) >> 2);

    return (void *)((uint32_t)addr + 4);
}
void
wr_btdm_osal_mmgmt_block_free(void *ptr)
{
    void *actual_addr;

    // TODO: Only ble controller uses this function
    actual_addr = (void *)ptr - 4;
    return btdm_osal_free(actual_addr);
}

void
wr_btdm_osal_mmgmt_block_copy(void *dst, const void *src, uint16_t size)
{
    // TODO: Only ble controller uses this function
    extern void r_ble_lll_mmgmt_block_copy(void *addr0, void *addr1, uint16_t size);
    r_ble_lll_mmgmt_block_copy((void *)dst, (void *)src, size);
}
#endif /* !CONFIG_BTDM_CTRL_MULTI_LINK_ENABLED */

/*
 ***************************************************************************************************
 * BTDM OSAL Efuse
 ***************************************************************************************************
 */
int
wr_btdm_osal_read_efuse_mac(uint8_t *mac)
{
    ESP_ERROR_CHECK(esp_read_mac((uint8_t *)mac, ESP_MAC_BT));
    return 0;
}

/*
 ***************************************************************************************************
 * BTDM OSAL Rand
 ***************************************************************************************************
 */
void
wr_btdm_osal_srand(uint32_t seed)
{
    srand(seed);
}

int
wr_btdm_osal_rand(void)
{
    // TODO: use esp_random
    // return (int)esp_random();

    static bool first = true;

    if (first) {
        uint8_t mac[6];
        uint32_t seed;

        first = false;
        btdm_osal_read_efuse_mac(mac);
        seed = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];
        srand(seed);
    }

    return rand();
}

/*
 ***************************************************************************************************
 * BTDM OSAL Initialization
 ***************************************************************************************************
 */
int
btdm_osal_elem_mempool_init(btdm_osal_elem_num_t *elem_num)
{
#if BTDM_MEMPOOL_ALLOC
    int rc;

    s_btdm_osal_evtq_deepth = elem_num->evt_count;

    if (elem_num->evt_count) {
        s_btdm_osal_ev_buf = btdm_osal_malloc(
            BTDM_MEMPOOL_SIZE(elem_num->evt_count, sizeof(struct btdm_osal_event_freertos)) *
                sizeof(btdm_membuf_t),
            BTDM_OSAL_MALLOC_F_INTERNAL);
        if (!s_btdm_osal_ev_buf) {
            return -1;
        }
        rc = btdm_mempool_init(&s_btdm_osal_ev_pool, elem_num->evt_count,
                               sizeof(struct btdm_osal_event_freertos), s_btdm_osal_ev_buf,
                               "s_btdm_osal_ev_pool");
        if (rc) {
            return -2;
        }
    }

    if (elem_num->evtq_count) {
        s_btdm_osal_evq_buf = btdm_osal_malloc(
            BTDM_MEMPOOL_SIZE(elem_num->evtq_count, sizeof(struct btdm_osal_eventq_freertos)) *
                sizeof(btdm_membuf_t),
            BTDM_OSAL_MALLOC_F_INTERNAL);
        if (!s_btdm_osal_evq_buf) {
            return -3;
        }
        rc = btdm_mempool_init(&s_btdm_osal_evq_pool, elem_num->evtq_count,
                               sizeof(struct btdm_osal_eventq_freertos), s_btdm_osal_evq_buf,
                               "s_btdm_osal_evq_pool");
        if (rc) {
            return -4;
        }
    }

    if (elem_num->co_count) {
        s_btdm_osal_co_buf = btdm_osal_malloc(
            BTDM_MEMPOOL_SIZE(elem_num->co_count, sizeof(struct btdm_osal_callout_freertos)) *
                sizeof(btdm_membuf_t),
            BTDM_OSAL_MALLOC_F_INTERNAL);
        if (!s_btdm_osal_co_buf) {
            return -5;
        }
        rc = btdm_mempool_init(&s_btdm_osal_co_pool, elem_num->co_count,
                               sizeof(struct btdm_osal_callout_freertos), s_btdm_osal_co_buf,
                               "s_btdm_osal_co_pool");
        if (rc) {
            return -6;
        }
    }

    if (elem_num->sem_count) {
        s_btdm_osal_sem_buf = btdm_osal_malloc(
            BTDM_MEMPOOL_SIZE(elem_num->sem_count, sizeof(struct btdm_osal_sem_freertos)) *
                sizeof(btdm_membuf_t),
            BTDM_OSAL_MALLOC_F_INTERNAL);
        if (!s_btdm_osal_sem_buf) {
            return -7;
        }
        rc = btdm_mempool_init(&s_btdm_osal_sem_pool, elem_num->sem_count,
                               sizeof(struct btdm_osal_sem_freertos),
                               s_btdm_osal_sem_buf, "s_btdm_osal_sem_pool");
        if (rc) {
            return -8;
        }
    }

    if (elem_num->mutex_count) {
        s_btdm_osal_mutex_buf = btdm_osal_malloc(
            BTDM_MEMPOOL_SIZE(elem_num->mutex_count, sizeof(struct btdm_osal_mutex_freertos)) *
                sizeof(btdm_membuf_t),
            BTDM_OSAL_MALLOC_F_INTERNAL);
        if (!s_btdm_osal_mutex_buf) {
            return -9;
        }
        rc = btdm_mempool_init(&s_btdm_osal_mutex_pool, elem_num->mutex_count,
                               sizeof(struct btdm_osal_mutex_freertos), s_btdm_osal_mutex_buf,
                               "s_btdm_osal_mutex_pool");
        if (rc) {
            return -10;
        }
    }

    return 0;
#else
    return 0;
#endif // BTDM_MEMPOOL_ALLOC
}

void
btdm_osal_elem_mempool_deinit(void)
{
#if BTDM_MEMPOOL_ALLOC
    if (s_btdm_osal_ev_buf) {
        BTDM_OSAL_ASSERT(s_btdm_osal_ev_pool.mp_num_free == s_btdm_osal_ev_pool.mp_num_blocks);
        btdm_osal_free(s_btdm_osal_ev_buf);
        s_btdm_osal_ev_buf = NULL;
    }
    if (s_btdm_osal_evq_buf) {
        BTDM_OSAL_ASSERT(s_btdm_osal_evq_pool.mp_num_free == s_btdm_osal_evq_pool.mp_num_blocks);
        btdm_osal_free(s_btdm_osal_evq_buf);
        s_btdm_osal_evq_buf = NULL;
    }
    if (s_btdm_osal_co_buf) {
        BTDM_OSAL_ASSERT(s_btdm_osal_co_pool.mp_num_free == s_btdm_osal_co_pool.mp_num_blocks);
        btdm_osal_free(s_btdm_osal_co_buf);
        s_btdm_osal_co_buf = NULL;
    }
    if (s_btdm_osal_sem_buf) {
        BTDM_OSAL_ASSERT(s_btdm_osal_sem_pool.mp_num_free == s_btdm_osal_sem_pool.mp_num_blocks);
        btdm_osal_free(s_btdm_osal_sem_buf);
        s_btdm_osal_sem_buf = NULL;
    }
    if (s_btdm_osal_mutex_buf) {
        BTDM_OSAL_ASSERT(s_btdm_osal_mutex_pool.mp_num_free == s_btdm_osal_mutex_pool.mp_num_blocks);
        btdm_osal_free(s_btdm_osal_mutex_buf);
        s_btdm_osal_mutex_buf = NULL;
    }
#endif // BTDM_MEMPOOL_ALLOC
}
