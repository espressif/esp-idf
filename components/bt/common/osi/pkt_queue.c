/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osi/pkt_queue.h"
#include "osi/allocator.h"
#include "osi/mutex.h"


STAILQ_HEAD(pkt_queue_header, pkt_linked_item);

struct pkt_queue {
    osi_mutex_t lock;
    size_t length;
    struct pkt_queue_header header;
} pkt_queue_t;

struct pkt_queue *pkt_queue_create(void)
{
    struct pkt_queue *queue = calloc(1, sizeof(struct pkt_queue));
    if (queue == NULL) {
        return NULL;
    }
    if (osi_mutex_new(&queue->lock) != 0) {
        osi_free(queue);
    }
    struct pkt_queue_header *p = &queue->header;
    STAILQ_INIT(p);

    return queue;
}

static void pkt_queue_cleanup(struct pkt_queue *queue, pkt_queue_free_cb free_cb)
{
    if (queue == NULL) {
        return;
    }

    struct pkt_queue_header *header = &queue->header;
    pkt_linked_item_t *item = STAILQ_FIRST(header);
    pkt_linked_item_t *tmp;

    pkt_queue_free_cb free_func = (free_cb != NULL) ? free_cb : (pkt_queue_free_cb)osi_free_func;

    while (item != NULL) {
        tmp = STAILQ_NEXT(item, next);
        free_func(item);
        item = tmp;
        queue->length--;
    }
    STAILQ_INIT(header);
    queue->length = 0;
}

void pkt_queue_flush(struct pkt_queue *queue, pkt_queue_free_cb free_cb)
{
    if (queue == NULL) {
        return;
    }
    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    pkt_queue_cleanup(queue, free_cb);
    osi_mutex_unlock(&queue->lock);
}

void pkt_queue_destroy(struct pkt_queue *queue, pkt_queue_free_cb free_cb)
{
    if (queue == NULL) {
        return;
    }
    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    pkt_queue_cleanup(queue, free_cb);
    osi_mutex_unlock(&queue->lock);

    osi_mutex_free(&queue->lock);
    osi_free(queue);
}

pkt_linked_item_t *pkt_queue_dequeue(struct pkt_queue *queue)
{
    if (queue == NULL || queue->length == 0) {
        return NULL;
    }

    struct pkt_linked_item *item;
    struct pkt_queue_header *header;
    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    header = &queue->header;
    item = STAILQ_FIRST(header);
    if (item != NULL) {
        STAILQ_REMOVE_HEAD(header, next);
        if (queue->length > 0) {
            queue->length--;
        }
    }
    osi_mutex_unlock(&queue->lock);

    return item;
}

bool pkt_queue_enqueue(struct pkt_queue *queue, pkt_linked_item_t *item)
{
    if (queue == NULL || item == NULL) {
        return false;
    }

    struct pkt_queue_header *header;
    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    header = &queue->header;
    STAILQ_INSERT_TAIL(header, item, next);
    queue->length++;
    osi_mutex_unlock(&queue->lock);

    return true;
}

size_t pkt_queue_length(const struct pkt_queue *queue)
{
    if (queue == NULL) {
        return 0;
    }
    return queue->length;
}

bool pkt_queue_is_empty(const struct pkt_queue *queue)
{
    return pkt_queue_length(queue) == 0;
}

pkt_linked_item_t *pkt_queue_try_peek_first(struct pkt_queue *queue)
{
    if (queue == NULL) {
        return NULL;
    }

    struct pkt_queue_header *header = &queue->header;
    pkt_linked_item_t *item;
    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    item = STAILQ_FIRST(header);
    osi_mutex_unlock(&queue->lock);

    return item;
}
