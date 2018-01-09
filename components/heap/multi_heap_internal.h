// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

/* Internal definitions for the "implementation" of the multi_heap API,
   as defined in multi_heap.c.

   If heap poisioning is disabled, these are aliased directly to the public API.

   If heap poisoning is enabled, wrapper functions call each of these.
*/
void *multi_heap_malloc_impl(multi_heap_handle_t heap, size_t size);
void multi_heap_free_impl(multi_heap_handle_t heap, void *p);
void *multi_heap_realloc_impl(multi_heap_handle_t heap, void *p, size_t size);
multi_heap_handle_t multi_heap_register_impl(void *start, size_t size);
void multi_heap_get_info_impl(multi_heap_handle_t heap, multi_heap_info_t *info);
size_t multi_heap_free_size_impl(multi_heap_handle_t heap);
size_t multi_heap_minimum_free_size_impl(multi_heap_handle_t heap);
size_t multi_heap_get_allocated_size_impl(multi_heap_handle_t heap, void *p);

/* Some internal functions for heap poisoning use */

/* Check an allocated block's poison bytes are correct. Called by multi_heap_check(). */
bool multi_heap_internal_check_block_poisoning(void *start, size_t size, bool is_free, bool print_errors);

/* Fill a region of memory with the free or malloced pattern.
   Called when merging blocks, to overwrite the old block header.
*/
void multi_heap_internal_poison_fill_region(void *start, size_t size, bool is_free);

/* Allow heap poisoning to lock/unlock the heap to avoid race conditions
   if multi_heap_check() is running concurrently.
*/
void multi_heap_internal_lock(multi_heap_handle_t heap);

void multi_heap_internal_unlock(multi_heap_handle_t heap);

struct heap_block;

/* Block in the heap

   Heap implementation uses two single linked lists, a block list (all blocks) and a free list (free blocks).

   'header' holds a pointer to the next block (used or free) ORed with a free flag (the LSB of the pointer.) is_free() and get_next_block() utility functions allow typed access to these values.

   'next_free' is valid if the block is free and is a pointer to the next block in the free list.
*/
typedef struct heap_block {
    intptr_t header;                  /* Encodes next block in heap (used or unused) and also free/used flag */
    union {
        uint8_t data[1];              /* First byte of data, valid if block is used. Actual size of data is 'block_data_size(block)' */
        struct heap_block *next_free; /* Pointer to next free block, valid if block is free */
    };
} heap_block_t;

/* These masks apply to the 'header' field of heap_block_t */
#define BLOCK_FREE_FLAG 0x1  /* If set, this block is free & next_free pointer is valid */
#define NEXT_BLOCK_MASK (~3) /* AND header with this mask to get pointer to next block (free or used) */

/* Metadata header for the heap, stored at the beginning of heap space.

   'first_block' is a "fake" first block, minimum length, used to provide a pointer to the first used & free block in
   the heap. This block is never allocated or merged into an adjacent block.

   'last_block' is a pointer to a final free block of length 0, which is added at the end of the heap when it is
   registered. This block is also never allocated or merged into an adjacent block.
 */
typedef struct multi_heap_metadata {
    void *lock;
    size_t free_bytes;
    size_t minimum_free_bytes;
    heap_block_t *last_block;
    heap_block_t first_block; /* initial 'free block', never allocated */
} heap_metadata_t;
