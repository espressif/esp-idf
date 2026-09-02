/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTDM_MEM_DEBUG_H_
#define _BTDM_MEM_DEBUG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable BTDM OSAL malloc/free leak tracking.
 * Set to 0 to compile out the tracking hooks.
 */
#ifndef BTDM_MEM_DEBUG
#define BTDM_MEM_DEBUG (0)
#endif

#if BTDM_MEM_DEBUG

/**
 * @brief Record an allocation (address, size, and auto-incremented sequence).
 *
 * @param ptr   Allocated memory address
 * @param size  Allocation size in bytes
 */
void btdm_mem_debug_record(void *ptr, uint32_t size);

/**
 * @brief Clear the record for a freed address.
 *
 * @param ptr   Memory address being freed
 */
void btdm_mem_debug_clear(void *ptr);

/**
 * @brief Print all outstanding (not yet freed) allocations.
 *
 * Prints address, size and sequence number for each leak.
 */
void btdm_mem_debug_dump(void);

/**
 * @brief Get number of currently tracked (unfreed) allocations.
 */
uint32_t btdm_mem_debug_get_count(void);

#else /* !BTDM_MEM_DEBUG */

static inline void btdm_mem_debug_record(void *ptr, uint32_t size)
{
    (void)ptr;
    (void)size;
}

static inline void btdm_mem_debug_clear(void *ptr)
{
    (void)ptr;
}

static inline void btdm_mem_debug_dump(void)
{
}

static inline uint32_t btdm_mem_debug_get_count(void)
{
    return 0;
}

#endif /* BTDM_MEM_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_MEM_DEBUG_H_ */
