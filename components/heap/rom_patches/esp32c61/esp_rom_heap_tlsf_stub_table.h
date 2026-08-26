/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <stddef.h>
#include "rom_patch_tlsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TLSF ROM vtable stub table for esp32c61.
 *
 * This struct mirrors the TLSF function-pointer vtable stored in ROM.
 * Its layout MUST exactly match the ROM vtable for the target chip.
 * It is used by tlsf_set_rom_patches() to intercept TLSF calls.
 */
struct heap_tlsf_stub_table_t {
    tlsf_t (*tlsf_create)(void *mem, size_t max_bytes);
    tlsf_t (*tlsf_create_with_pool)(void *mem, size_t pool_bytes, size_t max_bytes);
    pool_t (*tlsf_get_pool)(tlsf_t tlsf);
    pool_t (*tlsf_add_pool)(tlsf_t tlsf, void *mem, size_t bytes);
    void (*tlsf_remove_pool)(tlsf_t tlsf, pool_t pool);

    void *(*tlsf_malloc)(tlsf_t tlsf, size_t size);
    void *(*tlsf_memalign)(tlsf_t tlsf, size_t align, size_t size);
    void *(*tlsf_memalign_offs)(tlsf_t tlsf, size_t align, size_t size, size_t offset);
    void *(*tlsf_realloc)(tlsf_t tlsf, void *ptr, size_t size);
    void (*tlsf_free)(tlsf_t tlsf, void *ptr);

    size_t (*tlsf_block_size)(void *ptr);
    size_t (*tlsf_size)(tlsf_t tlsf);
    size_t (*tlsf_align_size)(void);
    size_t (*tlsf_block_size_min)(void);
    size_t (*tlsf_block_size_max)(tlsf_t tlsf);
    size_t (*tlsf_pool_overhead)(void);
    size_t (*tlsf_alloc_overhead)(void);

    void (*tlsf_walk_pool)(pool_t pool, tlsf_walker walker, void *user);

    int (*tlsf_check)(tlsf_t tlsf);
    int (*tlsf_check_pool)(pool_t pool);
};

#ifdef __cplusplus
}
#endif
