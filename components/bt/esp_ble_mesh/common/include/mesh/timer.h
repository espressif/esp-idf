/*
 * SPDX-FileCopyrightText: 2016 Wind River Systems, Inc.
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_TIMER_H_
#define _BLE_MESH_TIMER_H_

#include "mesh/types.h"
#include "mesh/slist.h"
#include "mesh/atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

/* number of nsec per usec */
#define NSEC_PER_USEC   1000

/* number of microseconds per millisecond */
#define USEC_PER_MSEC   1000

/* number of milliseconds per second */
#define MSEC_PER_SEC    1000

/* number of microseconds per second */
#define USEC_PER_SEC    ((USEC_PER_MSEC) * (MSEC_PER_SEC))

/* number of nanoseconds per second */
#define NSEC_PER_SEC    ((NSEC_PER_USEC) * (USEC_PER_MSEC) * (MSEC_PER_SEC))

/* timeout is not in use */
#define _INACTIVE       (-1)

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

struct k_work {
    k_work_handler_t handler;
    int index;
    void *user_data;
};

#define _K_WORK_INITIALIZER(work_handler)   \
{                                           \
    .handler = work_handler,                \
    .user_data = NULL,                      \
}

/**
 * @brief Generate null timeout delay.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * not to wait if the requested operation cannot be performed immediately.
 *
 * @return Timeout delay value.
 */
#define K_NO_WAIT       0

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
#define K_MSEC(ms)      (ms)

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
#define K_SECONDS(s)    K_MSEC((s) * MSEC_PER_SEC)

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
#define K_MINUTES(m)    K_SECONDS((m) * 60)

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
#define K_HOURS(h)      K_MINUTES((h) * 60)

/**
 * @brief Generate timeout delay from days.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a d days to perform the requested operation.
 *
 * @param d Duration in days.
 *
 * @return Timeout delay value.
 */
#define K_DAYS(d)       K_HOURS((d) * 24)

/**
 * @brief Generate infinite timeout delay.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait as long as necessary to perform the requested operation.
 *
 * @return Timeout delay value.
 */
#define K_FOREVER       (-1)

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
uint32_t k_uptime_get_32(void);

struct k_delayed_work {
    struct k_work work;
};

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
int k_delayed_work_submit(struct k_delayed_work *work, int32_t delay);

int k_delayed_work_submit_periodic(struct k_delayed_work *work, int32_t period);

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
int32_t k_delayed_work_remaining_get(struct k_delayed_work *work);

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
    if (work && work->handler) {
        work->handler(work);
    }
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

int k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);

/**
 * @brief Get system uptime.
 *
 * This routine returns the elapsed time since the system booted,
 * in milliseconds.
 *
 * @return Current uptime.
 */
int64_t k_uptime_get(void);

void bt_mesh_timer_init(void);
void bt_mesh_timer_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_TIMER_H_ */
