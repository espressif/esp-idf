/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
#include "esp_system.h"

#ifdef CONFIG_HEAP_USE_HOOKS
#define CALL_HOOK(hook, ...) {      \
    if (hook != NULL) {             \
        hook(__VA_ARGS__);          \
    }                               \
}
#else
#define CALL_HOOK(hook, ...) {}
#endif

/* Forward declaration for base function, put in IRAM.
 * These functions don't check for errors after trying to allocate memory. */
static void *heap_caps_realloc_base( void *ptr, size_t size, uint32_t caps );
static void *heap_caps_calloc_base( size_t n, size_t size, uint32_t caps );
static void *heap_caps_malloc_base( size_t size, uint32_t caps );

/*
This file, combined with a region allocator that supports multiple heaps, solves the problem that the ESP32 has RAM
that's slightly heterogeneous. Some RAM can be byte-accessed, some allows only 32-bit accesses, some can execute memory,
some can be remapped by the MMU to only be accessed by a certain PID etc. In order to allow the most flexible memory
allocation possible, this code makes it possible to request memory that has certain capabilities. The code will then use
its knowledge of how the memory is configured along with a priority scheme to allocate that memory in the most sane way
possible. This should optimize the amount of RAM accessible to the code without hardwiring addresses.
*/

static esp_alloc_failed_hook_t alloc_failed_callback;

#ifdef CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS
HEAP_IRAM_ATTR static void hex_to_str(char buf[8], uint32_t n)
{
    for (int i = 0; i < 8; i++) {
        uint8_t b4 = (n >> (28 - i * 4)) & 0b1111;
        buf[i] = b4 <= 9 ? '0' + b4 : 'a' + b4 - 10;
    }
}
HEAP_IRAM_ATTR static void fmt_abort_str(char dest[48], size_t size, uint32_t caps)
{
    char sSize[8];
    char sCaps[8];
    hex_to_str(sSize, size);
    hex_to_str(sCaps, caps);
    memcpy(dest, "Mem alloc fail. size 0x00000000 caps 0x00000000", 48);
    memcpy(dest + 23, sSize, 8);
    memcpy(dest + 39, sCaps, 8);
}
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

HEAP_IRAM_ATTR NOINLINE_ATTR static void heap_caps_alloc_failed(size_t requested_size, uint32_t caps, const char *function_name)
{
    if (alloc_failed_callback) {
        alloc_failed_callback(requested_size, caps, function_name);
    }

#ifdef CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS
    char buf[48];
    fmt_abort_str(buf, requested_size, caps);
    esp_system_abort(buf);
#endif
}

esp_err_t heap_caps_register_failed_alloc_callback(esp_alloc_failed_hook_t callback)
{
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    alloc_failed_callback = callback;

    return ESP_OK;
}

bool heap_caps_match(const heap_t *heap, uint32_t caps)
{
    return heap->heap != NULL && ((get_all_caps(heap) & caps) == caps);
}


/*
This function should not be called directly as it does not
check for failure / call heap_caps_alloc_failed()
*/
HEAP_IRAM_ATTR static void *heap_caps_malloc_base( size_t size, uint32_t caps)
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
Routine to allocate a bit of memory with certain capabilities. caps is a bitfield of MALLOC_CAP_* bits.
*/
HEAP_IRAM_ATTR void *heap_caps_malloc( size_t size, uint32_t caps){

    void* ptr = heap_caps_malloc_base(size, caps);


    if (!ptr && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
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
HEAP_IRAM_ATTR void *heap_caps_malloc_default( size_t size )
{
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    } else {

        // use heap_caps_malloc_base() since we'll
        // check for allocation failure ourselves

        void *r;
        if (size <= (size_t)malloc_alwaysinternal_limit) {
            r=heap_caps_malloc_base( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
        } else {
            r=heap_caps_malloc_base( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM );
        }
        if (r==NULL && size > 0) {
            //try again while being less picky
            r=heap_caps_malloc_base( size, MALLOC_CAP_DEFAULT );
        }

        // allocation failure?
        if (r==NULL && size > 0){
            heap_caps_alloc_failed(size, MALLOC_CAP_DEFAULT, __func__);
        }

        return r;
    }
}

/*
 Same for realloc()
 Note: keep the logic in here the same as in heap_caps_malloc_default (or merge the two as soon as this gets more complex...)
 */
HEAP_IRAM_ATTR void *heap_caps_realloc_default( void *ptr, size_t size )
{
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
    } else {

        // We use heap_caps_realloc_base() since we'll
        // handle allocation failure ourselves

        void *r;
        if (size <= (size_t)malloc_alwaysinternal_limit) {
            r=heap_caps_realloc_base( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
        } else {
            r=heap_caps_realloc_base( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM);
        }

        if (r==NULL && size>0) {
            //We needed to allocate memory, but we didn't. Try again while being less picky.
            r=heap_caps_realloc_base( ptr, size, MALLOC_CAP_DEFAULT);
        }

        // allocation failure?
        if (r==NULL && size>0){
            heap_caps_alloc_failed(size, MALLOC_CAP_DEFAULT, __func__);
        }
        return r;
    }
}

/*
 Memory allocation as preference in decreasing order.
 */
HEAP_IRAM_ATTR void *heap_caps_malloc_prefer( size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    uint32_t caps = MALLOC_CAP_DEFAULT;
    while (num--) {
        caps = va_arg( argp, uint32_t );
        r = heap_caps_malloc_base( size, caps );
        if (r != NULL || size == 0) {
            break;
        }
    }

    if (r == NULL && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }
    va_end( argp );
    return r;
}

/*
 Memory reallocation as preference in decreasing order.
 */
HEAP_IRAM_ATTR void *heap_caps_realloc_prefer( void *ptr, size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    uint32_t caps = MALLOC_CAP_DEFAULT;
    while (num--) {
        caps = va_arg( argp, uint32_t );
        r = heap_caps_realloc_base( ptr, size, caps );
        if (r != NULL || size == 0) {
            break;
        }
    }

    if (r == NULL && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }
    va_end( argp );
    return r;
}

/*
 Memory callocation as preference in decreasing order.
 */
HEAP_IRAM_ATTR void *heap_caps_calloc_prefer( size_t n, size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    uint32_t caps = MALLOC_CAP_DEFAULT;
    while (num--) {
        caps = va_arg( argp, uint32_t );
        r = heap_caps_calloc_base( n, size, caps );
        if (r != NULL || size == 0){
            break;
        }
    }

    if (r == NULL && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }
    va_end( argp );
    return r;
}

/* Find the heap which belongs to ptr, or return NULL if it's
   not in any heap.

   (This confirms if ptr is inside the heap's region, doesn't confirm if 'ptr'
   is an allocated block or is some other random address inside the heap.)
*/
HEAP_IRAM_ATTR static heap_t *find_containing_heap(void *ptr )
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
HEAP_IRAM_ATTR static void *heap_caps_realloc_base( void *ptr, size_t size, uint32_t caps)
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

HEAP_IRAM_ATTR void *heap_caps_realloc( void *ptr, size_t size, uint32_t caps)
{
    ptr = heap_caps_realloc_base(ptr, size, caps);


    if (ptr == NULL && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
}

/*
This function should not be called directly as it does not
check for failure / call heap_caps_alloc_failed()
*/
HEAP_IRAM_ATTR static void *heap_caps_calloc_base( size_t n, size_t size, uint32_t caps)
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

HEAP_IRAM_ATTR void *heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    void* ptr = heap_caps_calloc_base(n, size, caps);


    if (!ptr && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
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
    memset(info, 0, sizeof(multi_heap_info_t));

    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            multi_heap_info_t hinfo;
            multi_heap_get_info(heap->heap, &hinfo);

            info->total_free_bytes += hinfo.total_free_bytes - MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(0);
            info->total_allocated_bytes += (hinfo.total_allocated_bytes -
                                           hinfo.allocated_blocks * MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(0));
            info->largest_free_block = MAX(info->largest_free_block,
                                           hinfo.largest_free_block);
            info->largest_free_block -= info->largest_free_block ? MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(0) : 0;
            info->minimum_free_bytes += hinfo.minimum_free_bytes - MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(0);
            info->allocated_blocks += hinfo.allocated_blocks;
            info->free_blocks += hinfo.free_blocks;
            info->total_blocks += hinfo.total_blocks;
        }
    }
}

void heap_caps_print_heap_info( uint32_t caps )
{
    multi_heap_info_t info;
    printf("Heap summary for capabilities 0x%08"PRIX32":\n", caps);
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
    // add the block owner bytes back to ptr before handing over
    // to multi heap layer.
    ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(ptr);
    heap_t *heap = find_containing_heap(ptr);
    assert(heap);
    size_t size = multi_heap_get_allocated_size(heap->heap, ptr);
    return MULTI_HEAP_REMOVE_BLOCK_OWNER_SIZE(size);
}

HEAP_IRAM_ATTR void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps)
{
    void *ret = NULL;

    if(!alignment) {
        return NULL;
    }

    //Alignment must be a power of two:
    if((alignment & (alignment - 1)) != 0) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

    if (MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size) > HEAP_SIZE_MAX) {
        // Avoids int overflow when adding small numbers to size, or
        // calculating 'end' from start+size, by limiting 'size' to the possible range
        heap_caps_alloc_failed(size, caps, __func__);

        return NULL;
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
                    // Just try to alloc, nothing special. Provide the size of the block owner
                    // as an offset to prevent a miscalculation of the alignment.
                    ret = multi_heap_aligned_alloc_offs(heap->heap, MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size), alignment, MULTI_HEAP_BLOCK_OWNER_SIZE());
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

    heap_caps_alloc_failed(size, caps, __func__);

    //Nothing usable found.
    return NULL;
}

HEAP_IRAM_ATTR void heap_caps_aligned_free(void *ptr)
{
    heap_caps_free(ptr);
}

void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size, uint32_t caps)
{
    size_t size_bytes;
    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    void *ptr = heap_caps_aligned_alloc(alignment,size_bytes, caps);
    if(ptr != NULL) {
        memset(ptr, 0, size_bytes);
    }

    return ptr;
}
