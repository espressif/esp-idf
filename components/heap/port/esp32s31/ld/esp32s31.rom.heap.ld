/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.heap.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group heap
 ***************************************/

/* Functions */
tlsf_create = 0x2f8003d0;
tlsf_create_with_pool = 0x2f8003d4;
tlsf_get_pool = 0x2f8003d8;
tlsf_add_pool = 0x2f8003dc;
tlsf_remove_pool = 0x2f8003e0;
tlsf_malloc = 0x2f8003e4;
tlsf_memalign = 0x2f8003e8;
tlsf_memalign_offs = 0x2f8003ec;
tlsf_malloc_addr = 0x2f8003f0;
tlsf_realloc = 0x2f8003f4;
tlsf_free = 0x2f8003f8;
tlsf_block_size = 0x2f8003fc;
tlsf_size = 0x2f800400;
tlsf_pool_overhead = 0x2f800404;
tlsf_alloc_overhead = 0x2f800408;
tlsf_walk_pool = 0x2f80040c;
tlsf_check = 0x2f800410;
tlsf_check_pool = 0x2f800414;
tlsf_poison_fill_pfunc_set = 0x2f800418;
tlsf_poison_check_pfunc_set = 0x2f80041c;
multi_heap_get_block_address_impl = 0x2f800420;
multi_heap_get_allocated_size_impl = 0x2f800424;
multi_heap_register_impl = 0x2f800428;
multi_heap_set_lock = 0x2f80042c;
multi_heap_os_funcs_init = 0x2f800430;
multi_heap_internal_lock = 0x2f800434;
multi_heap_internal_unlock = 0x2f800438;
multi_heap_get_first_block = 0x2f80043c;
multi_heap_get_next_block = 0x2f800440;
multi_heap_is_free = 0x2f800444;
multi_heap_malloc_impl = 0x2f800448;
multi_heap_free_impl = 0x2f80044c;
multi_heap_realloc_impl = 0x2f800450;
multi_heap_aligned_alloc_impl_offs = 0x2f800454;
multi_heap_aligned_alloc_impl = 0x2f800458;
multi_heap_check = 0x2f80045c;
multi_heap_dump = 0x2f800460;
multi_heap_free_size_impl = 0x2f800464;
multi_heap_minimum_free_size_impl = 0x2f800468;
multi_heap_get_info_impl = 0x2f80046c;
/* Data (.data, .bss, .rodata) */
heap_tlsf_table_ptr = 0x2f07ffcc;

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
