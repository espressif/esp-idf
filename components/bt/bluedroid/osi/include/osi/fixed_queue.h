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

#ifndef _FIXED_QUEUE_H_
#define _FIXED_QUEUE_H_

#include <stdbool.h>
#include "osi/list.h"

#ifndef QUEUE_SIZE_MAX
#define QUEUE_SIZE_MAX                    254
#endif

struct fixed_queue_t;

typedef struct fixed_queue_t fixed_queue_t;
//typedef struct reactor_t reactor_t;

typedef void (*fixed_queue_free_cb)(void *data);
typedef void (*fixed_queue_cb)(fixed_queue_t *queue);

// Creates a new fixed queue with the given |capacity|. If more elements than
// |capacity| are added to the queue, the caller is blocked until space is
// made available in the queue. Returns NULL on failure. The caller must free
// the returned queue with |fixed_queue_free|.
fixed_queue_t *fixed_queue_new(size_t capacity);

// Freeing a queue that is currently in use (i.e. has waiters
// blocked on it) results in undefined behaviour.
void fixed_queue_free(fixed_queue_t *queue, fixed_queue_free_cb free_cb);

// Returns a value indicating whether the given |queue| is empty. If |queue|
// is NULL, the return value is true.
bool fixed_queue_is_empty(fixed_queue_t *queue);

// Returns the length of the |queue|. If |queue| is NULL, the return value
// is 0.
size_t fixed_queue_length(fixed_queue_t *queue);

// Returns the maximum number of elements this queue may hold. |queue| may
// not be NULL.
size_t fixed_queue_capacity(fixed_queue_t *queue);

// Enqueues the given |data| into the |queue|. The caller will be blocked
// if nore more space is available in the queue. Neither |queue| nor |data|
// may be NULL.
void fixed_queue_enqueue(fixed_queue_t *queue, void *data);

// Dequeues the next element from |queue|. If the queue is currently empty,
// this function will block the caller until an item is enqueued. This
// function will never return NULL. |queue| may not be NULL.
void *fixed_queue_dequeue(fixed_queue_t *queue);

// Tries to enqueue |data| into the |queue|. This function will never block
// the caller. If the queue capacity would be exceeded by adding one more
// element, this function returns false immediately. Otherwise, this function
// returns true. Neither |queue| nor |data| may be NULL.
bool fixed_queue_try_enqueue(fixed_queue_t *queue, void *data);

// Tries to dequeue an element from |queue|. This function will never block
// the caller. If the queue is empty, this function returns NULL immediately.
// Otherwise, the next element in the queue is returned. |queue| may not be
// NULL.
void *fixed_queue_try_dequeue(fixed_queue_t *queue);

// Returns the first element from |queue|, if present, without dequeuing it.
// This function will never block the caller. Returns NULL if there are no
// elements in the queue or |queue| is NULL.
void *fixed_queue_try_peek_first(fixed_queue_t *queue);

// Returns the last element from |queue|, if present, without dequeuing it.
// This function will never block the caller. Returns NULL if there are no
// elements in the queue or |queue| is NULL.
void *fixed_queue_try_peek_last(fixed_queue_t *queue);

// Tries to remove a |data| element from the middle of the |queue|. This
// function will never block the caller. If the queue is empty or NULL, this
// function returns NULL immediately. |data| may not be NULL. If the |data|
// element is found in the queue, a pointer to the removed data is returned,
// otherwise NULL.
void *fixed_queue_try_remove_from_queue(fixed_queue_t *queue, void *data);

// Returns the iterateable list with all entries in the |queue|. This function
// will never block the caller. |queue| may not be NULL.
//
// NOTE: The return result of this function is not thread safe: the list could
// be modified by another thread, and the result would be unpredictable.
// TODO: The usage of this function should be refactored, and the function
// itself should be removed.
list_t *fixed_queue_get_list(fixed_queue_t *queue);

// This function returns a valid file descriptor. Callers may perform one
// operation on the fd: select(2). If |select| indicates that the file
// descriptor is readable, the caller may call |fixed_queue_enqueue| without
// blocking. The caller must not close the returned file descriptor. |queue|
// may not be NULL.
//int fixed_queue_get_enqueue_fd(const fixed_queue_t *queue);

// This function returns a valid file descriptor. Callers may perform one
// operation on the fd: select(2). If |select| indicates that the file
// descriptor is readable, the caller may call |fixed_queue_dequeue| without
// blocking. The caller must not close the returned file descriptor. |queue|
// may not be NULL.
//int fixed_queue_get_dequeue_fd(const fixed_queue_t *queue);

// Registers |queue| with |reactor| for dequeue operations. When there is an element
// in the queue, ready_cb will be called. The |context| parameter is passed, untouched,
// to the callback routine. Neither |queue|, nor |reactor|, nor |read_cb| may be NULL.
// |context| may be NULL.
void fixed_queue_register_dequeue(fixed_queue_t *queue, fixed_queue_cb ready_cb);

// Unregisters the dequeue ready callback for |queue| from whichever reactor
// it is registered with, if any. This function is idempotent.
void fixed_queue_unregister_dequeue(fixed_queue_t *queue);

void fixed_queue_process(fixed_queue_t *queue);

list_t *fixed_queue_get_list(fixed_queue_t *queue);

#endif
