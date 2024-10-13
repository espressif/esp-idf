/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Opaque handle to a registered heap */
typedef struct multi_heap_info *multi_heap_handle_t;

/**
 * @brief Callback called when walking the given heap blocks of memory
 *
 * @param block_ptr Pointer to the block data
 * @param block_size The size of the block
 * @param block_used Block status. 0: free, 1: allocated
 * @param user_data Opaque pointer to user defined data
 *
 * @return True if the walker is expected to continue the heap traversal
 *         False if the walker is expected to stop the traversal of the heap
 */
typedef bool (*multi_heap_walker_cb_t)(void *block_ptr, size_t block_size, int block_used, void *user_data);

/**
 * @brief Call the tlsf_walk_pool function of the heap given as parameter with
 * the walker function passed as parameter
 *
 * @param heap The heap to traverse
 * @param walker_func The walker to trigger on each block of the heap
 * @param user_data Opaque pointer to user defined data
 */
void multi_heap_walk(multi_heap_handle_t heap, multi_heap_walker_cb_t walker_func, void *user_data);

#ifdef __cplusplus
}
#endif
