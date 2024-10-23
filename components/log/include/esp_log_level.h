/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log level
 */
typedef enum {
    ESP_LOG_NONE    = 0,    /*!< No log output */
    ESP_LOG_ERROR   = 1,    /*!< Critical errors, software module can not recover on its own */
    ESP_LOG_WARN    = 2,    /*!< Error conditions from which recovery measures have been taken */
    ESP_LOG_INFO    = 3,    /*!< Information messages which describe normal flow of events */
    ESP_LOG_DEBUG   = 4,    /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ESP_LOG_VERBOSE = 5,    /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
    ESP_LOG_MAX     = 6,    /*!< Number of levels supported */
} esp_log_level_t;

/** @cond */

// LOG_LOCAL_LEVEL controls what log levels are included in the binary.
#ifndef LOG_LOCAL_LEVEL
#if BOOTLOADER_BUILD
#define LOG_LOCAL_LEVEL  CONFIG_BOOTLOADER_LOG_LEVEL
#else // !BOOTLOADER_BUILD
#define LOG_LOCAL_LEVEL  CONFIG_LOG_MAXIMUM_LEVEL
#endif // !BOOTLOADER_BUILD
#endif // LOG_LOCAL_LEVEL

#ifdef NON_OS_BUILD

#define _ESP_LOG_ENABLED(log_level) (LOG_LOCAL_LEVEL >= (log_level))
#define _ESP_LOG_EARLY_ENABLED(log_level) _ESP_LOG_ENABLED(log_level)

#else // !NON_OS_BUILD

#if CONFIG_LOG_MASTER_LEVEL
#define _ESP_LOG_ENABLED(log_level) (esp_log_get_level_master() >= (log_level) && LOG_LOCAL_LEVEL >= (log_level))
#else // !CONFIG_LOG_MASTER_LEVEL
#define _ESP_LOG_ENABLED(log_level) (LOG_LOCAL_LEVEL >= (log_level))
#endif // !CONFIG_LOG_MASTER_LEVEL

/* For early log, there is no log tag filtering. So we want to log only if both the LOG_LOCAL_LEVEL and the
currently configured min log level are higher than the log level */
#define _ESP_LOG_EARLY_ENABLED(log_level) (LOG_LOCAL_LEVEL >= (log_level) && esp_log_get_default_level() >= (log_level))

#endif // !NON_OS_BUILD

/** @endcond */

/**
 * @brief Get the default log level.
 *
 * This function returns the default log level.
 * The default log level is used by the definition of ESP_LOGx macros and
 * can be overridden for specific tags using `esp_log_level_set("*", level)`.
 * If CONFIG_LOG_DYNAMIC_LEVEL_CONTROL=n, changing the default log level is not possible.
 *
 * @return The default log level.
 */
__attribute__((always_inline))
static inline esp_log_level_t esp_log_get_default_level(void)
{
#if CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
    extern esp_log_level_t esp_log_default_level;
    return esp_log_default_level;
#else
    return (esp_log_level_t) CONFIG_LOG_DEFAULT_LEVEL;
#endif
}

#if defined(CONFIG_LOG_MASTER_LEVEL) || __DOXYGEN__

/**
 * @brief Master log level.
 *
 * Optional master log level to check against for ESP_LOGx macros before calling
 * esp_log_write. Allows one to set a higher CONFIG_LOG_MAXIMUM_LEVEL but not
 * impose a performance hit during normal operation (only when instructed). An
 * application may set esp_log_set_level_master(level) to globally enforce a
 * maximum log level. ESP_LOGx macros above this level will be skipped immediately,
 * rather than calling esp_log or esp_log_write and doing a cache hit.
 *
 * @note The tradeoff is increased application size.
 *
 * @param level  Master log level
 */
void esp_log_set_level_master(esp_log_level_t level);

/**
 * @brief Returns master log level.
 * @return Master log level
 */
esp_log_level_t esp_log_get_level_master(void);

#endif // CONFIG_LOG_MASTER_LEVEL

/**
 * @brief Set log level for given tag
 *
 * If logging for given component has already been enabled, changes previous setting.
 *
 * @note Note that this function can not raise log level above the level set using
 *       CONFIG_LOG_MAXIMUM_LEVEL setting in menuconfig.
 *
 * To raise log level above the default one for a given file, define
 * LOG_LOCAL_LEVEL to one of the ESP_LOG_* values, before including esp_log.h in this file.
 *
 * If CONFIG_LOG_DYNAMIC_LEVEL_CONTROL is not selected the static (no-op) implementation of log level is used.
 * Changing the log level is not possible, esp_log_level_set does not work.
 *
 * @param tag   Tag of the log entries to enable. Must be a non-NULL zero terminated string.
 *              Value "*" resets log level for all tags to the given value.
 *              If the tag is NULL then a silent return happens.
 * @param level Selects log level to enable.
 *              Only logs at this and lower verbosity levels will be shown.
 */
void esp_log_level_set(const char* tag, esp_log_level_t level);

/**
 * @brief Get log level for a given tag, can be used to avoid expensive log statements
 *
 * If CONFIG_LOG_DYNAMIC_LEVEL_CONTROL is not selected the static (no-op) implementation of log level is used.
 * Changing the log level is not possible, esp_log_level_set does not work. This function returns the default log level.
 *
 * @param tag   Tag of the log to query current level. Must be a zero terminated string.
 *              If tag is NULL then the default log level is returned (see esp_log_get_default_level()).
 * @return      The current log level for the given tag.
 */
esp_log_level_t esp_log_level_get(const char* tag);

#ifdef __cplusplus
}
#endif
