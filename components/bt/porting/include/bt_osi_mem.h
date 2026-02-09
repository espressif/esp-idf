/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"

void *bt_osi_mem_malloc(size_t size);

void *bt_osi_mem_calloc(size_t n, size_t size);

void *bt_osi_mem_malloc_internal(size_t size);

void *bt_osi_mem_calloc_internal(size_t n, size_t size);

void bt_osi_mem_free(void *ptr);

void bt_osi_mem_free_internal(void *ptr);
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
void bt_osi_mem_count_limit_set(uint16_t count_limit);
#endif // CONFIG_BT_LE_MEM_CHECK_ENABLED

#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
size_t bt_osi_mem_internal_used_size_get(void);
size_t bt_osi_mem_used_size_get(void);
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED

#if CONFIG_BT_NIMBLE_MEM_DEBUG
/**
 * @brief Initialize NimBLE memory debug module
 */
void nimble_mem_dbg_init(void);

/**
 * @brief Record memory allocation information
 *
 * @param p     Pointer to allocated memory
 * @param size  Size of allocation
 * @param func  Function name where allocation occurred
 * @param line  Line number where allocation occurred
 */
void nimble_mem_dbg_record(void *p, int size, const char *func, int line);

/**
 * @brief Clean up memory allocation record
 *
 * @param p     Pointer to memory being freed
 * @param func  Function name where free occurred
 * @param line  Line number where free occurred
 */
void nimble_mem_dbg_clean(void *p, const char *func, int line);

/**
 * @brief Display all memory debug information
 */
void nimble_mem_dbg_show(void);

/**
 * @brief Get maximum memory size used
 *
 * @return Maximum memory size in bytes
 */
uint32_t nimble_mem_dbg_get_max_size(void);

/**
 * @brief Get current memory size in use
 *
 * @return Current memory size in bytes
 */
uint32_t nimble_mem_dbg_get_current_size(void);

/**
 * @brief Start tracking memory usage for a specific section
 *
 * @param index Section index (0 to NIMBLE_MEM_DBG_MAX_SECTION_NUM-1)
 */
void nimble_mem_dbg_set_section_start(uint8_t index);

/**
 * @brief Stop tracking memory usage for a specific section
 *
 * @param index Section index (0 to NIMBLE_MEM_DBG_MAX_SECTION_NUM-1)
 */
void nimble_mem_dbg_set_section_end(uint8_t index);

/**
 * @brief Get maximum memory size used in a specific section
 *
 * @param index Section index (0 to NIMBLE_MEM_DBG_MAX_SECTION_NUM-1)
 * @return Maximum memory size in bytes for this section
 */
uint32_t nimble_mem_dbg_get_max_size_section(uint8_t index);

/**
 * @brief Reallocate memory with debug tracking
 *
 * @param ptr   Pointer to memory to reallocate
 * @param new_size New size of allocation
 * @param func  Function name where realloc occurred
 * @param line  Line number where realloc occurred
 * @return Pointer to reallocated memory
 */
void *nimble_mem_dbg_realloc(void *ptr, size_t new_size, const char *func, int line);

#endif // CONFIG_BT_NIMBLE_MEM_DEBUG
