/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include "esp_log_level.h"

/**
 * @brief Set the log level for a specific log tag in the cache.
 *
 * This function searches for the specified log tag in the cache and updates its
 * log level with the provided level. If the tag is found in the cache, its log
 * level will be updated to the new value. If the tag is not present in the
 * cache, this function does nothing.
 *
 * @param tag   The log tag for which to set the log level in the cache.
 * @param level The log level to be set for the specified log tag.
 */
void esp_log_cache_set_level(const char *tag, esp_log_level_t level);

/**
 * @brief Get the log level for a specific log tag from the cache.
 *
 * This function retrieves the log level for the specified log tag from the
 * cache. If the tag is found in the cache, its corresponding log level will be
 * returned via the `level` pointer argument. If the tag is not present in the
 * cache, this function returns `false` to indicate a cache miss, and the
 * `level` value remains unchanged.
 *
 * @param tag   The log tag for which to retrieve the log level from the cache.
 * @param level Pointer to a variable where the retrieved log level will be
 * stored.
 * @return `true`  if the log level was successfully retrieved from the cache,
 *         `false` if the tag was not found in the cache.
 */
bool esp_log_cache_get_level(const char *tag, esp_log_level_t *level);

/**
 * @brief Clear the log level cache.
 *
 * This function clears the log level cache, removing all entries from the cache
 * and resetting its state. After calling this function, the cache will be empty
 * and ready to store new log tag entries.
 */
void esp_log_cache_clean(void);

/**
 * @brief Add a log tag entry to the cache.
 *
 * This function adds a new log tag entry to the cache with the provided log
 * level. If the cache is not full, the new entry will be appended to the end of
 * the cache. If the cache is already full, the oldest entry in the cache will
 * be replaced with the new entry, and the cache will be rearranged to maintain
 * binary min-heap ordering.
 *
 * @param tag   The log tag to be added to the cache.
 * @param level The log level to be associated with the log tag in the cache.
 */
void esp_log_cache_add(const char *tag, esp_log_level_t level);
