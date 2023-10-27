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
#include <sys/param.h>
#include <multi_heap.h>
#include "multi_heap_internal.h"

/* Note: Keep platform-specific parts in this header, this source
   file should depend on libc only */
#include "multi_heap_platform.h"

/* Defines compile-time configuration macros */
#include "multi_heap_config.h"

#if CONFIG_HEAP_TLSF_USE_ROM_IMPL
/* Header containing the declaration of tlsf_poison_fill_pfunc_set()
 * and tlsf_poison_check_pfunc_set() used to register callbacks to
 * fill and check memory region with given patterns in the heap
 * components.
 */
#include "esp_rom_tlsf.h"
#endif

#ifdef MULTI_HEAP_POISONING

/* Alias MULTI_HEAP_POISONING_SLOW to SLOW for better readabilty */
#ifdef SLOW
#error "external header has defined SLOW"
#endif
#ifdef MULTI_HEAP_POISONING_SLOW
#define SLOW 1
#endif

#define MALLOC_FILL_PATTERN 0xce
#define FREE_FILL_PATTERN 0xfe

#define HEAD_CANARY_PATTERN 0xABBA1234
#define TAIL_CANARY_PATTERN 0xBAAD5678


#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

typedef struct {
    uint32_t head_canary;
    size_t alloc_size;
} poison_head_t;

typedef struct {
    uint32_t tail_canary;
} poison_tail_t;

#define POISON_OVERHEAD (sizeof(poison_head_t) + sizeof(poison_tail_t))

/* Given a "poisoned" region with pre-data header 'head', and actual data size 'alloc_size', fill in the head and tail
   region checks.

   Returns the pointer to the actual usable data buffer (ie after 'head')
*/
__attribute__((noinline))  static uint8_t *poison_allocated_region(poison_head_t *head, size_t alloc_size)
{
    uint8_t *data = (uint8_t *)(&head[1]); /* start of data ie 'real' allocated buffer */
    poison_tail_t *tail = (poison_tail_t *)(data + alloc_size);
    head->alloc_size = alloc_size;
    head->head_canary = HEAD_CANARY_PATTERN;

    uint32_t tail_canary = TAIL_CANARY_PATTERN;
    if ((intptr_t)tail % sizeof(void *) == 0) {
        tail->tail_canary = tail_canary;
    } else {
        /* unaligned tail_canary */
        memcpy(&tail->tail_canary, &tail_canary, sizeof(uint32_t));
    }

    return data;
}

/* Given a pointer to some allocated data, check the head & tail poison structures (before & after it) that were
   previously injected by poison_allocated_region().

   Returns a pointer to the poison header structure, or NULL if the poison structures are corrupt.
*/
__attribute__((noinline)) static poison_head_t *verify_allocated_region(void *data, bool print_errors)
{
    poison_head_t *head = (poison_head_t *)((intptr_t)data - sizeof(poison_head_t));
    poison_tail_t *tail = (poison_tail_t *)((intptr_t)data + head->alloc_size);

    /* check if the beginning of the data was overwritten */
    if (head->head_canary != HEAD_CANARY_PATTERN) {
        if (print_errors) {
            MULTI_HEAP_STDERR_PRINTF("CORRUPT HEAP: Bad head at %p. Expected 0x%08x got 0x%08x\n", &head->head_canary,
                   HEAD_CANARY_PATTERN, head->head_canary);
        }
        return NULL;
    }

    /* check if the end of the data was overrun */
    uint32_t canary;
    if ((intptr_t)tail % sizeof(void *) == 0) {
        canary = tail->tail_canary;
    } else {
        /* tail is unaligned */
        memcpy(&canary, &tail->tail_canary, sizeof(canary));
    }
    if (canary != TAIL_CANARY_PATTERN) {
        if (print_errors) {
            MULTI_HEAP_STDERR_PRINTF("CORRUPT HEAP: Bad tail at %p. Expected 0x%08x got 0x%08x\n", &tail->tail_canary,
                   TAIL_CANARY_PATTERN, canary);
        }
        return NULL;
    }

    return head;
}

#ifdef SLOW
/* Go through a region that should have the specified fill byte 'pattern',
   verify it.

   if expect_free is true, expect FREE_FILL_PATTERN otherwise MALLOC_FILL_PATTERN.

   if swap_pattern is true, swap patterns in the buffer (ie replace MALLOC_FILL_PATTERN with FREE_FILL_PATTERN, and vice versa.)

   Returns true if verification checks out.

   This function has the attribute noclone to prevent the compiler to create a clone on flash where expect_free is removed (as this
   function is called only with expect_free == true throughout the component).
*/
__attribute__((noinline)) NOCLONE_ATTR
static bool verify_fill_pattern(void *data, size_t size, const bool print_errors, const bool expect_free, bool swap_pattern)
{
    const uint32_t FREE_FILL_WORD = (FREE_FILL_PATTERN << 24) | (FREE_FILL_PATTERN << 16) | (FREE_FILL_PATTERN << 8) | FREE_FILL_PATTERN;
    const uint32_t MALLOC_FILL_WORD = (MALLOC_FILL_PATTERN << 24) | (MALLOC_FILL_PATTERN << 16) | (MALLOC_FILL_PATTERN << 8) | MALLOC_FILL_PATTERN;

    const uint32_t EXPECT_WORD = expect_free ? FREE_FILL_WORD : MALLOC_FILL_WORD;
    const uint32_t REPLACE_WORD = expect_free ? MALLOC_FILL_WORD : FREE_FILL_WORD;
    bool valid = true;

    /* Use 4-byte operations as much as possible */
    if ((intptr_t)data % 4 == 0) {
        uint32_t *p = data;
        while (size >= 4) {
            if (*p != EXPECT_WORD) {
                if (print_errors) {
                    MULTI_HEAP_STDERR_PRINTF("CORRUPT HEAP: Invalid data at %p. Expected 0x%08x got 0x%08x\n", p, EXPECT_WORD, *p);
                }
                valid = false;
#ifndef NDEBUG
                /* If an assertion is going to fail as soon as we're done verifying the pattern, leave the rest of the
                   buffer contents as-is for better post-mortem analysis
                */
                swap_pattern = false;
#endif
            }
            if (swap_pattern) {
                *p = REPLACE_WORD;
            }
            p++;
            size -= 4;
        }
        data = p;
    }

    uint8_t *p = data;
    for (size_t i = 0; i < size; i++) {
        if (p[i] != (uint8_t)EXPECT_WORD) {
            if (print_errors) {
                MULTI_HEAP_STDERR_PRINTF("CORRUPT HEAP: Invalid data at %p. Expected 0x%02x got 0x%02x\n", p, (uint8_t)EXPECT_WORD, *p);
            }
            valid = false;
#ifndef NDEBUG
            swap_pattern = false; // same as above
#endif
        }
        if (swap_pattern) {
            p[i] = (uint8_t)REPLACE_WORD;
        }
    }
    return valid;
}

/*!
 * @brief Definition of the weak function declared in TLSF repository.
 * The call of this function assures that the header of an absorbed
 * block is filled with the correct pattern in case of comprehensive
 * heap poisoning.
 *
 * @param start: pointer to the start of the memory region to fill
 * @param size: size of the memory region to fill
 * @param is_free: Indicate if the pattern to use the fill the region should be
 * an after free or after allocation pattern.
 */
void block_absorb_post_hook(void *start, size_t size, bool is_free)
{
    multi_heap_internal_poison_fill_region(start, size, is_free);
}
#endif

void *multi_heap_aligned_alloc(multi_heap_handle_t heap, size_t size, size_t alignment)
{
    return multi_heap_aligned_alloc_offs(heap, size, alignment, 0);
}

void *multi_heap_aligned_alloc_offs(multi_heap_handle_t heap, size_t size, size_t alignment, size_t offset)
{
    if (!size) {
        return NULL;
    }

    if (size > SIZE_MAX  - POISON_OVERHEAD) {
        return NULL;
    }

    multi_heap_internal_lock(heap);
    poison_head_t *head = multi_heap_aligned_alloc_impl_offs(heap, size + POISON_OVERHEAD,
                                                             alignment, offset + sizeof(poison_head_t));
    uint8_t *data = NULL;
    if (head != NULL) {
        data = poison_allocated_region(head, size);
#ifdef SLOW
        /* check everything we got back is FREE_FILL_PATTERN & swap for MALLOC_FILL_PATTERN */
        bool ret = verify_fill_pattern(data, size, true, true, true);
        assert( ret );
#endif
    } else {
        multi_heap_internal_unlock(heap);
        return NULL;
    }

    multi_heap_internal_unlock(heap);

    return data;
}

void *multi_heap_malloc(multi_heap_handle_t heap, size_t size)
{
    if (!size) {
        return NULL;
    }

    if(size > SIZE_MAX - POISON_OVERHEAD) {
        return NULL;
    }

    multi_heap_internal_lock(heap);
    poison_head_t *head = multi_heap_malloc_impl(heap, size + POISON_OVERHEAD);
    uint8_t *data = NULL;
    if (head != NULL) {
        data = poison_allocated_region(head, size);
#ifdef SLOW
        /* check everything we got back is FREE_FILL_PATTERN & swap for MALLOC_FILL_PATTERN */
        bool ret = verify_fill_pattern(data, size, true, true, true);
        assert( ret );
#endif
    }

    multi_heap_internal_unlock(heap);
    return data;
}

/* This function has the noclone attribute to prevent the compiler to optimize out the
 * check for p == NULL and create a clone function placed in flash. */
NOCLONE_ATTR void multi_heap_free(multi_heap_handle_t heap, void *p)
{
    if (p == NULL) {
        return;
    }
    multi_heap_internal_lock(heap);

    poison_head_t *head = verify_allocated_region(p, true);
    assert(head != NULL);

    #ifdef SLOW
    /* replace everything with FREE_FILL_PATTERN, including the poison head/tail */
    memset(head, FREE_FILL_PATTERN,
           head->alloc_size + POISON_OVERHEAD);
    #endif
    multi_heap_free_impl(heap, head);

    multi_heap_internal_unlock(heap);
}

void multi_heap_aligned_free(multi_heap_handle_t heap, void *p)
{
    multi_heap_free(heap, p);
}

void *multi_heap_realloc(multi_heap_handle_t heap, void *p, size_t size)
{
    poison_head_t *head = NULL;
    poison_head_t *new_head;
    void *result = NULL;

    if(size > SIZE_MAX - POISON_OVERHEAD) {
        return NULL;
    }
    if (p == NULL) {
        return multi_heap_malloc(heap, size);
    }
    if (size == 0) {
        multi_heap_free(heap, p);
        return NULL;
    }

    /* p != NULL, size != 0 */
    head = verify_allocated_region(p, true);
    assert(head != NULL);

    multi_heap_internal_lock(heap);

#ifndef SLOW
    new_head = multi_heap_realloc_impl(heap, head, size + POISON_OVERHEAD);
    if (new_head != NULL) {
        /* For "fast" poisoning, we only overwrite the head/tail of the new block so it's safe
           to poison, so no problem doing this even if realloc resized in place.
        */
        result = poison_allocated_region(new_head, size);
    }
#else // SLOW
    /* When slow poisoning is enabled, it becomes very fiddly to try and correctly fill memory when resizing in place
       (where the buffer may be moved (including to an overlapping address with the old buffer), grown, or shrunk in
       place.)

       For now we just malloc a new buffer, copy, and free. :|

       Note: If this ever changes, multi_heap defrag realloc test should be enabled.
    */
    size_t orig_alloc_size = head->alloc_size;

    new_head = multi_heap_malloc_impl(heap, size + POISON_OVERHEAD);
    if (new_head != NULL) {
        result = poison_allocated_region(new_head, size);
        memcpy(result, p, MIN(size, orig_alloc_size));
        multi_heap_free(heap, p);
    }
#endif

    multi_heap_internal_unlock(heap);

    return result;
}

void *multi_heap_get_block_address(multi_heap_block_handle_t block)
{
    char *head = multi_heap_get_block_address_impl(block);
    return head + sizeof(poison_head_t);
}

multi_heap_handle_t multi_heap_register(void *start, size_t size)
{
#ifdef SLOW
    if (start != NULL) {
        memset(start, FREE_FILL_PATTERN, size);
    }
#endif
#if CONFIG_HEAP_TLSF_USE_ROM_IMPL
    tlsf_poison_fill_pfunc_set(multi_heap_internal_poison_fill_region);
    tlsf_poison_check_pfunc_set(multi_heap_internal_check_block_poisoning);
#endif // CONFIG_HEAP_TLSF_USE_ROM_IMPL
    return multi_heap_register_impl(start, size);
}

static inline __attribute__((always_inline)) void subtract_poison_overhead(size_t *arg) {
    if (*arg > POISON_OVERHEAD) {
        *arg -= POISON_OVERHEAD;
    } else {
        *arg = 0;
    }
}

size_t multi_heap_get_allocated_size(multi_heap_handle_t heap, void *p)
{
    poison_head_t *head = verify_allocated_region(p, true);
    assert(head != NULL);
    size_t result = multi_heap_get_allocated_size_impl(heap, head);
    subtract_poison_overhead(&result);
    return result;
}

void multi_heap_get_info(multi_heap_handle_t heap, multi_heap_info_t *info)
{
    multi_heap_get_info_impl(heap, info);
    /* don't count the heap poison head & tail overhead in the allocated bytes size */
    info->total_allocated_bytes -= info->allocated_blocks * POISON_OVERHEAD;
    /* trim largest_free_block to account for poison overhead */
    subtract_poison_overhead(&info->largest_free_block);
    /* similarly, trim total_free_bytes so there's no suggestion that
       a block this big may be available. */
    subtract_poison_overhead(&info->total_free_bytes);
    subtract_poison_overhead(&info->minimum_free_bytes);
}

size_t multi_heap_free_size(multi_heap_handle_t heap)
{
    size_t r = multi_heap_free_size_impl(heap);
    subtract_poison_overhead(&r);
    return r;
}

size_t multi_heap_minimum_free_size(multi_heap_handle_t heap)
{
    size_t r = multi_heap_minimum_free_size_impl(heap);
    subtract_poison_overhead(&r);
    return r;
}

/* Internal hooks used by multi_heap to manage poisoning, while keeping some modularity */

bool multi_heap_internal_check_block_poisoning(void *start, size_t size, bool is_free, bool print_errors)
{
    if (is_free) {
#ifdef SLOW
        return verify_fill_pattern(start, size, print_errors, true, false);
#else
        return true; /* can only verify empty blocks in SLOW mode */
#endif
    } else {
        void *data = (void *)((intptr_t)start + sizeof(poison_head_t));
        poison_head_t *head = verify_allocated_region(data, print_errors);
        if (head != NULL && head->alloc_size > size - POISON_OVERHEAD) {
            /* block can be bigger than alloc_size, for reasons of alignment & fragmentation,
               but block can never be smaller than head->alloc_size... */
            if (print_errors) {
                MULTI_HEAP_STDERR_PRINTF("CORRUPT HEAP: Size at %p expected <=0x%08x got 0x%08x\n", &head->alloc_size,
                       size - POISON_OVERHEAD, head->alloc_size);
            }
            return false;
        }
        return head != NULL;
    }
}

void multi_heap_internal_poison_fill_region(void *start, size_t size, bool is_free)
{
    memset(start, is_free ? FREE_FILL_PATTERN : MALLOC_FILL_PATTERN, size);
}

#else // !MULTI_HEAP_POISONING

#ifdef MULTI_HEAP_POISONING_SLOW
#error "MULTI_HEAP_POISONING_SLOW requires MULTI_HEAP_POISONING"
#endif

#endif  // MULTI_HEAP_POISONING
