/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FIXED_PKT_QUEUE_H_
#define _FIXED_PKT_QUEUE_H_


#include "osi/pkt_queue.h"
#include "osi/semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FIXED_PKT_QUEUE_SIZE_MAX
#define FIXED_PKT_QUEUE_SIZE_MAX                    254
#endif

#define FIXED_PKT_QUEUE_MAX_TIMEOUT                 OSI_SEM_MAX_TIMEOUT

struct fixed_pkt_queue_t;

typedef struct fixed_pkt_queue_t fixed_pkt_queue_t;

typedef void (*fixed_pkt_queue_free_cb)(pkt_linked_item_t *data);
typedef void (*fixed_pkt_queue_cb)(fixed_pkt_queue_t *queue);

// Creates a new fixed queue with the given |capacity|. If more elements than
// |capacity| are added to the queue, the caller is blocked until space is
// made available in the queue. Returns NULL on failure. The caller must free
// the returned queue with |fixed_pkt_queue_free|.
fixed_pkt_queue_t *fixed_pkt_queue_new(size_t capacity);

// Freeing a queue that is currently in use (i.e. has waiters
// blocked on it) results in undefined behaviour.
void fixed_pkt_queue_free(fixed_pkt_queue_t *queue, fixed_pkt_queue_free_cb free_cb);

// Returns a value indicating whether the given |queue| is empty. If |queue|
// is NULL, the return value is true.
bool fixed_pkt_queue_is_empty(fixed_pkt_queue_t *queue);

// Returns the length of the |queue|. If |queue| is NULL, the return value
// is 0.
size_t fixed_pkt_queue_length(fixed_pkt_queue_t *queue);

// Returns the maximum number of elements this queue may hold. |queue| may
// not be NULL.
size_t fixed_pkt_queue_capacity(fixed_pkt_queue_t *queue);

// Enqueues the given |data| into the |queue|. The caller will be blocked or immediately return or wait for timeout according to the parameter timeout.
// If enqueue failed, it will return false, otherwise return true
bool fixed_pkt_queue_enqueue(fixed_pkt_queue_t *queue, pkt_linked_item_t *linked_pkt, uint32_t timeout);

// Dequeues the next element from |queue|. If the queue is currently empty,
// this function will block the caller until an item is enqueued or immediately return or wait for timeout according to the parameter timeout.
// If dequeue failed, it will return NULL, otherwise return a point.
pkt_linked_item_t *fixed_pkt_queue_dequeue(fixed_pkt_queue_t *queue, uint32_t timeout);

// Returns the first element from |queue|, if present, without dequeuing it.
// This function will never block the caller. Returns NULL if there are no
// elements in the queue or |queue| is NULL.
pkt_linked_item_t *fixed_pkt_queue_try_peek_first(fixed_pkt_queue_t *queue);

// Registers |queue| with |reactor| for dequeue operations. When there is an element
// in the queue, ready_cb will be called. The |context| parameter is passed, untouched,
// to the callback routine. Neither |queue|, nor |reactor|, nor |read_cb| may be NULL.
// |context| may be NULL.
void fixed_pkt_queue_register_dequeue(fixed_pkt_queue_t *queue, fixed_pkt_queue_cb ready_cb);

// Unregisters the dequeue ready callback for |queue| from whichever reactor
// it is registered with, if any. This function is idempotent.
void fixed_pkt_queue_unregister_dequeue(fixed_pkt_queue_t *queue);

void fixed_pkt_queue_process(fixed_pkt_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif
