/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_private/log_attr.h"
#include "esp_log_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ESP_LOG_MODE_BINARY_EN
#define ESP_LOG_ATTR                    NOLOAD_ATTR
#define ESP_LOG_ATTR_STR(str)           (__builtin_constant_p(str) ? ESP_LOG_NOLOAD_STR(str) : str)
#define ESP_LOG_ATTR_DRAM_STR(str)      ESP_LOG_NOLOAD_STR(str)
#else
#define ESP_LOG_ATTR
#define ESP_LOG_ATTR_STR(str)           (str)
#define ESP_LOG_ATTR_DRAM_STR(str)      DRAM_STR(str)
#endif

/**
 * @brief Macro to initialize a static log tag string.
 *
 * This macro defines a static constant character array for a log tag with the specified name.
 * If the configuration enables binary log mode and disables tag level implementation,
 * the tag is placed with `NOLOAD_ATTR`.
 *
 * @param tag  The variable name for the log tag.
 * @param str  The string literal representing the log tag.
 *
 * Usage example:
 *     ESP_LOG_ATTR_TAG(TAG, "TAG_NAME");
 */
#if ESP_LOG_MODE_BINARY_EN && (NON_OS_BUILD || CONFIG_LOG_TAG_LEVEL_IMPL_NONE)
#define ESP_LOG_ATTR_TAG(tag, str)      static __attribute__((unused)) NOLOAD_ATTR const char tag[] = str
#else
#define ESP_LOG_ATTR_TAG(tag, str)      static __attribute__((unused)) const char *tag = str
#endif

/**
 * @brief Macro to initialize a static log tag string in DRAM.
 *
 * This macro defines a static constant character array for a log tag with the specified name,
 * placing it in DRAM. It is used when the log tag needs to be accessible even when the cache is disabled.
 * If the configuration enables binary log mode and disables tag level implementation,
 * the tag is placed with `NOLOAD_ATTR`.
 *
 * @param tag  The variable name for the log tag.
 * @param str  The string literal representing the log tag.
 *
 * Usage example:
 *     ESP_LOG_ATTR_TAG_DRAM(TAG, "TAG_NAME");
 */
#if ESP_LOG_MODE_BINARY_EN && (NON_OS_BUILD || CONFIG_LOG_TAG_LEVEL_IMPL_NONE)
#define ESP_LOG_ATTR_TAG_DRAM(tag, str) ESP_LOG_ATTR_TAG(tag, str)
#else
#define ESP_LOG_ATTR_TAG_DRAM(tag, str) static __attribute__((unused)) DRAM_ATTR const char tag[] = str
#endif

#ifdef __cplusplus
}
#endif
