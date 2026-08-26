/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file implements a simple array cache approach for storing and retrieving log
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
 * the maximum number of log tags that can be stored in the cache.
 * When the cache is full, the oldest entry is replaced with the new one.
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

#define TAG_CACHE_SIZE (CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE)
#define IS_CACHE_FULL (s_cache_entry_count >= TAG_CACHE_SIZE)
#define ENTRY_COUNT() (IS_CACHE_FULL ? TAG_CACHE_SIZE : s_cache_entry_count)

typedef struct {
    const char *tag;
    uint32_t level;
} cached_tag_entry_t;

static cached_tag_entry_t s_cache[TAG_CACHE_SIZE];
static uint32_t s_cache_entry_count = 0;

void esp_log_cache_set_level(const char *tag, esp_log_level_t level)
{
    // search in the cache and update the entry it if exists
    uint32_t entry_count = ENTRY_COUNT();
    for (uint32_t i = 0; i < entry_count; ++i) {
        if (strcmp(s_cache[i].tag, tag) == 0) {
            s_cache[i].level = level;
            return;
        }
    }
}

bool esp_log_cache_get_level(const char *tag, esp_log_level_t *level)
{
    // Look for tag in cache
    uint32_t entry_count = ENTRY_COUNT();
    for (uint32_t i = 0; i < entry_count; ++i) {
        if (s_cache[i].tag == tag) {
            // Return level from cache
            *level = (esp_log_level_t) s_cache[i].level;
            return true;
        }
    }
    // Not found in cache
    return false;
}

void esp_log_cache_clean(void)
{
    s_cache_entry_count = 0;
}

void esp_log_cache_add(const char *tag, esp_log_level_t level)
{
    s_cache[s_cache_entry_count++ % TAG_CACHE_SIZE] = (cached_tag_entry_t) {
        .level = level,
        .tag = tag
    };
    if (s_cache_entry_count == UINT32_MAX) {
        s_cache_entry_count = TAG_CACHE_SIZE;
    }
}
