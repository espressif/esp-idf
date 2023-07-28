/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "osi/semaphore.h"
#include "esp_task.h"
#include "bt_common.h"

#define OSI_THREAD_MAX_TIMEOUT OSI_SEM_MAX_TIMEOUT

struct osi_thread;
struct osi_event;

typedef struct osi_thread osi_thread_t;

typedef void (*osi_thread_func_t)(void *context);

typedef enum {
    OSI_THREAD_CORE_0 = 0,
    OSI_THREAD_CORE_1,
    OSI_THREAD_CORE_AFFINITY,
} osi_thread_core_t;

/*
 * brief: Create a thread or task
 * param name: thread name
 * param stack_size: thread stack size
 * param priority: thread priority
 * param core: the CPU core which this thread run, OSI_THREAD_CORE_AFFINITY means unspecific CPU core
 * param work_queue_num: speicify queue number, the queue[0] has highest priority, and the priority is decrease by index
 * return : if create successfully, return thread handler; otherwise return NULL.
 */
osi_thread_t *osi_thread_create(const char *name, size_t stack_size, int priority, osi_thread_core_t core, uint8_t work_queue_num, const size_t work_queue_len[]);

/*
 * brief: Destroy a thread or task
 * param thread: point of thread handler
 */
void osi_thread_free(osi_thread_t *thread);

/*
 * brief: Post an msg to a thread and told the thread call the function
 * param thread: point of thread handler
 * param func: callback function that called by target thread
 * param context: argument of callback function
 * param queue_idx: the queue which the msg send to
 * param timeout: post timeout, OSI_THREAD_MAX_TIMEOUT means blocking forever, 0 means never blocking, others means block millisecond
 * return : if post successfully, return true, otherwise return false
 */
bool osi_thread_post(osi_thread_t *thread, osi_thread_func_t func, void *context, int queue_idx, uint32_t timeout);

/*
 * brief: Set the priority of thread
 * param thread: point of thread handler
 * param priority: priority
 * return : if set successfully, return true, otherwise return false
 */
bool osi_thread_set_priority(osi_thread_t *thread, int priority);

/* brief: Get thread name
 * param thread: point of thread handler
 * return: constant point of thread name
 */
const char *osi_thread_name(osi_thread_t *thread);

/* brief: Get the size of the specified queue
 * param thread: point of thread handler
 * param wq_idx: the queue index of the thread
 * return: queue size
 */
int osi_thread_queue_wait_size(osi_thread_t *thread, int wq_idx);

/*
 * brief: Create an osi_event struct and register the handler function and its argument
 *        An osi_event is a kind of work that can be posted to the workqueue of osi_thread to process,
 *        but the work can have at most one instance the thread workqueue before it is processed. This
 *        allows the "single post, multiple data processing" jobs.
 * param func: the handler to process the job
 * param context: the argument to be passed to the handler function when the job is being processed
 * return: NULL if no memory, otherwise a valid struct pointer
 */
struct osi_event *osi_event_create(osi_thread_func_t func, void *context);

/*
 * brief: Bind an osi_event to a specific work queue for an osi_thread.
 *        After binding is completed, a function call of API osi_thread_post_event will send a work
 *        to the workqueue of the thread, with specified queue index.
 * param func: event: the pointer to osi_event that is created using osi_event_create
 * param thread: the pointer to osi_thread that is created using osi_thread_create
 * param queue_idx: the index of the workqueue of the specified osi_thread, with range starting from 0 to work_queue_num - 1
 * return: true if osi_event binds to the thread's workqueue successfully, otherwise false
 */
bool osi_event_bind(struct osi_event* event, osi_thread_t *thread, int queue_idx);

/*
 * brief: Destroy the osi_event struct created by osi_event_create and free the allocated memory
 * param event: the pointer to osi_event
 */
void osi_event_delete(struct osi_event* event);

/*
 * brief: try sending a work to the binded thread's workqueue, so that it can be handled by the worker thread
 * param event: pointer to osi_event, created by osi_event_create
 * param timeout: post timeout, OSI_THREAD_MAX_TIMEOUT means blocking forever, 0 means never blocking, others means block millisecond
 * return: true if the message is enqueued to the thread workqueue, otherwise failed
 * note: if the return value of function is false, it is the case that the workqueue of the thread is full, and users
 *       are expected to post the event sometime later to get the work handled.
 */
bool osi_thread_post_event(struct osi_event *event, uint32_t timeout);

#endif /* __THREAD_H__ */
