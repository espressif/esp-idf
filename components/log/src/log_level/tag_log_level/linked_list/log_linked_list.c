/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file implements a linked list-based approach for storing and retrieving
 * log tag levels in the ESP-IDF log library. It provides functions to set and
 * get log levels for specific tags using a linked list data structure. The
 * linked list stores log tags and their corresponding log levels as individual
 * nodes.
 *
 * The esp_log_linked_list_set_level() function allows users to set the log level
 * for a specific tag, and if the tag does not exist in the linked list, it adds
 * a new entry for that tag. The esp_log_linked_list_get_level() function
 * retrieves the log level for a given tag from the linked list. Please note that
 * this approach searches through the entire tag string to find a matching tag
 * during log level retrieval.
 *
 * The esp_log_linked_list_clean() function enables users to clear the entire linked
 * list, freeing the memory occupied by the list entries.
 *
 * The linked list approach provides a simple and memory-efficient method for
 * managing log levels, making it suitable for use in environments with limited
 * resources. However, due to its linear search nature, log level retrieval from
 * the linked list may become slower as the number of tags increases.
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"
#include "log_linked_list.h"
#include "esp_err.h"
#include "sdkconfig.h"

typedef struct uncached_tag_entry_ {
    SLIST_ENTRY(uncached_tag_entry_) entries;
    uint8_t level;          // esp_log_level_t as uint8_t
    char tag[0];            // beginning of a zero-terminated string
} uncached_tag_entry_t;

static SLIST_HEAD(log_tags_head, uncached_tag_entry_) s_log_tags = SLIST_HEAD_INITIALIZER(s_log_tags);

static inline esp_err_t add_to_list(const char *tag, esp_log_level_t level);
static inline bool set_log_level(const char *tag, esp_log_level_t level);

bool esp_log_linked_list_set_level(const char *tag, esp_log_level_t level)
{
    if (!set_log_level(tag, level)) {
        // no existing tag, append new one
        return add_to_list(tag, level) == ESP_OK;
    }
    return true;
}

// Getting the log level from linked list
// if tag is not found then returns false.
bool esp_log_linked_list_get_level(const char *tag, esp_log_level_t *level)
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

void esp_log_linked_list_clean(void)
{
    uncached_tag_entry_t *it;
    while ((it = SLIST_FIRST(&s_log_tags)) != NULL) {
        SLIST_REMOVE_HEAD(&s_log_tags, entries);
        free(it);
    }
}

static inline esp_err_t add_to_list(const char *tag, esp_log_level_t level)
{
    // allocate new linked list entry and append it to the head of the list
    size_t tag_len = strlen(tag) + 1;
    size_t entry_size = offsetof(uncached_tag_entry_t, tag) + tag_len;
    uncached_tag_entry_t *new_entry = (uncached_tag_entry_t *) malloc(entry_size);
    if (!new_entry) {
        return ESP_ERR_NO_MEM;
    }
    new_entry->level = (uint8_t) level;
    memcpy(new_entry->tag, tag, tag_len);
    SLIST_INSERT_HEAD(&s_log_tags, new_entry, entries);
    return ESP_OK;
}

static inline bool set_log_level(const char *tag, esp_log_level_t level)
{
    // search for existing tag
    uncached_tag_entry_t *it = NULL;
    SLIST_FOREACH(it, &s_log_tags, entries) {
        if (strcmp(it->tag, tag) == 0) {
            // one tag in the linked list matched, update the level
            it->level = level;
            return true;
        }
    }
    return false;
}
