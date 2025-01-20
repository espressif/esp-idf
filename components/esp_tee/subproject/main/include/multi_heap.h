/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <string.h>
#include "tlsf.h"

/* multi_heap is a heap implementation for handling multiple
   heterogeneous heaps in a single program.

   Any contiguous block of memory can be registered as a heap.
*/

#ifdef __cplusplus
extern "C" {
#endif
struct multi_heap_info {
    void *lock;
    size_t free_bytes;
    size_t minimum_free_bytes;
    size_t pool_size;
    void *heap_data;
};

typedef struct multi_heap_info heap_t;

/** @brief Opaque handle to a registered heap */
typedef struct multi_heap_info *multi_heap_handle_t;

/** @brief malloc() a buffer in a given heap
 *
 * Semantics are the same as standard malloc(), only the returned buffer will be allocated in the TEE heap.
 *
 * @param size Size of desired buffer.
 *
 * @return Pointer to new memory, or NULL if allocation fails.
 */
void *tee_heap_malloc(size_t size);

/** @brief calloc() a buffer in a given heap
 *
 * Semantics are the same as standard calloc(), only the returned buffer will be allocated in the TEE heap.
 *
 * @param size Size of desired buffer.
 *
 * @return Pointer to new memory, or NULL if allocation fails.
 */
void *tee_heap_calloc(size_t n, size_t size);

/**
 * @brief allocate a chunk of memory with specific alignment
 *
 * @param heap  Handle to a registered heap.
 * @param size  size in bytes of memory chunk
 * @param alignment  how the memory must be aligned
 *
 * @return pointer to the memory allocated, NULL on failure
 */
void *tee_heap_aligned_alloc(size_t size, size_t alignment);

/** @brief free() a buffer in a given heap.
 *
 * Semantics are the same as standard free(), only the argument 'p' must be NULL or have been allocated in the TEE heap.
 *
 * @param p NULL, or a pointer previously returned from multi_heap_malloc() or multi_heap_realloc() for the same heap.
 */
void tee_heap_free(void *p);

/** @brief Register a new heap for use
 *
 * This function initialises a heap at the specified address, and returns a handle for future heap operations.
 *
 * There is no equivalent function for deregistering a heap - if all blocks in the heap are free, you can immediately start using the memory for other purposes.
 *
 * @param start Start address of the memory to use for a new heap.
 * @param size Size (in bytes) of the new heap.
 *
 * @return Handle of a new heap ready for use, or NULL if the heap region was too small to be initialised.
 */
int tee_heap_register(void *start, size_t size);

/**
 * @brief Dump free and minimum free TEE heap information to stdout
 *
 */
void tee_heap_dump_free_size(void);

/** @brief Dump TEE heap information to stdout
 *
 * For debugging purposes, this function dumps information about every block in the heap to stdout.
 *
 */
void tee_heap_dump_info(void);

#ifdef __cplusplus
}
#endif
