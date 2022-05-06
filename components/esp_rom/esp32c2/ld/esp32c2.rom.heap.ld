/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Multi heap function
 */

PROVIDE (multi_heap_malloc = multi_heap_malloc_impl);
PROVIDE (multi_heap_free = multi_heap_free_impl);
PROVIDE (multi_heap_realloc = multi_heap_realloc_impl);
PROVIDE (multi_heap_get_allocated_size = multi_heap_get_allocated_size_impl);
PROVIDE (multi_heap_register = multi_heap_register_impl);
PROVIDE (multi_heap_get_info = multi_heap_get_info_impl);
PROVIDE (multi_heap_free_size = multi_heap_free_size_impl);
PROVIDE (multi_heap_minimum_free_size = multi_heap_minimum_free_size_impl);
PROVIDE (multi_heap_get_block_address = multi_heap_get_block_address_impl);
PROVIDE (multi_heap_aligned_alloc = multi_heap_aligned_alloc_impl);
PROVIDE (multi_heap_aligned_free = multi_heap_aligned_free_impl);
PROVIDE (multi_heap_check = multi_heap_check);
PROVIDE (multi_heap_set_lock = multi_heap_set_lock);
PROVIDE (multi_heap_internal_lock = multi_heap_internal_lock);
PROVIDE (multi_heap_internal_unlock = multi_heap_internal_unlock);
