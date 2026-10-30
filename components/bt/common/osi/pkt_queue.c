/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osi/pkt_queue.h"
#include "osi/allocator.h"
#include "osi/mutex.h"


struct pkt_queue {
    osi_mutex_t lock;
    slist_t *list;
} pkt_queue_t;

static slist_node_t *pkt_queue_node(pkt_linked_item_t *item)
{
    return &item->node;
}

static pkt_linked_item_t *pkt_queue_item(slist_node_t *node)
{
    return (pkt_linked_item_t *)node;
}

static pkt_linked_item_t *pkt_queue_take_front(struct pkt_queue *queue)
{
    slist_node_t *node;

    if (slist_is_empty(queue->list)) {
        return NULL;
    }

    node = slist_front(queue->list);
    slist_delete(queue->list, node);
    return pkt_queue_item(node);
}

struct pkt_queue *pkt_queue_create(void)
{
    struct pkt_queue *queue = osi_calloc(sizeof(struct pkt_queue));
    if (queue == NULL) {
        return NULL;
    }
    if (osi_mutex_new(&queue->lock) != 0) {
        osi_free(queue);
        return NULL;
    }
    queue->list = slist_new(NULL);
    if (queue->list == NULL) {
        osi_mutex_free(&queue->lock);
        osi_free(queue);
        return NULL;
    }

    return queue;
}

static void pkt_queue_cleanup(struct pkt_queue *queue, pkt_queue_free_cb free_cb)
{
    if (queue == NULL) {
        return;
    }

    pkt_queue_free_cb free_func = (free_cb != NULL) ? free_cb : (pkt_queue_free_cb)osi_free_func;
    pkt_linked_item_t *item;

    while ((item = pkt_queue_take_front(queue)) != NULL) {
        free_func(item);
    }
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

    slist_free(queue->list);
    osi_mutex_free(&queue->lock);
    osi_free(queue);
}

pkt_linked_item_t *pkt_queue_dequeue_and_len(struct pkt_queue *queue, size_t *out_len)
{
    if (queue == NULL) {
        if (out_len != NULL) {
            *out_len = 0;
        }
        return NULL;
    }

    pkt_linked_item_t *item;

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    item = pkt_queue_take_front(queue);
    if (out_len != NULL) {
        *out_len = slist_length(queue->list);
    }
    osi_mutex_unlock(&queue->lock);

    return item;
}

pkt_linked_item_t *pkt_queue_dequeue(struct pkt_queue *queue)
{
    return pkt_queue_dequeue_and_len(queue, NULL);
}

bool pkt_queue_enqueue_and_len(struct pkt_queue *queue, pkt_linked_item_t *item, size_t *out_len)
{
    if (queue == NULL || item == NULL) {
        return false;
    }

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    slist_append(queue->list, pkt_queue_node(item));
    if (out_len != NULL) {
        *out_len = slist_length(queue->list);
    }
    osi_mutex_unlock(&queue->lock);

    return true;
}

bool pkt_queue_enqueue(struct pkt_queue *queue, pkt_linked_item_t *item)
{
    return pkt_queue_enqueue_and_len(queue, item, NULL);
}

size_t pkt_queue_length(const struct pkt_queue *queue)
{
    if (queue == NULL) {
        return 0;
    }
    return slist_length(queue->list);
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

    slist_node_t *node;

    osi_mutex_lock(&queue->lock, OSI_MUTEX_MAX_TIMEOUT);
    node = slist_is_empty(queue->list) ? NULL : slist_front(queue->list);
    osi_mutex_unlock(&queue->lock);

    return pkt_queue_item(node);
}
