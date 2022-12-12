/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osi/allocator.h"
#include "osi/pkt_queue.h"
#include "osi/fixed_pkt_queue.h"
#include "osi/osi.h"
#include "osi/semaphore.h"

typedef struct fixed_pkt_queue_t {
    struct pkt_queue *pkt_list;
    osi_sem_t enqueue_sem;
    osi_sem_t dequeue_sem;
    size_t capacity;
    fixed_pkt_queue_cb dequeue_ready;
} fixed_pkt_queue_t;

fixed_pkt_queue_t *fixed_pkt_queue_new(size_t capacity)
{
    fixed_pkt_queue_t *ret = osi_calloc(sizeof(fixed_pkt_queue_t));
    if (!ret) {
        goto error;
    }

    ret->capacity = capacity;
    ret->pkt_list = pkt_queue_create();
    if (!ret->pkt_list) {
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

error:
    fixed_pkt_queue_free(ret, NULL);
    return NULL;
}

void fixed_pkt_queue_free(fixed_pkt_queue_t *queue, fixed_pkt_queue_free_cb free_cb)
{
    if (queue == NULL) {
        return;
    }

    fixed_pkt_queue_unregister_dequeue(queue);

    pkt_queue_destroy(queue->pkt_list, (pkt_queue_free_cb)free_cb);
    queue->pkt_list = NULL;

    if (queue->enqueue_sem) {
        osi_sem_free(&queue->enqueue_sem);
    }
    if (queue->dequeue_sem) {
        osi_sem_free(&queue->dequeue_sem);
    }
    osi_free(queue);
}

bool fixed_pkt_queue_is_empty(fixed_pkt_queue_t *queue)
{
    if (queue == NULL) {
        return true;
    }

    return pkt_queue_is_empty(queue->pkt_list);
}

size_t fixed_pkt_queue_length(fixed_pkt_queue_t *queue)
{
    if (queue == NULL) {
        return 0;
    }
    return pkt_queue_length(queue->pkt_list);
}

size_t fixed_pkt_queue_capacity(fixed_pkt_queue_t *queue)
{
    assert(queue != NULL);

    return queue->capacity;
}

bool fixed_pkt_queue_enqueue(fixed_pkt_queue_t *queue, pkt_linked_item_t *linked_pkt, uint32_t timeout)
{
    bool ret = false;

    assert(queue != NULL);
    assert(linked_pkt != NULL);

    if (osi_sem_take(&queue->enqueue_sem, timeout) != 0) {
        return false;
    }

    ret = pkt_queue_enqueue(queue->pkt_list, linked_pkt);

    assert(ret == true);
    osi_sem_give(&queue->dequeue_sem);

    return ret;
}

pkt_linked_item_t *fixed_pkt_queue_dequeue(fixed_pkt_queue_t *queue, uint32_t timeout)
{
    pkt_linked_item_t *ret = NULL;

    assert(queue != NULL);

    if (osi_sem_take(&queue->dequeue_sem, timeout) != 0) {
        return NULL;
    }
    ret = pkt_queue_dequeue(queue->pkt_list);

    osi_sem_give(&queue->enqueue_sem);

    return ret;
}

pkt_linked_item_t *fixed_pkt_queue_try_peek_first(fixed_pkt_queue_t *queue)
{
    if (queue == NULL) {
        return NULL;
    }

    return pkt_queue_try_peek_first(queue->pkt_list);
}

void fixed_pkt_queue_register_dequeue(fixed_pkt_queue_t *queue, fixed_pkt_queue_cb ready_cb)
{
    assert(queue != NULL);
    assert(ready_cb != NULL);

    queue->dequeue_ready = ready_cb;
}

void fixed_pkt_queue_unregister_dequeue(fixed_pkt_queue_t *queue)
{
    assert(queue != NULL);

    queue->dequeue_ready = NULL;
}

void fixed_pkt_queue_process(fixed_pkt_queue_t *queue)
{
    assert(queue != NULL);

    if (queue->dequeue_ready) {
        queue->dequeue_ready(queue);
    }
}
