/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "multi_heap.h"
#include <sdkconfig.h>
#include "esp_err.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH
#define HEAP_IRAM_ATTR
#else
#define HEAP_IRAM_ATTR IRAM_ATTR
#endif

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
#define MALLOC_CAP_SPIRAM           (1<<10) ///< Memory must be in SPI RAM
#define MALLOC_CAP_INTERNAL         (1<<11) ///< Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off
#define MALLOC_CAP_DEFAULT          (1<<12) ///< Memory can be returned in a non-capability-specific memory allocation (e.g. malloc(), calloc()) call
#define MALLOC_CAP_IRAM_8BIT        (1<<13) ///< Memory must be in IRAM and allow unaligned access
#define MALLOC_CAP_RETENTION        (1<<14) ///< Memory must be able to accessed by retention DMA
#define MALLOC_CAP_RTCRAM           (1<<15) ///< Memory must be in RTC fast memory

#define MALLOC_CAP_INVALID          (1<<31) ///< Memory can't be used / list end marker

/**
 * @brief callback called when an allocation operation fails, if registered
 * @param size in bytes of failed allocation
 * @param caps capabilities requested of failed allocation
 * @param function_name function which generated the failure
 */
typedef void (*esp_alloc_failed_hook_t) (size_t size, uint32_t caps, const char * function_name);

/**
 * @brief registers a callback function to be invoked if a memory allocation operation fails
 * @param callback caller defined callback to be invoked
 * @return ESP_OK if callback was registered.
 */
esp_err_t heap_caps_register_failed_alloc_callback(esp_alloc_failed_hook_t callback);

#ifdef CONFIG_HEAP_USE_HOOKS
/**
 * @brief callback called after every allocation
 * @param ptr the allocated memory
 * @param size in bytes of the allocation
 * @param caps Bitwise OR of MALLOC_CAP_* flags indicating the type of memory allocated.
 * @note this hook is called on the same thread as the allocation, which may be within a low level operation.
 * You should refrain from doing heavy work, logging, flash writes, or any locking.
 */
__attribute__((weak)) HEAP_IRAM_ATTR void esp_heap_trace_alloc_hook(void* ptr, size_t size, uint32_t caps);

/**
 * @brief callback called after every free
 * @param ptr the memory that was freed
 * @note this hook is called on the same thread as the allocation, which may be within a low level operation.
 * You should refrain from doing heavy work, logging, flash writes, or any locking.
 */
__attribute__((weak)) HEAP_IRAM_ATTR void esp_heap_trace_free_hook(void* ptr);
#endif

/**
 * @brief Allocate a chunk of memory which has the given capabilities
 *
 * Equivalent semantics to libc malloc(), for capability-aware memory.
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
 *  In IDF, ``free(p)`` is equivalent to ``heap_caps_free(p)``.
 *
 * @param ptr Pointer to memory previously returned from heap_caps_malloc() or heap_caps_realloc(). Can be NULL.
 */
void heap_caps_free( void *ptr);

/**
 * @brief Reallocate memory previously allocated via heap_caps_malloc() or heap_caps_realloc().
 *
 * Equivalent semantics to libc realloc(), for capability-aware memory.
 *
 * In IDF, ``realloc(p, s)`` is equivalent to ``heap_caps_realloc(p, s, MALLOC_CAP_8BIT)``.
 *
 * 'caps' parameter can be different to the capabilities that any original 'ptr' was allocated with. In this way,
 * realloc can be used to "move" a buffer if necessary to ensure it meets a new set of capabilities.
 *
 * @param ptr Pointer to previously allocated memory, or NULL for a new allocation.
 * @param size Size of the new buffer requested, or 0 to free the buffer.
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory desired for the new allocation.
 *
 * @return Pointer to a new buffer of size 'size' with capabilities 'caps', or NULL if allocation failed.
 */
void *heap_caps_realloc( void *ptr, size_t size, uint32_t caps);

/**
 * @brief Allocate an aligned chunk of memory which has the given capabilities
 *
 * Equivalent semantics to libc aligned_alloc(), for capability-aware memory.
 * @param alignment  How the pointer received needs to be aligned
 *                   must be a power of two
 * @param size Size, in bytes, of the amount of memory to allocate
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory to be returned
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 *
 *
 */
void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps);

/**
 * @brief Used to deallocate memory previously allocated with heap_caps_aligned_alloc
 *
 * @param ptr Pointer to the memory allocated
 * @note This function is deprecated, please consider using heap_caps_free() instead
 */
void __attribute__((deprecated))  heap_caps_aligned_free(void *ptr);

/**
 * @brief Allocate an aligned chunk of memory which has the given capabilities. The initialized value in the memory is set to zero.
 *
 * @param alignment  How the pointer received needs to be aligned
 *                   must be a power of two
 * @param n    Number of continuing chunks of memory to allocate
 * @param size Size, in bytes, of a chunk of memory to allocate
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory to be returned
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 *
 */
void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size, uint32_t caps);


/**
 * @brief Allocate a chunk of memory which has the given capabilities. The initialized value in the memory is set to zero.
 *
 * Equivalent semantics to libc calloc(), for capability-aware memory.
 *
 * In IDF, ``calloc(p)`` is equivalent to ``heap_caps_calloc(p, MALLOC_CAP_8BIT)``.
 *
 * @param n    Number of continuing chunks of memory to allocate
 * @param size Size, in bytes, of a chunk of memory to allocate
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory to be returned
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 */
void *heap_caps_calloc(size_t n, size_t size, uint32_t caps);

/**
 * @brief Get the total size of all the regions that have the given capabilities
 *
 * This function takes all regions capable of having the given capabilities allocated in them
 * and adds up the total space they have.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return total size in bytes
 */

size_t heap_caps_get_total_size(uint32_t caps);

/**
 * @brief Get the total free size of all the regions that have the given capabilities
 *
 * This function takes all regions capable of having the given capabilities allocated in them
 * and adds up the free space they have.
 *
 * @note Note that because of heap fragmentation it is probably not possible to allocate a single block of memory
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
 * This adds all the low watermarks of the regions capable of delivering the memory
 * with the given capabilities.
 *
 * @note Note the result may be less than the global all-time minimum available heap of this kind, as "low watermarks" are
 * tracked per-region. Individual regions' heaps may have reached their "low watermarks" at different points in time. However,
 * this result still gives a "worst case" indication for all-time minimum free heap.
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
 * Returns the largest value of ``s`` for which ``heap_caps_malloc(s, caps)`` will succeed.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return Size of the largest free block in bytes.
 */
size_t heap_caps_get_largest_free_block( uint32_t caps );


/**
 * @brief Get heap info for all regions with the given capabilities.
 *
 * Calls multi_heap_info() on all heaps which share the given capabilities. The information returned is an aggregate
 * across all matching heaps. The meanings of fields are the same as defined for multi_heap_info_t, except that
 * ``minimum_free_bytes`` has the same caveats described in heap_caps_get_minimum_free_size().
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
 * Calls multi_heap_info on all heaps which share the given capabilities, and
 * prints a two-line summary for each, then a total summary.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 */
void heap_caps_print_heap_info( uint32_t caps );

/**
 * @brief Check integrity of all heap memory in the system.
 *
 * Calls multi_heap_check on all heaps. Optionally print errors if heaps are corrupt.
 *
 * Calling this function is equivalent to calling heap_caps_check_integrity
 * with the caps argument set to MALLOC_CAP_INVALID.
 *
 * @param print_errors Print specific errors if heap corruption is found.
 *
 * @note Please increase the value of `CONFIG_ESP_INT_WDT_TIMEOUT_MS` when using this API
 * with PSRAM enabled.
 *
 * @return True if all heaps are valid, False if at least one heap is corrupt.
 */
bool heap_caps_check_integrity_all(bool print_errors);

/**
 * @brief Check integrity of all heaps with the given capabilities.
 *
 * Calls multi_heap_check on all heaps which share the given capabilities. Optionally
 * print errors if the heaps are corrupt.
 *
 * See also heap_caps_check_integrity_all to check all heap memory
 * in the system and heap_caps_check_integrity_addr to check memory
 * around a single address.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 * @param print_errors Print specific errors if heap corruption is found.
 *
 * @note Please increase the value of `CONFIG_ESP_INT_WDT_TIMEOUT_MS` when using this API
 * with PSRAM capability flag.
 *
 * @return True if all heaps are valid, False if at least one heap is corrupt.
 */
bool heap_caps_check_integrity(uint32_t caps, bool print_errors);

/**
 * @brief Check integrity of heap memory around a given address.
 *
 * This function can be used to check the integrity of a single region of heap memory,
 * which contains the given address.
 *
 * This can be useful if debugging heap integrity for corruption at a known address,
 * as it has a lower overhead than checking all heap regions. Note that if the corrupt
 * address moves around between runs (due to timing or other factors) then this approach
 * won't work, and you should call heap_caps_check_integrity or
 * heap_caps_check_integrity_all instead.
 *
 * @note The entire heap region around the address is checked, not only the adjacent
 * heap blocks.
 *
 * @param addr Address in memory. Check for corruption in region containing this address.
 * @param print_errors Print specific errors if heap corruption is found.
 *
 * @return True if the heap containing the specified address is valid,
 * False if at least one heap is corrupt or the address doesn't belong to a heap region.
 */
bool heap_caps_check_integrity_addr(intptr_t addr, bool print_errors);

/**
 * @brief Enable malloc() in external memory and set limit below which
 *        malloc() attempts are placed in internal memory.
 *
 * When external memory is in use, the allocation strategy is to initially try to
 * satisfy smaller allocation requests with internal memory and larger requests
 * with external memory. This sets the limit between the two, as well as generally
 * enabling allocation in external memory.
 *
 * @param limit       Limit, in bytes.
 */
void heap_caps_malloc_extmem_enable(size_t limit);

/**
 * @brief Allocate a chunk of memory as preference in decreasing order.
 *
 * @attention The variable parameters are bitwise OR of MALLOC_CAP_* flags indicating the type of memory.
 *            This API prefers to allocate memory with the first parameter. If failed, allocate memory with
 *            the next parameter. It will try in this order until allocating a chunk of memory successfully
 *            or fail to allocate memories with any of the parameters.
 *
 * @param size Size, in bytes, of the amount of memory to allocate
 * @param num Number of variable parameters
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 */
void *heap_caps_malloc_prefer( size_t size, size_t num, ... );

/**
 * @brief Reallocate a chunk of memory as preference in decreasing order.
 *
 * @param ptr Pointer to previously allocated memory, or NULL for a new allocation.
 * @param size Size of the new buffer requested, or 0 to free the buffer.
 * @param num Number of variable paramters
 *
 * @return Pointer to a new buffer of size 'size', or NULL if allocation failed.
 */
void *heap_caps_realloc_prefer( void *ptr, size_t size, size_t num, ... );

/**
 * @brief Allocate a chunk of memory as preference in decreasing order.
 *
 * @param n    Number of continuing chunks of memory to allocate
 * @param size Size, in bytes, of a chunk of memory to allocate
 * @param num  Number of variable paramters
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 */
void *heap_caps_calloc_prefer( size_t n, size_t size, size_t num, ... );

/**
 * @brief Dump the full structure of all heaps with matching capabilities.
 *
 * Prints a large amount of output to serial (because of locking limitations,
 * the output bypasses stdout/stderr). For each (variable sized) block
 * in each matching heap, the following output is printed on a single line:
 *
 * - Block address (the data buffer returned by malloc is 4 bytes after this
 *   if heap debugging is set to Basic, or 8 bytes otherwise).
 * - Data size (the data size may be larger than the size requested by malloc,
 *   either due to heap fragmentation or because of heap debugging level).
 * - Address of next block in the heap.
 * - If the block is free, the address of the next free block is also printed.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 */
void heap_caps_dump(uint32_t caps);

/**
 * @brief Dump the full structure of all heaps.
 *
 * Covers all registered heaps. Prints a large amount of output to serial.
 *
 * Output is the same as for heap_caps_dump.
 *
 */
void heap_caps_dump_all(void);

/**
 * @brief Return the size that a particular pointer was allocated with.
 *
 * @param ptr Pointer to currently allocated heap memory. Must be a pointer value previously
 * returned by heap_caps_malloc, malloc, calloc, etc. and not yet freed.
 *
 * @note The app will crash with an assertion failure if the pointer is not valid.
 *
 * @return Size of the memory allocated at this block.
 *
 */
size_t heap_caps_get_allocated_size( void *ptr );

#ifdef __cplusplus
}
#endif
