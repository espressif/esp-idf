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

#include "bt_defs.h"
#include "allocator.h"
#include "fixed_queue.h"
#include "list.h"
#include "osi.h"
#include "osi_arch.h"
#include "bt_trace.h"

typedef struct fixed_queue_t {

    list_t *list;
    osi_sem_t enqueue_sem;
    osi_sem_t dequeue_sem;
    pthread_mutex_t lock;
    size_t capacity;

    fixed_queue_cb dequeue_ready;
    /*
      reactor_object_t *dequeue_object;
      fixed_queue_cb dequeue_ready;
      void *dequeue_context;
    */
} fixed_queue_t;

//static void internal_dequeue_ready(void *context);

fixed_queue_t *fixed_queue_new(size_t capacity)
{
    fixed_queue_t *ret = osi_calloc(sizeof(fixed_queue_t));
    if (!ret) {
        goto error;
    }

    pthread_mutex_init(&ret->lock, NULL);
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
    if (!queue) {
        return;
    }

//  fixed_queue_unregister_dequeue(queue);

    if (free_cb)
        for (node = list_begin(queue->list); node != list_end(queue->list); node = list_next(node)) {
            free_cb(list_node(node));
        }

    list_free(queue->list);
    osi_sem_free(&queue->enqueue_sem);
    osi_sem_free(&queue->dequeue_sem);
    pthread_mutex_destroy(&queue->lock);
    osi_free(queue);
}

bool fixed_queue_is_empty(fixed_queue_t *queue)
{
    bool is_empty = false;
    assert(queue != NULL);

    pthread_mutex_lock(&queue->lock);
    is_empty = list_is_empty(queue->list);
    pthread_mutex_unlock(&queue->lock);

    return is_empty;
}

size_t fixed_queue_capacity(fixed_queue_t *queue)
{
    assert(queue != NULL);

    return queue->capacity;
}

void fixed_queue_enqueue(fixed_queue_t *queue, void *data)
{
    assert(queue != NULL);
    assert(data != NULL);

    osi_sem_wait(&queue->enqueue_sem, 0);

    pthread_mutex_lock(&queue->lock);

    list_append(queue->list, data);
    pthread_mutex_unlock(&queue->lock);

    osi_sem_signal(&queue->dequeue_sem);
}

void *fixed_queue_dequeue(fixed_queue_t *queue)
{
    void *ret = NULL;
    assert(queue != NULL);

    osi_sem_wait(&queue->dequeue_sem, 0);

    pthread_mutex_lock(&queue->lock);
    ret = list_front(queue->list);
    list_remove(queue->list, ret);
    pthread_mutex_unlock(&queue->lock);

    osi_sem_signal(&queue->enqueue_sem);

    return ret;
}

/*
void *fixed_queue_try_dequeue(fixed_queue_t *queue) {
  void *ret = NULL;
  assert(queue != NULL);

  if (!semaphore_try_wait(queue->dequeue_sem))
    return NULL;

  pthread_mutex_lock(&queue->lock);
  ret = list_front(queue->list);
  list_remove(queue->list, ret);
  pthread_mutex_unlock(&queue->lock);

  semaphore_post(queue->enqueue_sem);

  return ret;
}

int fixed_queue_get_dequeue_fd(const fixed_queue_t *queue) {
  assert(queue != NULL);
  return semaphore_get_fd(queue->dequeue_sem);
}
*/
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
    if (queue == NULL) {
        return;
    }

    if (queue->dequeue_ready) {
        queue->dequeue_ready(queue);
    }
}
/*
static void internal_dequeue_ready(void *context) {
  assert(context != NULL);

  fixed_queue_t *queue = context;
  queue->dequeue_ready(queue, queue->dequeue_context);
}
*/
