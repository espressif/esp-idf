/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include "multi_heap.h"
#include "multi_heap_internal.h"

#if !CONFIG_HEAP_TLSF_USE_ROM_IMPL
#include "tlsf.h"
#include "tlsf_block_functions.h"
#endif

/* Note: Keep platform-specific parts in this header, this source
   file should depend on libc only */
#include "multi_heap_platform.h"

/* Defines compile-time configuration macros */
#include "multi_heap_config.h"

#if (!defined MULTI_HEAP_POISONING)

void *multi_heap_aligned_alloc_offs(multi_heap_handle_t heap, size_t size, size_t alignment, size_t offset)
{
    return multi_heap_aligned_alloc_impl_offs(heap, size, alignment, offset);
}

#if (!defined CONFIG_HEAP_TLSF_USE_ROM_IMPL)
/* if no heap poisoning, public API aliases directly to these implementations */
void *multi_heap_malloc(multi_heap_handle_t heap, size_t size)
    __attribute__((alias("multi_heap_malloc_impl")));

void *multi_heap_aligned_alloc(multi_heap_handle_t heap, size_t size, size_t alignment)
    __attribute__((alias("multi_heap_aligned_alloc_impl")));

void multi_heap_aligned_free(multi_heap_handle_t heap, void *p)
    __attribute__((alias("multi_heap_free_impl")));

void multi_heap_free(multi_heap_handle_t heap, void *p)
    __attribute__((alias("multi_heap_free_impl")));

void *multi_heap_realloc(multi_heap_handle_t heap, void *p, size_t size)
    __attribute__((alias("multi_heap_realloc_impl")));

size_t multi_heap_get_allocated_size(multi_heap_handle_t heap, void *p)
    __attribute__((alias("multi_heap_get_allocated_size_impl")));

multi_heap_handle_t multi_heap_register(void *start, size_t size)
    __attribute__((alias("multi_heap_register_impl")));

void multi_heap_get_info(multi_heap_handle_t heap, multi_heap_info_t *info)
    __attribute__((alias("multi_heap_get_info_impl")));

size_t multi_heap_free_size(multi_heap_handle_t heap)
    __attribute__((alias("multi_heap_free_size_impl")));

size_t multi_heap_minimum_free_size(multi_heap_handle_t heap)
    __attribute__((alias("multi_heap_minimum_free_size_impl")));

void *multi_heap_get_block_address(multi_heap_block_handle_t block)
    __attribute__((alias("multi_heap_get_block_address_impl")));

#endif // !CONFIG_HEAP_TLSF_USE_ROM_IMPL
#endif // !MULTI_HEAP_POISONING

#define ALIGN(X) ((X) & ~(sizeof(void *)-1))
#define ALIGN_UP(X) ALIGN((X)+sizeof(void *)-1)
#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))


typedef struct multi_heap_info {
    void *lock;
    size_t free_bytes;
    size_t minimum_free_bytes;
    size_t pool_size;
    void* heap_data;
} heap_t;

#if CONFIG_HEAP_TLSF_USE_ROM_IMPL

void _multi_heap_lock(void *lock)
{
    MULTI_HEAP_LOCK(lock);
}

void _multi_heap_unlock(void *lock)
{
    MULTI_HEAP_UNLOCK(lock);
}

multi_heap_os_funcs_t multi_heap_os_funcs = {
    .lock = _multi_heap_lock,
    .unlock = _multi_heap_unlock,
};

void multi_heap_in_rom_init(void)
{
    multi_heap_os_funcs_init(&multi_heap_os_funcs);
}

#else // CONFIG_HEAP_TLSF_USE_ROM_IMPL

/* Check a block is valid for this heap. Used to verify parameters. */
__attribute__((noinline)) NOCLONE_ATTR static void assert_valid_block(const heap_t *heap, const block_header_t *block)
{
    pool_t pool = tlsf_get_pool(heap->heap_data);
    void *ptr = block_to_ptr(block);

    MULTI_HEAP_ASSERT((ptr >= pool) &&
                    (ptr < pool + heap->pool_size),
                    (uintptr_t)ptr);
}

void *multi_heap_get_block_address_impl(multi_heap_block_handle_t block)
{
    return block_to_ptr(block);
}

size_t multi_heap_get_allocated_size_impl(multi_heap_handle_t heap, void *p)
{
    return tlsf_block_size(p);
}

multi_heap_handle_t multi_heap_register_impl(void *start_ptr, size_t size)
{
    assert(start_ptr);
    if(size < (sizeof(heap_t))) {
        //Region too small to be a heap.
        return NULL;
    }

    heap_t *result = (heap_t *)start_ptr;
    size -= sizeof(heap_t);

    /* Do not specify any maximum size for the allocations so that the default configuration is used */
    const size_t max_bytes = 0;

    result->heap_data = tlsf_create_with_pool(start_ptr + sizeof(heap_t), size, max_bytes);
    if(!result->heap_data) {
        return NULL;
    }

    result->lock = NULL;
    result->free_bytes = size - tlsf_size(result->heap_data);
    result->pool_size = size;
    result->minimum_free_bytes = result->free_bytes;
    return result;
}

void multi_heap_set_lock(multi_heap_handle_t heap, void *lock)
{
    heap->lock = lock;
}

void multi_heap_internal_lock(multi_heap_handle_t heap)
{
    MULTI_HEAP_LOCK(heap->lock);
}

void multi_heap_internal_unlock(multi_heap_handle_t heap)
{
    MULTI_HEAP_UNLOCK(heap->lock);
}

multi_heap_block_handle_t multi_heap_get_first_block(multi_heap_handle_t heap)
{
    assert(heap != NULL);
    pool_t pool = tlsf_get_pool(heap->heap_data);
    block_header_t* block = offset_to_block(pool, -(int)block_header_overhead);

    return (multi_heap_block_handle_t)block;
}

multi_heap_block_handle_t multi_heap_get_next_block(multi_heap_handle_t heap, multi_heap_block_handle_t block)
{
    assert(heap != NULL);
    assert_valid_block(heap, block);
    block_header_t* next = block_next(block);

    if(block_size(next) == 0) {
        //Last block:
        return NULL;
    } else {
        return (multi_heap_block_handle_t)next;
    }

}

bool multi_heap_is_free(multi_heap_block_handle_t block)
{
    return block_is_free(block);
}

void *multi_heap_malloc_impl(multi_heap_handle_t heap, size_t size)
{
    if (size == 0 || heap == NULL) {
        return NULL;
    }


    multi_heap_internal_lock(heap);
    void *result = tlsf_malloc(heap->heap_data, size);
    if(result) {
        heap->free_bytes -= tlsf_block_size(result);
        heap->free_bytes -= tlsf_alloc_overhead();
        if (heap->free_bytes < heap->minimum_free_bytes) {
            heap->minimum_free_bytes = heap->free_bytes;
        }
    }
    multi_heap_internal_unlock(heap);

    return result;
}

void multi_heap_free_impl(multi_heap_handle_t heap, void *p)
{
    if (heap == NULL || p == NULL) {
        return;
    }

    assert_valid_block(heap, block_from_ptr(p));

    multi_heap_internal_lock(heap);
    heap->free_bytes += tlsf_block_size(p);
    heap->free_bytes += tlsf_alloc_overhead();
    tlsf_free(heap->heap_data, p);
    multi_heap_internal_unlock(heap);
}

void *multi_heap_realloc_impl(multi_heap_handle_t heap, void *p, size_t size)
{
    assert(heap != NULL);

    if (p == NULL) {
        return multi_heap_malloc_impl(heap, size);
    }

    assert_valid_block(heap, block_from_ptr(p));

    if (heap == NULL) {
        return NULL;
    }

    multi_heap_internal_lock(heap);
    size_t previous_block_size =  tlsf_block_size(p);
    void *result = tlsf_realloc(heap->heap_data, p, size);
    if(result) {
        /* No need to subtract the tlsf_alloc_overhead() as it has already
         * been subtracted when allocating the block at first with malloc */
        heap->free_bytes += previous_block_size;
        heap->free_bytes -= tlsf_block_size(result);
        if (heap->free_bytes < heap->minimum_free_bytes) {
            heap->minimum_free_bytes = heap->free_bytes;
        }
    }

    multi_heap_internal_unlock(heap);

    return result;
}

void *multi_heap_aligned_alloc_impl_offs(multi_heap_handle_t heap, size_t size, size_t alignment, size_t offset)
{
    if(heap == NULL) {
        return NULL;
    }

    if(!size) {
        return NULL;
    }

    //Alignment must be a power of two:
    if(((alignment & (alignment - 1)) != 0) ||(!alignment)) {
        return NULL;
    }

    multi_heap_internal_lock(heap);
    void *result = tlsf_memalign_offs(heap->heap_data, alignment, size, offset);
    if(result) {
        heap->free_bytes -= tlsf_block_size(result);
        heap->free_bytes -= tlsf_alloc_overhead();
        if(heap->free_bytes < heap->minimum_free_bytes) {
            heap->minimum_free_bytes = heap->free_bytes;
        }
    }
    multi_heap_internal_unlock(heap);

    return result;
}


void *multi_heap_aligned_alloc_impl(multi_heap_handle_t heap, size_t size, size_t alignment)
{
    return multi_heap_aligned_alloc_impl_offs(heap, size, alignment, 0);
}

#ifdef MULTI_HEAP_POISONING
/*!
 * @brief Global definition of print_errors set in multi_heap_check() when
 * MULTI_HEAP_POISONING is active. Allows the transfer of the value to
 * multi_heap_poisoning.c without having to propagate it to the tlsf submodule
 * and back.
 */
static bool g_print_errors = false;

/*!
 * @brief Definition of the weak function declared in TLSF repository.
 * The call of this function execute a check for block poisoning on the memory
 * chunk passed as parameter.
 *
 * @param start: pointer to the start of the memory region to check for corruption
 * @param size: size of the memory region to check for corruption
 * @param is_free: indicate if the pattern to use the fill the region should be
 * an after free or after allocation pattern.
 *
 * @return bool: true if the the memory is not corrupted, false if the memory if corrupted.
 */
bool tlsf_check_hook(void *start, size_t size, bool is_free)
{
    return multi_heap_internal_check_block_poisoning(start, size, is_free, g_print_errors);
}
#endif // MULTI_HEAP_POISONING

bool multi_heap_check(multi_heap_handle_t heap, bool print_errors)
{
    bool valid = true;
    assert(heap != NULL);

    multi_heap_internal_lock(heap);

#ifdef MULTI_HEAP_POISONING
    g_print_errors = print_errors;
#else
    (void) print_errors;
#endif

    if(tlsf_check(heap->heap_data)) {
        valid = false;
    }

    if(tlsf_check_pool(tlsf_get_pool(heap->heap_data))) {
        valid = false;
    }

    multi_heap_internal_unlock(heap);
    return valid;
}

__attribute__((noinline)) static void multi_heap_dump_tlsf(void* ptr, size_t size, int used, void* user)
{
    (void)user;
    MULTI_HEAP_STDERR_PRINTF("Block %p data, size: %d bytes, Free: %s \n",
                            (void *)ptr,
                            size,
                            used ? "No" : "Yes");
}

void multi_heap_dump(multi_heap_handle_t heap)
{
    assert(heap != NULL);

    multi_heap_internal_lock(heap);
    MULTI_HEAP_STDERR_PRINTF("Showing data for heap: %p \n", (void *)heap);
    tlsf_walk_pool(tlsf_get_pool(heap->heap_data), multi_heap_dump_tlsf, NULL);
    multi_heap_internal_unlock(heap);
}

size_t multi_heap_free_size_impl(multi_heap_handle_t heap)
{
    if (heap == NULL) {
        return 0;
    }

    return heap->free_bytes;
}

size_t multi_heap_minimum_free_size_impl(multi_heap_handle_t heap)
{
    if (heap == NULL) {
        return 0;
    }

    return heap->minimum_free_bytes;
}

__attribute__((noinline)) static void multi_heap_get_info_tlsf(void* ptr, size_t size, int used, void* user)
{
    multi_heap_info_t *info = user;

    if(used) {
        info->allocated_blocks++;
    } else {
        info->free_blocks++;

        if(size > info->largest_free_block ) {
            info->largest_free_block = size;
        }
    }

    info->total_blocks++;
}

void multi_heap_get_info_impl(multi_heap_handle_t heap, multi_heap_info_t *info)
{
    uint32_t overhead;

    memset(info, 0, sizeof(multi_heap_info_t));

    if (heap == NULL) {
        return;
    }

    multi_heap_internal_lock(heap);
    tlsf_walk_pool(tlsf_get_pool(heap->heap_data), multi_heap_get_info_tlsf, info);
    /* TLSF has an overhead per block. Calculate the total amount of overhead, it shall not be
     * part of the allocated bytes */
    overhead = info->allocated_blocks * tlsf_alloc_overhead();
    info->total_allocated_bytes = (heap->pool_size - tlsf_size(heap->heap_data)) - heap->free_bytes - overhead;
    info->minimum_free_bytes = heap->minimum_free_bytes;
    info->total_free_bytes = heap->free_bytes;
    info->largest_free_block = tlsf_fit_size(heap->heap_data, info->largest_free_block);
    multi_heap_internal_unlock(heap);
}
#endif
