/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 */

#ifndef _BT_OSAL_FREERTOS_H_
#define _BT_OSAL_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sdkconfig.h"

/* Use esp timer instead of FreeRTOS timer to implement the callout. */
#ifdef CONFIG_BT_OSAL_USE_ESP_TIMER
#define BT_OSAL_USE_ESP_TIMER       (1)
#else
#define BT_OSAL_USE_ESP_TIMER       (0)
#endif

typedef void bt_osal_event_fn(struct bt_osal_event *ev);

struct bt_osal_event_freertos {
    bool queued;
    bt_osal_event_fn *fn;
    void *arg;
    struct bt_osal_event *next; /*!< Next event in the queue's list, or NULL; valid only while @ref queued */
};

struct bt_osal_eventq_freertos {
    struct bt_osal_event *head; /*!< First event to dequeue, NULL when the queue is empty */
    struct bt_osal_event *tail; /*!< Last event in the list, NULL when the queue is empty */
    SemaphoreHandle_t sem;      /*!< Binary "work pending" flag waking a blocked eventq_get(); the list is the source of truth, this only signals */
    portMUX_TYPE lock;          /*!< Spinlock protecting head/tail and each event's next (task- and ISR-safe) */
    TaskHandle_t task;          /*!< Handle of the task started to process this event queue, NULL if none */
    SemaphoreHandle_t done;     /*!< Signalled by the worker just before it self-deletes, so eventq_deinit() can wait for a clean exit; NULL if no task */
    bool stop;                  /*!< Set by eventq_deinit() to ask the worker to exit at its next safe point (between events) */
};

struct bt_osal_callout_freertos {
#if BT_OSAL_USE_ESP_TIMER
   esp_timer_handle_t handle;
#else
    TimerHandle_t handle;
#endif
    struct bt_osal_eventq *evq;
    struct bt_osal_event ev;
};

struct bt_osal_mutex_freertos {
    SemaphoreHandle_t handle;
};

struct bt_osal_sem_freertos {
    SemaphoreHandle_t handle;
};

typedef void bt_osal_event_fn_freertos(struct bt_osal_event_freertos *ev);

// Eventq
struct bt_osal_event *bt_osal_freertos_eventq_get(struct bt_osal_eventq *evq,
                                              bt_osal_time_t tmo);

void bt_osal_freertos_eventq_put(struct bt_osal_eventq *evq,
                             struct bt_osal_event *ev);

void bt_osal_freertos_eventq_put_to_front(struct bt_osal_eventq *evq,
                                      struct bt_osal_event *ev);

void bt_osal_freertos_eventq_remove(struct bt_osal_eventq *evq,
                                struct bt_osal_event *ev);

bt_osal_error_t bt_osal_freertos_eventq_start(struct bt_osal_eventq *evq,
                                          const struct bt_osal_task_info *info);

bt_osal_error_t bt_osal_freertos_eventq_post_func(struct bt_osal_eventq *evq,
                                              bt_osal_event_fn *fn, void *arg);

// Mutex
bt_osal_error_t bt_osal_freertos_mutex_init(struct bt_osal_mutex *mu);
bt_osal_error_t bt_osal_freertos_mutex_deinit(struct bt_osal_mutex *mu);

bt_osal_error_t bt_osal_freertos_mutex_pend(struct bt_osal_mutex *mu,
                                        bt_osal_time_t timeout);

bt_osal_error_t bt_osal_freertos_mutex_release(struct bt_osal_mutex *mu);

// Semephore
bt_osal_error_t bt_osal_freertos_sem_init(struct bt_osal_sem *sem, uint16_t tokens);
bt_osal_error_t bt_osal_freertos_sem_deinit(struct bt_osal_sem *sem);

bt_osal_error_t bt_osal_freertos_sem_pend(struct bt_osal_sem *sem,
                                      bt_osal_time_t timeout);

bt_osal_error_t bt_osal_freertos_sem_release(struct bt_osal_sem *sem);

// Callout
bt_osal_error_t bt_osal_freertos_callout_init(struct bt_osal_callout *co,
                               struct bt_osal_eventq *evq,
                               bt_osal_event_fn *ev_cb, void *ev_arg);

void bt_osal_freertos_callout_deinit(struct bt_osal_callout *co);

void bt_osal_freertos_callout_stop(struct bt_osal_callout *co);

bool bt_osal_freertos_callout_is_active(struct bt_osal_callout *co);

bt_osal_time_t bt_osal_freertos_callout_get_ticks(struct bt_osal_callout *co);

bt_osal_error_t bt_osal_freertos_callout_reset(struct bt_osal_callout *co,
                                           bt_osal_time_t ticks);

bt_osal_time_t bt_osal_freertos_callout_remaining_ticks(struct bt_osal_callout *co,
						    bt_osal_time_t now);

// Time
bt_osal_error_t bt_osal_freertos_time_ms_to_ticks(uint32_t ms,
                                              bt_osal_time_t *out_ticks);

bt_osal_error_t bt_osal_freertos_time_ticks_to_ms(bt_osal_time_t ticks,
                                              uint32_t *out_ms);

// Hardware
uint32_t bt_osal_freertos_hw_enter_critical(void);

void bt_osal_freertos_hw_exit_critical(uint32_t ctx);

/**
 * @brief Allocate and populate the OSAL function table
 *
 * Must be called before any bt_osal_* API is used; they all dispatch through the
 * table. Asserts if the table cannot be allocated.
 */
void bt_osal_freertos_funcs_init(void);

/**
 * @brief Free the OSAL function table allocated by bt_osal_freertos_funcs_init()
 */
void bt_osal_freertos_funcs_deinit(void);

/**
 * @brief Get the OSAL function table
 *
 * @return Pointer to the table, or NULL if bt_osal_freertos_funcs_init() has not
 *         been called (or the table has since been deinitialized)
 */
struct bt_osal_funcs_t *bt_osal_freertos_funcs_get(void);

#ifdef __cplusplus
}
#endif

#endif  /* _BT_OSAL_FREERTOS_H_ */
