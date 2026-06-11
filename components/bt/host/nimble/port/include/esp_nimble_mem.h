/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_NIMBLE_MEM_H__
#define __ESP_NIMBLE_MEM_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *nimble_mem_malloc(size_t size);

void *nimble_mem_calloc(size_t n, size_t size);

void *nimble_mem_realloc(void *ptr, size_t size);

void nimble_mem_free(void *ptr);

#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
size_t nimble_mem_used_size_get(void);
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

#endif // CONFIG_BT_NIMBLE_MEM_DEBUG


#if CONFIG_BT_NIMBLE_MEM_DEBUG

#define nimble_platform_mem_malloc(size)                        \
({                                                              \
    void *p;                                                    \
    do {                                                        \
        p = nimble_mem_malloc(size);                            \
        nimble_mem_dbg_record(p, size, __func__, __LINE__);     \
    } while (0);                                                \
    p;                                                          \
})

#define nimble_platform_mem_calloc(count, size)                 \
({                                                              \
    void *p;                                                    \
    do {                                                        \
        p = nimble_mem_calloc(count, size);                     \
        nimble_mem_dbg_record(p, (count) * (size), __func__, __LINE__); \
    } while (0);                                                \
    p;                                                          \
})

#define nimble_platform_mem_realloc(ptr, new_size)              \
({                                                              \
    void *_old = (void *)(ptr);                                 \
    size_t _nsz = (size_t)(new_size);                           \
    void *_new = nimble_mem_realloc(_old, _nsz);                \
    if (_new == NULL && _nsz > 0) {                             \
        /* realloc failed: original block still alive, keep its debug record */ \
    } else {                                                    \
        /* success or free (new_size==0): clean old, record new */ \
        if (_old) nimble_mem_dbg_clean(_old, __func__, __LINE__); \
        if (_new) nimble_mem_dbg_record(_new, _nsz, __func__, __LINE__); \
    }                                                           \
    _new;                                                       \
})

#define nimble_platform_mem_free(ptr)                            \
do {                                                             \
    void *tmp_ptr = (void *)(ptr);                               \
    nimble_mem_dbg_clean(tmp_ptr, __func__, __LINE__);           \
    nimble_mem_free(tmp_ptr);                                    \
} while (0)

#else

#define nimble_platform_mem_malloc nimble_mem_malloc
#define nimble_platform_mem_calloc nimble_mem_calloc
#define nimble_platform_mem_realloc nimble_mem_realloc
#define nimble_platform_mem_free nimble_mem_free

#endif // CONFIG_BT_NIMBLE_MEM_DEBUG


#ifdef __cplusplus
}
#endif

#endif /* __ESP_NIMBLE_MEM_H__ */
