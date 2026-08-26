/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 */

#ifndef _BT_OSAL_OS_H_
#define _BT_OSAL_OS_H_

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
        (sizeof(array) / sizeof((array)[0]))
#endif

extern int ets_printf(const char *fmt, ...);
#define  BT_OSAL_ASSERT(con)                         \
    do{                                             \
        if(!(con))  {                               \
            ets_printf("assertion:%s\n",#con);      \
            ets_printf("line:%d,function:%s\n", __LINE__, __func__);\
            assert(0);                              \
        }                                           \
    }while(0)

#define BT_OSAL_OS_ALIGNMENT    (4)/*bt_osal_get_os_alignment()*/

#define BT_OSAL_TIME_FOREVER    bt_osal_get_time_forever()

/* This should be compatible with TickType_t */
typedef uint32_t bt_osal_time_t;
typedef int32_t bt_osal_stime_t;

struct bt_osal_event;
typedef void bt_osal_event_fn(struct bt_osal_event *ev);

struct bt_osal_event {
    void *event;
};

struct bt_osal_eventq {
    void *eventq;
};

struct bt_osal_callout {
    void *co;
};

struct bt_osal_mutex {
    void *mutex;
};

struct bt_osal_sem {
    void *sem;
};

/**
 * @brief Core affinity value meaning "let the scheduler pick any core"
 */
#define BT_OSAL_TASK_NO_AFFINITY    ((BaseType_t)tskNO_AFFINITY)

/**
 * @brief Attributes for the task started to service an event queue
 *
 * Passed to bt_osal_eventq_start() so each queue's worker task can be given its
 * own name, priority, stack and core affinity.
 */
struct bt_osal_task_info {
    const char *name;     /*!< Name of the task */
    uint8_t prio;         /*!< FreeRTOS priority of the task */
    uint32_t stack_size;  /*!< Stack depth of the task, in bytes */
    BaseType_t core_id;   /*!< Core to pin the task to, or BT_OSAL_TASK_NO_AFFINITY to leave it unpinned */
};

/*
 * Simple APIs are just defined as static inline below, but some are a bit more
 * complex or require some global state variables and thus are defined in .c
 * file instead and static inline wrapper just calls proper implementation.
 * We need declarations of these functions and they are defined in header below.
 */
#include "bt_osal_freertos.h"

struct bt_osal_funcs_t {
    bool (*p_bt_osal_os_started)(void);
    void *(*p_bt_osal_get_current_task_id)(void);
    void (*p_bt_osal_eventq_init)(struct bt_osal_eventq *);
    void (*p_bt_osal_eventq_deinit)(struct bt_osal_eventq *);
    struct bt_osal_event * (*p_bt_osal_eventq_get)(struct bt_osal_eventq *, bt_osal_time_t);
    void (*p_bt_osal_eventq_put)(struct bt_osal_eventq *, struct bt_osal_event *);
    void (*p_bt_osal_eventq_remove)(struct bt_osal_eventq *, struct bt_osal_event *);
    void (*p_bt_osal_event_run)(struct bt_osal_event *);
    bool (*p_bt_osal_eventq_is_empty)(struct bt_osal_eventq *);
    void (*p_bt_osal_event_init)(struct bt_osal_event *, bt_osal_event_fn *, void *);
    void (*p_bt_osal_event_deinit)(struct bt_osal_event *);
    void (*p_bt_osal_event_reset)(struct bt_osal_event *);
    bool (*p_bt_osal_event_is_queued)(struct bt_osal_event *);
    void * (*p_bt_osal_event_get_arg)(struct bt_osal_event *);
    void (*p_bt_osal_event_set_arg)(struct bt_osal_event *, void *);
    bt_osal_error_t (*p_bt_osal_mutex_init)(struct bt_osal_mutex *);
    bt_osal_error_t (*p_bt_osal_mutex_deinit)(struct bt_osal_mutex *);
    bt_osal_error_t (*p_bt_osal_mutex_pend)(struct bt_osal_mutex *, bt_osal_time_t);
    bt_osal_error_t (*p_bt_osal_mutex_release)(struct bt_osal_mutex *);
    bt_osal_error_t (*p_bt_osal_sem_init)(struct bt_osal_sem *, uint16_t);
    bt_osal_error_t (*p_bt_osal_sem_deinit)(struct bt_osal_sem *);
    bt_osal_error_t (*p_bt_osal_sem_pend)(struct bt_osal_sem *, bt_osal_time_t);
    bt_osal_error_t (*p_bt_osal_sem_release)(struct bt_osal_sem *);
    uint16_t (*p_bt_osal_sem_get_count)(struct bt_osal_sem *);
    bt_osal_error_t (*p_bt_osal_callout_init)(struct bt_osal_callout *, struct bt_osal_eventq *, bt_osal_event_fn *, void *);
    bt_osal_error_t (*p_bt_osal_callout_reset)(struct bt_osal_callout *, bt_osal_time_t);
    void (*p_bt_osal_callout_stop)(struct bt_osal_callout *);
    void (*p_bt_osal_callout_deinit)(struct bt_osal_callout *);
    void (*p_bt_osal_callout_mem_reset)(struct bt_osal_callout *);
    bool (*p_bt_osal_callout_is_active)(struct bt_osal_callout *);
    bt_osal_time_t (*p_bt_osal_callout_get_ticks)(struct bt_osal_callout *);
    uint32_t (*p_bt_osal_callout_remaining_ticks)(struct bt_osal_callout *, bt_osal_time_t);
    void (*p_bt_osal_callout_set_arg)(struct bt_osal_callout *, void *);
    uint32_t (*p_bt_osal_time_get)(void);
    bt_osal_error_t (*p_bt_osal_time_ms_to_ticks)(uint32_t ms, bt_osal_time_t *);
    bt_osal_error_t (*p_bt_osal_time_ticks_to_ms)(bt_osal_time_t, uint32_t *);
    bt_osal_time_t (*p_bt_osal_time_ms_to_ticks32)(uint32_t);
    uint32_t (*p_bt_osal_time_ticks_to_ms32)(bt_osal_time_t);
    void (*p_bt_osal_time_delay)(bt_osal_time_t);
    void (*p_bt_osal_hw_set_isr)(int, uint32_t);
    uint32_t (*p_bt_osal_hw_enter_critical)(void);
    void (*p_bt_osal_hw_exit_critical)(uint32_t);
    uint32_t (*p_bt_osal_get_time_forever)(void);
    uint8_t (*p_bt_osal_hw_is_in_critical)(void);
    void (*p_bt_osal_eventq_put_to_front)(struct bt_osal_eventq *, struct bt_osal_event *);
    bt_osal_error_t (*p_bt_osal_eventq_start)(struct bt_osal_eventq *evq, const struct bt_osal_task_info *info);
    bt_osal_error_t (*p_bt_osal_eventq_post_func)(struct bt_osal_eventq *evq, bt_osal_event_fn *fn, void *arg);
};

extern struct bt_osal_funcs_t *bt_osal_funcs;

static inline bool
IRAM_ATTR bt_osal_os_started(void)
{
    return bt_osal_funcs->p_bt_osal_os_started();
}

static inline void *
IRAM_ATTR bt_osal_get_current_task_id(void)
{
    return bt_osal_funcs->p_bt_osal_get_current_task_id();
}

static inline void
IRAM_ATTR bt_osal_eventq_init(struct bt_osal_eventq *evq)
{
    return bt_osal_funcs->p_bt_osal_eventq_init(evq);
}

static inline void
IRAM_ATTR bt_osal_eventq_deinit(struct bt_osal_eventq *evq)
{
    return bt_osal_funcs->p_bt_osal_eventq_deinit(evq);
}

static inline struct bt_osal_event *
IRAM_ATTR bt_osal_eventq_get(struct bt_osal_eventq *evq, bt_osal_time_t tmo)
{
    return bt_osal_funcs->p_bt_osal_eventq_get(evq, tmo);
}

static inline void
IRAM_ATTR bt_osal_eventq_put(struct bt_osal_eventq *evq, struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_eventq_put(evq, ev);
}

static inline void
IRAM_ATTR bt_osal_eventq_put_to_front(struct bt_osal_eventq *evq, struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_eventq_put_to_front(evq, ev);
}

static inline void
IRAM_ATTR bt_osal_eventq_remove(struct bt_osal_eventq *evq, struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_eventq_remove(evq, ev);
}

static inline void
IRAM_ATTR bt_osal_event_run(struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_event_run(ev);
}

static inline bool
IRAM_ATTR bt_osal_eventq_is_empty(struct bt_osal_eventq *evq)
{
    return bt_osal_funcs->p_bt_osal_eventq_is_empty(evq);
}

static inline void
IRAM_ATTR bt_osal_event_init(struct bt_osal_event *ev, bt_osal_event_fn *fn,
                   void *arg)
{
    return bt_osal_funcs->p_bt_osal_event_init(ev, fn, arg);
}

static inline bool
IRAM_ATTR bt_osal_event_is_queued(struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_event_is_queued(ev);
}

static inline void *
IRAM_ATTR bt_osal_event_get_arg(struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_event_get_arg(ev);
}

static inline void
IRAM_ATTR bt_osal_event_set_arg(struct bt_osal_event *ev, void *arg)
{
    return bt_osal_funcs->p_bt_osal_event_set_arg(ev, arg);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_mutex_init(struct bt_osal_mutex *mu)
{
    return bt_osal_funcs->p_bt_osal_mutex_init(mu);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_mutex_deinit(struct bt_osal_mutex *mu)
{
   return bt_osal_funcs->p_bt_osal_mutex_deinit(mu);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_mutex_pend(struct bt_osal_mutex *mu, bt_osal_time_t timeout)
{
    return bt_osal_funcs->p_bt_osal_mutex_pend(mu, timeout);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_mutex_release(struct bt_osal_mutex *mu)
{
    return bt_osal_funcs->p_bt_osal_mutex_release(mu);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_sem_init(struct bt_osal_sem *sem, uint16_t tokens)
{
   return bt_osal_funcs->p_bt_osal_sem_init(sem, tokens);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_sem_deinit(struct bt_osal_sem *sem)
{
    return bt_osal_funcs->p_bt_osal_sem_deinit(sem);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_sem_pend(struct bt_osal_sem *sem, bt_osal_time_t timeout)
{
    return bt_osal_funcs->p_bt_osal_sem_pend(sem, timeout);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_sem_release(struct bt_osal_sem *sem)
{
    return bt_osal_funcs->p_bt_osal_sem_release(sem);
}

static inline uint16_t
IRAM_ATTR bt_osal_sem_get_count(struct bt_osal_sem *sem)
{
    return bt_osal_funcs->p_bt_osal_sem_get_count(sem);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_callout_init(struct bt_osal_callout *co, struct bt_osal_eventq *evq,
                     bt_osal_event_fn *ev_cb, void *ev_arg)
{
    return bt_osal_funcs->p_bt_osal_callout_init(co, evq, ev_cb, ev_arg);
}

static inline void
IRAM_ATTR bt_osal_callout_deinit(struct bt_osal_callout *co)
{
    return bt_osal_funcs->p_bt_osal_callout_deinit(co);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_callout_reset(struct bt_osal_callout *co, bt_osal_time_t ticks)
{
    return bt_osal_funcs->p_bt_osal_callout_reset(co, ticks);
}

static inline void
IRAM_ATTR bt_osal_callout_stop(struct bt_osal_callout *co)
{
    return bt_osal_funcs->p_bt_osal_callout_stop(co);
}

static inline bool
IRAM_ATTR bt_osal_callout_is_active(struct bt_osal_callout *co)
{
    return bt_osal_funcs->p_bt_osal_callout_is_active(co);
}

static inline bt_osal_time_t
IRAM_ATTR bt_osal_callout_get_ticks(struct bt_osal_callout *co)
{
    return bt_osal_funcs->p_bt_osal_callout_get_ticks(co);
}

static inline bt_osal_time_t
IRAM_ATTR bt_osal_callout_remaining_ticks(struct bt_osal_callout *co,
                                bt_osal_time_t time)
{
    return bt_osal_funcs->p_bt_osal_callout_remaining_ticks(co, time);
}

static inline void
IRAM_ATTR bt_osal_callout_set_arg(struct bt_osal_callout *co, void *arg)
{
    return bt_osal_funcs->p_bt_osal_callout_set_arg(co, arg);
}

static inline bt_osal_time_t
IRAM_ATTR bt_osal_time_get(void)
{
    return bt_osal_funcs->p_bt_osal_time_get();
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_time_ms_to_ticks(uint32_t ms, bt_osal_time_t *out_ticks)
{
    return bt_osal_funcs->p_bt_osal_time_ms_to_ticks(ms, out_ticks);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_time_ticks_to_ms(bt_osal_time_t ticks, uint32_t *out_ms)
{
    return bt_osal_funcs->p_bt_osal_time_ticks_to_ms(ticks, out_ms);
}

static inline bt_osal_time_t
IRAM_ATTR bt_osal_time_ms_to_ticks32(uint32_t ms)
{
    return bt_osal_funcs->p_bt_osal_time_ms_to_ticks32(ms);
}

static inline uint32_t
IRAM_ATTR bt_osal_time_ticks_to_ms32(bt_osal_time_t ticks)
{
    return bt_osal_funcs->p_bt_osal_time_ticks_to_ms32(ticks);
}

static inline void
IRAM_ATTR bt_osal_time_delay(bt_osal_time_t ticks)
{
    return bt_osal_funcs->p_bt_osal_time_delay(ticks);
}

#if NIMBLE_CFG_CONTROLLER
static inline void
IRAM_ATTR bt_osal_hw_set_isr(int irqn, uint32_t addr)
{
    return bt_osal_funcs->p_bt_osal_hw_set_isr(irqn, addr);
}
#endif

static inline uint32_t
IRAM_ATTR bt_osal_hw_enter_critical(void)
{
    return bt_osal_funcs->p_bt_osal_hw_enter_critical();
}

static inline void
IRAM_ATTR bt_osal_hw_exit_critical(uint32_t ctx)
{
    return bt_osal_funcs->p_bt_osal_hw_exit_critical(ctx);
}

static inline bool
IRAM_ATTR bt_osal_hw_is_in_critical(void)
{
    return bt_osal_funcs->p_bt_osal_hw_is_in_critical();
}

static inline bt_osal_time_t
IRAM_ATTR bt_osal_get_time_forever(void)
{
    return bt_osal_funcs->p_bt_osal_get_time_forever();
}

static inline void
IRAM_ATTR bt_osal_callout_mem_reset(struct bt_osal_callout *co)
{
    return bt_osal_funcs->p_bt_osal_callout_mem_reset(co);
}

static inline void
IRAM_ATTR bt_osal_event_deinit(struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_event_deinit(ev);
}

static inline void
IRAM_ATTR bt_osal_event_reset(struct bt_osal_event *ev)
{
    return bt_osal_funcs->p_bt_osal_event_reset(ev);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_eventq_start(struct bt_osal_eventq *evq, const struct bt_osal_task_info *info)
{
    return bt_osal_funcs->p_bt_osal_eventq_start(evq, info);
}

static inline bt_osal_error_t
IRAM_ATTR bt_osal_eventq_post_func(struct bt_osal_eventq *evq, bt_osal_event_fn *fn, void *arg)
{
    return bt_osal_funcs->p_bt_osal_eventq_post_func(evq, fn, arg);
}

#ifdef __cplusplus
}
#endif

#endif  /* _BT_OSAL_OS_H_ */
