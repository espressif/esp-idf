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
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "multi_heap.h"
#include "esp_log.h"
#include "heap_private.h"

/*
This file, combined with a region allocator that supports multiple heaps, solves the problem that the ESP32 has RAM
that's slightly heterogeneous. Some RAM can be byte-accessed, some allows only 32-bit accesses, some can execute memory,
some can be remapped by the MMU to only be accessed by a certain PID etc. In order to allow the most flexible memory
allocation possible, this code makes it possible to request memory that has certain capabilities. The code will then use
its knowledge of how the memory is configured along with a priority scheme to allocate that memory in the most sane way
possible. This should optimize the amount of RAM accessible to the code without hardwiring addresses.
*/

/*
  This takes a memory chunk in a region that can be addressed as both DRAM as well as IRAM. It will convert it to
  IRAM in such a way that it can be later freed. It assumes both the address as well as the length to be word-aligned.
  It returns a region that's 1 word smaller than the region given because it stores the original Dram address there.
*/
IRAM_ATTR static void *dram_alloc_to_iram_addr(void *addr, size_t len)
{
    uintptr_t dstart = (uintptr_t)addr; //First word
    uintptr_t dend = dstart + len; //Last word + 4
    assert(esp_ptr_in_diram_dram((void *)dstart));
    assert(esp_ptr_in_diram_dram((void *)dend));
    assert((dstart & 3) == 0);
    assert((dend & 3) == 0);
#if CONFIG_IDF_TARGET_ESP32
    uint32_t istart = SOC_DIRAM_IRAM_LOW + (SOC_DIRAM_DRAM_HIGH - dend);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    uint32_t istart = SOC_DIRAM_IRAM_LOW + (dstart - SOC_DIRAM_DRAM_LOW);
#endif
    uint32_t *iptr = (uint32_t *)istart;
    *iptr = dstart;
    return iptr + 1;
}

bool heap_caps_match(const heap_t *heap, uint32_t caps)
{
    return heap->heap != NULL && ((get_all_caps(heap) & caps) == caps);
}

/*
Routine to allocate a bit of memory with certain capabilities. caps is a bitfield of MALLOC_CAP_* bits.
*/
IRAM_ATTR void *heap_caps_malloc( size_t size, uint32_t caps )
{
    void *ret = NULL;

    if (size > HEAP_SIZE_MAX) {
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
                    if ((caps & MALLOC_CAP_EXEC) && esp_ptr_in_diram_dram((void *)heap->start)) {
                        //This is special, insofar that what we're going to get back is a DRAM address. If so,
                        //we need to 'invert' it (lowest address in DRAM == highest address in IRAM and vice-versa) and
                        //add a pointer to the DRAM equivalent before the address we're going to return.
                        ret = multi_heap_malloc(heap->heap, size + 4);  // int overflow checked above
                        if (ret != NULL) {
                            return dram_alloc_to_iram_addr(ret, size + 4);  // int overflow checked above
                        }
                    } else {
                        //Just try to alloc, nothing special.
                        ret = multi_heap_malloc(heap->heap, size);
                        if (ret != NULL) {
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


#define MALLOC_DISABLE_EXTERNAL_ALLOCS -1
//Dual-use: -1 (=MALLOC_DISABLE_EXTERNAL_ALLOCS) disables allocations in external memory, >=0 sets the limit for allocations preferring internal memory.
static int malloc_alwaysinternal_limit=MALLOC_DISABLE_EXTERNAL_ALLOCS;

void heap_caps_malloc_extmem_enable(size_t limit)
{
    malloc_alwaysinternal_limit=limit;
}

/*
 Default memory allocation implementation. Should return standard 8-bit memory. malloc() essentially resolves to this function.
*/
IRAM_ATTR void *heap_caps_malloc_default( size_t size )
{
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    } else {
        void *r;
        if (size <= malloc_alwaysinternal_limit) {
            r=heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
        } else {
            r=heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM );
        }
        if (r==NULL) {
            //try again while being less picky
            r=heap_caps_malloc( size, MALLOC_CAP_DEFAULT );
        }
        return r;
    }
}

/*
 Same for realloc()
 Note: keep the logic in here the same as in heap_caps_malloc_default (or merge the two as soon as this gets more complex...)
 */
IRAM_ATTR void *heap_caps_realloc_default( void *ptr, size_t size )
{
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
    } else {
        void *r;
        if (size <= malloc_alwaysinternal_limit) {
            r=heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
        } else {
            r=heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM );
        }
        if (r==NULL && size>0) {
            //We needed to allocate memory, but we didn't. Try again while being less picky.
            r=heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT );
        }
        return r;
    }
}

/*
 Memory allocation as preference in decreasing order.
 */
IRAM_ATTR void *heap_caps_malloc_prefer( size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    while (num--) {
        uint32_t caps = va_arg( argp, uint32_t );
        r = heap_caps_malloc( size, caps );
        if (r != NULL) {
            break;
        }
    }
    va_end( argp );
    return r;
}

/*
 Memory reallocation as preference in decreasing order.
 */
IRAM_ATTR void *heap_caps_realloc_prefer( void *ptr, size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    while (num--) {
        uint32_t caps = va_arg( argp, uint32_t );
        r = heap_caps_realloc( ptr, size, caps );
        if (r != NULL || size == 0) {
            break;
        }
    }
    va_end( argp );
    return r;
}

/*
 Memory callocation as preference in decreasing order.
 */
IRAM_ATTR void *heap_caps_calloc_prefer( size_t n, size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    while (num--) {
        uint32_t caps = va_arg( argp, uint32_t );
        r = heap_caps_calloc( n, size, caps );
        if (r != NULL) break;
    }
    va_end( argp );
    return r;
}

/* Find the heap which belongs to ptr, or return NULL if it's
   not in any heap.

   (This confirms if ptr is inside the heap's region, doesn't confirm if 'ptr'
   is an allocated block or is some other random address inside the heap.)
*/
IRAM_ATTR static heap_t *find_containing_heap(void *ptr )
{
    intptr_t p = (intptr_t)ptr;
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap->heap != NULL && p >= heap->start && p < heap->end) {
            return heap;
        }
    }
    return NULL;
}

IRAM_ATTR void heap_caps_free( void *ptr)
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

    heap_t *heap = find_containing_heap(ptr);
    assert(heap != NULL && "free() target pointer is outside heap areas");
    multi_heap_free(heap->heap, ptr);
}

IRAM_ATTR void *heap_caps_realloc( void *ptr, size_t size, int caps)
{
    if (ptr == NULL) {
        return heap_caps_malloc(size, caps);
    }

    if (size == 0) {
        heap_caps_free(ptr);
        return NULL;
    }

    if (size > HEAP_SIZE_MAX) {
        return NULL;
    }

    heap_t *heap = find_containing_heap(ptr);

    assert(heap != NULL && "realloc() pointer is outside heap areas");

    // are the existing heap's capabilities compatible with the
    // requested ones?
    bool compatible_caps = (caps & get_all_caps(heap)) == caps;

    if (compatible_caps) {
        // try to reallocate this memory within the same heap
        // (which will resize the block if it can)
        void *r = multi_heap_realloc(heap->heap, ptr, size);
        if (r != NULL) {
            return r;
        }
    }

    // if we couldn't do that, try to see if we can reallocate
    // in a different heap with requested capabilities.
    void *new_p = heap_caps_malloc(size, caps);
    if (new_p != NULL) {
        size_t old_size = multi_heap_get_allocated_size(heap->heap, ptr);
        assert(old_size > 0);
        memcpy(new_p, ptr, MIN(size, old_size));
        heap_caps_free(ptr);
        return new_p;
    }
    return NULL;
}

IRAM_ATTR void *heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    void *result;
    size_t size_bytes;

    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    result = heap_caps_malloc(size_bytes, caps);
    if (result != NULL) {
        bzero(result, size_bytes);
    }
    return result;
}

size_t heap_caps_get_total_size(uint32_t caps)
{
    size_t total_size = 0;
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            total_size += (heap->end - heap->start);
        }
    }
    return total_size;
}

size_t heap_caps_get_free_size( uint32_t caps )
{
    size_t ret = 0;
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            ret += multi_heap_free_size(heap->heap);
        }
    }
    return ret;
}

size_t heap_caps_get_minimum_free_size( uint32_t caps )
{
    size_t ret = 0;
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            ret += multi_heap_minimum_free_size(heap->heap);
        }
    }
    return ret;
}

size_t heap_caps_get_largest_free_block( uint32_t caps )
{
    multi_heap_info_t info;
    heap_caps_get_info(&info, caps);
    return info.largest_free_block;
}

void heap_caps_get_info( multi_heap_info_t *info, uint32_t caps )
{
    bzero(info, sizeof(multi_heap_info_t));

    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            multi_heap_info_t hinfo;
            multi_heap_get_info(heap->heap, &hinfo);

            info->total_free_bytes += hinfo.total_free_bytes;
            info->total_allocated_bytes += hinfo.total_allocated_bytes;
            info->largest_free_block = MAX(info->largest_free_block,
                                           hinfo.largest_free_block);
            info->minimum_free_bytes += hinfo.minimum_free_bytes;
            info->allocated_blocks += hinfo.allocated_blocks;
            info->free_blocks += hinfo.free_blocks;
            info->total_blocks += hinfo.total_blocks;
        }
    }
}

void heap_caps_print_heap_info( uint32_t caps )
{
    multi_heap_info_t info;
    printf("Heap summary for capabilities 0x%08X:\n", caps);
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            multi_heap_get_info(heap->heap, &info);

            printf("  At 0x%08x len %d free %d allocated %d min_free %d\n",
                   heap->start, heap->end - heap->start, info.total_free_bytes, info.total_allocated_bytes, info.minimum_free_bytes);
            printf("    largest_free_block %d alloc_blocks %d free_blocks %d total_blocks %d\n",
                   info.largest_free_block, info.allocated_blocks,
                   info.free_blocks, info.total_blocks);
        }
    }
    printf("  Totals:\n");
    heap_caps_get_info(&info, caps);

    printf("    free %d allocated %d min_free %d largest_free_block %d\n", info.total_free_bytes, info.total_allocated_bytes, info.minimum_free_bytes, info.largest_free_block);
}

bool heap_caps_check_integrity(uint32_t caps, bool print_errors)
{
    bool all_heaps = caps & MALLOC_CAP_INVALID;
    bool valid = true;

    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap->heap != NULL
            && (all_heaps || (get_all_caps(heap) & caps) == caps)) {
            valid = multi_heap_check(heap->heap, print_errors) && valid;
        }
    }

    return valid;
}

bool heap_caps_check_integrity_all(bool print_errors)
{
    return heap_caps_check_integrity(MALLOC_CAP_INVALID, print_errors);
}

bool heap_caps_check_integrity_addr(intptr_t addr, bool print_errors)
{
    heap_t *heap = find_containing_heap((void *)addr);
    if (heap == NULL) {
        return false;
    }
    return multi_heap_check(heap->heap, print_errors);
}

void heap_caps_dump(uint32_t caps)
{
    bool all_heaps = caps & MALLOC_CAP_INVALID;
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap->heap != NULL
            && (all_heaps || (get_all_caps(heap) & caps) == caps)) {
            multi_heap_dump(heap->heap);
        }
    }
}

void heap_caps_dump_all(void)
{
    heap_caps_dump(MALLOC_CAP_INVALID);
}

size_t heap_caps_get_allocated_size( void *ptr )
{
    heap_t *heap = find_containing_heap(ptr);
    size_t size = multi_heap_get_allocated_size(heap->heap, ptr);
    return size;
}
