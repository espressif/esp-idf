/*
 * Copyright (c) 2016, Wind River Systems, Inc.
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MESH_KERNEL_
#define _MESH_KERNEL_
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../../bluedroid/osi/include/osi/mutex.h"
#include "mesh_types.h"
#include "mesh_slist.h"
#include "dlist.h"
#include "mesh_atomic.h"

typedef int atomic_t;
typedef atomic_t atomic_val_t;

/* number of nsec per usec */
#define NSEC_PER_USEC 1000

/* number of microseconds per millisecond */
#define USEC_PER_MSEC 1000

/* number of milliseconds per second */
#define MSEC_PER_SEC 1000

/* number of microseconds per second */
#define USEC_PER_SEC ((USEC_PER_MSEC) * (MSEC_PER_SEC))

/* number of nanoseconds per second */
#define NSEC_PER_SEC ((NSEC_PER_USEC) * (USEC_PER_MSEC) * (MSEC_PER_SEC))

#define ATOMIC_MASK(bit) (1 << ((bit) & (ATOMIC_BITS - 1)))
#define ATOMIC_ELEM(addr, bit) ((addr) + ((bit) / ATOMIC_BITS))

/* timeout is not in use */
#define _INACTIVE (-1)

/**
 * @} end defgroup stack_apis
 */
struct k_work;

/**
 * @typedef k_work_handler_t
 * @brief Work item handler function type.
 *
 * A work item's handler function is executed by a workqueue's thread
 * when the work item is processed by the workqueue.
 *
 * @param work Address of the work item.
 *
 * @return N/A
 */
typedef void (*k_work_handler_t)(struct k_work *work);


typedef sys_dlist_t _wait_q_t;

struct k_work {
    void *_reserved;        /* Used by k_queue implementation. */
    k_work_handler_t handler;
    atomic_t flags[1];
    int index;
};

#define _K_WORK_INITIALIZER(work_handler) \
    { \
    ._reserved = NULL, \
    .handler = work_handler, \
    .flags = { 0 } \
    }

struct _timeout;

typedef void (*_timeout_func_t)(struct _timeout *t);

struct _timeout {
    sys_dnode_t node;
    //struct k_thread *thread;
    sys_dlist_t *wait_q;
    s32_t delta_ticks_from_prev;
    _timeout_func_t func;
};

/**
 * @brief Define an array of atomic variables.
 *
 * This macro defines an array of atomic variables containing at least
 * @a num_bits bits.
 *
 * @note
 * If used from file scope, the bits of the array are initialized to zero;
 * if used from within a function, the bits are left uninitialized.
 *
 * @param name Name of array of atomic variables.
 * @param num_bits Number of bits needed.
 */
#define ATOMIC_DEFINE(name, num_bits) \
    atomic_t name[1 + ((num_bits) - 1) / ATOMIC_BITS]

/**
 * @brief Generate null timeout delay.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * not to wait if the requested operation cannot be performed immediately.
 *
 * @return Timeout delay value.
 */
#define K_NO_WAIT 0

/**
 * @brief Generate timeout delay from milliseconds.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a ms milliseconds to perform the requested operation.
 *
 * @param ms Duration in milliseconds.
 *
 * @return Timeout delay value.
 */
#define K_MSEC(ms)               (ms)

/**
 * @brief Generate timeout delay from seconds.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a s seconds to perform the requested operation.
 *
 * @param s Duration in seconds.
 *
 * @return Timeout delay value.
 */
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)

/**
 * @brief Generate timeout delay from minutes.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a m minutes to perform the requested operation.
 *
 * @param m Duration in minutes.
 *
 * @return Timeout delay value.
 */
#define K_MINUTES(m)   K_SECONDS((m) * 60)

/**
 * @brief Generate timeout delay from hours.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a h hours to perform the requested operation.
 *
 * @param h Duration in hours.
 *
 * @return Timeout delay value.
 */
#define K_HOURS(h)     K_MINUTES((h) * 60)

/**
 * @brief Generate infinite timeout delay.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait as long as necessary to perform the requested operation.
 *
 * @return Timeout delay value.
 */
#define K_FOREVER (-1)


/**
 * @brief Get system uptime (32-bit version).
 *
 * This routine returns the lower 32-bits of the elapsed time since the system
 * booted, in milliseconds.
 *
 * This routine can be more efficient than k_uptime_get(), as it reduces the
 * need for interrupt locking and 64-bit math. However, the 32-bit result
 * cannot hold a system uptime time larger than approximately 50 days, so the
 * caller must handle possible rollovers.
 *
 * @return Current uptime.
 */
u32_t k_uptime_get_32(void);

struct k_sem {
    osi_mutex_t mutex;
};

struct k_queue {
    sys_slist_t data_q;
    union {
        _wait_q_t wait_q;
        // TODO: The following not used now, delete it temporarily.
        //_POLL_EVENT;
    };
    // TODO: The following not used now, delete it temporarily.
    //_OBJECT_TRACING_NEXT_PTR(k_queue);
};

struct k_work_q {
    struct k_queue queue;
};



/**
 * @cond INTERNAL_HIDDEN
 */

struct k_fifo {
    struct k_queue _queue;
};

struct k_lifo {
    struct k_queue _queue;
};

struct k_delayed_work {
    struct k_work work;
    struct _timeout timeout;
    struct k_work_q *work_q;
};

#define _K_QUEUE_INITIALIZER(obj) \
    { \
    .wait_q = SYS_DLIST_STATIC_INIT(&obj.wait_q), \
    .data_q = SYS_SLIST_STATIC_INIT(&obj.data_q), \
    }


#define _K_FIFO_INITIALIZER(obj) \
    { \
    ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }

#define _K_LIFO_INITIALIZER(obj) \
    { \
    ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }


/**
 * @brief Statically define and initialize a fifo.
 *
 * The fifo can be accessed outside the module where it is defined using:
 *
 * @code extern struct k_fifo <name>; @endcode
 *
 * @param name Name of the fifo.
 */
#define K_FIFO_DEFINE(name) \
    struct k_fifo name \
        __in_section(_k_queue, static, name) = \
        _K_FIFO_INITIALIZER(name)


/* initialize the timeouts part of k_thread when enabled in the kernel */

static inline void _init_timeout(struct _timeout *t, _timeout_func_t func)
{
    /*
     * Must be initialized here and when dequeueing a timeout so that code
     * not dealing with timeouts does not have to handle this, such as when
     * waiting forever on a semaphore.
     */
    t->delta_ticks_from_prev = _INACTIVE;

    /*
     * Must be initialized here so that k_wakeup can
     * verify the thread is not on a wait queue before aborting a timeout.
     */
    t->wait_q = NULL;

    /*
     * Function must be initialized before being potentially called.
     */
    t->func = func;

    /*
     * These are initialized when enqueing on the timeout queue:
     *
     *   thread->timeout.node.next
     *   thread->timeout.node.prev
     */
}

unsigned int irq_lock(void);

void irq_unlock(unsigned int key);

/**
 *
 * @brief Atomic get.
 *
 * This routine performs an atomic read on @a target.
 *
 * @param target Address of atomic variable.
 *
 * @return Value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline atomic_val_t atomic_get(const atomic_t *target)
{
    return __atomic_load_n(target, __ATOMIC_SEQ_CST);
}
#else
extern atomic_val_t atomic_get(const atomic_t *target);
#endif

/**
 *
 * @brief Atomic bitwise inclusive OR.
 *
 * This routine atomically sets @a target to the bitwise inclusive OR of
 * @a target and @a value.
 *
 * @param target Address of atomic variable.
 * @param value Value to OR.
 *
 * @return Previous value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline atomic_val_t atomic_or(atomic_t *target, atomic_val_t value)
{
    return __atomic_fetch_or(target, value, __ATOMIC_SEQ_CST);
}
#else
extern atomic_val_t atomic_or(atomic_t *target, atomic_val_t value);
#endif

void mesh_k_init(void);

/**
 * @brief Submit a delayed work item to the system workqueue.
 *
 * This routine schedules work item @a work to be processed by the system
 * workqueue after a delay of @a delay milliseconds. The routine initiates
 * an asynchronous countdown for the work item and then returns to the caller.
 * Only when the countdown completes is the work item actually submitted to
 * the workqueue and becomes pending.
 *
 * Submitting a previously submitted delayed work item that is still
 * counting down cancels the existing submission and restarts the countdown
 * using the new delay. If the work item is currently pending on the
 * workqueue's queue because the countdown has completed it is too late to
 * resubmit the item, and resubmission fails without impacting the work item.
 * If the work item has already been processed, or is currently being processed,
 * its work is considered complete and the work item can be resubmitted.
 *
 * @warning
 * Work items submitted to the system workqueue should avoid using handlers
 * that block or yield since this may prevent the system workqueue from
 * processing other work items in a timely manner.
 *
 * @note Can be called by ISRs.
 *
 * @param work Address of delayed work item.
 * @param delay Delay before submitting the work item (in milliseconds).
 *
 * @retval 0 Work item countdown started.
 * @retval -EINPROGRESS Work item is already pending.
 * @retval -EINVAL Work item is being processed or has completed its work.
 * @retval -EADDRINUSE Work item is pending on a different workqueue.
 */
int k_delayed_work_submit(struct k_delayed_work *work,
                          s32_t delay);

/**
 * @brief Get time remaining before a delayed work gets scheduled.
 *
 * This routine computes the (approximate) time remaining before a
 * delayed work gets executed. If the delayed work is not waiting to be
 * scheduled, it returns zero.
 *
 * @param work     Delayed work item.
 *
 * @return Remaining time (in milliseconds).
 */
s32_t k_delayed_work_remaining_get(struct k_delayed_work *work);

/**
 * @brief Submit a work item to the system workqueue.
 *
 * This routine submits work item @a work to be processed by the system
 * workqueue. If the work item is already pending in the workqueue's queue
 * as a result of an earlier submission, this routine has no effect on the
 * work item. If the work item has already been processed, or is currently
 * being processed, its work is considered complete and the work item can be
 * resubmitted.
 *
 * @warning
 * Work items submitted to the system workqueue should avoid using handlers
 * that block or yield since this may prevent the system workqueue from
 * processing other work items in a timely manner.
 *
 * @note Can be called by ISRs.
 *
 * @param work Address of work item.
 *
 * @return N/A
 */
static inline void k_work_submit(struct k_work *work)
{
    // TODO:
    if (work != NULL) {
        work->handler(work);
    }

    return;
    //k_work_submit_to_queue(&k_sys_work_q, work);
}

/**
 * @brief Initialize a work item.
 *
 * This routine initializes a workqueue work item, prior to its first use.
 *
 * @param work Address of work item.
 * @param handler Function to invoke each time work item is processed.
 *
 * @return N/A
 */
static inline void k_work_init(struct k_work *work, k_work_handler_t handler)
{
    work->handler = handler;
}

int k_delayed_work_cancel(struct k_delayed_work *work);

int k_delayed_work_free(struct k_delayed_work *work);

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);

/**
 * @brief Get system uptime.
 *
 * This routine returns the elapsed time since the system booted,
 * in milliseconds.
 *
 * @return Current uptime.
 */
s64_t k_uptime_get(void);

/**
 * @brief Put the current thread to sleep.
 *
 * This routine puts the current thread to sleep for @a duration
 * milliseconds.
 *
 * @param duration Number of milliseconds to sleep.
 *
 * @return N/A
 */
void k_sleep(s32_t duration);

/**
 * @brief Give a semaphore.
 *
 * This routine gives @a sem, unless the semaphore is already at its maximum
 * permitted count.
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return N/A
 */
void k_sem_give(struct k_sem *sem);

/**
 * @brief Initialize a semaphore.
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem Address of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param limit Maximum permitted semaphore count.
 *
 * @return N/A
 */
void k_sem_init(struct k_sem *sem, unsigned int initial_count,
                unsigned int limit);

/**
 * @brief Take a semaphore.
 *
 * This routine takes @a sem.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param sem Address of the semaphore.
 * @param timeout Waiting period to take the semaphore (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @note When porting code from the nanokernel legacy API to the new API, be
 * careful with the return value of this function. The return value is the
 * reverse of the one of nano_sem_take family of APIs: 0 means success, and
 * non-zero means failure, while the nano_sem_take family returns 1 for success
 * and 0 for failure.
 *
 * @retval 0 Semaphore taken.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int k_sem_take(struct k_sem *sem, s32_t timeout);

#endif /* #ifndef _MESH_KERNEL_ */

