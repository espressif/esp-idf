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

#include <string.h>

#include "osi/allocator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "osi/semaphore.h"
#include "osi/thread.h"
#include "stdatomic.h"

typedef struct work_item_s {
    osi_thread_func_t func;
    void *context;
} work_item_t;

typedef struct work_queue_s {
    QueueHandle_t queue;
} work_queue_t;

struct osi_thread_s {
    TaskHandle_t task;              /*!< Store the task object */
    atomic_uintptr_t signal;        /*!< task thread signal */
    uint8_t work_queue_num;         /*!< Number of work queues */
    work_queue_t work_queues[];     /*!< variable length queue array */
};

struct osi_event {
    osi_thread_t *thread;
    work_item_t item;
    atomic_bool is_queued;
    uint8_t queue_idx;
};

static const size_t DEFAULT_WORK_QUEUE_CAPACITY = 100;
#define DEFAULT_JOIN_TIMEOUT_MS         1000U

static bool osi_work_queue_create(work_queue_t* work_queue, size_t capacity)
{
    work_queue->queue = xQueueCreate(capacity, sizeof(work_item_t));
    if (work_queue->queue) {
        return true;
    }
    return false;
}

static void osi_work_queue_delete(work_queue_t* work_queue)
{
    if (work_queue->queue) {
        vQueueDelete(work_queue->queue);
        work_queue->queue = NULL;
    }
}

static uint32_t osi_thread_task_take(uint32_t timeout_ms)
{
    return ulTaskNotifyTake(pdTRUE, osi_ms_to_ticks(timeout_ms));
}

static void osi_thread_task_give(TaskHandle_t handle)
{
    if (handle) {
        xTaskNotifyGive(handle);
    }
}

static bool osi_thead_work_queue_get(work_queue_t* work_queue, work_item_t *item)
{
    assert (work_queue->queue != NULL);
    assert (item != NULL);

    if (xQueueReceive(work_queue->queue, item, 0) == pdTRUE) {
        OSI_TRACE_VERBOSE("queue item received");
        return true;
    }
    OSI_TRACE_VERBOSE("queue empty");
    return false;
}

static bool osi_thead_work_queue_put(work_queue_t* work_queue, const work_item_t *item, uint32_t timeout_ms)
{
    assert (work_queue->queue != NULL);
    assert (item != NULL);

    if (xQueueSend(work_queue->queue, item, osi_ms_to_ticks(timeout_ms)) == pdTRUE) {
        return true;
    }
    return false;
}

static size_t osi_thead_work_queue_len(work_queue_t* work_queue)
{
    assert (work_queue->queue != NULL);

    return (size_t)uxQueueMessagesWaiting(work_queue->queue);
}

static void osi_thread_run(void *arg)
{
    osi_thread_t *thread = (osi_thread_t*)arg;
    work_item_t item;
    uint8_t idx;

    OSI_TRACE_DEBUG("task %p started", thread->task);
    // signal task that we are running and clear value
    osi_thread_task_give((TaskHandle_t)atomic_exchange(&thread->signal, 0));

    do {
        // wait to be notified through thread->task
        osi_thread_task_take(OSI_SEM_MAX_TIMEOUT);
        OSI_TRACE_VERBOSE("task %p active", thread->task);

        idx = 0;
        // we should exit when thread->signal is set
        while ((atomic_load(&thread->signal) == 0) && idx < thread->work_queue_num) {
            if (osi_thead_work_queue_get(&thread->work_queues[idx], &item) == true) {
                item.func(item.context);
                idx = 0;
            } else {
                idx++;
            }
        }
    } while (atomic_load(&thread->signal) == 0);

    OSI_TRACE_WARNING("task %p exiting", thread->task);

    thread->task = NULL;
    // notify that we are done
    osi_thread_task_give((TaskHandle_t)atomic_exchange(&thread->signal, 0));

    vTaskDelete(NULL);
}

static inline uint32_t osi_thread_join(osi_thread_t *thread, uint32_t wait_ms)
{
    // wake up osi thread
    osi_thread_task_give(thread->task);
    // wait for osi thread to complete
    return osi_thread_task_take(wait_ms);
}

static void osi_thread_free_internal(osi_thread_t *thread)
{
    // setting values to 0/NULL to prevent potential use after free/double free
    if (thread->task)
    {
        vTaskDelete(thread->task);
        thread->task = NULL;
    }
    OSI_TRACE_DEBUG("freeing thread %p", thread);
    for (uint8_t n = 0; n < thread->work_queue_num; n++) {
        osi_work_queue_delete(&thread->work_queues[n]);
    }
    thread->work_queue_num = 0;
    osi_free(thread);
}

static void osi_thread_stop(osi_thread_t *thread)
{
    const TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    // don't try to stop yourself
    if (handle != thread->task) {
        // set signal value to end task in osi_thread_run
        atomic_store(&thread->signal, (uintptr_t)handle);
        // join
        OSI_TRACE_DEBUG("waiting for task %p to join", thread->task);
        osi_thread_join(thread, DEFAULT_JOIN_TIMEOUT_MS);
        // free resources
        osi_thread_free_internal(thread);
    }
}

//in linux, the stack_size, priority and core may not be set here, the code will be ignore the arguments
osi_thread_t *osi_thread_create(const char *name, size_t stack_size, int priority, osi_thread_core_t core, uint8_t work_queue_num, const size_t work_queue_len[])
{
    if (stack_size == 0 ||
            core < OSI_THREAD_CORE_0 || core > OSI_THREAD_CORE_AFFINITY ||
            work_queue_num == 0) {
        OSI_TRACE_ERROR("thread [%s] invalid params", name ? name : __func__);
        return NULL;
    }

    osi_thread_t *thread = (osi_thread_t *)osi_calloc(sizeof(osi_thread_t) + work_queue_num * sizeof(work_queue_t));
    if (thread == NULL) {
        OSI_TRACE_ERROR("thread alloc");
        return NULL;
    }
    OSI_TRACE_VERBOSE("new thread %p with %u work queues", thread, work_queue_num);

    for (uint8_t *i = &thread->work_queue_num; *i < work_queue_num; ++*i) {
        size_t queue_len = work_queue_len[*i] ? work_queue_len[*i] : DEFAULT_WORK_QUEUE_CAPACITY;
        if (osi_work_queue_create(&thread->work_queues[*i], queue_len) == false) {
            OSI_TRACE_ERROR("thread work queue[%u] create", *i);
            goto _err;
        }
    }

    atomic_store(&thread->signal, (uintptr_t)xTaskGetCurrentTaskHandle());

    if (xTaskCreatePinnedToCore(osi_thread_run, name, stack_size, thread, priority, &thread->task, core) == pdPASS) {
        OSI_TRACE_DEBUG("waiting for new task %p to start", thread->task);
        // wait for osi thread task to start
        osi_thread_task_take(OSI_SEM_MAX_TIMEOUT);

        return thread;
    }
    OSI_TRACE_ERROR("task create");

_err:
    osi_thread_free_internal(thread);
    return NULL;
}

void osi_thread_free(osi_thread_t *thread)
{
    if (thread) {
        OSI_TRACE_DEBUG("stopping task %p", thread->task);
        osi_thread_stop(thread);
    }
}

bool osi_thread_post(osi_thread_t *thread, osi_thread_func_t func, void *context, int queue_idx, uint32_t timeout)
{
    assert(thread != NULL);
    assert(func != NULL);

    if ((uint32_t)queue_idx < thread->work_queue_num) {
        work_item_t item = {
            .func = func,
            .context = context
        };

        if (osi_thead_work_queue_put(&thread->work_queues[queue_idx], &item, timeout) == true) {
            OSI_TRACE_VERBOSE("thread %p new item in work queue[%d]", thread, queue_idx);
            osi_thread_task_give(thread->task);
            return true;
        }
        OSI_TRACE_ERROR("thread %p work queue[%u] full", thread, queue_idx);
    } else {
        OSI_TRACE_ERROR("thread %p post to invalid work queue[%d]", thread, queue_idx);
    }
    return false;
}

bool osi_thread_set_priority(osi_thread_t *thread, int priority)
{
    assert(thread != NULL);

    vTaskPrioritySet(thread->task, priority);
    return true;
}

const char *osi_thread_name(osi_thread_t *thread)
{
    assert(thread != NULL);

    return pcTaskGetName(thread->task);
}

int osi_thread_queue_wait_size(osi_thread_t *thread, int wq_idx)
{
    assert (thread != NULL);

    if ((uint32_t)wq_idx >= thread->work_queue_num) {
        OSI_TRACE_ERROR("thread %p invalid work queue index", thread);
        return 0;
    }

    return (int)osi_thead_work_queue_len(&thread->work_queues[wq_idx]);
}

osi_event_t *osi_event_create(osi_thread_func_t func, void *context)
{
    osi_event_t *event = NULL;
    if (func) {
        event = osi_calloc(sizeof(osi_event_t));
        if (event) {
            event->item.func = func;
            event->item.context = context;
        }
    }
    return event;
}

void osi_event_delete(osi_event_t* event)
{
    if (event != NULL) {
        memset(event, 0, sizeof(osi_event_t));
        osi_free(event);
    }
}

bool osi_event_bind(osi_event_t* event, osi_thread_t *thread, int queue_idx)
{
    if (event == NULL || event->thread != NULL) {
        return false;
    }

    if (thread == NULL || (uint32_t)queue_idx >= thread->work_queue_num) {
        return false;
    }

    event->thread = thread;
    event->queue_idx = queue_idx;

    return true;
}

static void osi_thread_generic_event_handler(void *context)
{
    osi_event_t *event = (osi_event_t *)context;
    if (event != NULL && event->item.func != NULL) {
        atomic_store(&event->is_queued, false);
        event->item.func(event->item.context);
    }
}

bool osi_thread_post_event(osi_event_t *event, uint32_t timeout)
{
    assert(event != NULL && event->thread != NULL);
    assert(event->queue_idx >= 0 && event->queue_idx < event->thread->work_queue_num);

    bool ret = false;
    bool acquire_cnt = atomic_exchange(&event->is_queued, true);
    if (!acquire_cnt) {
        ret = osi_thread_post(event->thread, osi_thread_generic_event_handler, event, event->queue_idx, timeout);
        if (!ret) {
            // clear "is_queued" when post failure, to allow for following event posts
            atomic_store(&event->is_queued, false);
        }
    }

    return ret;
}
