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

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "osi/allocator.h"
#include "osi/list.h"
#include "osi/mutex.h"
#include "osi/semaphore.h"
#include "osi/thread.h"

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
    osi_thread_t *thread;
    size_t ref_count;
    uint8_t flags;
    uint8_t queue_idx;
};

#define OSI_EVENT_FLAG_QUEUED        (1U << 0)
#define OSI_EVENT_FLAG_POSTING       (1U << 1)
#define OSI_EVENT_FLAG_DELETING      (1U << 2)
#define OSI_EVENT_FLAG_RUNNING       (1U << 3)

#define OSI_EVENT_HAS_FLAG(event, flag) (((event)->flags & (flag)) != 0)
#define OSI_EVENT_SET_FLAG(event, flag) ((event)->flags |= (uint8_t)(flag))
#define OSI_EVENT_CLEAR_FLAG(event, flag) ((event)->flags &= (uint8_t)(~(flag)))

static const size_t DEFAULT_WORK_QUEUE_CAPACITY = 100;
static list_t *s_osi_event_list;
static osi_mutex_t s_osi_event_lock;

#if OSI_THREAD_DEBUG
static void osi_thread_run_item(osi_thread_t *thread, int wq_idx, struct work_item *item);
#endif

static void osi_thread_generic_event_handler(void *context);
static void osi_thread_generic_event_drain(void *context);

static void osi_event_lock(void)
{
    assert(s_osi_event_lock != NULL);
    osi_mutex_lock(&s_osi_event_lock, OSI_MUTEX_MAX_TIMEOUT);
}

static void osi_event_unlock(void)
{
    osi_mutex_unlock(&s_osi_event_lock);
}

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
                #if OSI_THREAD_DEBUG
                osi_thread_run_item(thread, idx, &item);
                #else
                item.func(item.context);
                #endif
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
            if (thread->work_queues && thread->work_queues[i]) {
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

    return NULL;
}

void osi_thread_free(osi_thread_t *thread)
{
    if (!thread)
        return;

    osi_thread_stop(thread);

    /* The thread has stopped, so any work items still queued will never be
     * drained by osi_thread_run. We must reclaim them here before the queues
     * are destroyed, but we MUST NOT blindly execute their handlers:
     *
     *  - Event work items (func == osi_thread_generic_event_handler) hold a
     *    reference on their osi_event. If that reference is never released the
     *    osi_event leaks. We reclaim it via osi_thread_generic_event_drain(),
     *    which only drops the queued reference (and frees the event if it was
     *    already deleted) WITHOUT running the user callback. This is safe on
     *    every shutdown path, including ones where the osi_event subsystem has
     *    already been torn down (osi_thread_event_deinit() freed
     *    s_osi_event_lock): the release path uses the per-event lock only and
     *    never touches the global s_osi_event_lock.
     *
     *  - Any other work item was posted directly via osi_thread_post() with an
     *    arbitrary handler (e.g. btu_hci_msg_process, bta_sys_event, alarm
     *    handlers). Running such a handler here would dispatch into
     *    protocol-stack state (L2CAP/BTA/...) that may already have been freed
     *    by the caller before osi_thread_free() (e.g. BTU_ShutDown() calls
     *    btu_task_shut_down() first), turning the drain into a use-after-free.
     *    These items are therefore discarded, matching the pre-existing
     *    behavior where a destroyed queue silently dropped its contents. */
    for (int i = 0; i < thread->work_queue_num; i++) {
        struct work_item item;
        while (thread->work_queues[i] &&
               osi_thead_work_queue_get(thread->work_queues[i], &item) == true) {
            if (item.func == osi_thread_generic_event_handler) {
                osi_thread_generic_event_drain(item.context);
            }
        }
    }

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


static bool osi_event_add_alive_locked(struct osi_event *event)
{
    assert(s_osi_event_list != NULL);
    return list_append(s_osi_event_list, event);
}

static void osi_event_free(struct osi_event *event)
{
    if (event != NULL) {
        osi_mutex_free(&event->lock);
        memset(event, 0, sizeof(struct osi_event));
        osi_free(event);
    }
}

static bool osi_event_is_idle(const struct osi_event *event)
{
    return !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_QUEUED) &&
           !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_POSTING) &&
           !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_RUNNING);
}

static bool osi_event_should_free(const struct osi_event *event)
{
    return OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_DELETING) &&
           osi_event_is_idle(event);
}

static bool osi_event_can_bind_locked(const struct osi_event *event, osi_thread_t *thread, int queue_idx)
{
    return !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_DELETING) &&
           event->thread == NULL &&
           thread != NULL &&
           queue_idx >= 0 &&
           queue_idx < thread->work_queue_num;
}

static bool osi_event_can_post_locked(const struct osi_event *event)
{
    if (event->thread == NULL || event->queue_idx >= event->thread->work_queue_num) {
        return false;
    }

    return !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_DELETING) &&
           event->item.func != NULL &&
           !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_QUEUED) &&
           !OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_POSTING);
}

static bool osi_event_is_alive_locked(const struct osi_event *event)
{
    /* Do not dereference event here: callers may pass a stale pointer racing
     * with osi_event_delete(). The alive list is the ownership boundary. */
    return s_osi_event_list != NULL && list_contains(s_osi_event_list, event);
}

static bool osi_event_remove_alive_locked(struct osi_event *event)
{
    bool removed = false;

    if (s_osi_event_list == NULL) {
        return false;
    }

    removed = list_delete(s_osi_event_list, event);
    return removed;
}

struct osi_event *osi_event_create(osi_thread_func_t func, void *context)
{
    bool added = false;
    struct osi_event *event = osi_calloc(sizeof(struct osi_event));

    if (event == NULL) {
        return NULL;
    }

    if (osi_mutex_new(&event->lock) != 0) {
        osi_free(event);
        return NULL;
    }

    event->item.func = func;
    event->item.context = context;
    event->ref_count = 1;

    osi_event_lock();
    added = osi_event_add_alive_locked(event);
    osi_event_unlock();
    if (added) {
        return event;
    }

    osi_mutex_free(&event->lock);
    osi_free(event);
    return NULL;
}

/* ref_count is protected by the per-event lock (event->lock), NOT the global
 * s_osi_event_lock. This keeps the reference-release path independent of the
 * global event subsystem: it must stay valid even after
 * osi_thread_event_deinit() has freed s_osi_event_lock (e.g. when a thread is
 * freed on a shutdown path that tears the event subsystem down first). The
 * global lock is only used to gate the alive-list membership that decides
 * whether a new reference may be acquired. */
static bool osi_event_acquire(struct osi_event *event)
{
    bool acquired = false;

    if (event == NULL) {
        return false;
    }

    /* Hold the global lock so the event cannot be removed from the alive list
     * (and thus cannot be freed) while we take a fresh reference. Nesting is
     * always global-lock-outer, event->lock-inner; no path takes them in the
     * reverse order, so this cannot deadlock. */
    osi_event_lock();
    if (osi_event_is_alive_locked(event)) {
        osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
        assert(event->ref_count > 0);
        event->ref_count++;
        osi_mutex_unlock(&event->lock);
        acquired = true;
    }
    osi_event_unlock();

    return acquired;
}

static void osi_event_retain(struct osi_event *event)
{
    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    assert(event->ref_count > 0);
    event->ref_count++;
    osi_mutex_unlock(&event->lock);
}

static void osi_event_release(struct osi_event *event)
{
    bool should_free = false;

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    assert(event->ref_count > 0);
    event->ref_count--;
    if (event->ref_count == 0) {
        should_free = osi_event_should_free(event);
    }
    osi_mutex_unlock(&event->lock);

    if (should_free) {
        osi_event_free(event);
    }
}

void osi_event_delete(struct osi_event *event)
{
    bool removed = false;

    if (event == NULL) {
        return;
    }

    osi_event_lock();
    removed = osi_event_remove_alive_locked(event);
    osi_event_unlock();
    if (!removed) {
        return;
    }

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    OSI_EVENT_SET_FLAG(event, OSI_EVENT_FLAG_DELETING);
    event->item.func = NULL;
    event->item.context = NULL;
    osi_mutex_unlock(&event->lock);

    osi_event_release(event);
}

bool osi_event_bind(struct osi_event *event, osi_thread_t *thread, int queue_idx)
{
    bool ret = false;

    if (!osi_event_acquire(event)) {
        return false;
    }

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    if (osi_event_can_bind_locked(event, thread, queue_idx)) {
        event->thread = thread;
        event->queue_idx = queue_idx;
        ret = true;
    }
    osi_mutex_unlock(&event->lock);

    osi_event_release(event);

    return ret;
}

static void osi_thread_generic_event_handler(void *context)
{
    struct osi_event *event = (struct osi_event *)context;
    osi_thread_func_t func = NULL;
    void *func_context = NULL;

    if (event == NULL) {
        return;
    }

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    OSI_EVENT_CLEAR_FLAG(event, OSI_EVENT_FLAG_QUEUED);
    if (OSI_EVENT_HAS_FLAG(event, OSI_EVENT_FLAG_DELETING)) {
        osi_mutex_unlock(&event->lock);
        osi_event_release(event);
        return;
    }
    OSI_EVENT_SET_FLAG(event, OSI_EVENT_FLAG_RUNNING);
    func = event->item.func;
    func_context = event->item.context;
    osi_mutex_unlock(&event->lock);

    if (func != NULL) {
        func(func_context);
    }

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    OSI_EVENT_CLEAR_FLAG(event, OSI_EVENT_FLAG_RUNNING);
    osi_mutex_unlock(&event->lock);

    osi_event_release(event);
}

/* Reclaim a queued event work item during thread teardown WITHOUT invoking the
 * user callback. It only clears the QUEUED flag and drops the reference the
 * queued item owns (osi_thread_post_event() retained it); this frees the event
 * if it was already deleted, and leaves a still-live event untouched. Unlike
 * osi_thread_generic_event_handler(), it never dispatches into stack state that
 * may already have been freed on the shutdown path. */
static void osi_thread_generic_event_drain(void *context)
{
    struct osi_event *event = (struct osi_event *)context;

    if (event == NULL) {
        return;
    }

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    OSI_EVENT_CLEAR_FLAG(event, OSI_EVENT_FLAG_QUEUED);
    osi_mutex_unlock(&event->lock);

    osi_event_release(event);
}

bool osi_thread_post_event(struct osi_event *event, uint32_t timeout)
{
    bool ret = false;
    osi_thread_t *thread = NULL;
    uint8_t queue_idx = 0;

    if (!osi_event_acquire(event)) {
        return false;
    }

    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    if (!osi_event_can_post_locked(event)) {
        osi_mutex_unlock(&event->lock);
        osi_event_release(event);
        return false;
    }
    OSI_EVENT_SET_FLAG(event, OSI_EVENT_FLAG_QUEUED);
    OSI_EVENT_SET_FLAG(event, OSI_EVENT_FLAG_POSTING);
    thread = event->thread;
    queue_idx = event->queue_idx;
    osi_mutex_unlock(&event->lock);

    /* The queued work item owns a reference until the generic handler drains it. */
    osi_event_retain(event);
    ret = osi_thread_post(thread, osi_thread_generic_event_handler, event, queue_idx, timeout);
    osi_mutex_lock(&event->lock, OSI_MUTEX_MAX_TIMEOUT);
    OSI_EVENT_CLEAR_FLAG(event, OSI_EVENT_FLAG_POSTING);
    if (!ret) {
        // clear "is_queued" when post failure, to allow for following event posts
        OSI_EVENT_CLEAR_FLAG(event, OSI_EVENT_FLAG_QUEUED);
    }
    osi_mutex_unlock(&event->lock);

    if (!ret) {
        osi_event_release(event);
    }
    osi_event_release(event);

    return ret;
}

int osi_thread_event_init(void)
{
    int ret = -1;

    do {
        if (osi_mutex_new(&s_osi_event_lock) != 0) {
            break;
        }

        s_osi_event_list = list_new(NULL);
        if (s_osi_event_list == NULL) {
            break;
        }

        ret = 0;
    } while (0);

    if (ret != 0) {
        osi_thread_event_deinit();
    }

    return ret;
}

void osi_thread_event_deinit(void)
{
    if (s_osi_event_list != NULL) {
        list_free(s_osi_event_list);
        s_osi_event_list = NULL;
    }
    osi_mutex_free(&s_osi_event_lock);
}

#if OSI_THREAD_DEBUG
static void osi_thread_run_item(osi_thread_t *thread, int wq_idx, struct work_item *item)
{
    uint32_t pre_time;
    uint32_t pre_msg_cnt;
    uint32_t cur_time;
    uint32_t cur_msg_cnt;

    pre_time = esp_log_timestamp();
    pre_msg_cnt = uxQueueMessagesWaiting(thread->work_queues[wq_idx]->queue);
    item->func(item->context);
    cur_time = esp_log_timestamp();
    cur_msg_cnt = uxQueueMessagesWaiting(thread->work_queues[wq_idx]->queue);
    if ((cur_time - pre_time) >= OSI_THREAD_BLOCK_TIME ||
        (cur_msg_cnt > pre_msg_cnt && (cur_msg_cnt - pre_msg_cnt) >= OSI_THREAD_BLOCK_MSG)) {
        OSI_TRACE_ERROR("%s was blocked while running item: %p exec_time=[%u %u] msg_inc=[%u %u]",
            pcTaskGetName(thread->thread_handle), item->func, cur_time, pre_time, cur_msg_cnt, pre_msg_cnt);
        assert(0);
    }
}

void osi_thread_workqueue_dump(osi_thread_t *thread)
{
    int idx = 0;
    struct work_item item;

    vTaskSuspendAll();

    while (idx < thread->work_queue_num) {
        if (osi_thead_work_queue_get(thread->work_queues[idx], &item) == true) {
            esp_rom_printf("[%u] %p %p\n", idx, item.func, item.context);
            idx = 0;
            continue;
        } else {
            idx++;
        }
    }

    xTaskResumeAll();
}
#endif // OSI_THREAD_DEBUG
