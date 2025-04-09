/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <string.h>
#include "esp_err.h"

/* multi_heap is a heap implementation for handling multiple
   heterogeneous heaps in a single program.

   Any contiguous block of memory can be registered as a heap.
*/

#ifdef __cplusplus
extern "C" {
#endif
struct multi_heap_info {
    void *lock;
    size_t free_bytes;
    size_t minimum_free_bytes;
    size_t pool_size;
    void *heap_data;
};

typedef struct multi_heap_info heap_t;

/** @brief Opaque handle to a registered heap */
typedef struct multi_heap_info *multi_heap_handle_t;

/** @brief Initialize the TEE heap
 *
 * This function initialises the TEE heap at the specified address, and
 * sets up a handle for future heap operations.
 *
 * @param start Start address of the memory to use for the TEE heap.
 * @param size Size (in bytes) of the TEE heap.
 *
 * @return ESP_OK on success, or an `esp_err_t` error code on failure.
 */
esp_err_t esp_tee_heap_init(void *start, size_t size);

/**
 * @brief Allocate a buffer (malloc) in the TEE heap
 *
 * This function allocates a buffer of the specified size in the TEE heap.
 * The semantics are the same as the standard malloc().
 *
 * @param size The size of the desired buffer in bytes.
 *
 * @return A pointer to the newly allocated memory, or NULL if the allocation fails.
 */
void *esp_tee_heap_malloc(size_t size);

/**
 * @brief Allocate and zero-initialize (calloc) a buffer in the TEE heap
 *
 * This function allocates a buffer for an array of 'n' elements, each of 'size' bytes,
 * and initializes all bytes in the allocated storage to zero. The semantics are the same
 * as the standard calloc().
 *
 * @param n The number of elements to allocate.
 * @param size The size of each element in bytes.
 *
 * @return A pointer to the newly allocated and zero-initialized memory, or NULL if the allocation fails.
 */
void *esp_tee_heap_calloc(size_t n, size_t size);

/**
 * @brief Allocate a memory chunk with specific alignment in the TEE heap
 *
 * This function allocates a chunk of memory of the specified size with the specified alignment
 * in the TEE heap.
 *
 * @param size The size in bytes of the memory chunk.
 * @param alignment The alignment requirement for the memory chunk.
 *
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void *esp_tee_heap_aligned_alloc(size_t size, size_t alignment);

/**
 * @brief Free a buffer in the TEE heap
 *
 * This function frees a buffer that was previously allocated in the TEE heap.
 * The semantics are the same as the standard free().
 *
 * @param p A pointer to the memory to be freed, or NULL. The pointer must have been
 *          returned from esp_tee_heap_malloc(), esp_tee_heap_calloc(), or esp_tee_heap_aligned_alloc().
 */
void esp_tee_heap_free(void *p);

/**
 * @brief Get the size of available TEE heap
 *
 * @return Available TEE heap size, in bytes
 */
size_t esp_tee_heap_get_free_size(void);

/**
 * @brief Get the minimum TEE heap that has ever been available
 *
 * @return Minimum free TEE heap ever available, in bytes
 */
size_t esp_tee_heap_get_min_free_size(void);

/**
 * @brief Dump info about the entire structure of the TEE heap
 *
 * This function outputs detailed information about every block in the TEE heap to stdout.
 * (Intended for debugging purposes)
 */
void esp_tee_heap_dump_info(void);

#ifdef __cplusplus
}
#endif
