/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h4.rom.heap.ld for esp32h4
 *
 *
 * Generated from ./target/esp32h4/interface-esp32h4.yml md5sum 14f04fa4b1cf69c4e6eec57d641026c4
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group heap
 ***************************************/

/* Functions */
tlsf_create = 0x400003d8;
tlsf_create_with_pool = 0x400003dc;
tlsf_get_pool = 0x400003e0;
tlsf_add_pool = 0x400003e4;
tlsf_remove_pool = 0x400003e8;
tlsf_malloc = 0x400003ec;
tlsf_memalign = 0x400003f0;
tlsf_memalign_offs = 0x400003f4;
tlsf_malloc_addr = 0x400003f8;
tlsf_realloc = 0x400003fc;
tlsf_free = 0x40000400;
tlsf_block_size = 0x40000404;
tlsf_size = 0x40000408;
tlsf_pool_overhead = 0x4000040c;
tlsf_alloc_overhead = 0x40000410;
tlsf_walk_pool = 0x40000414;
tlsf_check = 0x40000418;
tlsf_check_pool = 0x4000041c;
tlsf_poison_fill_pfunc_set = 0x40000420;
tlsf_poison_check_pfunc_set = 0x40000424;
multi_heap_get_block_address_impl = 0x40000428;
multi_heap_get_allocated_size_impl = 0x4000042c;
multi_heap_register_impl = 0x40000430;
multi_heap_set_lock = 0x40000434;
multi_heap_os_funcs_init = 0x40000438;
multi_heap_internal_lock = 0x4000043c;
multi_heap_internal_unlock = 0x40000440;
multi_heap_get_first_block = 0x40000444;
multi_heap_get_next_block = 0x40000448;
multi_heap_is_free = 0x4000044c;
multi_heap_malloc_impl = 0x40000450;
multi_heap_free_impl = 0x40000454;
multi_heap_realloc_impl = 0x40000458;
multi_heap_aligned_alloc_impl_offs = 0x4000045c;
multi_heap_aligned_alloc_impl = 0x40000460;
multi_heap_check = 0x40000464;
multi_heap_dump = 0x40000468;
multi_heap_free_size_impl = 0x4000046c;
multi_heap_minimum_free_size_impl = 0x40000470;
multi_heap_get_info_impl = 0x40000474;
/* Data (.data, .bss, .rodata) */
heap_tlsf_table_ptr = 0x4085ffd4;

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
PROVIDE (multi_heap_os_funcs_init = multi_heap_mutex_init);
PROVIDE (multi_heap_internal_lock = multi_heap_internal_lock);
PROVIDE (multi_heap_internal_unlock = multi_heap_internal_unlock);
