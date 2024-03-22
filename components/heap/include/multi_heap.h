/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/* multi_heap is a heap implementation for handling multiple
   heterogenous heaps in a single program.

   Any contiguous block of memory can be registered as a heap.
*/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Opaque handle to a registered heap */
typedef struct multi_heap_info *multi_heap_handle_t;

/**
 * @brief allocate a chunk of memory with specific alignment
 *
 * @param heap  Handle to a registered heap.
 * @param size  size in bytes of memory chunk
 * @param alignment  how the memory must be aligned
 *
 * @return pointer to the memory allocated, NULL on failure
 */
void *multi_heap_aligned_alloc(multi_heap_handle_t heap, size_t size, size_t alignment);

/** @brief malloc() a buffer in a given heap
 *
 * Semantics are the same as standard malloc(), only the returned buffer will be allocated in the specified heap.
 *
 * @param heap Handle to a registered heap.
 * @param size Size of desired buffer.
 *
 * @return Pointer to new memory, or NULL if allocation fails.
 */
void *multi_heap_malloc(multi_heap_handle_t heap, size_t size);

/** @brief free() a buffer aligned in a given heap.
 *
 * @param heap Handle to a registered heap.
 * @param p NULL, or a pointer previously returned from multi_heap_aligned_alloc() for the same heap.
 * @note This function is deprecated, consider using multi_heap_free() instead
 */
void __attribute__((deprecated)) multi_heap_aligned_free(multi_heap_handle_t heap, void *p);

/** @brief free() a buffer in a given heap.
 *
 * Semantics are the same as standard free(), only the argument 'p' must be NULL or have been allocated in the specified heap.
 *
 * @param heap Handle to a registered heap.
 * @param p NULL, or a pointer previously returned from multi_heap_malloc() or multi_heap_realloc() for the same heap.
 */
void multi_heap_free(multi_heap_handle_t heap, void *p);

/** @brief realloc() a buffer in a given heap.
 *
 * Semantics are the same as standard realloc(), only the argument 'p' must be NULL or have been allocated in the specified heap.
 *
 * @param heap Handle to a registered heap.
 * @param p NULL, or a pointer previously returned from multi_heap_malloc() or multi_heap_realloc() for the same heap.
 * @param size Desired new size for buffer.
 *
 * @return New buffer of 'size' containing contents of 'p', or NULL if reallocation failed.
 */
void *multi_heap_realloc(multi_heap_handle_t heap, void *p, size_t size);


/** @brief Return the size that a particular pointer was allocated with.
 *
 * @param heap Handle to a registered heap.
 * @param p Pointer, must have been previously returned from multi_heap_malloc() or multi_heap_realloc() for the same heap.
 *
 * @return Size of the memory allocated at this block. May be more than the original size argument, due
 * to padding and minimum block sizes.
 */
size_t multi_heap_get_allocated_size(multi_heap_handle_t heap, void *p);


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
multi_heap_handle_t multi_heap_register(void *start, size_t size);


/** @brief Associate a private lock pointer with a heap
 *
 * The lock argument is supplied to the MULTI_HEAP_LOCK() and MULTI_HEAP_UNLOCK() macros, defined in multi_heap_platform.h.
 *
 * The lock in question must be recursive.
 *
 * When the heap is first registered, the associated lock is NULL.
 *
 * @param heap Handle to a registered heap.
 * @param lock Optional pointer to a locking structure to associate with this heap.
 */
void multi_heap_set_lock(multi_heap_handle_t heap, void* lock);

/** @brief Dump heap information to stdout
 *
 * For debugging purposes, this function dumps information about every block in the heap to stdout.
 *
 * @param heap Handle to a registered heap.
 */
void multi_heap_dump(multi_heap_handle_t heap);

/** @brief Check heap integrity
 *
 * Walks the heap and checks all heap data structures are valid. If any errors are detected, an error-specific message
 * can be optionally printed to stderr. Print behaviour can be overridden at compile time by defining
 * MULTI_CHECK_FAIL_PRINTF in multi_heap_platform.h.
 *
 * @note This function is not thread-safe as it sets a global variable with the value of print_errors.
 *
 * @param heap Handle to a registered heap.
 * @param print_errors If true, errors will be printed to stderr.
 * @return true if heap is valid, false otherwise.
 */
bool multi_heap_check(multi_heap_handle_t heap, bool print_errors);

/** @brief Return free heap size
 *
 * Returns the number of bytes available in the heap.
 *
 * Equivalent to the total_free_bytes member returned by multi_heap_get_heap_info().
 *
 * Note that the heap may be fragmented, so the actual maximum size for a single malloc() may be lower. To know this
 * size, see the largest_free_block member returned by multi_heap_get_heap_info().
 *
 * @param heap Handle to a registered heap.
 * @return Number of free bytes.
 */
size_t multi_heap_free_size(multi_heap_handle_t heap);

/** @brief Return the lifetime minimum free heap size
 *
 * Equivalent to the minimum_free_bytes member returned by multi_heap_get_info().
 *
 * Returns the lifetime "low watermark" of possible values returned from multi_free_heap_size(), for the specified
 * heap.
 *
 * @param heap Handle to a registered heap.
 * @return Number of free bytes.
 */
size_t multi_heap_minimum_free_size(multi_heap_handle_t heap);

/** @brief Structure to access heap metadata via multi_heap_get_info */
typedef struct {
    size_t total_free_bytes;      ///<  Total free bytes in the heap. Equivalent to multi_free_heap_size().
    size_t total_allocated_bytes; ///<  Total bytes allocated to data in the heap.
    size_t largest_free_block;    ///<  Size of the largest free block in the heap. This is the largest malloc-able size.
    size_t minimum_free_bytes;    ///<  Lifetime minimum free heap size. Equivalent to multi_minimum_free_heap_size().
    size_t allocated_blocks;      ///<  Number of (variable size) blocks allocated in the heap.
    size_t free_blocks;           ///<  Number of (variable size) free blocks in the heap.
    size_t total_blocks;          ///<  Total number of (variable size) blocks in the heap.
} multi_heap_info_t;

/** @brief Return metadata about a given heap
 *
 * Fills a multi_heap_info_t structure with information about the specified heap.
 *
 * @param heap Handle to a registered heap.
 * @param info Pointer to a structure to fill with heap metadata.
 */
void multi_heap_get_info(multi_heap_handle_t heap, multi_heap_info_t *info);

/**
 * @brief Perform an aligned allocation from the provided offset
 *
 * @param heap The heap in which to perform the allocation
 * @param size The size of the allocation
 * @param alignment How the memory must be aligned
 * @param offset The offset at which the alignment should start
 * @return void* The ptr to the allocated memory
 */
void *multi_heap_aligned_alloc_offs(multi_heap_handle_t heap, size_t size, size_t alignment, size_t offset);

/**
 * @brief Reset the minimum_free_bytes value (setting it to free_bytes) and return the former value
 *
 * @param heap The heap in which the reset is taking place
 * @return size_t the value of minimum_free_bytes before it is reset
 */
size_t multi_heap_reset_minimum_free_bytes(multi_heap_handle_t heap);

/**
 * @brief Set the value of minimum_free_bytes to new_minimum_free_bytes_value or keep
 * the current value of minimum_free_bytes if it is smaller than new_minimum_free_bytes_value
 *
 * @param heap The heap in which the restore is taking place
 * @param new_minimum_free_bytes_value The value to restore the minimum_free_bytes to
 */
void multi_heap_restore_minimum_free_bytes(multi_heap_handle_t heap, const size_t new_minimum_free_bytes_value);

/**
 * @brief Callback called when walking the given heap blocks of memory
 *
 * @param block_ptr Pointer to the block data
 * @param block_size The size of the block
 * @param block_used Block status. 0: free, 1: allocated
 * @param user_data Opaque pointer to user defined data
 *
 * @return True if the walker is expected to continue the heap traversal
 *         False if the walker is expected to stop the traversal of the heap
 */
typedef bool (*multi_heap_walker_cb_t)(void *block_ptr, size_t block_size, int block_used, void *user_data);

/**
 * @brief Call the tlsf_walk_pool function of the heap given as parameter with
 * the walker function passed as parameter
 *
 * @param heap The heap to traverse
 * @param walker_func The walker to trigger on each block of the heap
 * @param user_data Opaque pointer to user defined data
 */
void multi_heap_walk(multi_heap_handle_t heap, multi_heap_walker_cb_t walker_func, void *user_data);

#ifdef __cplusplus
}
#endif
