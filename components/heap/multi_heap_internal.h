/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/* Define a noclone attribute when compiled with GCC as certain functions
 * in the heap component should not be cloned by the compiler */
#if defined __has_attribute && __has_attribute(noclone)
#define NOCLONE_ATTR __attribute((noclone))
#else
#define NOCLONE_ATTR
#endif

/* Define a structure that contains some function pointers that point to OS-related functions.
   An instance of this structure will be provided to the heap in ROM for use if needed.
*/
typedef struct {
    void (*lock)(void *lock);
    void (*unlock)(void *lock);
} multi_heap_os_funcs_t;

/** @brief Initialize structure pointer that points a structure that contains OS-related functions pointers.
 *
 * @param heap_os_funcs Points to a structure that contains some OS-related function pointers.
 * @return None.
 *
 */
void multi_heap_os_funcs_init(multi_heap_os_funcs_t *heap_os_funcs);

/* Opaque handle to a heap block */
typedef const struct block_header_t *multi_heap_block_handle_t;

/* Internal definitions for the "implementation" of the multi_heap API,
   as defined in multi_heap.c.

   If heap poisioning is disabled, these are aliased directly to the public API.

   If heap poisoning is enabled, wrapper functions call each of these.
*/

void *multi_heap_malloc_impl(multi_heap_handle_t heap, size_t size);

/* Allocate a memory region of minimum `size` bytes, aligned on `alignment`. */
void *multi_heap_aligned_alloc_impl(multi_heap_handle_t heap, size_t size, size_t alignment);

/* Allocate a memory region of minimum `size` bytes, where memory's `offset` is aligned on `alignment`. */
void *multi_heap_aligned_alloc_impl_offs(multi_heap_handle_t heap, size_t size, size_t alignment, size_t offset);

void multi_heap_free_impl(multi_heap_handle_t heap, void *p);
void *multi_heap_realloc_impl(multi_heap_handle_t heap, void *p, size_t size);
multi_heap_handle_t multi_heap_register_impl(void *start, size_t size);
void multi_heap_get_info_impl(multi_heap_handle_t heap, multi_heap_info_t *info);
size_t multi_heap_free_size_impl(multi_heap_handle_t heap);
size_t multi_heap_minimum_free_size_impl(multi_heap_handle_t heap);
size_t multi_heap_get_allocated_size_impl(multi_heap_handle_t heap, void *p);
void *multi_heap_get_block_address_impl(multi_heap_block_handle_t block);

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

/* Some internal functions for heap debugging code to use */

/* Get the handle to the first (fixed free) block in a heap */
multi_heap_block_handle_t multi_heap_get_first_block(multi_heap_handle_t heap);

/* Get the handle to the next block in a heap, with validation */
multi_heap_block_handle_t multi_heap_get_next_block(multi_heap_handle_t heap, multi_heap_block_handle_t block);

/* Test if a heap block is free */
bool multi_heap_is_free(const multi_heap_block_handle_t block);

/* Get the data address of a heap block */
void *multi_heap_get_block_address(multi_heap_block_handle_t block);

/* Get the owner identification for a heap block */
void *multi_heap_get_block_owner(multi_heap_block_handle_t block);
