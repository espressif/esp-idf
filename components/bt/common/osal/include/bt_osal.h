/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file bt_osal.h
 * @brief OS Abstraction Layer (OSAL) for the Bluetooth host
 *
 * Provides the OS primitives the Bluetooth host relies on: event queues,
 * mutexes, semaphores, callouts (one-shot timers), time conversion and
 * critical sections.
 *
 * Every API declared here dispatches through the function table installed by
 * bt_osal_freertos_funcs_init(), which MUST be called before any other bt_osal_*
 * API is used.
 *
 */

#ifndef _BT_OSAL_H_
#define _BT_OSAL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bt_osal_event;

/**
 * @brief Event handler invoked when a queued event is dispatched
 *
 * @param ev Event being dispatched. Use bt_osal_event_get_arg() to retrieve the
 *           argument registered at bt_osal_event_init() time.
 */
typedef void bt_osal_event_fn(struct bt_osal_event *ev);

/**
 * @brief OSAL error codes
 */
enum bt_osal_error {
    BT_OSAL_OK = 0,                /*!< Success */
    BT_OSAL_ENOMEM = 1,            /*!< Out of memory */
    BT_OSAL_EINVAL = 2,            /*!< Invalid argument or invalid object state */
    BT_OSAL_INVALID_PARAM = 3,     /*!< Object is not initialized, or allocation for it failed */
    BT_OSAL_MEM_NOT_ALIGNED = 4,   /*!< Memory is not aligned as required */
    BT_OSAL_BAD_MUTEX = 5,         /*!< Mutex is not owned by the calling task */
    BT_OSAL_TIMEOUT = 6,           /*!< Operation timed out before it could complete */
    BT_OSAL_ERR_IN_ISR = 7,        /*!< Operation is not allowed from interrupt context */
    BT_OSAL_ERR_PRIV = 8,          /*!< Operation requires privileges the caller does not have */
    BT_OSAL_OS_NOT_STARTED = 9,    /*!< The OS scheduler has not been started yet */
    BT_OSAL_ENOENT = 10,           /*!< Requested object does not exist */
    BT_OSAL_EBUSY = 11,            /*!< Object is busy */
    BT_OSAL_ERROR = 12,            /*!< Unspecified error */
};

/**
 * @brief OSAL error code type
 */
typedef enum bt_osal_error bt_osal_error_t;

/* Include OS-specific definitions */
#include "bt_osal_os.h"

/*
 * Generic
 */

/**
 * @brief Check whether the OS scheduler is running
 *
 * @return true if the scheduler has been started, false otherwise
 */
bool bt_osal_os_started(void);

/**
 * @brief Get an opaque identifier of the calling task
 *
 * @return Handle of the calling task
 */
void *bt_osal_get_current_task_id(void);

/*
 * Event queue
 */

/**
 * @brief Initialize an event queue
 *
 * Allocates the queue's backing storage on first use. Calling this on an
 * already-initialized queue reuses the storage and discards any pending events,
 * leaving the queue empty.
 *
 * @note Allocates memory, so it must not be called from interrupt context.
 *
 * @param evq Event queue to initialize
 */
void bt_osal_eventq_init(struct bt_osal_eventq *evq);

/**
 * @brief Deinitialize an event queue and release its resources
 *
 * Deletes the queue's processing task if one was started, then frees the
 * backing storage. It is safe to call this on a queue that was never
 * initialized or already deinitialized.
 *
 * @param evq Event queue to deinitialize
 */
void bt_osal_eventq_deinit(struct bt_osal_eventq *evq);

/**
 * @brief Dequeue the next event, waiting for one if the queue is empty
 *
 * The returned event is no longer marked as queued, so it may be posted again.
 *
 * @note From interrupt context @p tmo MUST be 0; a non-zero timeout asserts.
 *
 * @param evq Event queue to take the event from
 * @param tmo Maximum time to wait, in ticks. Use 0 to poll, or
 *            BT_OSAL_TIME_FOREVER to wait indefinitely.
 *
 * @return Next event in the queue, or NULL if none became available within @p tmo
 */
struct bt_osal_event *bt_osal_eventq_get(struct bt_osal_eventq *evq,
                                         bt_osal_time_t tmo);

/**
 * @brief Append an event to the back of an event queue
 *
 * Does nothing if @p ev is already queued, so an event is never enqueued twice.
 *
 * @param evq Event queue to post to
 * @param ev  Event to post
 */
void bt_osal_eventq_put(struct bt_osal_eventq *evq, struct bt_osal_event *ev);

/**
 * @brief Insert an event at the front of an event queue
 *
 * Same as bt_osal_eventq_put() except the event is dequeued before all events
 * already pending. Does nothing if @p ev is already queued.
 *
 * @param evq Event queue to post to
 * @param ev  Event to post
 */
void bt_osal_eventq_put_to_front(struct bt_osal_eventq *evq,
                                 struct bt_osal_event *ev);

/**
 * @brief Remove a pending event from an event queue
 *
 * Does nothing if @p ev is not currently queued.
 *
 * @param evq Event queue holding the event
 * @param ev  Event to remove
 */
void bt_osal_eventq_remove(struct bt_osal_eventq *evq,
                           struct bt_osal_event *ev);

/**
 * @brief Start a dedicated task that processes an event queue
 *
 * Spawns a task that blocks on bt_osal_eventq_get() and dispatches each dequeued
 * event with bt_osal_event_run(), so events posted to @p evq are handled
 * asynchronously in this task's context. The queue must already be initialized
 * with bt_osal_eventq_init().
 *
 * @note Only one processing task may be started per queue.
 *
 * @param evq  Event queue to process
 * @param info Attributes (name, priority, stack size, core affinity) of the
 *             processing task; @c info->name must not be NULL. Set
 *             @c info->core_id to BT_OSAL_TASK_NO_AFFINITY to leave the task
 *             unpinned — a zero-initialized @p info pins it to core 0.
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_EINVAL if @p info or @c info->name is NULL, @c info->core_id is
 *        neither a valid core nor BT_OSAL_TASK_NO_AFFINITY, the queue is not
 *        initialized, or it already has a processing task
 *      - BT_OSAL_ENOMEM if the task could not be created
 */
bt_osal_error_t bt_osal_eventq_start(struct bt_osal_eventq *evq,
                              const struct bt_osal_task_info *info);

/**
 * @brief Post a one-shot function to be run from an event queue
 *
 * Allocates a self-contained event, posts it to @p evq, and runs @p fn when the
 * event is dispatched by the queue's processing task (see bt_osal_eventq_start()).
 * The event's backing storage is freed automatically after @p fn returns, so no
 * event object needs to be managed by the caller. Inside @p fn the argument
 * @p arg is retrievable via bt_osal_event_get_arg().
 *
 * @note Allocates memory, so it must not be called from interrupt context.
 *
 * @param evq Event queue to post the work to
 * @param fn  Handler to run when the event is dispatched
 * @param arg Argument passed to @p fn
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_EINVAL if the queue is not initialized or @p fn is NULL
 *      - BT_OSAL_ERR_IN_ISR if called from interrupt context
 *      - BT_OSAL_ENOMEM if the work item could not be allocated
 */
bt_osal_error_t bt_osal_eventq_post_func(struct bt_osal_eventq *evq,
                                bt_osal_event_fn *fn, void *arg);

/**
 * @brief Initialize an event with its handler and argument
 *
 * Allocates the event's backing storage on first use. Calling this on an
 * already-initialized event reuses the storage and overwrites the handler and
 * argument.
 *
 * @note Allocates memory, so it must not be called from interrupt context.
 *
 * @param ev  Event to initialize
 * @param fn  Handler to run when the event is dispatched
 * @param arg Argument passed to @p fn, retrievable via bt_osal_event_get_arg()
 */
void bt_osal_event_init(struct bt_osal_event *ev, bt_osal_event_fn *fn,
                        void *arg);

/**
 * @brief Deinitialize an event and free its backing storage
 *
 * The event must not be queued; remove it with bt_osal_eventq_remove() first.
 * It is safe to call this on an event that was never initialized or already
 * deinitialized.
 *
 * @param ev Event to deinitialize
 */
void bt_osal_event_deinit(struct bt_osal_event *ev);

/**
 * @brief Clear the queued flag of an event
 *
 * Only clears the flag; it does NOT unlink the event from a queue it is still
 * pending on. To take a queued event out of its queue, use
 * bt_osal_eventq_remove().
 *
 * @warning The event MUST NOT be queued when this is called. Resetting a still-
 *          queued event desynchronizes its flag from the queue and, with the
 *          intrusive-list backend, leaves it linked while marked free — a later
 *          put() then relinks the same node and corrupts the list. Callers must
 *          bt_osal_eventq_remove() (or let it be dispatched) first; the flag is
 *          checked with an assertion in debug builds.
 *
 * @param ev Event to reset (must not be currently queued)
 */
void bt_osal_event_reset(struct bt_osal_event *ev);

/**
 * @brief Check whether an event is pending in an event queue
 *
 * @param ev Event to query
 *
 * @return true if the event is queued and not yet dispatched, false otherwise
 */
bool bt_osal_event_is_queued(struct bt_osal_event *ev);

/**
 * @brief Get the argument registered with an event
 *
 * @param ev Event to query
 *
 * @return Argument passed to bt_osal_event_init() or bt_osal_event_set_arg()
 */
void *bt_osal_event_get_arg(struct bt_osal_event *ev);

/**
 * @brief Replace the argument registered with an event
 *
 * @param ev  Event to update
 * @param arg New argument to pass to the event handler
 */
void bt_osal_event_set_arg(struct bt_osal_event *ev, void *arg);

/**
 * @brief Check whether an event queue holds no pending events
 *
 * @param evq Event queue to query
 *
 * @return true if the queue is empty, false otherwise
 */
bool bt_osal_eventq_is_empty(struct bt_osal_eventq *evq);

/**
 * @brief Run an event's handler in the calling context
 *
 * Invokes the handler registered by bt_osal_event_init() synchronously; the
 * event is not required to have been queued.
 *
 * @param ev Event to dispatch
 */
void bt_osal_event_run(struct bt_osal_event *ev);

/*
 * Mutexes
 */

/**
 * @brief Initialize a recursive mutex
 *
 * Allocates the mutex on first use; calling this on an already-initialized
 * mutex leaves it untouched and succeeds.
 *
 * @note Allocates memory, so it must not be called from interrupt context.
 *
 * @param mu Mutex to initialize
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_ENOMEM if the mutex could not be allocated
 */
bt_osal_error_t bt_osal_mutex_init(struct bt_osal_mutex *mu);

/**
 * @brief Acquire a mutex, waiting for it if it is held by another task
 *
 * The mutex is recursive: a task already owning it acquires it again without
 * blocking, and must release it once per successful pend.
 *
 * @note Must not be called from interrupt context; doing so asserts.
 *
 * @param mu      Mutex to acquire
 * @param timeout Maximum time to wait, in ticks. Use 0 to try without blocking,
 *                or BT_OSAL_TIME_FOREVER to wait indefinitely.
 *
 * @return
 *      - BT_OSAL_OK if the mutex was acquired
 *      - BT_OSAL_TIMEOUT if it was still held when @p timeout elapsed
 *      - BT_OSAL_INVALID_PARAM if the mutex is not initialized
 */
bt_osal_error_t bt_osal_mutex_pend(struct bt_osal_mutex *mu,
                                   bt_osal_time_t timeout);

/**
 * @brief Release a mutex held by the calling task
 *
 * A recursively acquired mutex is only released to other tasks once this has
 * been called as many times as it was successfully pended.
 *
 * @note Must not be called from interrupt context; doing so asserts.
 *
 * @param mu Mutex to release
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_BAD_MUTEX if the calling task does not own the mutex
 *      - BT_OSAL_INVALID_PARAM if the mutex is not initialized
 */
bt_osal_error_t bt_osal_mutex_release(struct bt_osal_mutex *mu);

/**
 * @brief Deinitialize a mutex and free its resources
 *
 * The mutex must not be held when it is deinitialized.
 *
 * @param mu Mutex to deinitialize
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_INVALID_PARAM if the mutex is not initialized
 */
bt_osal_error_t bt_osal_mutex_deinit(struct bt_osal_mutex *mu);

/*
 * Semaphores
 */

/**
 * @brief Initialize a counting semaphore
 *
 * Allocates the semaphore on first use; calling this on an already-initialized
 * semaphore leaves it untouched (including its token count) and succeeds.
 *
 * @note Allocates memory, so it must not be called from interrupt context.
 *
 * @param sem    Semaphore to initialize
 * @param tokens Initial number of tokens
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_ENOMEM if the semaphore could not be allocated
 */
bt_osal_error_t bt_osal_sem_init(struct bt_osal_sem *sem, uint16_t tokens);

/**
 * @brief Take one token from a semaphore, waiting if none is available
 *
 * @note From interrupt context @p timeout MUST be 0; a non-zero timeout asserts.
 *
 * @param sem     Semaphore to take a token from
 * @param timeout Maximum time to wait, in ticks. Use 0 to try without blocking,
 *                or BT_OSAL_TIME_FOREVER to wait indefinitely.
 *
 * @return
 *      - BT_OSAL_OK if a token was taken
 *      - BT_OSAL_TIMEOUT if no token became available within @p timeout
 *      - BT_OSAL_INVALID_PARAM if the semaphore is not initialized
 */
bt_osal_error_t bt_osal_sem_pend(struct bt_osal_sem *sem,
                                 bt_osal_time_t timeout);

/**
 * @brief Return one token to a semaphore, unblocking a waiter if any
 *
 * @param sem Semaphore to give a token to
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_INVALID_PARAM if the semaphore is not initialized
 *      - BT_OSAL_ERROR if the token could not be returned (semaphore already full)
 */
bt_osal_error_t bt_osal_sem_release(struct bt_osal_sem *sem);

/**
 * @brief Deinitialize a semaphore and free its resources
 *
 * No task may be pending on the semaphore when it is deinitialized.
 *
 * @param sem Semaphore to deinitialize
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_INVALID_PARAM if the semaphore is not initialized
 */
bt_osal_error_t bt_osal_sem_deinit(struct bt_osal_sem *sem);

/**
 * @brief Get the number of tokens currently held by a semaphore
 *
 * @param sem Semaphore to query
 *
 * @return Current token count
 */
uint16_t bt_osal_sem_get_count(struct bt_osal_sem *sem);

/*
 * Callouts
 */

/**
 * @brief Initialize a callout (one-shot timer)
 *
 * The callout is created stopped; arm it with bt_osal_callout_reset().
 *
 * On expiry, if @p evq is non-NULL the callout's event is posted to that queue
 * and @p ev_cb runs in the context of the task processing the queue. If @p evq
 * is NULL, @p ev_cb is instead run directly in timer context, where it must not
 * block and must keep its stack usage small.
 *
 * Allocates the callout's backing storage on first use; calling this on an
 * already-initialized callout reuses the storage and rebinds the queue, handler
 * and argument.
 *
 * @note Allocates memory, so it must not be called from interrupt context.
 *
 * @param co     Callout to initialize
 * @param evq    Event queue to post the event to on expiry, or NULL to run
 *               @p ev_cb in timer context
 * @param ev_cb  Handler to run when the callout expires
 * @param ev_arg Argument passed to @p ev_cb
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_ENOMEM if the callout or its underlying timer could not be created
 */
bt_osal_error_t bt_osal_callout_init(struct bt_osal_callout *co, struct bt_osal_eventq *evq,
                          bt_osal_event_fn *ev_cb, void *ev_arg);

/**
 * @brief Stop a callout, delete its timer and free its resources
 *
 * It is safe to call this on a callout that was never initialized or already
 * deinitialized.
 *
 * @param co Callout to deinitialize
 */
void bt_osal_callout_deinit(struct bt_osal_callout *co);

/**
 * @brief (Re)arm a callout to expire after the given delay
 *
 * Stops the callout if it is already running, then restarts it, so the delay is
 * always measured from this call.
 *
 * @param co    Callout to arm
 * @param ticks Delay before expiry, in ticks
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_INVALID_PARAM or BT_OSAL_EINVAL if the underlying timer rejected the request
 *      - BT_OSAL_ERROR on any other failure
 */
bt_osal_error_t bt_osal_callout_reset(struct bt_osal_callout *co,
                                      bt_osal_time_t ticks);

/**
 * @brief Stop a callout so it will not expire
 *
 * Does nothing if the callout is not running. If a previous expiry already
 * posted an event to the callout's event queue, that event is removed from
 * the queue as part of stopping (unless it has already been dequeued for processing).
 *
 * @param co Callout to stop
 */
void bt_osal_callout_stop(struct bt_osal_callout *co);

/**
 * @brief Check whether a callout is armed and has not expired yet
 *
 * @param co Callout to query
 *
 * @return true if the callout is running, false otherwise
 */
bool bt_osal_callout_is_active(struct bt_osal_callout *co);

/**
 * @brief Get the absolute time at which a callout will expire
 *
 * @param co Callout to query
 *
 * @return Expiry time in ticks, or 0 if it cannot be determined
 */
bt_osal_time_t bt_osal_callout_get_ticks(struct bt_osal_callout *co);

/**
 * @brief Get the time left before a callout expires
 *
 * @param co   Callout to query
 * @param time Reference time to measure from, in ticks (typically bt_osal_time_get())
 *
 * @return Ticks remaining until expiry, or 0 if the callout has already expired
 *         at @p time or its expiry time cannot be determined
 */
bt_osal_time_t bt_osal_callout_remaining_ticks(struct bt_osal_callout *co,
                                               bt_osal_time_t time);

/**
 * @brief Replace the argument passed to a callout's handler
 *
 * @param co  Callout to update
 * @param arg New argument to pass to the handler on expiry
 */
void bt_osal_callout_set_arg(struct bt_osal_callout *co,
                             void *arg);

/**
 * @brief Clear the queued flag of a callout's event
 *
 * Lets the callout be armed again after its event was dropped without being
 * dispatched. It does not stop the timer.
 *
 * @param co Callout whose event should be reset
 */
void bt_osal_callout_mem_reset(struct bt_osal_callout *co);

/*
 * Time functions
 */

/**
 * @brief Get the current time
 *
 * @return Time since boot, in ticks
 */
bt_osal_time_t bt_osal_time_get(void);

/**
 * @brief Get the tick value that means "wait forever"
 *
 * Use it as the timeout of a pend/get to block indefinitely. Also available as
 * the BT_OSAL_TIME_FOREVER macro.
 *
 * @return Tick value representing an infinite timeout
 */
bt_osal_time_t bt_osal_get_time_forever(void);

/**
 * @brief Convert milliseconds to ticks
 *
 * @param[in]  ms        Duration in milliseconds
 * @param[out] out_ticks Resulting duration in ticks
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_EINVAL if the result does not fit in bt_osal_time_t
 */
bt_osal_error_t bt_osal_time_ms_to_ticks(uint32_t ms, bt_osal_time_t *out_ticks);

/**
 * @brief Convert ticks to milliseconds
 *
 * @param[in]  ticks  Duration in ticks
 * @param[out] out_ms Resulting duration in milliseconds
 *
 * @return
 *      - BT_OSAL_OK on success
 *      - BT_OSAL_EINVAL if the result does not fit in uint32_t
 */
bt_osal_error_t bt_osal_time_ticks_to_ms(bt_osal_time_t ticks, uint32_t *out_ms);

/**
 * @brief Convert milliseconds to ticks, without overflow checking
 *
 * @param ms Duration in milliseconds
 *
 * @return Duration in ticks
 */
bt_osal_time_t bt_osal_time_ms_to_ticks32(uint32_t ms);

/**
 * @brief Convert ticks to milliseconds, without overflow checking
 *
 * @param ticks Duration in ticks
 *
 * @return Duration in milliseconds
 */
uint32_t bt_osal_time_ticks_to_ms32(bt_osal_time_t ticks);

/**
 * @brief Block the calling task for the given duration
 *
 * @note Must not be called from interrupt context.
 *
 * @param ticks Duration to block for, in ticks
 */
void bt_osal_time_delay(bt_osal_time_t ticks);

/*
 * Hardware-specific
 *
 * These symbols should be most likely defined by application since they are
 * specific to hardware, not to OS.
 */

#if NIMBLE_CFG_CONTROLLER

/**
 * @brief Install an interrupt handler for the given interrupt
 *
 * @param irqn Interrupt number to install the handler for
 * @param addr Address of the interrupt handler
 */
void bt_osal_hw_set_isr(int irqn, uint32_t addr);

#endif

/**
 * @brief Enter a critical section, disabling interrupts
 *
 * Critical sections nest: each call must be paired with a
 * bt_osal_hw_exit_critical() call, and interrupts are only re-enabled by the
 * outermost one. Keep the section as short as possible and never block in it.
 *
 * @return Context to be passed back to bt_osal_hw_exit_critical()
 */
uint32_t bt_osal_hw_enter_critical(void);

/**
 * @brief Leave a critical section entered with bt_osal_hw_enter_critical()
 *
 * @param ctx Context returned by the matching bt_osal_hw_enter_critical() call
 */
void bt_osal_hw_exit_critical(uint32_t ctx);

/**
 * @brief Check whether the caller is inside a critical section
 *
 * @return true if at least one critical section is currently entered, false otherwise
 */
bool bt_osal_hw_is_in_critical(void);

#ifdef __cplusplus
}
#endif

#endif  /* _BT_OSAL_H_ */
