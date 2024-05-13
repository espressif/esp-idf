/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.heap.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum f5c146321f24f88ad1f27234da5aed11
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group heap
 ***************************************/

/* Functions */
tlsf_create = 0x4000040c;
tlsf_create_with_pool = 0x40000410;
tlsf_get_pool = 0x40000414;
tlsf_add_pool = 0x40000418;
tlsf_remove_pool = 0x4000041c;
tlsf_malloc = 0x40000420;
tlsf_memalign = 0x40000424;
tlsf_memalign_offs = 0x40000428;
tlsf_realloc = 0x4000042c;
tlsf_free = 0x40000430;
tlsf_block_size = 0x40000434;
tlsf_size = 0x40000438;
tlsf_align_size = 0x4000043c;
tlsf_block_size_min = 0x40000440;
tlsf_block_size_max = 0x40000444;
tlsf_pool_overhead = 0x40000448;
tlsf_alloc_overhead = 0x4000044c;
tlsf_walk_pool = 0x40000450;
tlsf_check = 0x40000454;
tlsf_check_pool = 0x40000458;
tlsf_poison_fill_pfunc_set = 0x4000045c;
tlsf_poison_check_pfunc_set = 0x40000460;
multi_heap_get_block_address_impl = 0x40000464;
multi_heap_get_allocated_size_impl = 0x40000468;
multi_heap_register_impl = 0x4000046c;
multi_heap_set_lock = 0x40000470;
multi_heap_os_funcs_init = 0x40000474;
multi_heap_internal_lock = 0x40000478;
multi_heap_internal_unlock = 0x4000047c;
multi_heap_get_first_block = 0x40000480;
multi_heap_get_next_block = 0x40000484;
multi_heap_is_free = 0x40000488;
multi_heap_malloc_impl = 0x4000048c;
multi_heap_free_impl = 0x40000490;
multi_heap_realloc_impl = 0x40000494;
multi_heap_aligned_alloc_impl_offs = 0x40000498;
multi_heap_aligned_alloc_impl = 0x4000049c;
multi_heap_check = 0x400004a0;
multi_heap_dump = 0x400004a4;
multi_heap_free_size_impl = 0x400004a8;
multi_heap_minimum_free_size_impl = 0x400004ac;
multi_heap_get_info_impl = 0x400004b0;
/* Data (.data, .bss, .rodata) */
heap_tlsf_table_ptr = 0x4085ffd8;

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
