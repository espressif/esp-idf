/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_NIMBLE_MEM_H__
#define __ESP_NIMBLE_MEM_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// #pragma message "This file should be replaced with bt_osi_mem.h, used here for compatibility"

#include "bt_osi_mem.h"

#if CONFIG_BT_NIMBLE_MEM_DEBUG

#define nimble_platform_mem_malloc(size)                        \
({                                                              \
    void *p;                                                    \
    do {                                                        \
        p = bt_osi_mem_malloc(size);                            \
        nimble_mem_dbg_record(p, size, __func__, __LINE__);     \
    } while (0);                                                \
    p;                                                          \
})

#define nimble_platform_mem_calloc(count, size)                 \
({                                                              \
    void *p;                                                    \
    do {                                                        \
        p = bt_osi_mem_calloc(count, size);                     \
        nimble_mem_dbg_record(p, (count) * (size), __func__, __LINE__); \
    } while (0);                                                \
    p;                                                          \
})

#define nimble_platform_mem_realloc(ptr, new_size)              \
({                                                              \
    void *p;                                                    \
    do {                                                        \
        p = nimble_mem_dbg_realloc(ptr, new_size, __func__, __LINE__); \
    } while (0);                                                \
    p;                                                          \
})

#define nimble_platform_mem_free(ptr)                            \
do {                                                              \
    void *tmp_ptr = (void *)(ptr);                                \
    nimble_mem_dbg_clean(tmp_ptr, __func__, __LINE__);           \
    bt_osi_mem_free(tmp_ptr);                                     \
} while (0)

#else

#define nimble_platform_mem_malloc bt_osi_mem_malloc
#define nimble_platform_mem_calloc bt_osi_mem_calloc
#define nimble_platform_mem_realloc realloc
#define nimble_platform_mem_free bt_osi_mem_free

#endif // CONFIG_BT_NIMBLE_MEM_DEBUG


#ifdef __cplusplus
}
#endif

#endif /* __ESP_NIMBLE_MEM_H__ */
