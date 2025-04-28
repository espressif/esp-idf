/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file is a patch for the multi_heap.c file stored in ROM
 * - added function multi_heap_walk
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "sdkconfig.h"
#include "esp_rom_multi_heap.h"

// Hook to force the linker to include this file
void esp_rom_include_multi_heap_patch(void)
{
}

/*!
 * @brief Opaque types for TLSF implementation
 */
typedef void* tlsf_t;
typedef void* pool_t;
typedef void* tlsf_walker;

typedef struct multi_heap_info {
    void *lock;
    size_t free_bytes;
    size_t minimum_free_bytes;
    size_t pool_size;
    void* heap_data;
} heap_t;

extern void tlsf_walk_pool(pool_t pool, tlsf_walker walker, void* user);
extern pool_t tlsf_get_pool(tlsf_t tlsf);
extern void multi_heap_internal_lock(multi_heap_handle_t heap);
extern void multi_heap_internal_unlock(multi_heap_handle_t heap);

void multi_heap_walk(multi_heap_handle_t heap, multi_heap_walker_cb_t walker_func, void *user_data)
{
    assert(heap != NULL);

    multi_heap_internal_lock(heap);
    tlsf_walk_pool(tlsf_get_pool(heap->heap_data), walker_func, user_data);
    multi_heap_internal_unlock(heap);
}

/**
 * @brief Structure used in multi_heap_find_containing_block to retain
 * information while walking a given heap to find the allocated block
 * containing the pointer ptr.
 *
 * @note The block_ptr gets filled with the pointer to the allocated block
 * containing the ptr.
 */
typedef struct containing_block_data {
    void *ptr; ///< Pointer to find the containing block of
    void *block_ptr; ///< Pointer to the containing block
} containing_block_data_t;


#if CONFIG_HEAP_POISONING_DISABLED
    void *multi_heap_find_containing_block(multi_heap_handle_t heap, void *ptr)
        __attribute__((alias("multi_heap_find_containing_block_impl")));
#endif

typedef struct block_header_t
{
	/* Points to the previous physical block. */
	struct block_header_t* prev_phys_block;

	/* The size of this block, excluding the block header. */
	size_t size;

	/* Next and previous free blocks. */
	struct block_header_t* next_free;
	struct block_header_t* prev_free;
} block_header_t;

#define tlsf_cast(t, exp)	((t) (exp))
#define block_header_free_bit (1UL << 0)
#define block_header_prev_free_bit (1UL << 1)
#define block_header_overhead (sizeof(size_t))
#define block_start_offset (offsetof(block_header_t, size) + sizeof(size_t))

#if !defined (tlsf_assert)
#define tlsf_assert assert
#endif

static inline __attribute__((always_inline)) void* block_to_ptr(const block_header_t* block)
{
	return tlsf_cast(void*,
		tlsf_cast(unsigned char*, block) + block_start_offset);
}
static size_t block_size(const block_header_t* block)
{
	return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}
static block_header_t* offset_to_block(const void* ptr, size_t size)
{
	return tlsf_cast(block_header_t*, tlsf_cast(ptrdiff_t, ptr) + size);
}
static int block_is_free(const block_header_t* block)
{
	return tlsf_cast(int, block->size & block_header_free_bit);
}
static int block_is_last(const block_header_t* block)
{
	return block_size(block) == 0;
}
static block_header_t* block_next(const block_header_t* block)
{
	block_header_t* next = offset_to_block(block_to_ptr(block),
		block_size(block) - block_header_overhead);
	tlsf_assert(!block_is_last(block));
	return next;
}

void* tlsf_find_containing_block(pool_t pool, void *ptr)
{
    block_header_t* block = offset_to_block(pool, -(int)block_header_overhead);

    while (block && !block_is_last(block))
    {
        if (!block_is_free(block)) {
            void *block_end = block_to_ptr(block) + block_size(block);
            if (block_to_ptr(block) <= ptr && block_end > ptr) {
                // we found the containing block, return
                return block_to_ptr(block);
            }
        }

        block = block_next(block);
    }

    return NULL;
}

void *multi_heap_find_containing_block_impl(multi_heap_handle_t heap, void *ptr)
{
    void *block_ptr = tlsf_find_containing_block(tlsf_get_pool(heap->heap_data), ptr);
    assert(block_ptr);
    return block_ptr;
}
