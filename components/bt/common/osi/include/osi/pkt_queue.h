/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PKT_LIST_H_
#define _PKT_LIST_H_

#include "sys/queue.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pkt_queue;

typedef struct pkt_linked_item {
    STAILQ_ENTRY(pkt_linked_item) next;
    uint8_t data[];
} pkt_linked_item_t;

#define BT_PKT_LINKED_HDR_SIZE (sizeof (pkt_linked_item_t))

typedef void (*pkt_queue_free_cb)(pkt_linked_item_t *item);

/*
 * brief: create a pkt_queue instance. pkt_queue is a wrapper class of a FIFO implemented by single linked list.
 *        The enqueue and dequeue operations of the FIFO are protected against race conditions of multiple tasks
 * return: NULL if not enough memory, otherwise a valid pointer
 */
struct pkt_queue *pkt_queue_create(void);

/*
 * brief: enqueue one item to the FIFO
 * param queue: pkt_queue instance created using pkt_queue_create
 * param item: the item to be enqueued to the FIFO
 * return: true if enqueued successfully, false when the arguments passed in are invalid
 */
bool pkt_queue_enqueue(struct pkt_queue *queue, pkt_linked_item_t *item);

/*
 * brief: dequeue one item for the FIFO
 * param queue: pkt_queue instance created using pkt_queue_create
 * return: pointer of type pkt_linked_item_t dequeued, NULL if the queue is empty or upon exception
 */
pkt_linked_item_t *pkt_queue_dequeue(struct pkt_queue *queue);

/*
 * brief: get the pointer of the first item from the FIFO but not get it dequeued
 * param queue: pkt_queue instance created using pkt_queue_create
 * return: pointer of the first item in the FIFO, NULL if the FIFO is empty
 */
pkt_linked_item_t *pkt_queue_try_peek_first(struct pkt_queue *queue);

/*
 * brief: retrieve the number of items existing in the FIFO
 * param queue: pkt_queue instance created using pkt_queue_create
 * return: total number of items in the FIFO
 */
size_t pkt_queue_length(const struct pkt_queue *queue);

/*
 * brief: retrieve the status whether the FIFO is empty
 * param queue: pkt_queue instance created using pkt_queue_create
 * return: false if the FIFO is not empty, otherwise true
 */
bool pkt_queue_is_empty(const struct pkt_queue *queue);

/*
 * brief: delete the item in the FIFO one by one
 * param free_cb: destructor function for each item in the FIFO, if set to NULL, will use osi_free_func by default
 */
void pkt_queue_flush(struct pkt_queue *queue, pkt_queue_free_cb free_cb);

/*
 * brief: delete the items in the FIFO and then destroy the pkt_queue instance.
 * param free_cb: destructor function for each item in the FIFO, if set to NULL, will use osi_free_func by default
 */
void pkt_queue_destroy(struct pkt_queue *queue, pkt_queue_free_cb free_cb);

#ifdef __cplusplus
}
#endif

#endif
