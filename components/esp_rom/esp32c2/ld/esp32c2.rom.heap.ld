/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/***************************************
 Group heap
 ***************************************/

/* Functions */
PROVIDE( tlsf_create = 0x400002dc );
PROVIDE( tlsf_create_with_pool = 0x400002e0 );
PROVIDE( tlsf_get_pool = 0x400002e4 );
PROVIDE( tlsf_add_pool = 0x400002e8 );
PROVIDE( tlsf_remove_pool = 0x400002ec );
PROVIDE( tlsf_malloc = 0x400002f0 );
PROVIDE( tlsf_memalign = 0x400002f4 );
PROVIDE( tlsf_memalign_offs = 0x400002f8 );
PROVIDE( tlsf_realloc = 0x400002fc );
PROVIDE( tlsf_free = 0x40000300 );
PROVIDE( tlsf_block_size = 0x40000304 );
PROVIDE( tlsf_size = 0x40000308 );
PROVIDE( tlsf_align_size = 0x4000030c );
PROVIDE( tlsf_block_size_min = 0x40000310 );
PROVIDE( tlsf_block_size_max = 0x40000314 );
PROVIDE( tlsf_pool_overhead = 0x40000318 );
PROVIDE( tlsf_alloc_overhead = 0x4000031c );
PROVIDE( tlsf_walk_pool = 0x40000320 );
PROVIDE( tlsf_check = 0x40000324 );
PROVIDE( tlsf_check_pool = 0x40000328 );
PROVIDE( tlsf_poison_fill_pfunc_set = 0x4000032c );
PROVIDE( multi_heap_get_block_address_impl = 0x40000330 );
PROVIDE( multi_heap_get_allocated_size_impl = 0x40000334 );
PROVIDE( multi_heap_register_impl = 0x40000338 );
PROVIDE( multi_heap_set_lock = 0x4000033c );
PROVIDE( multi_heap_os_funcs_init = 0x40000340 );
PROVIDE( multi_heap_internal_lock = 0x40000344 );
PROVIDE( multi_heap_internal_unlock = 0x40000348 );
PROVIDE( multi_heap_get_first_block = 0x4000034c );
PROVIDE( multi_heap_get_next_block = 0x40000350 );
PROVIDE( multi_heap_is_free = 0x40000354 );
PROVIDE( multi_heap_malloc_impl = 0x40000358 );
PROVIDE( multi_heap_free_impl = 0x4000035c );
PROVIDE( multi_heap_realloc_impl = 0x40000360 );
PROVIDE( multi_heap_aligned_alloc_impl_offs = 0x40000364 );
PROVIDE( multi_heap_aligned_alloc_impl = 0x40000368 );
PROVIDE( multi_heap_check = 0x4000036c );
PROVIDE( multi_heap_dump = 0x40000370 );
PROVIDE( multi_heap_free_size_impl = 0x40000374 );
PROVIDE( multi_heap_minimum_free_size_impl = 0x40000378 );
PROVIDE( multi_heap_get_info_impl = 0x4000037c );
/* Data (.data, .bss, .rodata) */
PROVIDE( heap_tlsf_table_ptr = 0x3fcdffec );

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
