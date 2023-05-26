/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Log library implementation notes.
 *
 * Log library stores all tags provided to esp_log_level_set as a linked
 * list. See uncached_tag_entry_t structure.
 *
 * To avoid looking up log level for given tag each time message is
 * printed, this library caches pointers to tags. Because the suggested
 * way of creating tags uses one 'TAG' constant per file, this caching
 * should be effective. Cache is a binary min-heap of cached_tag_entry_t
 * items, ordering is done on 'generation' member. In this context,
 * generation is an integer which is incremented each time an operation
 * with cache is performed. When cache is full, new item is inserted in
 * place of an oldest item (that is, with smallest 'generation' value).
 * After that, bubble-down operation is performed to fix ordering in the
 * min-heap.
 *
 * The potential problem with wrap-around of cache generation counter is
 * ignored for now. This will happen if someone happens to output more
 * than 4 billion log entries, at which point wrap-around will not be
 * the biggest problem.
 *
 */

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "esp_log.h"
#include "esp_log_private.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
// Enable consistency checks and cache statistics in this file.
#define LOG_BUILTIN_CHECKS
#endif

#include "sys/queue.h"

// Number of tags to be cached. Must be 2**n - 1, n >= 2.
#define TAG_CACHE_SIZE 31

typedef struct {
    const char *tag;
    uint32_t level : 3;
    uint32_t generation : 29;
} cached_tag_entry_t;

typedef struct uncached_tag_entry_ {
    SLIST_ENTRY(uncached_tag_entry_) entries;
    uint8_t level;  // esp_log_level_t as uint8_t
    char tag[0];    // beginning of a zero-terminated string
} uncached_tag_entry_t;

#ifdef CONFIG_LOG_MASTER_LEVEL
esp_log_level_t g_master_log_level = CONFIG_LOG_DEFAULT_LEVEL;
#endif
esp_log_level_t esp_log_default_level = CONFIG_LOG_DEFAULT_LEVEL;
static SLIST_HEAD(log_tags_head, uncached_tag_entry_) s_log_tags = SLIST_HEAD_INITIALIZER(s_log_tags);
static cached_tag_entry_t s_log_cache[TAG_CACHE_SIZE];
static uint32_t s_log_cache_max_generation = 0;
static uint32_t s_log_cache_entry_count = 0;
static vprintf_like_t s_log_print_func = &vprintf;

#ifdef LOG_BUILTIN_CHECKS
static uint32_t s_log_cache_misses = 0;
#endif


static inline bool get_cached_log_level(const char *tag, esp_log_level_t *level);
static inline bool get_uncached_log_level(const char *tag, esp_log_level_t *level);
static inline void add_to_cache(const char *tag, esp_log_level_t level);
static void heap_bubble_down(int index);
static inline void heap_swap(int i, int j);
static inline bool should_output(esp_log_level_t level_for_message, esp_log_level_t level_for_tag);
static inline void clear_log_level_list(void);

vprintf_like_t esp_log_set_vprintf(vprintf_like_t func)
{
    esp_log_impl_lock();
    vprintf_like_t orig_func = s_log_print_func;
    s_log_print_func = func;
    esp_log_impl_unlock();
    return orig_func;
}

#ifdef CONFIG_LOG_MASTER_LEVEL
esp_log_level_t esp_log_get_level_master(void)
{
    return g_master_log_level;
}

void esp_log_set_level_master(esp_log_level_t level)
{
    g_master_log_level = level;
}
#endif // CONFIG_LOG_MASTER_LEVEL

void esp_log_level_set(const char *tag, esp_log_level_t level)
{
    esp_log_impl_lock();

    // for wildcard tag, remove all linked list items and clear the cache
    if (strcmp(tag, "*") == 0) {
        esp_log_default_level = level;
        clear_log_level_list();
        esp_log_impl_unlock();
        return;
    }

    // search for existing tag
    uncached_tag_entry_t *it = NULL;
    SLIST_FOREACH(it, &s_log_tags, entries) {
        if (strcmp(it->tag, tag) == 0) {
            // one tag in the linked list matched, update the level
            it->level = level;
            // quit with it != NULL
            break;
        }
    }
    // no existing tag, append new one
    if (it == NULL) {
        // allocate new linked list entry and append it to the head of the list
        size_t tag_len = strlen(tag) + 1;
        size_t entry_size = offsetof(uncached_tag_entry_t, tag) + tag_len;
        uncached_tag_entry_t *new_entry = (uncached_tag_entry_t *) malloc(entry_size);
        if (!new_entry) {
            esp_log_impl_unlock();
            return;
        }
        new_entry->level = (uint8_t) level;
        memcpy(new_entry->tag, tag, tag_len); // we know the size and strncpy would trigger a compiler warning here
        SLIST_INSERT_HEAD(&s_log_tags, new_entry, entries);
    }

    // search in the cache and update the entry it if exists
    for (uint32_t i = 0; i < s_log_cache_entry_count; ++i) {
#ifdef LOG_BUILTIN_CHECKS
        assert(i == 0 || s_log_cache[(i - 1) / 2].generation < s_log_cache[i].generation);
#endif
        if (strcmp(s_log_cache[i].tag, tag) == 0) {
            s_log_cache[i].level = level;
            break;
        }
    }
    esp_log_impl_unlock();
}


/* Common code for getting the log level from cache, esp_log_impl_lock()
   should be called before calling this function. The function unlocks,
   as indicated in the name.
*/
static esp_log_level_t s_log_level_get_and_unlock(const char *tag)
{
    esp_log_level_t level_for_tag;
    // Look for the tag in cache first, then in the linked list of all tags
    if (!get_cached_log_level(tag, &level_for_tag)) {
        if (!get_uncached_log_level(tag, &level_for_tag)) {
            level_for_tag = esp_log_default_level;
        }
        add_to_cache(tag, level_for_tag);
#ifdef LOG_BUILTIN_CHECKS
        ++s_log_cache_misses;
#endif
    }
    esp_log_impl_unlock();

    return level_for_tag;
}

esp_log_level_t esp_log_level_get(const char *tag)
{
    esp_log_impl_lock();
    return s_log_level_get_and_unlock(tag);
}

void clear_log_level_list(void)
{
    uncached_tag_entry_t *it;
    while ((it = SLIST_FIRST(&s_log_tags)) != NULL) {
        SLIST_REMOVE_HEAD(&s_log_tags, entries);
        free(it);
    }
    s_log_cache_entry_count = 0;
    s_log_cache_max_generation = 0;
#ifdef LOG_BUILTIN_CHECKS
    s_log_cache_misses = 0;
#endif
}

void esp_log_writev(esp_log_level_t level,
                   const char *tag,
                   const char *format,
                   va_list args)
{
    if (!esp_log_impl_lock_timeout()) {
        return;
    }
    esp_log_level_t level_for_tag = s_log_level_get_and_unlock(tag);
    if (!should_output(level, level_for_tag)) {
        return;
    }

    (*s_log_print_func)(format, args);

}

void esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_writev(level, tag, format, list);
    va_end(list);
}

static inline bool get_cached_log_level(const char *tag, esp_log_level_t *level)
{
    // Look for `tag` in cache
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
    }
    return true;
}

static inline void add_to_cache(const char *tag, esp_log_level_t level)
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
        return;
    }

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

static inline bool get_uncached_log_level(const char *tag, esp_log_level_t *level)
{
    // Walk the linked list of all tags and see if given tag is present in the list.
    // This is slow because tags are compared as strings.
    uncached_tag_entry_t *it;
    SLIST_FOREACH(it, &s_log_tags, entries) {
        if (strcmp(tag, it->tag) == 0) {
            *level = it->level;
            return true;
        }
    }
    return false;
}

static inline bool should_output(esp_log_level_t level_for_message, esp_log_level_t level_for_tag)
{
    return level_for_message <= level_for_tag;
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
