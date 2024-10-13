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
#include "esp_heap_caps.h"
#include "multi_heap.h"
#include "esp_log.h"
#include "heap_private.h"
#include "esp_system.h"

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
Routine to allocate a bit of memory with certain capabilities. caps is a bitfield of MALLOC_CAP_* bits.
*/
HEAP_IRAM_ATTR void *heap_caps_malloc( size_t size, uint32_t caps)
{
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

HEAP_IRAM_ATTR void *heap_caps_realloc( void *ptr, size_t size, uint32_t caps)
{
    ptr = heap_caps_realloc_base(ptr, size, caps);


    if (ptr == NULL && size > 0){
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
}

HEAP_IRAM_ATTR void *heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    void* ptr = heap_caps_calloc_base(n, size, caps);


    if (!ptr && size > 0){
        heap_caps_alloc_failed(n * size, caps, __func__);
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

static struct {
    size_t *values; // Array of minimum_free_bytes used to keep the different values when starting monitoring
    size_t counter; // Keep count of registered heap when monitoring to prevent any added heap to create an out of bound access on values
    multi_heap_lock_t mux; // protect access to min_free_bytes_monitoring fields in start/stop monitoring functions
} min_free_bytes_monitoring = {NULL, 0, MULTI_HEAP_LOCK_STATIC_INITIALIZER};

esp_err_t heap_caps_monitor_local_minimum_free_size_start(void)
{
    // update minimum_free_bytes on all affected heap, and store the "old value"
    // as a snapshot of the heaps minimum_free_bytes state.
    heap_t *heap = NULL;
    MULTI_HEAP_LOCK(&min_free_bytes_monitoring.mux);
    if (min_free_bytes_monitoring.values == NULL) {
        SLIST_FOREACH(heap, &registered_heaps, next) {
            min_free_bytes_monitoring.counter++;
        }
        min_free_bytes_monitoring.values = heap_caps_malloc(sizeof(size_t) * min_free_bytes_monitoring.counter, MALLOC_CAP_DEFAULT);
        assert(min_free_bytes_monitoring.values != NULL && "not enough memory to store min_free_bytes value");
        memset(min_free_bytes_monitoring.values, 0xFF, sizeof(size_t) * min_free_bytes_monitoring.counter);
    }

    heap = SLIST_FIRST(&registered_heaps);
    for (size_t counter = 0; counter < min_free_bytes_monitoring.counter; counter++) {
        size_t old_minimum = multi_heap_reset_minimum_free_bytes(heap->heap);

        if (min_free_bytes_monitoring.values[counter] > old_minimum) {
            min_free_bytes_monitoring.values[counter] = old_minimum;
        }

        heap = SLIST_NEXT(heap, next);
    }
    MULTI_HEAP_UNLOCK(&min_free_bytes_monitoring.mux);

    return ESP_OK;
}

esp_err_t heap_caps_monitor_local_minimum_free_size_stop(void)
{
    if (min_free_bytes_monitoring.values == NULL) {
        return ESP_FAIL;
    }

    MULTI_HEAP_LOCK(&min_free_bytes_monitoring.mux);
    heap_t *heap = SLIST_FIRST(&registered_heaps);
    for (size_t counter = 0; counter < min_free_bytes_monitoring.counter; counter++) {
        multi_heap_restore_minimum_free_bytes(heap->heap, min_free_bytes_monitoring.values[counter]);

        heap = SLIST_NEXT(heap, next);
    }

    heap_caps_free(min_free_bytes_monitoring.values);
    min_free_bytes_monitoring.values = NULL;
    min_free_bytes_monitoring.counter = 0;
    MULTI_HEAP_UNLOCK(&min_free_bytes_monitoring.mux);

    return ESP_OK;
}


void heap_caps_get_info( multi_heap_info_t *info, uint32_t caps )
{
    memset(info, 0, sizeof(multi_heap_info_t));

    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap_caps_match(heap, caps)) {
            multi_heap_info_t hinfo;
            multi_heap_get_info(heap->heap, &hinfo);

            info->total_free_bytes += hinfo.total_free_bytes - MULTI_HEAP_BLOCK_OWNER_SIZE();
            info->total_allocated_bytes += (hinfo.total_allocated_bytes -
                                           hinfo.allocated_blocks * MULTI_HEAP_BLOCK_OWNER_SIZE());
            info->largest_free_block = MAX(info->largest_free_block,
                                           hinfo.largest_free_block);
            info->largest_free_block -= info->largest_free_block ? MULTI_HEAP_BLOCK_OWNER_SIZE() : 0;
            info->minimum_free_bytes += hinfo.minimum_free_bytes - MULTI_HEAP_BLOCK_OWNER_SIZE();
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

static HEAP_IRAM_ATTR esp_err_t heap_caps_aligned_check_args(size_t alignment, size_t size, uint32_t caps, const char *funcname)
{
    if (!alignment) {
        return ESP_FAIL;
    }

    // Alignment must be a power of two:
    if ((alignment & (alignment - 1)) != 0) {
        return ESP_FAIL;
    }

    if (size == 0) {
        return ESP_FAIL;
    }

    if (MULTI_HEAP_ADD_BLOCK_OWNER_SIZE(size) > HEAP_SIZE_MAX) {
        // Avoids int overflow when adding small numbers to size, or
        // calculating 'end' from start+size, by limiting 'size' to the possible range
        heap_caps_alloc_failed(size, caps, funcname);
        return ESP_FAIL;
    }

    return ESP_OK;
}

HEAP_IRAM_ATTR void *heap_caps_aligned_alloc_default(size_t alignment, size_t size)
{
    void *ret = NULL;

    if (malloc_alwaysinternal_limit == MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_aligned_alloc(alignment, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    }

    if (heap_caps_aligned_check_args(alignment, size, MALLOC_CAP_DEFAULT, __func__) != ESP_OK) {
        return NULL;
    }

    if (size <= (size_t)malloc_alwaysinternal_limit) {
        ret = heap_caps_aligned_alloc_base(alignment, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    } else {
        ret = heap_caps_aligned_alloc_base(alignment, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM);
    }

    if (ret != NULL) {
        return ret;
    }

    ret = heap_caps_aligned_alloc_base(alignment, size, MALLOC_CAP_DEFAULT);

    if (ret == NULL) {
        heap_caps_alloc_failed(size, MALLOC_CAP_DEFAULT, __func__);
    }

    return ret;
}

HEAP_IRAM_ATTR void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps)
{
    void *ret = NULL;

    if (heap_caps_aligned_check_args(alignment, size, caps, __func__) != ESP_OK) {
        return NULL;
    }

    ret = heap_caps_aligned_alloc_base(alignment, size, caps);

    if (ret == NULL) {
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ret;
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

typedef struct walker_data {
        void *opaque_ptr;
        heap_caps_walker_cb_t cb_func;
        heap_t *heap;
} walker_data_t;

__attribute__((noinline)) static bool heap_caps_walker(void* block_ptr, size_t block_size, int block_used, void *user_data)
{
    walker_data_t *walker_data = (walker_data_t*)user_data;

    walker_heap_into_t heap_info = {
        (intptr_t)walker_data->heap->start,
        (intptr_t)walker_data->heap->end
    };
    walker_block_info_t block_info = {
        block_ptr,
        block_size,
        (bool)block_used
    };

    return walker_data->cb_func(heap_info, block_info, walker_data->opaque_ptr);
}

void heap_caps_walk(uint32_t caps, heap_caps_walker_cb_t walker_func, void *user_data)
{
    assert(walker_func != NULL);

    bool all_heaps = caps & MALLOC_CAP_INVALID;
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (heap->heap != NULL
            && (all_heaps || (get_all_caps(heap) & caps) == caps)) {
            walker_data_t walker_data = {user_data, walker_func, heap};
            multi_heap_walk(heap->heap, heap_caps_walker, &walker_data);
        }
    }
}

void heap_caps_walk_all(heap_caps_walker_cb_t walker_func, void *user_data)
{
    heap_caps_walk(MALLOC_CAP_INVALID, walker_func, user_data);
}
