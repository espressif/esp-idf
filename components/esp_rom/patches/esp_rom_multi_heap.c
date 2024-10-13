/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
