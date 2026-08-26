/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_assert.h"
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

#define ESP_LOG_LEVEL_LEN   (3) /*!< Number of bits used to represent the log level */
#define ESP_LOG_LEVEL_MASK  ((1 << ESP_LOG_LEVEL_LEN) - 1) /*!< Mask for log level */
/// Returns level from config
#define ESP_LOG_GET_LEVEL(config)  ((config) & ESP_LOG_LEVEL_MASK)

/** @cond */
ESP_STATIC_ASSERT(ESP_LOG_MAX <= ESP_LOG_LEVEL_MASK, "Log level items of esp_log_level_t must fit ESP_LOG_LEVEL_MASK");

/**
 * @brief Controls the log levels included in the binary.
 *
 * This define determines the maximum log level that will be compiled into the binary for the current build configuration.
 * It adjusts the verbosity of logging by specifying which log levels are included or excluded.
 *
 * If this define is not explicitly set in the user namespace, its value is derived from Kconfig options:
 * - `CONFIG_BOOTLOADER_LOG_LEVEL` for bootloader builds.
 * - `CONFIG_SECURE_TEE_LOG_LEVEL` for Secure TEE builds.
 * - `CONFIG_LOG_MAXIMUM_LEVEL` for all other builds.
 *
 * If explicitly defined in the user's code, it overrides these default values, allowing fine-grained control over log verbosity.
 */
#ifndef LOG_LOCAL_LEVEL
#if BOOTLOADER_BUILD
#define LOG_LOCAL_LEVEL  CONFIG_BOOTLOADER_LOG_LEVEL
#elif ESP_TEE_BUILD
#define LOG_LOCAL_LEVEL  CONFIG_SECURE_TEE_LOG_LEVEL
#else
#define LOG_LOCAL_LEVEL  CONFIG_LOG_MAXIMUM_LEVEL
#endif
#endif // LOG_LOCAL_LEVEL
/** @endcond */

/**
 * @brief Check if a specific log level is enabled at compile-time.
 *
 * This macro checks whether logging for the specified log level is enabled based on the
 * current local log level setting (`LOG_LOCAL_LEVEL`). It uses a compile-time check to
 * determine if logging for the specified level should be included in the binary,
 * helping to exclude logs that are not configured.
 *
 * @param configs it includes log configs and level.
 * @return true if the specified log level is enabled, false otherwise.
 */
#define ESP_LOG_ENABLED(configs) (LOG_LOCAL_LEVEL >= ESP_LOG_GET_LEVEL(configs))

/** @cond */
#if NON_OS_BUILD

#define _ESP_LOG_ENABLED(log_level) ESP_LOG_ENABLED(log_level)
#define _ESP_LOG_EARLY_ENABLED(log_level) ESP_LOG_ENABLED(log_level)

#else // !NON_OS_BUILD

#if CONFIG_LOG_MASTER_LEVEL
#define _ESP_LOG_ENABLED(log_level) (esp_log_get_level_master() >= ESP_LOG_GET_LEVEL(log_level) && ESP_LOG_ENABLED(log_level))
#else // !CONFIG_LOG_MASTER_LEVEL
#define _ESP_LOG_ENABLED(log_level) ESP_LOG_ENABLED(log_level)
#endif // !CONFIG_LOG_MASTER_LEVEL

/* For early log, there is no log tag filtering. So we want to log only if both the LOG_LOCAL_LEVEL and the
currently configured min log level are higher than the log level */
#define _ESP_LOG_EARLY_ENABLED(log_level) (ESP_LOG_ENABLED(log_level) && esp_log_get_default_level() >= ESP_LOG_GET_LEVEL(log_level))

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
#if BOOTLOADER_BUILD
    return (esp_log_level_t) CONFIG_BOOTLOADER_LOG_LEVEL;
#elif ESP_TEE_BUILD
    return (esp_log_level_t) CONFIG_SECURE_TEE_LOG_LEVEL;
#elif CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
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
 * Allows one to set a higher CONFIG_LOG_MAXIMUM_LEVEL but not
 * impose a performance hit during normal operation (only when instructed). An
 * application may set esp_log_set_level_master(level) to globally enforce a
 * maximum log level. ESP_LOG macros above this level will be skipped,
 * rather than doing a tag lookup.
 *
 * The Master log level is not applicable for the bootloader.
 *
 * @param level  Master log level
 */
void esp_log_set_level_master(esp_log_level_t level);

/**
 * @brief Returns master log level.
 * The Master log level is not applicable for the bootloader.
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
