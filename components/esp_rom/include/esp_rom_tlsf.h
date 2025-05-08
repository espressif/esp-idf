/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* tlsf_t: a TLSF structure. Can contain 1 to N pools. */
/* pool_t: a block of memory that TLSF can manage. */
typedef void* tlsf_t;
typedef void* pool_t;

/* Create/destroy a memory pool. */
tlsf_t tlsf_create(void* mem);
tlsf_t tlsf_create_with_pool(void* mem, size_t bytes);
pool_t tlsf_get_pool(tlsf_t tlsf);

/* Add/remove memory pools. */
pool_t tlsf_add_pool(tlsf_t tlsf, void* mem, size_t bytes);
void tlsf_remove_pool(tlsf_t tlsf, pool_t pool);

/* malloc/memalign/realloc/free replacements. */
void* tlsf_malloc(tlsf_t tlsf, size_t size);
void* tlsf_memalign(tlsf_t tlsf, size_t align, size_t size);
void* tlsf_memalign_offs(tlsf_t tlsf, size_t align, size_t size, size_t offset);
void* tlsf_realloc(tlsf_t tlsf, void* ptr, size_t size);
void tlsf_free(tlsf_t tlsf, void* ptr);

/* Returns internal block size, not original request size */
size_t tlsf_block_size(void* ptr);

/* Overheads/limits of internal structures. */
size_t tlsf_size(void);
size_t tlsf_pool_overhead(void);
size_t tlsf_alloc_overhead(void);

#if ESP_TEE_BUILD
/* NOTE: These declarations are only needed for the TEE build, since these
 * functions are (static inline) defined in tlsf_control_functions.h for
 * IDF builds.
 */
size_t tlsf_align_size(void);
size_t tlsf_block_size_min(void);
size_t tlsf_block_size_max(void);

/* NOTE: The consumer of this callback function (tlsf_walk_pool) is patched
 * in IDF builds to address issues in the ROM implementation. For TEE build,
 * the ROM declarations can be used directly, as heap integrity checking is not
 * supported.
 */
typedef void (*tlsf_walker)(void* ptr, size_t size, int used, void* user);
#else
typedef bool (*tlsf_walker)(void* ptr, size_t size, int used, void* user);
#endif

/* Debugging. */
void tlsf_walk_pool(pool_t pool, tlsf_walker walker, void* user);
/* Returns nonzero if any internal consistency check fails. */
int tlsf_check(tlsf_t tlsf);
int tlsf_check_pool(pool_t pool);

/*!
 * Defines the function prototypes for multi_heap_internal_poison_fill_region
 * and multi_heap_internal_check_block_poisoning, these two function will
 * be registered to the ROM tlsf IMPL through the function tlsf_poison_fill_pfunc_set()
 * and tlsf_poison_check_pfunc_set() when the heap poisoning feature is enabled.
 */
typedef void (*poison_fill_pfunc_t)(void *start, size_t size, bool is_free);
typedef bool (*poison_check_pfunc_t)(void *start, size_t size, bool is_free, bool print_errors);

/*!
 * @brief Set the function to call for filling memory region when
 * poisoning is configured.
 *
 * @note Please keep in mind that this function in ROM still accepts void*.
 *
 * @param pfunc The callback function to trigger for poisoning
 * a memory region.
 */
void tlsf_poison_fill_pfunc_set(poison_fill_pfunc_t pfunc);

/*!
 * @brief Set the function to call for checking memory region when
 * poisoning is configured.
 *
 * @param pfunc The callback function to trigger for checking
 * the content of a memory region.
 */
void tlsf_poison_check_pfunc_set(poison_check_pfunc_t pfunc);

#ifdef __cplusplus
}
#endif
