// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * Log library — implementation notes.
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

#ifndef BOOTLOADER_BUILD
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#endif

#include "esp_attr.h"
#include "xtensa/hal.h"
#include "soc/soc.h"
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "esp_log.h"


#ifndef BOOTLOADER_BUILD

// Number of tags to be cached. Must be 2**n - 1, n >= 2.
#define TAG_CACHE_SIZE 31

// Maximum time to wait for the mutex in a logging statement.
#define MAX_MUTEX_WAIT_MS 10
#define MAX_MUTEX_WAIT_TICKS ((MAX_MUTEX_WAIT_MS + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS)

// Uncomment this to enable consistency checks and cache statistics in this file.
// #define LOG_BUILTIN_CHECKS

typedef struct {
    const char* tag;
    uint32_t level : 3;
    uint32_t generation : 29;
} cached_tag_entry_t;

typedef struct uncached_tag_entry_{
    struct uncached_tag_entry_* next;
    uint8_t level;  // esp_log_level_t as uint8_t
    char tag[0];    // beginning of a zero-terminated string
} uncached_tag_entry_t;

static esp_log_level_t s_log_default_level = ESP_LOG_VERBOSE;
static uncached_tag_entry_t* s_log_tags_head = NULL;
static uncached_tag_entry_t* s_log_tags_tail = NULL;
static cached_tag_entry_t s_log_cache[TAG_CACHE_SIZE];
static uint32_t s_log_cache_max_generation = 0;
static uint32_t s_log_cache_entry_count = 0;
static vprintf_like_t s_log_print_func = &vprintf;
static SemaphoreHandle_t s_log_mutex = NULL;

#ifdef LOG_BUILTIN_CHECKS
static uint32_t s_log_cache_misses = 0;
#endif

static inline bool get_cached_log_level(const char* tag, esp_log_level_t* level);
static inline bool get_uncached_log_level(const char* tag, esp_log_level_t* level);
static inline void add_to_cache(const char* tag, esp_log_level_t level);
static void heap_bubble_down(int index);
static inline void heap_swap(int i, int j);
static inline bool should_output(esp_log_level_t level_for_message, esp_log_level_t level_for_tag);
static inline void clear_log_level_list();

void esp_log_set_vprintf(vprintf_like_t func)
{
    s_log_print_func = func;
}

void esp_log_level_set(const char* tag, esp_log_level_t level)
{
    if (!s_log_mutex) {
        s_log_mutex = xSemaphoreCreateMutex();
    }
    xSemaphoreTake(s_log_mutex, portMAX_DELAY);

    // for wildcard tag, remove all linked list items and clear the cache
    if (strcmp(tag, "*") == 0) {
        s_log_default_level = level;
        clear_log_level_list();
        xSemaphoreGive(s_log_mutex);
        return;
    }

    // allocate new linked list entry and append it to the endo of the list
    size_t entry_size = offsetof(uncached_tag_entry_t, tag) + strlen(tag) + 1;
    uncached_tag_entry_t* new_entry = (uncached_tag_entry_t*) malloc(entry_size);
    if (!new_entry) {
        xSemaphoreGive(s_log_mutex);
        return;
    }
    new_entry->next = NULL;
    new_entry->level = (uint8_t) level;
    strcpy(new_entry->tag, tag);
    if (s_log_tags_tail) {
        s_log_tags_tail->next = new_entry;
    }
    s_log_tags_tail = new_entry;
    if (!s_log_tags_head) {
        s_log_tags_head = new_entry;
    }
    xSemaphoreGive(s_log_mutex);
}

void clear_log_level_list()
{
    for (uncached_tag_entry_t* it = s_log_tags_head; it != NULL; ) {
        uncached_tag_entry_t* next = it->next;
        free(it);
        it = next;
    }
    s_log_tags_tail = NULL;
    s_log_tags_head = NULL;
    s_log_cache_entry_count = 0;
    s_log_cache_max_generation = 0;
#ifdef LOG_BUILTIN_CHECKS
    s_log_cache_misses = 0;
#endif

}

void IRAM_ATTR esp_log_write(esp_log_level_t level,
        const char* tag,
        const char* format, ...)
{
    if (!s_log_mutex) {
        s_log_mutex = xSemaphoreCreateMutex();
    }
    if (xSemaphoreTake(s_log_mutex, MAX_MUTEX_WAIT_TICKS) == pdFALSE) {
        return;
    }
    esp_log_level_t level_for_tag;
    // Look for the tag in cache first, then in the linked list of all tags
    if (!get_cached_log_level(tag, &level_for_tag)) {
        if (!get_uncached_log_level(tag, &level_for_tag)) {
            level_for_tag = s_log_default_level;
        }
        add_to_cache(tag, level_for_tag);
#ifdef LOG_BUILTIN_CHECKS
        ++s_log_cache_misses;
#endif
    }
    xSemaphoreGive(s_log_mutex);
    if (!should_output(level, level_for_tag)) {
        return;
    }

    va_list list;
    va_start(list, format);
    (*s_log_print_func)(format, list);
    va_end(list);
}

static inline bool get_cached_log_level(const char* tag, esp_log_level_t* level)
{
    // Look for `tag` in cache
    int i;
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

static inline void add_to_cache(const char* tag, esp_log_level_t level)
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

static inline bool get_uncached_log_level(const char* tag, esp_log_level_t* level)
{
    // Walk the linked list of all tags and see if given tag is present in the list.
    // This is slow because tags are compared as strings.
    for (uncached_tag_entry_t* it = s_log_tags_head; it != NULL; it = it->next) {
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
#endif //BOOTLOADER_BUILD


#ifndef BOOTLOADER_BUILD
#define ATTR IRAM_ATTR
#else
#define ATTR
#endif // BOOTLOADER_BUILD


uint32_t ATTR esp_log_early_timestamp()
{
    return xthal_get_ccount() / (CPU_CLK_FREQ_ROM / 1000);
}

#ifndef BOOTLOADER_BUILD

uint32_t IRAM_ATTR esp_log_timestamp()
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return esp_log_early_timestamp();
    }
    static uint32_t base = 0;
    if (base == 0) {
        base = esp_log_early_timestamp();
    }
    return base + xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
}

#else

uint32_t esp_log_timestamp() __attribute__((alias("esp_log_early_timestamp")));

#endif //BOOTLOADER_BUILD
