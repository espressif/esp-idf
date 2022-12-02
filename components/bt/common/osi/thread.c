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
#include "osi/mutex.h"

struct work_item {
    osi_thread_func_t func;
    void *context;
};

struct work_queue {
    QueueHandle_t queue;
    size_t capacity;
};

struct osi_thread {
  TaskHandle_t thread_handle;           /*!< Store the thread object */
  int  thread_id;                       /*!< May for some OS, such as Linux */
  bool stop;
  uint8_t work_queue_num;               /*!< Work queue number */
  struct work_queue **work_queues;      /*!< Point to queue array, and the priority inverse array index */
  osi_sem_t work_sem;
  osi_sem_t stop_sem;
};

struct osi_thread_start_arg {
  osi_thread_t *thread;
  osi_sem_t start_sem;
  int error;
};

struct osi_event {
    struct work_item item;
    osi_mutex_t lock;
    uint16_t is_queued;
    uint16_t queue_idx;
    osi_thread_t *thread;
};

static const size_t DEFAULT_WORK_QUEUE_CAPACITY = 100;

static struct work_queue *osi_work_queue_create(size_t capacity)
{
    if (capacity == 0) {
        return NULL;
    }

    struct work_queue *wq = (struct work_queue *)osi_malloc(sizeof(struct work_queue));
    if (wq != NULL) {
        wq->queue = xQueueCreate(capacity, sizeof(struct work_item));
        if (wq->queue != 0) {
            wq->capacity = capacity;
            return wq;
        } else {
            osi_free(wq);
        }
    }

    return NULL;
}

static void osi_work_queue_delete(struct work_queue *wq)
{
    if (wq != NULL) {
        if (wq->queue != 0) {
            vQueueDelete(wq->queue);
        }
        wq->queue = 0;
        wq->capacity = 0;
        osi_free(wq);
    }
    return;
}

static bool osi_thead_work_queue_get(struct work_queue *wq, struct work_item *item)
{
    assert (wq != NULL);
    assert (wq->queue != 0);
    assert (item != NULL);

    if (pdTRUE == xQueueReceive(wq->queue, item, 0)) {
        return true;
    } else {
        return false;
    }
}

static bool osi_thead_work_queue_put(struct work_queue *wq, const struct work_item *item, uint32_t timeout)
{
    assert (wq != NULL);
    assert (wq->queue != 0);
    assert (item != NULL);

    bool ret = true;
    if (timeout ==  OSI_SEM_MAX_TIMEOUT) {
        if (xQueueSend(wq->queue, item, portMAX_DELAY) != pdTRUE) {
            ret = false;
        }
    } else {
        if (xQueueSend(wq->queue, item, timeout / portTICK_PERIOD_MS) != pdTRUE) {
            ret = false;
        }
    }

    return ret;
}

static size_t osi_thead_work_queue_len(struct work_queue *wq)
{
    assert (wq != NULL);
    assert (wq->queue != 0);
    assert (wq->capacity != 0);

    size_t available_spaces = (size_t)uxQueueSpacesAvailable(wq->queue);

    if (available_spaces <= wq->capacity) {
        return wq->capacity - available_spaces;
    } else {
        assert (0);
    }
    return 0;
}

static void osi_thread_run(void *arg)
{
    struct osi_thread_start_arg *start = (struct osi_thread_start_arg *)arg;
    osi_thread_t *thread = start->thread;

    osi_sem_give(&start->start_sem);

    while (1) {
        int idx = 0;

        osi_sem_take(&thread->work_sem, OSI_SEM_MAX_TIMEOUT);

        if (thread->stop) {
            break;
        }

        struct work_item item;
        while (!thread->stop && idx < thread->work_queue_num) {
            if (osi_thead_work_queue_get(thread->work_queues[idx], &item) == true) {
                item.func(item.context);
                idx = 0;
                continue;
            } else {
                idx++;
            }
        }
    }

    thread->thread_handle = NULL;
    osi_sem_give(&thread->stop_sem);

    vTaskDelete(NULL);
}

static int osi_thread_join(osi_thread_t *thread, uint32_t wait_ms)
{
    assert(thread != NULL);
    return osi_sem_take(&thread->stop_sem, wait_ms);
}

static void osi_thread_stop(osi_thread_t *thread)
{
    int ret;

    assert(thread != NULL);

    //stop the thread
    thread->stop = true;
    osi_sem_give(&thread->work_sem);

    //join
    ret = osi_thread_join(thread, 1000); //wait 1000ms

    //if join failed, delete the task here
    if (ret != 0 && thread->thread_handle) {
        vTaskDelete(thread->thread_handle);
    }
}

//in linux, the stack_size, priority and core may not be set here, the code will be ignore the arguments
osi_thread_t *osi_thread_create(const char *name, size_t stack_size, int priority, osi_thread_core_t core, uint8_t work_queue_num, const size_t work_queue_len[])
{
    int ret;
    struct osi_thread_start_arg start_arg = {0};

    if (stack_size <= 0 ||
            core < OSI_THREAD_CORE_0 || core > OSI_THREAD_CORE_AFFINITY ||
            work_queue_num <= 0 || work_queue_len == NULL) {
        return NULL;
    }

    osi_thread_t *thread = (osi_thread_t *)osi_calloc(sizeof(osi_thread_t));
    if (thread == NULL) {
        goto _err;
    }

    thread->stop = false;
    thread->work_queues = (struct work_queue **)osi_calloc(sizeof(struct work_queue *) * work_queue_num);
    if (thread->work_queues == NULL) {
        goto _err;
    }
    thread->work_queue_num = work_queue_num;

    for (int i = 0; i < thread->work_queue_num; i++) {
        size_t queue_len = work_queue_len[i] ? work_queue_len[i] : DEFAULT_WORK_QUEUE_CAPACITY;
        thread->work_queues[i] = osi_work_queue_create(queue_len);
        if (thread->work_queues[i] == NULL) {
            goto _err;
        }
    }

    ret = osi_sem_new(&thread->work_sem, 1, 0);
    if (ret != 0) {
        goto _err;
    }

    ret = osi_sem_new(&thread->stop_sem, 1, 0);
    if (ret != 0) {
        goto _err;
    }

    start_arg.thread = thread;
    ret = osi_sem_new(&start_arg.start_sem, 1, 0);
    if (ret != 0) {
        goto _err;
    }

    if (xTaskCreatePinnedToCore(osi_thread_run, name, stack_size, &start_arg, priority, &thread->thread_handle, core) != pdPASS) {
        goto _err;
    }

    osi_sem_take(&start_arg.start_sem, OSI_SEM_MAX_TIMEOUT);
    osi_sem_free(&start_arg.start_sem);

    return thread;

_err:

    if (thread) {
        if (start_arg.start_sem) {
            osi_sem_free(&start_arg.start_sem);
        }

        if (thread->thread_handle) {
            vTaskDelete(thread->thread_handle);
        }

        for (int i = 0; i < thread->work_queue_num; i++) {
            if (thread->work_queues[i]) {
                osi_work_queue_delete(thread->work_queues[i]);
            }
            thread->work_queues[i] = NULL;
        }

        if (thread->work_queues) {
            osi_free(thread->work_queues);
            thread->work_queues = NULL;
        }

        if (thread->work_sem) {
            osi_sem_free(&thread->work_sem);
        }

        if (thread->stop_sem) {
            osi_sem_free(&thread->stop_sem);
        }

        osi_free(thread);
    }

    return NULL;
}

void osi_thread_free(osi_thread_t *thread)
{
    if (!thread)
        return;

    osi_thread_stop(thread);

    for (int i = 0; i < thread->work_queue_num; i++) {
        if (thread->work_queues[i]) {
            osi_work_queue_delete(thread->work_queues[i]);
            thread->work_queues[i] = NULL;
        }
    }

    if (thread->work_queues) {
        osi_free(thread->work_queues);
        thread->work_queues = NULL;
    }

    if (thread->work_sem) {
        osi_sem_free(&thread->work_sem);
    }

    if (thread->stop_sem) {
        osi_sem_free(&thread->stop_sem);
    }


    osi_free(thread);
}

bool osi_thread_post(osi_thread_t *thread, osi_thread_func_t func, void *context, int queue_idx, uint32_t timeout)
{
    assert(thread != NULL);
    assert(func != NULL);

    if (queue_idx >= thread->work_queue_num) {
        return false;
    }

    struct work_item item;

    item.func = func;
    item.context = context;

    if (osi_thead_work_queue_put(thread->work_queues[queue_idx], &item, timeout) == false) {
        return false;
    }

    osi_sem_give(&thread->work_sem);

    return true;
}

bool osi_thread_set_priority(osi_thread_t *thread, int priority)
{
    assert(thread != NULL);

    vTaskPrioritySet(thread->thread_handle, priority);
    return true;
}

const char *osi_thread_name(osi_thread_t *thread)
{
    assert(thread != NULL);

    return pcTaskGetName(thread->thread_handle);
}

int osi_thread_queue_wait_size(osi_thread_t *thread, int wq_idx)
{
    if (wq_idx < 0 || wq_idx >= thread->work_queue_num) {
        return -1;
    }

    return (int)(osi_thead_work_queue_len(thread->work_queues[wq_idx]));
}


struct osi_event *osi_event_create(osi_thread_func_t func, void *context)
{
    struct osi_event *event = osi_calloc(sizeof(struct osi_event));
    if (event != NULL) {
        if (osi_mutex_new(&event->lock) == 0) {
            event->item.func = func;
            event->item.context = context;
            return event;
        }
        osi_free(event);
    }

    return NULL;
}

void osi_event_delete(struct osi_event* event)
{
    if (event != NULL) {
        osi_mutex_free(&event->lock);
        memset(event, 0, sizeof(struct osi_event));
        osi_free(event);
    }
}

bool osi_event_bind(struct osi_event* event, osi_thread_t *thread, int queue_idx)
{
    if (event == NULL || event->thread != NULL) {
        return false;
    }

    if (thread == NULL || queue_idx >= thread->work_queue_num) {
        return false;
    }

    event->thread = thread;
    event->queue_idx = queue_idx;

    return true;
}

static void osi_thread_generic_event_handler(void *context)
{
    struct osi_event *event = (struct osi_event *)context;
    if (event != NULL && event->item.func != NULL) {
        osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
        event->is_queued = 0;
        osi_mutex_unlock(&event->lock);
        event->item.func(event->item.context);
    }
}

bool osi_thread_post_event(struct osi_event *event, uint32_t timeout)
{
    assert(event != NULL && event->thread != NULL);
    assert(event->queue_idx >= 0 && event->queue_idx < event->thread->work_queue_num);
    bool ret = false;
    if (event->is_queued == 0) {
        uint16_t acquire_cnt = 0;
        osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
        event->is_queued += 1;
        acquire_cnt = event->is_queued;
        osi_mutex_unlock(&event->lock);

        if (acquire_cnt == 1) {
            ret = osi_thread_post(event->thread, osi_thread_generic_event_handler, event, event->queue_idx, timeout);
            if (!ret) {
                // clear "is_queued" when post failure, to allow for following event posts
                osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
                event->is_queued = 0;
                osi_mutex_unlock(&event->lock);
            }
        }
    }

    return ret;
}
