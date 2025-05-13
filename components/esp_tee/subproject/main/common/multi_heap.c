/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdbool.h>
#include "esp_rom_tlsf.h"
#include "esp_rom_sys.h"
#include "tlsf_block_functions.h"
#include "multi_heap.h"

/* Handle to a registered TEE heap */
static multi_heap_handle_t tee_heap;

inline static void multi_heap_assert(bool condition, const char *format, int line, intptr_t address)
{
    /* Can't use libc assert() here as it calls printf() which can cause another malloc() for a newlib lock.
       Also, it's useful to be able to print the memory address where corruption was detected.
    */
    (void) condition;
}

#define MULTI_HEAP_ASSERT(CONDITION, ADDRESS) \
    multi_heap_assert((CONDITION), "CORRUPT HEAP: multi_heap.c:%d detected at 0x%08x\n", \
                        __LINE__, (intptr_t)(ADDRESS))

/* Check a block is valid for this heap. Used to verify parameters. */
static void assert_valid_block(const heap_t *heap, const block_header_t *block)
{
    pool_t pool = tlsf_get_pool(heap->heap_data);
    void *ptr = block_to_ptr(block);

    MULTI_HEAP_ASSERT((ptr >= pool) &&
                      (ptr < pool + heap->pool_size),
                      (uintptr_t)ptr);
}

esp_err_t esp_tee_heap_init(void *start_ptr, size_t size)
{
    assert(start_ptr);
    if (size < (tlsf_size() + tlsf_block_size_min() + sizeof(heap_t))) {
        // Region too small to be a heap.
        return ESP_ERR_INVALID_SIZE;
    }

    heap_t *result = (heap_t *)start_ptr;
    size -= sizeof(heap_t);

    result->heap_data = tlsf_create_with_pool(start_ptr + sizeof(heap_t), size);
    if (result->heap_data == NULL) {
        return ESP_FAIL;
    }

    result->lock = NULL;
    result->free_bytes = size - tlsf_size();
    result->pool_size = size;
    result->minimum_free_bytes = result->free_bytes;

    tee_heap = (multi_heap_handle_t)result;

    return ESP_OK;
}

void *esp_tee_heap_malloc(size_t size)
{
    if (tee_heap == NULL || size == 0) {
        return NULL;
    }

    void *result = tlsf_malloc(tee_heap->heap_data, size);
    if (result) {
        tee_heap->free_bytes -= tlsf_block_size(result);
        tee_heap->free_bytes -= tlsf_alloc_overhead();
        if (tee_heap->free_bytes < tee_heap->minimum_free_bytes) {
            tee_heap->minimum_free_bytes = tee_heap->free_bytes;
        }
    }

    return result;
}

void *esp_tee_heap_calloc(size_t n, size_t size)
{
    size_t reg_size = n * size;
    void *ptr = esp_tee_heap_malloc(reg_size);
    if (ptr != NULL) {
        memset(ptr, 0x00, reg_size);
    }
    return ptr;
}

void *esp_tee_heap_aligned_alloc(size_t size, size_t alignment)
{
    if (tee_heap == NULL || size == 0) {
        return NULL;
    }

    // Alignment must be a power of two
    if (((alignment & (alignment - 1)) != 0) || (!alignment)) {
        return NULL;
    }

    void *result = tlsf_memalign_offs(tee_heap->heap_data, alignment, size, 0x00);
    if (result) {
        tee_heap->free_bytes -= tlsf_block_size(result);
        tee_heap->free_bytes -= tlsf_alloc_overhead();
        if (tee_heap->free_bytes < tee_heap->minimum_free_bytes) {
            tee_heap->minimum_free_bytes = tee_heap->free_bytes;
        }
    }

    return result;
}

void esp_tee_heap_free(void *p)
{
    if (tee_heap == NULL || p == NULL) {
        return;
    }

    assert_valid_block(tee_heap, block_from_ptr(p));

    tee_heap->free_bytes += tlsf_block_size(p);
    tee_heap->free_bytes += tlsf_alloc_overhead();
    tlsf_free(tee_heap->heap_data, p);
}

void *malloc(size_t size)
{
    return esp_tee_heap_malloc(size);
}

void *calloc(size_t n, size_t size)
{
    return esp_tee_heap_calloc(n, size);
}

void free(void *ptr)
{
    esp_tee_heap_free(ptr);
}

size_t esp_tee_heap_get_free_size(void)
{
    return tee_heap->free_bytes;
}

size_t esp_tee_heap_get_min_free_size(void)
{
    return tee_heap->minimum_free_bytes;
}

static void heap_dump_tlsf(void* ptr, size_t size, int used, void* user)
{
    (void)user;
    esp_rom_printf("Block %p data, size: %d bytes, Free: %s\n",
                   (void *)ptr,
                   size,
                   used ? "No" : "Yes");
}

void esp_tee_heap_dump_info(void)
{
    esp_rom_printf("Showing data for TEE heap: %p (%uB)\n", (void *)tee_heap, tee_heap->pool_size);
    tlsf_walk_pool(tlsf_get_pool(tee_heap->heap_data), heap_dump_tlsf, NULL);
}

/* Definitions for functions from the heap component, used in files shared with ESP-IDF */

void *heap_caps_malloc(size_t alignment, size_t size, uint32_t caps)
{
    (void) caps;
    return esp_tee_heap_malloc(size);
}

void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps)
{
    (void) caps;
    return esp_tee_heap_aligned_alloc(size, alignment);
}

void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size, uint32_t caps)
{
    (void) caps;
    uint32_t reg_size = n * size;

    void *ptr = esp_tee_heap_aligned_alloc(reg_size, alignment);
    if (ptr != NULL) {
        memset(ptr, 0x00, reg_size);
    }
    return ptr;
}
