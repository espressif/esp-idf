/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
  This takes a memory chunk in a region that can be addressed as both DRAM as well as IRAM. It will convert it to
  IRAM in such a way that it can be later freed. It assumes both the address as well as the length to be word-aligned.
  It returns a region that's 1 word smaller than the region given because it stores the original Dram address there.
*/
HEAP_IRAM_ATTR static void *dram_alloc_to_iram_addr(void *addr, size_t len)
{
    uintptr_t dstart = (uintptr_t)addr; //First word
    uintptr_t dend __attribute__((unused)) = dstart + len - 4; //Last word
    assert(esp_ptr_in_diram_dram((void *)dstart));
    assert(esp_ptr_in_diram_dram((void *)dend));
    assert((dstart & 3) == 0);
    assert((dend & 3) == 0);
#if SOC_DIRAM_INVERTED // We want the word before the result to hold the DRAM address
    uint32_t *iptr = esp_ptr_diram_dram_to_iram((void *)dend);
#else
    uint32_t *iptr = esp_ptr_diram_dram_to_iram((void *)dstart);
#endif
    *iptr = dstart;
    return iptr + 1;
}

HEAP_IRAM_ATTR void heap_caps_free( void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    if (esp_ptr_in_diram_iram(ptr)) {
        //Memory allocated here is actually allocated in the DRAM alias region and
        //cannot be de-allocated as usual. dram_alloc_to_iram_addr stores a pointer to
        //the equivalent DRAM address, though; free that.
        uint32_t *dramAddrPtr = (uint32_t *)ptr;
        ptr = (void *)dramAddrPtr[-1];
    }
    void *block_owner_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(ptr);
    heap_t *heap = find_containing_heap(block_owner_ptr);
    assert(heap != NULL && "free() target pointer is outside heap areas");
    multi_heap_free(heap->heap, block_owner_ptr);

    CALL_HOOK(esp_heap_trace_free_hook, ptr);
}

/*
This function should not be called directly as it does not
check for failure / call heap_caps_alloc_failed()
*/
HEAP_IRAM_ATTR NOINLINE_ATTR void *heap_caps_malloc_base( size_t size, uint32_t caps)
{
    void *ret = NULL;

    // remove block owner size to HEAP_SIZE_MAX rather than adding the block owner size
    // to size to prevent overflows.
    if (size == 0 || size > MULTI_HEAP_REMOVE_BLOCK_OWNER_SIZE(HEAP_SIZE_MAX) ) {
        // Avoids int overflow when adding small numbers to size, or
        // calculating 'end' from start+size, by limiting 'size' to the possible range
        return NULL;
    }

    if (caps & MALLOC_CAP_EXEC) {
        //MALLOC_CAP_EXEC forces an alloc from IRAM. There is a region which has both this as well as the following
        //caps, but the following caps are not possible for IRAM.  Thus, the combination is impossible and we return
        //NULL directly, even although our heap capabilities (based on soc_memory_tags & soc_memory_regions) would
        //indicate there is a tag for this.
        if ((caps & MALLOC_CAP_8BIT) || (caps & MALLOC_CAP_DMA)) {
            return NULL;
        }
        caps |= MALLOC_CAP_32BIT; // IRAM is 32-bit accessible RAM
    }

    if (caps & MALLOC_CAP_32BIT) {
        /* 32-bit accessible RAM should allocated in 4 byte aligned sizes
         * (Future versions of ESP-IDF should possibly fail if an invalid size is requested)
         */
        size = (size + 3) & (~3); // int overflow checked above
    }

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
                    if ((caps & MALLOC_CAP_EXEC) && !esp_dram_match_iram() && esp_ptr_in_diram_dram((void *)heap->start)) {
                        //This is special, insofar that what we're going to get back is a DRAM address. If so,
                        //we need to 'invert' it (lowest address in DRAM == highest address in IRAM and vice-versa) and
                        //add a pointer to the DRAM equivalent before the address we're going to return.
                        ret = multi_heap_malloc(heap->heap, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size) + 4);  // int overflow checked above
                        if (ret != NULL) {
                            MULTI_HEAP_SET_BLOCK_OWNER(ret);
                            ret = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret);
                            uint32_t *iptr = dram_alloc_to_iram_addr(ret, size + 4);  // int overflow checked above
                            CALL_HOOK(esp_heap_trace_alloc_hook, iptr, size, caps);
                            return iptr;
                        }
                    } else {
                        //Just try to alloc, nothing special.
                        ret = multi_heap_malloc(heap->heap, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size));
                        if (ret != NULL) {
                            MULTI_HEAP_SET_BLOCK_OWNER(ret);
                            ret = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret);
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

/*
This function should not be called directly as it does not
check for failure / call heap_caps_alloc_failed()
*/
HEAP_IRAM_ATTR NOINLINE_ATTR void *heap_caps_realloc_base( void *ptr, size_t size, uint32_t caps)
{
    bool ptr_in_diram_case = false;
    heap_t *heap = NULL;
    void *dram_ptr = NULL;

    if (ptr == NULL) {
        return heap_caps_malloc_base(size, caps);
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

    //The pointer to memory may be aliased, we need to
    //recover the corresponding address before to manage a new allocation:
    if(esp_ptr_in_diram_iram((void *)ptr)) {
        uint32_t *dram_addr = (uint32_t *)ptr;
        dram_ptr  = (void *)dram_addr[-1];
        dram_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(dram_ptr);

        heap = find_containing_heap(dram_ptr);
        assert(heap != NULL && "realloc() pointer is outside heap areas");

        //with pointers that reside on diram space, we avoid using
        //the realloc implementation due to address translation issues,
        //instead force a malloc/copy/free
        ptr_in_diram_case = true;

    } else {
        heap = find_containing_heap(ptr);
        assert(heap != NULL && "realloc() pointer is outside heap areas");
    }

    // shift ptr by block owner offset. Since the ptr returned to the user
    // does not include the block owner bytes (that are located at the
    // beginning of the allocated memory) we have to add them back before
    // processing the realloc.
    ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(ptr);

    // are the existing heap's capabilities compatible with the
    // requested ones?
    bool compatible_caps = (caps & get_all_caps(heap)) == caps;

    if (compatible_caps && !ptr_in_diram_case) {
        // try to reallocate this memory within the same heap
        // (which will resize the block if it can)
        void *r = multi_heap_realloc(heap->heap, ptr, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size));
        if (r != NULL) {
            MULTI_HEAP_SET_BLOCK_OWNER(r);
            r = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(r);
            CALL_HOOK(esp_heap_trace_alloc_hook, r, size, caps);
            return r;
        }
    }

    // if we couldn't do that, try to see if we can reallocate
    // in a different heap with requested capabilities.
    void *new_p = heap_caps_malloc_base(size, caps);
    if (new_p != NULL) {
        size_t old_size = 0;

        //If we're dealing with aliased ptr, information regarding its containing
        //heap can only be obtained with translated address.
        if(ptr_in_diram_case) {
            old_size = multi_heap_get_allocated_size(heap->heap, dram_ptr);
        } else {
            old_size = multi_heap_get_allocated_size(heap->heap, ptr);
        }

        assert(old_size > 0);
        // do not copy the block owner bytes
        memcpy(new_p, MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ptr), MIN(size, old_size));
        // add the block owner bytes to ptr since they are removed in heap_caps_free
        heap_caps_free(MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ptr));
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

HEAP_IRAM_ATTR void *heap_caps_aligned_alloc_base(size_t alignment, size_t size, uint32_t caps)
{
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
                    // Just try to alloc, nothing special. Provide the size of the block owner
                    // as an offset to prevent a miscalculation of the alignment.
                    void *ret = multi_heap_aligned_alloc_offs(heap->heap, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size), alignment, MULTI_HEAP_BLOCK_OWNER_SIZE());
                    if (ret != NULL) {
                        MULTI_HEAP_SET_BLOCK_OWNER(ret);
                        ret = MULTI_HEAP_ADD_BLOCK_OWNER_OFFSET(ret);
                        CALL_HOOK(esp_heap_trace_alloc_hook, ret, size, caps);
                        return ret;
                    }
                }
            }
        }
    }

    //Nothing usable found.
    return NULL;
}
