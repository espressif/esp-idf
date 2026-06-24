/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "multi_heap.h"
#include "esp_log.h"
#include "heap_private.h"
#include "heap_kasan_layout.h"
#if CONFIG_HEAP_TASK_TRACKING
#include "esp_heap_task_info.h"
#include "esp_heap_task_info_internal.h"
#include "multi_heap_internal.h"
#endif

#ifdef CONFIG_HEAP_USE_HOOKS
#define CALL_HOOK(hook, ...) {      \
    if (hook != NULL) {             \
        hook(__VA_ARGS__);          \
    }                               \
}
#else
#define CALL_HOOK(hook, ...) {}
#endif

/*
 * KASAN redzone support: inflate allocations by 2*KASAN_RZ bytes and shift
 * the user pointer past the left redzone.  heap_kasan.c poisons the redzones.
 *
 * Final allocation layout (with CONFIG_HEAP_TASK_TRACKING=y):
 *
 *   [ block-owner word ][ left redzone ][ user bytes ][ right redzone ]
 *
 * The ordering is intentional: user underflows must hit the left redzone
 * before they can reach the task-tracking block-owner word.
 *
 * Pointer arithmetic macros live in heap_kasan_layout.h.
 */

//This is normally provided by the heap-memalign-hw component.
extern void esp_heap_adjust_alignment_to_hw(size_t *p_alignment, size_t *p_size, uint32_t *p_caps);

#if CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS
bool kasan_heap_should_defer_free(void);
void kasan_heap_clear_deferred_free(void);
#endif

//Default alignment the multiheap allocator / tlsf will align 'unaligned' memory to, in bytes
#define UNALIGNED_MEM_ALIGNMENT_BYTES 4

/*
  This takes a memory chunk in a region that can be addressed as both DRAM as well as IRAM. It will convert it to
  IRAM in such a way that it can be later freed. It assumes both the address as well as the length to be word-aligned.
  It returns a region that's 1 word smaller than the region given because it stores the original Dram address there.
*/
HEAP_IRAM_ATTR static void *dram_alloc_to_iram_addr(void *addr, size_t len)
{
    uintptr_t dstart = (uintptr_t)addr; //First word
    uintptr_t dend __attribute__((unused)) = dstart + len - 4; //Last word
    assert(esp_ptr_in_diram_dram((void *)dstart) || esp_ptr_in_rtc_dram_fast((void *)dstart));
    assert(esp_ptr_in_diram_dram((void *)dend) || esp_ptr_in_rtc_dram_fast((void *)dend));
    assert((dstart & 3) == 0);
    assert((dend & 3) == 0);
#if SOC_DIRAM_INVERTED // We want the word before the result to hold the DRAM address
    uint32_t *iptr = esp_ptr_diram_dram_to_iram((void *)dend);
#else
    uint32_t *iptr = NULL;
    if (esp_ptr_in_rtc_dram_fast((void *)dstart)) {
        iptr = esp_ptr_rtc_dram_to_iram((void *)dstart);
    } else {
        iptr = esp_ptr_diram_dram_to_iram((void *)dstart);
    }
#endif
    *iptr = dstart;
    return iptr + 1;
}

HEAP_IRAM_ATTR void heap_caps_free( void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    /*
     * KASAN free hook must see the same pointer that the alloc hook saw, i.e.
     * the user-visible (IRAM) pointer with the redzone shift applied.  Call
     * it before any DIRAM -> DRAM translation; otherwise the shadow update
     * would touch the wrong address range and use-after-free detection on
     * IRAM-aliased blocks would be incorrect.
     */
#if CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS
    CALL_HOOK(esp_heap_trace_free_hook, ptr);
#endif

    if ((!esp_dram_match_iram() && esp_ptr_in_diram_iram(ptr)) ||
        (!esp_rtc_dram_match_rtc_iram() && esp_ptr_in_rtc_iram_fast(ptr))) {
        //Memory allocated here is actually allocated in the DRAM alias region and
        //cannot be de-allocated as usual. dram_alloc_to_iram_addr stores a pointer to
        //the equivalent DRAM address, though; free that.
        uint32_t *dramAddrPtr = (uint32_t *)ptr;
        ptr = (void *)dramAddrPtr[-1];
    }

    /* Reverse the pointer transforms in the opposite order used on alloc:
     * user -> left redzone start -> block-owner word.
     */
    void *raw_ptr = KASAN_USER_TO_PTR(ptr);
    void *block_owner_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(raw_ptr);
    heap_t *heap = find_containing_heap(block_owner_ptr);
    assert(heap != NULL && "free() target pointer is outside heap areas");

#if CONFIG_HEAP_TASK_TRACKING
    heap_caps_update_per_task_info_free(heap, raw_ptr);
#endif

#if CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS
    if (kasan_heap_should_defer_free()) {
        kasan_heap_clear_deferred_free();
        return;
    }
    multi_heap_free(heap->heap, block_owner_ptr);
#else
    multi_heap_free(heap->heap, block_owner_ptr);
    CALL_HOOK(esp_heap_trace_free_hook, ptr);
#endif
}

HEAP_IRAM_ATTR static inline void *aligned_or_unaligned_alloc(multi_heap_handle_t heap, size_t size, size_t alignment, size_t offset) {
    if (alignment<=UNALIGNED_MEM_ALIGNMENT_BYTES) { //alloc and friends align to 32-bit by default
        return multi_heap_malloc(heap, size);
    } else {
        return multi_heap_aligned_alloc_offs(heap, size, alignment, offset);
    }
}

/*
This function should not be called directly as it does not check for failure / call heap_caps_alloc_failed()
Note that this function does 'unaligned' alloc calls if alignment <= UNALIGNED_MEM_ALIGNMENT_BYTES (=4) as the
allocator will align to that value by default.
*/
HEAP_IRAM_ATTR NOINLINE_ATTR void *heap_caps_aligned_alloc_base(size_t alignment, size_t size, uint32_t caps)
{
    void *ret = NULL;

    // Alignment, size and caps may need to be modified because of hardware requirements.
    esp_heap_adjust_alignment_to_hw(&alignment, &size, &caps);

    // remove block owner size to HEAP_SIZE_MAX rather than adding the block owner size
    // to size to prevent overflows.
    if (size == 0 || size > MULTI_HEAP_REMOVE_BLOCK_OWNER_SIZE(HEAP_SIZE_MAX) ) {
        // Avoids int overflow when adding small numbers to size, or
        // calculating 'end' from start+size, by limiting 'size' to the possible range
        return NULL;
    }

#if CONFIG_HEAP_HAS_EXEC_HEAP
    const bool exec_in_caps = caps & MALLOC_CAP_EXEC;
    if (exec_in_caps) {
        //MALLOC_CAP_EXEC forces an alloc from IRAM. There is a region which has both this as well as the following
        //caps, but the following caps are not possible for IRAM.  Thus, the combination is impossible and we return
        //NULL directly, even although our heap capabilities (based on soc_memory_tags & soc_memory_regions) would
        //indicate there is a tag for this.
        if ((caps & MALLOC_CAP_8BIT) || (caps & MALLOC_CAP_DMA)) {
            return NULL;
        }
        caps |= MALLOC_CAP_32BIT; // IRAM is 32-bit accessible RAM
    }
#else
    const bool exec_in_caps = false;
#endif

    if (caps & MALLOC_CAP_32BIT) {
        /* 32-bit accessible RAM should allocated in 4 byte aligned sizes
         * (Future versions of ESP-IDF should possibly fail if an invalid size is requested)
         */
        size = (size + 3) & (~3); // int overflow checked above
    }

    const size_t alloc_size = KASAN_ADD_RZ(size);

    for (int prio = 0; prio < SOC_MEMORY_TYPE_NO_PRIOS; prio++) {
        //Iterate over heaps and check capabilities at this priority
        heap_t *heap;
        SLIST_FOREACH(heap, &registered_heaps, next) {
            if (heap->heap == NULL) {
                continue;
            }
            if ((heap->caps[prio] & caps) != 0) {
                //Heap has at least one of the caps requested. If caps has other bits set that this prio
                //doesn't cover, see if they're available in other prios.
                if ((get_all_caps(heap) & caps) == caps) {
                    //This heap can satisfy all the requested capabilities. See if we can grab some memory using it.
                    // If MALLOC_CAP_EXEC is requested but the DRAM and IRAM are on the same addresses (like on esp32c6)
                    // proceed as for a default allocation.
                    if (exec_in_caps &&
                        ((!esp_dram_match_iram() && esp_ptr_in_diram_dram((void *)heap->start)) ||
                         (!esp_rtc_dram_match_rtc_iram() && esp_ptr_in_rtc_dram_fast((void *)heap->start)))) {
                        //This is special, insofar that what we're going to get back is a DRAM address. If so,
                        //we need to 'invert' it (lowest address in DRAM == highest address in IRAM and vice-versa) and
                        //add a pointer to the DRAM equivalent before the address we're going to return.
                        ret = aligned_or_unaligned_alloc(heap->heap, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(alloc_size) + 4,
                                                        alignment, MULTI_HEAP_BLOCK_OWNER_SIZE());  // int overflow checked above
                        if (ret != NULL) {
#if CONFIG_HEAP_TASK_TRACKING
                            heap_caps_update_per_task_info_alloc(heap,
                                                                 MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret),
                                                                 multi_heap_get_full_block_size(heap->heap, ret),
                                                                 get_all_caps(heap));
#endif

                            MULTI_HEAP_SET_BLOCK_OWNER(ret);
                            ret = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret);
                            ret = KASAN_PTR_TO_USER(ret);
                            uint32_t *iptr = dram_alloc_to_iram_addr(ret, size + 4);  // int overflow checked above
                            CALL_HOOK(esp_heap_trace_alloc_hook, iptr, size, caps);
                            return iptr;
                        }
                    } else {
                        //Just try to alloc, nothing special.
                        ret = aligned_or_unaligned_alloc(heap->heap, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(alloc_size),
                                                        alignment, MULTI_HEAP_BLOCK_OWNER_SIZE());
                        if (ret != NULL) {
#if CONFIG_HEAP_TASK_TRACKING
                            heap_caps_update_per_task_info_alloc(heap,
                                                                 MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret),
                                                                 multi_heap_get_full_block_size(heap->heap, ret),
                                                                 get_all_caps(heap));
#endif

                            MULTI_HEAP_SET_BLOCK_OWNER(ret);
                            ret = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret);
                            ret = KASAN_PTR_TO_USER(ret);
                            CALL_HOOK(esp_heap_trace_alloc_hook, ret, size, caps);
                            return ret;
                        }
                    }
                }
            }
        }
    }

    //Nothing usable found.
    return NULL;
}

//Wrapper for heap_caps_aligned_alloc_base as that can also do unaligned allocs.
HEAP_IRAM_ATTR NOINLINE_ATTR void *heap_caps_malloc_base( size_t size, uint32_t caps) {
    return heap_caps_aligned_alloc_base(UNALIGNED_MEM_ALIGNMENT_BYTES, size, caps);
}

/*
This function should not be called directly as it does not
check for failure / call heap_caps_alloc_failed()
*/
HEAP_IRAM_ATTR NOINLINE_ATTR void *heap_caps_realloc_base( void *ptr, size_t size, uint32_t caps)
{
    bool ptr_in_diram_case = false;
    heap_t *heap = NULL;
    void *dram_ptr = NULL;

    //See if memory needs alignment because of hardware reasons.
    size_t alignment = UNALIGNED_MEM_ALIGNMENT_BYTES;
    esp_heap_adjust_alignment_to_hw(&alignment, &size, &caps);

    if (ptr == NULL) {
        return heap_caps_aligned_alloc_base(alignment, size, caps);
    }

    if (size == 0) {
        heap_caps_free(ptr);
        return NULL;
    }

    // remove block owner size to HEAP_SIZE_MAX rather than adding the block owner size
    // to size to prevent overflows.
    if (size > MULTI_HEAP_REMOVE_BLOCK_OWNER_SIZE(HEAP_SIZE_MAX)) {
        return NULL;
    }

    /*
     * DIRAM aliasing detection must happen on the original user pointer:
     * dram_alloc_to_iram_addr stores the underlying DRAM address one word
     * before the IRAM pointer, so the KASAN redzone shift (which moves the
     * pointer by KASAN_RZ on the IRAM side) would land us in the wrong place
     * if we applied it first.
     */
    void *raw_ptr;
    if (esp_ptr_in_diram_iram(ptr)) {
        uint32_t *iram_addr = (uint32_t *)ptr;
        dram_ptr = (void *)iram_addr[-1];
        /* On the DRAM side the layout is [block-owner][left redzone][user]
         * so undo redzone first, then block-owner, matching alloc order. */
        dram_ptr = KASAN_USER_TO_PTR(dram_ptr);
        dram_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(dram_ptr);

        heap = find_containing_heap(dram_ptr);
        assert(heap != NULL && "realloc() pointer is outside heap areas");

        /* with pointers that reside on diram space, we avoid using
         * the realloc implementation due to address translation issues,
         * instead force a malloc/copy/free */
        ptr_in_diram_case = true;
        raw_ptr = NULL; /* not used in the diram path */
    } else {
        /* Reverse the alloc-time layout transform in the same order as free():
         * user -> left redzone start -> block-owner word.  Doing the
         * block-owner removal *before* find_containing_heap() matches the
         * free() path and the DIRAM branch above. */
        raw_ptr = KASAN_USER_TO_PTR(ptr);
        raw_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(raw_ptr);

        heap = find_containing_heap(raw_ptr);
        assert(heap != NULL && "realloc() pointer is outside heap areas");
    }

    const size_t alloc_size = KASAN_ADD_RZ(size);

    // are the existing heap's capabilities compatible with the
    // requested ones?
    bool compatible_caps = (caps & get_all_caps(heap)) == caps;

    //Note we don't try realloc() on memory that needs to be aligned, that is handled
    //by the fallthrough code.
    if (compatible_caps && !ptr_in_diram_case && alignment<=UNALIGNED_MEM_ALIGNMENT_BYTES) {
        // try to reallocate this memory within the same heap
        // (which will resize the block if it can)

#if CONFIG_HEAP_TASK_TRACKING
        size_t old_size = multi_heap_get_full_block_size(heap->heap, raw_ptr);
        TaskHandle_t old_task = MULTI_HEAP_GET_BLOCK_OWNER(raw_ptr);
#endif

        void *r = multi_heap_realloc(heap->heap, raw_ptr, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(alloc_size));
        if (r != NULL) {
            MULTI_HEAP_SET_BLOCK_OWNER(r);

#if CONFIG_HEAP_TASK_TRACKING
            heap_caps_update_per_task_info_realloc(heap,
                                                   MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(raw_ptr),
                                                   MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(r),
                                                   old_size, old_task,
                                                   multi_heap_get_full_block_size(heap->heap, r),
                                                   get_all_caps(heap));
#endif

            r = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(r);
            r = KASAN_PTR_TO_USER(r);
#if CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS
            /*
             * If multi_heap_realloc() relocated the block (r != ptr), the
             * old user range needs its shadow re-poisoned as use-after-free
             * so that lingering references hit a KASAN violation.  When the
             * block was resized in place, alloc hook below will simply
             * refresh the shadow over the new range.
             */
            if (r != ptr) {
                CALL_HOOK(esp_heap_trace_free_hook, ptr);
            }
#endif
            CALL_HOOK(esp_heap_trace_alloc_hook, r, size, caps);
            return r;
        }
    }

    // if we couldn't do that, try to see if we can reallocate
    // in a different heap with requested capabilities.
    void *new_p = heap_caps_aligned_alloc_base(alignment, size, caps);
    if (new_p != NULL) {
        size_t old_size = 0;

        //If we're dealing with aliased ptr, information regarding its containing
        //heap can only be obtained with translated address.
        if(ptr_in_diram_case) {
            old_size = multi_heap_get_allocated_size(heap->heap, dram_ptr);
        } else {
            old_size = multi_heap_get_allocated_size(heap->heap, raw_ptr);
        }

        assert(old_size > 0);
        old_size = MULTI_HEAP_REMOVE_BLOCK_OWNER_SIZE(old_size);
        /* Subtract KASAN redzone overhead to get the actual user-data size. */
        if (old_size > 2 * KASAN_RZ) {
            old_size -= 2 * KASAN_RZ;
        }
        memcpy(new_p, ptr, MIN(size, old_size));
        heap_caps_free(ptr);
        return new_p;
    }

    return NULL;
}

/*
This function should not be called directly as it does not
check for failure / call heap_caps_alloc_failed()
*/
HEAP_IRAM_ATTR void *heap_caps_calloc_base( size_t n, size_t size, uint32_t caps)
{
    void *result;
    size_t size_bytes;

    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    result = heap_caps_malloc_base(size_bytes, caps);
    if (result != NULL) {
        memset(result, 0, size_bytes);
    }
    return result;
}
