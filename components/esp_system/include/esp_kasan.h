/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief KASAN shadow nibble poison tags.
 *
 * Each 4-bit nibble in the shadow covers a 4-byte granule of real memory.
 * Values 0x0-0x3 indicate valid (or partially valid) memory; 0xC-0xF
 * indicate poisoned memory with the tag describing the reason.
 */
#define KASAN_POISON_HEAP_FREE  ((uint8_t)0xF) /**< Freed heap region */
#define KASAN_POISON_HEAP_LRZ   ((uint8_t)0xE) /**< Heap left redzone (before alloc) */
#define KASAN_POISON_HEAP_RRZ   ((uint8_t)0xD) /**< Heap right redzone (after alloc) */
#define KASAN_POISON_UNINIT     ((uint8_t)0xC) /**< Never-allocated / uninitialised */

#if CONFIG_COMPILER_KASAN
/**
 * @brief Initialise KASAN shadow memory.
 *
 * Must be called before heap_caps_init() so that the shadow region is ready
 * when the first allocation hook fires.
 */
void kasan_init_shadow(void);

/**
 * @brief Poison a memory region in the KASAN shadow.
 *
 * Marks [addr, addr+size) as invalid with the given @p tag.
 */
void kasan_poison_region(const void *addr, size_t size, uint8_t tag);

/**
 * @brief Unpoison a memory region in the KASAN shadow.
 *
 * Marks [addr, addr+size) as valid (accessible).
 */
void kasan_unpoison_region(const void *addr, size_t size);

/**
 * @brief Temporarily disable KASAN load/store checks on the current core.
 *
 * Increments a nested suppression counter; while it is non-zero, the
 * __asan_load_N / __asan_store_N runtime stubs return without touching shadow
 * memory.  Each call must be paired with kasan_enable_checks().
 *
 * Intended for short critical sections that manipulate cache/MMU state or
 * otherwise execute in conditions where accessing the KASAN shadow region
 * would itself fault (for example, the early SPI flash chip probe in
 * esp_flash_init_default_chip()).
 *
 * This API is safe to call from any context (task, ISR, panic handler).
 */
void kasan_disable_checks(void);

/**
 * @brief Re-enable KASAN load/store checks suppressed by kasan_disable_checks().
 *
 * Decrements the suppression counter.  Calls must be balanced; otherwise
 * checks remain disabled (or, if unbalanced the other way, the counter wraps
 * around and produces undefined behaviour).
 */
void kasan_enable_checks(void);
#endif

#if CONFIG_KASAN_NO_HALT
/**
 * @brief Return the number of KASAN errors reported since boot or last reset.
 */
uint32_t kasan_get_error_count(void);

/**
 * @brief Reset the KASAN error counter to zero.
 */
void kasan_reset_error_count(void);
#endif

#ifdef __cplusplus
}
#endif
