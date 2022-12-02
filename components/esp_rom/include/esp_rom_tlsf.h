/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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
