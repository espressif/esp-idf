/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file implements a cache-based approach for storing and retrieving log
 * tag levels in the ESP-IDF log library. It provides functions to set and get
 * log levels for specific tags using a cache data structure. The cache stores
 * recently accessed log tags and their corresponding log levels, providing
 * faster lookups for frequently used tags.
 *
 * The cache-based approach implemented in this file is intended to be used in
 * conjunction with the linked list approach for log tag level checks. The two
 * approaches complement each other to provide a more efficient and flexible log
 * level management system.
 *
 * The cache size is configurable using the TAG_CACHE_SIZE macro, which determines
 * the maximum number of log tags that can be stored in the cache. When the
 * cache is full, a binary min-heap ordering is used to replace the oldest entry
 * with a new one.
 *
 * The esp_log_cache_set_level function allows users to update the log level
 * for a specific tag in the cache. If the tag is not found in the cache, it
 * does not add a new entry.
 *
 * The esp_log_cache_get_level function retrieves the log level for a given tag
 * from the cache. If the tag is not found in the cache, it returns a cache miss
 * indication. At the top of layer esp_log_cache_add should be called to add the
 * tag into the cache.
 *
 * The cache-based approach aims to improve the efficiency of log level
 * retrieval by providing a balance between speed and memory usage. However, the
 * cache has a fixed size, and when it becomes full, the cache may evict less
 * frequently used tags, resulting in a cache miss.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "esp_log_level.h"
#include "esp_private/log_level.h"
#include "esp_compiler.h"
#include "esp_assert.h"
#include "sdkconfig.h"

#ifndef NDEBUG
// Enable consistency checks and cache statistics in this file.
#define LOG_BUILTIN_CHECKS
#endif

ESP_STATIC_ASSERT(((CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE & (CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE + 1)) == 0), "Number of tags to be cached must be 2**n - 1, n >= 2. [1, 3, 7, 15, 31, 63, 127, 255, ...]");
#define TAG_CACHE_SIZE (CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE)
#define MAX_GENERATION ((1 << 29) - 1)

typedef struct {
    const char *tag;
    uint32_t level : 3;
    uint32_t generation : 29; // this size should be the same in MAX_GENERATION
} cached_tag_entry_t;

static cached_tag_entry_t s_log_cache[TAG_CACHE_SIZE];
static uint32_t s_log_cache_max_generation = 0;
static uint32_t s_log_cache_entry_count = 0;

#ifdef LOG_BUILTIN_CHECKS
static uint32_t s_log_cache_misses = 0;
#endif

static void heap_bubble_down(int index);
static inline void heap_swap(int i, int j);
static void fix_cache_generation_overflow(void);

void esp_log_cache_set_level(const char *tag, esp_log_level_t level)
{
    // search in the cache and update the entry it if exists
    for (uint32_t i = 0; i < s_log_cache_entry_count; ++i) {
#ifdef LOG_BUILTIN_CHECKS
        assert(i == 0 || s_log_cache[(i - 1) / 2].generation < s_log_cache[i].generation);
#endif
        if (strcmp(s_log_cache[i].tag, tag) == 0) {
            s_log_cache[i].level = level;
            return;
        }
    }
}

bool esp_log_cache_get_level(const char *tag, esp_log_level_t *level)
{
    // Look for tag in cache
    uint32_t i;
    for (i = 0; i < s_log_cache_entry_count; ++i) {
#ifdef LOG_BUILTIN_CHECKS
        assert(i == 0 || s_log_cache[(i - 1) / 2].generation < s_log_cache[i].generation);
#endif
        if (s_log_cache[i].tag == tag) {
            break;
        }
    }
    if (i == s_log_cache_entry_count) { // Not found in cache
#ifdef LOG_BUILTIN_CHECKS
        ++s_log_cache_misses;
#endif
        return false;
    }
    // Return level from cache
    *level = (esp_log_level_t) s_log_cache[i].level;
    // If cache has been filled, start taking ordering into account
    // (other options are: dynamically resize cache, add "dummy" entries
    //  to the cache; this option was chosen because code is much simpler,
    //  and the unfair behavior of cache will show it self at most once, when
    //  it has just been filled)
    if (s_log_cache_entry_count == TAG_CACHE_SIZE) {
        // Update item generation
        s_log_cache[i].generation = s_log_cache_max_generation++;
        // Restore heap ordering
        heap_bubble_down(i);
        // Check for generation count wrap and fix if necessary
        if (s_log_cache_max_generation == MAX_GENERATION) {
            fix_cache_generation_overflow();
        }
    }
    return true;
}

void esp_log_cache_clean(void)
{
    s_log_cache_entry_count = 0;
    s_log_cache_max_generation = 0;
#ifdef LOG_BUILTIN_CHECKS
    s_log_cache_misses = 0;
#endif
}

void esp_log_cache_add(const char *tag, esp_log_level_t level)
{
    uint32_t generation = s_log_cache_max_generation++;
    // First consider the case when cache is not filled yet.
    // In this case, just add new entry at the end.
    // This happens to satisfy binary min-heap ordering.
    if (s_log_cache_entry_count < TAG_CACHE_SIZE) {
        s_log_cache[s_log_cache_entry_count] = (cached_tag_entry_t) {
            .generation = generation,
            .level = level,
            .tag = tag
        };
        ++s_log_cache_entry_count;
    } else {
        // Cache is full, so we replace the oldest entry (which is at index 0
        // because this is a min-heap) with the new one, and do bubble-down
        // operation to restore min-heap ordering.
        s_log_cache[0] = (cached_tag_entry_t) {
            .tag = tag,
            .level = level,
            .generation = generation
        };
        heap_bubble_down(0);
    }
    // Check for generation count wrap and fix if necessary
    if (s_log_cache_max_generation == MAX_GENERATION) {
        fix_cache_generation_overflow();
    }
}

static void fix_cache_generation_overflow(void)
{
    // Fix generation count wrap
    for (uint32_t i = 0; i < s_log_cache_entry_count; ++i) {
        s_log_cache[i].generation = i;
    }
    s_log_cache_max_generation = s_log_cache_entry_count;
}

static void heap_bubble_down(int index)
{
    while (index < TAG_CACHE_SIZE / 2) {
        int left_index = index * 2 + 1;
        int right_index = left_index + 1;
        int next = (s_log_cache[left_index].generation < s_log_cache[right_index].generation) ? left_index : right_index;
        heap_swap(index, next);
        index = next;
    }
}

static inline void heap_swap(int i, int j)
{
    cached_tag_entry_t tmp = s_log_cache[i];
    s_log_cache[i] = s_log_cache[j];
    s_log_cache[j] = tmp;
}
