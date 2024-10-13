/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdlib.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <string.h>

#include "esp_attr.h"
#include "esp_heap_caps.h"

#ifdef CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS
#include "esp_system.h"
#endif

static esp_alloc_failed_hook_t alloc_failed_callback;

static const uint32_t MAGIC_HEAP_SIZE = UINT32_MAX;

esp_err_t heap_caps_register_failed_alloc_callback(esp_alloc_failed_hook_t callback)
{
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    alloc_failed_callback = callback;

    return ESP_OK;
}

static void heap_caps_alloc_failed(size_t requested_size, uint32_t caps, const char *function_name)
{
    if (alloc_failed_callback) {
        alloc_failed_callback(requested_size, caps, function_name);
    }

#ifdef CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS
    esp_system_abort("Memory allocation failed");
#endif
}

/*
Routine to allocate a bit of memory with certain capabilities. caps is a bitfield of MALLOC_CAP_* bits.
*/
static void *heap_caps_malloc_base( size_t size, uint32_t caps)
{

    void *ptr = malloc(size);

    if (!ptr && size > 0) {
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
}

void *heap_caps_malloc( size_t size, uint32_t caps)
{
    return heap_caps_malloc_base(size, caps);
}



void heap_caps_malloc_extmem_enable(size_t limit)
{
    (void)limit;
    // No distinction between external vs internal memory on linux
}

void *heap_caps_malloc_default( size_t size )
{
    return heap_caps_malloc_base(size, MALLOC_CAP_DEFAULT);
}

void *heap_caps_malloc_prefer( size_t size, size_t num, ... )
{
    return heap_caps_malloc(size, MALLOC_CAP_DEFAULT);
}


static void *heap_caps_realloc_base( void *ptr, size_t size, uint32_t caps)
{
    ptr = realloc(ptr, size);

    if (ptr == NULL && size > 0) {
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
}

void *heap_caps_realloc( void *ptr, size_t size, uint32_t caps)
{
    return heap_caps_realloc_base(ptr, size, caps);
}

void *heap_caps_realloc_default( void *ptr, size_t size )
{
    return heap_caps_realloc_base(ptr, size, MALLOC_CAP_DEFAULT);
}

void *heap_caps_realloc_prefer( void *ptr, size_t size, size_t num, ... )
{
    return heap_caps_realloc_base(ptr, size, MALLOC_CAP_DEFAULT);
}

void heap_caps_free( void *ptr)
{
    free(ptr);
}

static void *heap_caps_calloc_base( size_t n, size_t size, uint32_t caps)
{
    size_t size_bytes;

    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    return calloc(n, size);
}

void *heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    void *ptr = heap_caps_calloc_base(n, size, caps);

    if (!ptr && size > 0) {
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
}


void *heap_caps_calloc_prefer( size_t n, size_t size, size_t num, ... )
{
    return heap_caps_calloc_base(n, size, MALLOC_CAP_DEFAULT);
}

size_t heap_caps_get_total_size(uint32_t caps)
{
    return MAGIC_HEAP_SIZE;
}

size_t heap_caps_get_free_size( uint32_t caps )
{
    return MAGIC_HEAP_SIZE;
}

size_t heap_caps_get_minimum_free_size( uint32_t caps )
{
    return MAGIC_HEAP_SIZE;
}

size_t heap_caps_get_largest_free_block( uint32_t caps )
{
    return MAGIC_HEAP_SIZE;
}

void heap_caps_get_info( multi_heap_info_t *info, uint32_t caps )
{
    memset(info, 0, sizeof(multi_heap_info_t));
}

void heap_caps_print_heap_info( uint32_t caps )
{
    printf("No heap summary available when building for the linux target");
}

bool heap_caps_check_integrity(uint32_t caps, bool print_errors)
{
    return true;
}

bool heap_caps_check_integrity_all(bool print_errors)
{
    return heap_caps_check_integrity(MALLOC_CAP_INVALID, print_errors);
}

bool heap_caps_check_integrity_addr(intptr_t addr, bool print_errors)
{
    return true;
}

void heap_caps_dump(uint32_t caps)
{
}

void heap_caps_dump_all(void)
{
    heap_caps_dump(MALLOC_CAP_INVALID);
}

size_t heap_caps_get_allocated_size( void *ptr )
{
    return 0;
}

void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps)
{
    void *ptr = aligned_alloc(alignment, size);

    if (!ptr && size > 0) {
        heap_caps_alloc_failed(size, caps, __func__);
    }

    return ptr;
}

void heap_caps_aligned_free(void *ptr)
{
    heap_caps_free(ptr);
}

void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size, uint32_t caps)
{
    size_t size_bytes;
    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    void *ptr = heap_caps_aligned_alloc(alignment, size_bytes, caps);
    if (ptr != NULL) {
        memset(ptr, 0, size_bytes);
    }

    return ptr;
}
