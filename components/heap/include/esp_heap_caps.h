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

#include <stdint.h>
#include <stdlib.h>
#include "multi_heap.h"

/**
 * @brief Flags to indicate the capabilities of the various memory systems
 */
#define MALLOC_CAP_EXEC             (1<<0)  ///< Memory must be able to run executable code
#define MALLOC_CAP_32BIT            (1<<1)  ///< Memory must allow for aligned 32-bit data accesses
#define MALLOC_CAP_8BIT             (1<<2)  ///< Memory must allow for 8/16/...-bit data accesses
#define MALLOC_CAP_DMA              (1<<3)  ///< Memory must be able to accessed by DMA
#define MALLOC_CAP_PID2             (1<<4)  ///< Memory must be mapped to PID2 memory space (PIDs are not currently used)
#define MALLOC_CAP_PID3             (1<<5)  ///< Memory must be mapped to PID3 memory space (PIDs are not currently used)
#define MALLOC_CAP_PID4             (1<<6)  ///< Memory must be mapped to PID4 memory space (PIDs are not currently used)
#define MALLOC_CAP_PID5             (1<<7)  ///< Memory must be mapped to PID5 memory space (PIDs are not currently used)
#define MALLOC_CAP_PID6             (1<<8)  ///< Memory must be mapped to PID6 memory space (PIDs are not currently used)
#define MALLOC_CAP_PID7             (1<<9)  ///< Memory must be mapped to PID7 memory space (PIDs are not currently used)
#define MALLOC_CAP_SPISRAM          (1<<10) ///< Memory must be in SPI SRAM
#define MALLOC_CAP_INVALID          (1<<31) ///< Memory can't be used / list end marker


/**
 * @brief Initialize the capability-aware heap allocator.
 *
 * This is called once in the IDF startup code. Do not call it
 * at other times.
 */
void heap_caps_init();

/**
 * @brief Enable heap(s) in memory regions where the startup stacks are located.
 *
 * On startup, the pro/app CPUs have a certain memory region they use as stack, so we
 * cannot do allocations in the regions these stack frames are. When FreeRTOS is
 * completely started, they do not use that memory anymore and heap(s) there can
 * be enabled.
 */
void heap_caps_enable_nonos_stack_heaps();

/**
 * @brief Allocate a chunk of memory which has the given capabilities
 *
 * Equivalent semantics to libc malloc(), for capability-aware memory.
 *
 * In IDF, malloc(p) is equivalent to heaps_caps_malloc(p, MALLOC_CAP_8BIT);
 *
 * @param size Size, in bytes, of the amount of memory to allocate
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory to be returned
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 */
void *heap_caps_malloc(size_t size, uint32_t caps);

/**
 * @brief Free memory previously allocated via heap_caps_malloc() or heap_caps_realloc().
 *
 * Equivalent semantics to libc free(), for capability-aware memory.
 *
 *  In IDF, free(p) is equivalent to heap_caps_free(p).
 *
 * @param ptr Pointer to memory previously returned from heap_caps_malloc() or heap_caps_realloc(). Can be NULL.
 */
void heap_caps_free( void *ptr);

/**
 * @brief Reallocate memory previously allocated via heaps_caps_malloc() or heaps_caps_realloc().
 *
 * Equivalent semantics to libc realloc(), for capability-aware memory.
 *
 * In IDF, realloc(p, s) is equivalent to heap_caps_realloc(p, s, MALLOC_CAP_8BIT).
 *
 * 'caps' parameter can be different to the capabilities that any original 'ptr' was allocated with. In this way,
 * realloc can be used to "move" a buffer if necessary to ensure it meets new set of capabilities.
 *
 * @param ptr Pointer to previously allocated memory, or NULL for a new allocation.
 * @param size Size of the new buffer requested, or 0 to free the buffer.
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory desired for the new allocation.
 *
 * @return Pointer to a new buffer of size 'size' with capabilities 'caps', or NULL if allocation failed.
 */
void *heap_caps_realloc( void *ptr, size_t size, int caps);


/**
 * @brief Get the total free size of all the regions that have the given capabilities
 *
 * This function takes all regions capable of having the given capabilities allocated in them
 * and adds up the free space they have.
 *
 * Note that because of heap fragmentation it is probably not possible to allocate a single block of memory
 * of this size. Use heap_caps_get_largest_free_block() for this purpose.

 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return Amount of free bytes in the regions
 */
size_t heap_caps_get_free_size( uint32_t caps );


/**
 * @brief Get the total minimum free memory of all regions with the given capabilities
 *
 * This adds all the low water marks of the regions capable of delivering the memory
 * with the given capabilities.
 *
 * Note the result may be less than the global all-time minimum available heap of this kind, as "low water marks" are
 * tracked per-heap. Individual heaps may have reached their "low water marks" at different points in time. However
 * this result still gives a "worst case" indication for all-time free heap.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return Amount of free bytes in the regions
 */
size_t heap_caps_get_minimum_free_size( uint32_t caps );

/**
 * @brief Get the largest free block of memory able to be allocated with the given capabilities.
 *
 * Returns the largest value of 's' for which heap_caps_malloc(s, caps) will succeed.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return Size of largest free block in bytes.
 */
size_t heap_caps_get_largest_free_block( uint32_t caps );


/**
 * @brief Get heap info for all regions with the given capabilities.
 *
 * Calls multi_heap_info() on all heaps which share the given capabilities.  The information returned is an aggregate
 * across all matching heaps.  The meanings of fields are the same as defined for multi_heap_info_t, except that
 * minimum_free_bytes has the same caveats described in heap_caps_get_minimum_free_size().
 *
 * @param info        Pointer to a structure which will be filled with relevant
 *                    heap metadata.
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 */
void heap_caps_get_info( multi_heap_info_t *info, uint32_t caps );


/**
 * @brief Print a summary of all memory with the given capabilities.
 *
 * Calls multi_heap_info() on all heaps which share the given capabilities, and
 * prints a two-line summary for each, then a total summary.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 */
void heap_caps_print_heap_info( uint32_t caps );

