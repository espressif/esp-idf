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

#include "osi/allocator.h"
#include "osi/fixed_queue.h"
#include "osi/list.h"
#include "osi/osi.h"
#include "osi/mutex.h"
#include "osi/semaphore.h"

typedef struct fixed_queue_t {

    list_t *list;
    osi_sem_t enqueue_sem;
    osi_sem_t dequeue_sem;
    osi_mutex_t lock;
    size_t capacity;

    fixed_queue_cb dequeue_ready;
} fixed_queue_t;


fixed_queue_t *fixed_queue_new(size_t capacity)
{
    fixed_queue_t *ret = osi_calloc(sizeof(fixed_queue_t));
    if (!ret) {
        goto error;
    }

    osi_mutex_new(&ret->lock);
    ret->capacity = capacity;

    ret->list = list_new(NULL);
    if (!ret->list) {
        goto error;
    }


    osi_sem_new(&ret->enqueue_sem, capacity, capacity);
    if (!ret->enqueue_sem) {
        goto error;
    }

    osi_sem_new(&ret->dequeue_sem, capacity, 0);
    if (!ret->dequeue_sem) {
        goto error;
    }

    return ret;

error:;
    fixed_queue_free(ret, NULL);
    return NULL;
}

void fixed_queue_free(fixed_queue_t *queue, fixed_queue_free_cb free_cb)
{
    const list_node_t *node;

    if (queue == NULL) {
	    return;
	}

    fixed_queue_unregister_dequeue(queue);

    if (free_cb) {
        for (node = list_begin(queue->list); node != list_end(queue->list); node = list_next(node)) {
            free_cb(list_node(node));
        }
    }

    list_free(queue->list);
    osi_sem_free(&queue->enqueue_sem);
    osi_sem_free(&queue->dequeue_sem);
    osi_mutex_free(&queue->lock);
    osi_free(queue);
}

bool fixed_queue_is_empty(fixed_queue_t *queue)
{
    bool is_empty = false;

    if (queue == NULL) {
        return true;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    is_empty = list_is_empty(queue->list);
    osi_mutex_unlock(&queue->lock);

    return is_empty;
}

size_t fixed_queue_length(fixed_queue_t *queue)
{
    size_t length;

    if (queue == NULL) {
        return 0;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    length = list_length(queue->list);
    osi_mutex_unlock(&queue->lock);

    return length;
}
size_t fixed_queue_capacity(fixed_queue_t *queue)
{
    assert(queue != NULL);

    return queue->capacity;
}

bool fixed_queue_enqueue(fixed_queue_t *queue, void *data, uint32_t timeout)
{
    bool status=false; //Flag whether enqueued success

    assert(queue != NULL);
    assert(data != NULL);

    if (osi_sem_take(&queue->enqueue_sem, timeout) != 0) {
        return false;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    status = list_append(queue->list, data); //Check whether enqueued success
    osi_mutex_unlock(&queue->lock);

    if(status == true )
        osi_sem_give(&queue->dequeue_sem);

    return status;
}

void *fixed_queue_dequeue(fixed_queue_t *queue, uint32_t timeout)
{
    void *ret = NULL;

    assert(queue != NULL);

    if (osi_sem_take(&queue->dequeue_sem, timeout) != 0) {
        return NULL;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    ret = list_front(queue->list);
    list_remove(queue->list, ret);
    osi_mutex_unlock(&queue->lock);

    osi_sem_give(&queue->enqueue_sem);

    return ret;
}

void *fixed_queue_try_peek_first(fixed_queue_t *queue)
{
    void *ret = NULL;

    if (queue == NULL) {
        return NULL;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    ret = list_is_empty(queue->list) ? NULL : list_front(queue->list);
    osi_mutex_unlock(&queue->lock);

    return ret;
}

void *fixed_queue_try_peek_last(fixed_queue_t *queue)
{
    void *ret = NULL;

    if (queue == NULL) {
        return NULL;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    ret = list_is_empty(queue->list) ? NULL : list_back(queue->list);
    osi_mutex_unlock(&queue->lock);

    return ret;
}

void *fixed_queue_try_remove_from_queue(fixed_queue_t *queue, void *data)
{
    bool removed = false;

    if (queue == NULL) {
        return NULL;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    if (list_contains(queue->list, data) &&
            osi_sem_take(&queue->dequeue_sem, 0) == 0) {
        removed = list_remove(queue->list, data);
        assert(removed);
    }
    osi_mutex_unlock(&queue->lock);

    if (removed) {
        osi_sem_give(&queue->enqueue_sem);
        return data;
    }

    return NULL;
}

list_t *fixed_queue_get_list(fixed_queue_t *queue)
{
    assert(queue != NULL);

    // NOTE: This function is not thread safe, and there is no point for
    // calling osi_mutex_lock() / osi_mutex_unlock()
    return queue->list;
}

void fixed_queue_register_dequeue(fixed_queue_t *queue, fixed_queue_cb ready_cb)
{
    assert(queue != NULL);
    assert(ready_cb != NULL);

    queue->dequeue_ready = ready_cb;
}

void fixed_queue_unregister_dequeue(fixed_queue_t *queue)
{
    assert(queue != NULL);

    queue->dequeue_ready = NULL;
}

void fixed_queue_process(fixed_queue_t *queue)
{
    assert(queue != NULL);

    if (queue->dequeue_ready) {
        queue->dequeue_ready(queue);
    }
}
