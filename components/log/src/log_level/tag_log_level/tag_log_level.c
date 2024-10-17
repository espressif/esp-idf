/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_log_level.h"
#include "esp_private/log_lock.h"
#include "esp_private/log_level.h"
#include "sdkconfig.h"

#if CONFIG_LOG_TAG_LEVEL_IMPL_LINKED_LIST || CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_AND_LINKED_LIST
#include "linked_list/log_linked_list.h"
#endif

#if CONFIG_LOG_TAG_LEVEL_CACHE_ARRAY || CONFIG_LOG_TAG_LEVEL_CACHE_BINARY_MIN_HEAP
#define CACHE_ENABLED 1
#include "cache/log_cache.h"
#else
#define CACHE_ENABLED 0
#endif

#if !CONFIG_LOG_TAG_LEVEL_IMPL_NONE

static inline void log_level_set(const char *tag, esp_log_level_t level);
static esp_log_level_t log_level_get(const char *tag, bool timeout);

static inline void log_level_set(const char *tag, esp_log_level_t level)
{
    if (tag == NULL) {
        // expected non-NULL zero terminated string.
        return;
    }
    esp_log_impl_lock();
    // for wildcard tag, remove all linked list items and clear the cache
    if (strcmp(tag, "*") == 0) {
        esp_log_set_default_level(level);
        esp_log_linked_list_clean();
#if CACHE_ENABLED
        esp_log_cache_clean();
#endif
    } else {
        __attribute__((unused)) bool success = esp_log_linked_list_set_level(tag, level);
#if CACHE_ENABLED
        if (success) {
            esp_log_cache_set_level(tag, level);
        }
#endif
    }
    esp_log_impl_unlock();
}

static esp_log_level_t log_level_get(const char *tag, bool timeout)
{
    esp_log_level_t level_for_tag = esp_log_get_default_level();
    if (tag == NULL) {
        return level_for_tag;
    }
    if (timeout) {
        if (esp_log_impl_lock_timeout() == false) {
            return ESP_LOG_NONE;
        }
    } else {
        esp_log_impl_lock();
    }
#if CACHE_ENABLED
    bool cache_miss = !esp_log_cache_get_level(tag, &level_for_tag);
    if (cache_miss) {
        esp_log_linked_list_get_level(tag, &level_for_tag);
        esp_log_cache_add(tag, level_for_tag);
    }
#else
    esp_log_linked_list_get_level(tag, &level_for_tag);
#endif
    esp_log_impl_unlock();
    return level_for_tag;
}

#endif // !CONFIG_LOG_TAG_LEVEL_IMPL_NONE

void esp_log_level_set(const char *tag, esp_log_level_t level)
{
#if CONFIG_LOG_TAG_LEVEL_IMPL_NONE
    (void)tag;
    esp_log_set_default_level(level);
#else
    log_level_set(tag, level);
#endif
}

esp_log_level_t esp_log_level_get_timeout(const char *tag)
{
#if CONFIG_LOG_TAG_LEVEL_IMPL_NONE
    (void)tag;
    return esp_log_get_default_level();
#else
    return log_level_get(tag, true);
#endif
}

esp_log_level_t esp_log_level_get(const char *tag)
{
#if CONFIG_LOG_TAG_LEVEL_IMPL_NONE
    (void)tag;
    return esp_log_get_default_level();
#else
    return log_level_get(tag, false);
#endif
}
