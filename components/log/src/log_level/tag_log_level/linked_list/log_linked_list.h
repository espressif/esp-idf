/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include "esp_log_level.h"

/**
 * @brief Set the log level for a specific log tag using the linked list
 * approach.
 *
 * If the tag already exists in the linked list, its log level will be updated
 * with the new value. If the tag is not found in the linked list, a new entry
 * will be appended to the linked list with the provided log level.
 *
 * @param tag The log tag for which to set the log level.
 * @param level The log level to be set for the specified log tag.
 * @return true   If the log level was successfully set or updated,
 *         false  Otherwise.
 */
bool esp_log_linked_list_set_level(const char *tag, esp_log_level_t level);

/**
 * @brief Get the log level for a specific log tag using the linked list
 * approach.
 *
 * If the tag is found in the linked list, its corresponding log level will be
 * returned.
 *
 * @param tag The log tag for which to retrieve the log level.
 * @param level Pointer to a variable where the retrieved log level will be
 * stored.
 * @return true  if the log level was successfully retrieved from the linked list,
 *         false if the tag was not found in the linked list.
 */
bool esp_log_linked_list_get_level(const char *tag, esp_log_level_t *level);

/**
 * @brief Clears the linked list of all tags and frees the allocated memory for
 * each tag entry.
 *
 * This function clears the linked list of all tags that have been set using
 * esp_log_level_set(). It also frees the memory allocated for each tag entry in
 * the linked list.
 */
void esp_log_linked_list_clean(void);
