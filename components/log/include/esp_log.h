/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "esp_log_config.h"
#include "esp_log_level.h"
#include "esp_log_color.h"
#include "esp_log_buffer.h"
#include "esp_log_timestamp.h"
#include "esp_log_write.h"
#include "esp_log_format.h"
#include "esp_log_args.h"
#include "esp_log_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logs a formatted message using the provided log message configs and a variable argument list.
 *
 * @param config    Configuration and level of the log message.
 * @param tag       The tag string used to indicate the component from which to log.
 *                  It is also used to check whether logging is enabled for that tag (depends on CONFIG_LOG_TAG_LEVEL_IMPL).
 *                  If NULL then the tag is not printed.
 * @param format    The format string for the log message.
 * @param ...       Optional arguments to be formatted according to the format string.
 */
void esp_log(esp_log_config_t config, const char* tag, const char* format, ...);

/**
 * @brief Logs a formatted message using the provided log message configs and a variable argument list.
 *
 * @param config    Configuration and level of the log message.
 * @param tag       The tag string used to indicate the component from which to log.
 *                  It is also used to check whether logging is enabled for that tag (depends on CONFIG_LOG_TAG_LEVEL_IMPL).
 *                  If NULL then the tag is not printed.
 * @param format    The format string for the log message.
 * @param args      List of arguments.
 */
void esp_log_va(esp_log_config_t config, const char *tag, const char *format, va_list args);

/// macro to output logs in startup code, before heap allocator and syscalls have been initialized.
/// Log at ``ESP_LOG_ERROR`` level. @see ``printf``,``ESP_LOGE``,``ESP_DRAM_LOGE``

/**
 * In the future, we want to become compatible with clang.
 * Hence, we provide two versions of the following macros which are using variadic arguments.
 * The first one is using the GNU extension ``##__VA_ARGS__``. The second one is using the C++20 feature ``__VA_OPT__(,)``.
 * This allows users to compile their code with standard C++20 enabled instead of the GNU extension.
 * Below C++20, we haven't found any good alternative to using ``##__VA_ARGS__``.
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
/** @cond */
#define ESP_EARLY_LOGE(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_ERROR, E __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_EARLY_LOGW(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_WARN, W __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_EARLY_LOGI(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_INFO, I __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_EARLY_LOGD(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_DEBUG, D __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_EARLY_LOGV(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_VERBOSE, V __VA_OPT__(,) __VA_ARGS__); } while(0)

#if ESP_LOG_VERSION == 1 && NON_OS_BUILD
#define ESP_LOGE(tag, format, ...) do { ESP_EARLY_LOGE(tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGW(tag, format, ...) do { ESP_EARLY_LOGW(tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGI(tag, format, ...) do { ESP_EARLY_LOGI(tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGD(tag, format, ...) do { ESP_EARLY_LOGD(tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGV(tag, format, ...) do { ESP_EARLY_LOGV(tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#else
#define ESP_LOGE(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGW(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN, tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGI(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGD(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG, tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_LOGV(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format __VA_OPT__(,) __VA_ARGS__); } while(0)
#endif

#define ESP_DRAM_LOGE(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_ERROR, E __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_DRAM_LOGW(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_WARN, W __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_DRAM_LOGI(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_INFO, I __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_DRAM_LOGD(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_DEBUG, D __VA_OPT__(,) __VA_ARGS__); } while(0)
#define ESP_DRAM_LOGV(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_VERBOSE, V __VA_OPT__(,) __VA_ARGS__); } while(0)
/** @endcond */
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
/**
 * @brief Early log macros to output logs in startup code, before heap allocator and syscalls have been initialized.
 * The log level can be changed per-tag using ``esp_log_level_set(TAG, level)``.
 */
/// macro to output logs in startup code at ``ESP_LOG_ERROR`` level.
#define ESP_EARLY_LOGE(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_ERROR, E, ##__VA_ARGS__); } while(0)
/// macro to output logs in startup code at ``ESP_LOG_WARN`` level.
#define ESP_EARLY_LOGW(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_WARN, W, ##__VA_ARGS__); } while(0)
/// macro to output logs in startup code at ``ESP_LOG_INFO`` level.
#define ESP_EARLY_LOGI(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_INFO, I, ##__VA_ARGS__); } while(0)
/// macro to output logs in startup code at ``ESP_LOG_DEBUG`` level.
#define ESP_EARLY_LOGD(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_DEBUG, D, ##__VA_ARGS__); } while(0)
/// macro to output logs in startup code at ``ESP_LOG_VERBOSE`` level.
#define ESP_EARLY_LOGV(tag, format, ...) do { ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_VERBOSE, V, ##__VA_ARGS__); } while(0)

/**
 * @brief Normal logging macros to output logs.
 * The log level can be changed per-tag using ``esp_log_level_set(TAG, level)``.
 */
#if ESP_LOG_VERSION == 1 && NON_OS_BUILD
/// macro to output logs at ``ESP_LOG_ERROR`` level.
#define ESP_LOGE(tag, format, ...) do { ESP_EARLY_LOGE(tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_WARN`` level.
#define ESP_LOGW(tag, format, ...) do { ESP_EARLY_LOGW(tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_INFO`` level.
#define ESP_LOGI(tag, format, ...) do { ESP_EARLY_LOGI(tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_DEBUG`` level.
#define ESP_LOGD(tag, format, ...) do { ESP_EARLY_LOGD(tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_VERBOSE`` level.
#define ESP_LOGV(tag, format, ...) do { ESP_EARLY_LOGV(tag, format, ##__VA_ARGS__); } while(0)
#else
/// macro to output logs at ``ESP_LOG_ERROR`` level.
#define ESP_LOGE(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_WARN`` level.
#define ESP_LOGW(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN, tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_INFO`` level.
#define ESP_LOGI(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_DEBUG`` level.
#define ESP_LOGD(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG, tag, format, ##__VA_ARGS__); } while(0)
/// macro to output logs at ``ESP_LOG_VERBOSE`` level.
#define ESP_LOGV(tag, format, ...) do { ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format, ##__VA_ARGS__); } while(0)
#endif

/**
 * @brief Macros to output logs when the cache is disabled.
 * Unlike normal logging macros, it's possible to use this macro when interrupts are disabled or inside an ISR.
 * Placing log strings in DRAM reduces available DRAM, so only use when absolutely essential.
 *
 * Usage: `ESP_DRAM_LOGE(DRAM_STR("my_tag"), "format", ...), or `ESP_DRAM_LOGE(TAG, "format", ...)`,
 * where TAG is a char* that points to a str in the DRAM.
 */
/// macro to output logs when the cache is disabled at ``ESP_LOG_ERROR`` level.
#define ESP_DRAM_LOGE(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_ERROR, E, ##__VA_ARGS__); } while(0)
/// macro to output logs when the cache is disabled at ``ESP_LOG_WARN`` level.
#define ESP_DRAM_LOGW(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_WARN, W, ##__VA_ARGS__); } while(0)
/// macro to output logs when the cache is disabled at ``ESP_LOG_INFO`` level.
#define ESP_DRAM_LOGI(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_INFO, I, ##__VA_ARGS__); } while(0)
/// macro to output logs when the cache is disabled at ``ESP_LOG_DEBUG`` level.
#define ESP_DRAM_LOGD(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_DEBUG, D, ##__VA_ARGS__); } while(0)
/// macro to output logs when the cache is disabled at ``ESP_LOG_VERBOSE`` level.
#define ESP_DRAM_LOGV(tag, format, ...) do { ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_VERBOSE, V, ##__VA_ARGS__); } while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

/// runtime macro to output logs at a specified configs. Also check the level with ``LOG_LOCAL_LEVEL``.
#if ESP_LOG_VERSION == 2
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_LOG_LEVEL_LOCAL(configs, tag, format, ...) do { if (ESP_LOG_ENABLED(configs)) { ESP_LOG_LEVEL(configs, tag, format __VA_OPT__(,) __VA_ARGS__); } } while(0)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_LOG_LEVEL_LOCAL(configs, tag, format, ...) do { if (ESP_LOG_ENABLED(configs)) { ESP_LOG_LEVEL(configs, tag, format, ##__VA_ARGS__); } } while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
#else // ESP_LOG_VERSION == 1
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_LOG_LEVEL_LOCAL(configs, tag, format, ...) do { if (_ESP_LOG_ENABLED(configs)) { ESP_LOG_LEVEL(configs, tag, format __VA_OPT__(,) __VA_ARGS__); } } while(0)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_LOG_LEVEL_LOCAL(configs, tag, format, ...) do { if (_ESP_LOG_ENABLED(configs)) { ESP_LOG_LEVEL(configs, tag, format, ##__VA_ARGS__); } } while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
#endif // ESP_LOG_VERSION == 1

/** runtime macro to output logs at a specified level and with ESP_LOG_CONFIGS_DEFAULT.
 *
 * @param configs it includes level and other log configurations.
 * @param tag tag of the log, which can be used to change the log level by ``esp_log_level_set`` at runtime.
 * @param format format of the output log. See ``printf``
 * @param ... variables to be replaced into the log. See ``printf``
 *
 * @see ``printf``
 */
#if ESP_LOG_VERSION == 2
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        esp_log(ESP_LOG_CONFIG_INIT((configs) | ESP_LOG_CONFIGS_DEFAULT), tag, ESP_LOG_ATTR_STR(format) ESP_LOG_ARGS(__VA_ARGS__)); \
    } while(0)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        esp_log(ESP_LOG_CONFIG_INIT((configs) | ESP_LOG_CONFIGS_DEFAULT), tag, ESP_LOG_ATTR_STR(format) ESP_LOG_ARGS(__VA_ARGS__)); \
    } while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

#else // ESP_LOG_VERSION == 1

#if defined(__cplusplus) && (__cplusplus >  201703L)
#if CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_ERROR)        { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR),   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_WARN)    { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_WARN),    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_DEBUG)   { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_DEBUG),   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_VERBOSE) { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_VERBOSE), tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else                                                  { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_INFO),    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
    } while(0)
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_ERROR)        { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR),   tag, LOG_SYSTEM_TIME_FORMAT(E, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_WARN)    { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_WARN),    tag, LOG_SYSTEM_TIME_FORMAT(W, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_DEBUG)   { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_DEBUG),   tag, LOG_SYSTEM_TIME_FORMAT(D, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_VERBOSE) { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_VERBOSE), tag, LOG_SYSTEM_TIME_FORMAT(V, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else                                                  { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_INFO),    tag, LOG_SYSTEM_TIME_FORMAT(I, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
    } while(0)
#elif NON_OS_BUILD
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_ERROR)        { ESP_EARLY_LOGE(tag, format __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_WARN)    { ESP_EARLY_LOGW(tag, format __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_DEBUG)   { ESP_EARLY_LOGD(tag, format __VA_OPT__(,) __VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_VERBOSE) { ESP_EARLY_LOGV(tag, format __VA_OPT__(,) __VA_ARGS__); } \
        else                                                  { ESP_EARLY_LOGI(tag, format __VA_OPT__(,) __VA_ARGS__); } \
    } while(0)
#endif //CONFIG_LOG_TIMESTAMP_SOURCE_xxx
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#if CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_ERROR)        { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR),   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_WARN)    { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_WARN),    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_DEBUG)   { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_DEBUG),   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_VERBOSE) { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_VERBOSE), tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else                                                  { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_INFO),    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
    } while(0)
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_ERROR)        { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR),   tag, LOG_SYSTEM_TIME_FORMAT(E, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_WARN)    { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_WARN),    tag, LOG_SYSTEM_TIME_FORMAT(W, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_DEBUG)   { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_DEBUG),   tag, LOG_SYSTEM_TIME_FORMAT(D, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_VERBOSE) { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_VERBOSE), tag, LOG_SYSTEM_TIME_FORMAT(V, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else                                                  { esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_INFO),    tag, LOG_SYSTEM_TIME_FORMAT(I, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
    } while(0)
#elif NON_OS_BUILD
#define ESP_LOG_LEVEL(configs, tag, format, ...) do { \
        if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_ERROR)        { ESP_EARLY_LOGE(tag, format, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_WARN)    { ESP_EARLY_LOGW(tag, format, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_DEBUG)   { ESP_EARLY_LOGD(tag, format, ##__VA_ARGS__); } \
        else if (ESP_LOG_GET_LEVEL(configs)==ESP_LOG_VERBOSE) { ESP_EARLY_LOGV(tag, format, ##__VA_ARGS__); } \
        else                                                  { ESP_EARLY_LOGI(tag, format, ##__VA_ARGS__); } \
    } while(0)
#endif //CONFIG_LOG_TIMESTAMP_SOURCE_xxx
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
#endif // ESP_LOG_VERSION == 1

/** @cond */

#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_LOG_EARLY_IMPL(tag, format, configs, log_tag_letter, ...) do { \
    if (ESP_LOG_VERSION == 1) { \
        if (_ESP_LOG_EARLY_ENABLED(configs)) { esp_rom_printf(LOG_FORMAT(log_tag_letter, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
    } else { \
        if (ESP_LOG_ENABLED(configs)) { esp_log(ESP_LOG_CONFIG_INIT((configs) | ESP_LOG_CONFIGS_DEFAULT | ESP_LOG_CONFIG_CONSTRAINED_ENV), tag, ESP_LOG_ATTR_STR(format) ESP_LOG_ARGS(__VA_ARGS__)); } \
    } } while(0)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_LOG_EARLY_IMPL(tag, format, configs, log_tag_letter, ...) do { \
    if (ESP_LOG_VERSION == 1) { \
        if (_ESP_LOG_EARLY_ENABLED(configs)) { esp_rom_printf(LOG_FORMAT(log_tag_letter, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
    } else { \
        if (ESP_LOG_ENABLED(configs)) { esp_log(ESP_LOG_CONFIG_INIT((configs) | ESP_LOG_CONFIGS_DEFAULT | ESP_LOG_CONFIG_CONSTRAINED_ENV), tag, ESP_LOG_ATTR_STR(format) ESP_LOG_ARGS(__VA_ARGS__)); } \
    } } while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_DRAM_LOG_IMPL(tag, format, configs, log_tag_letter, ...) do { \
    if (ESP_LOG_VERSION == 1) { \
        if (_ESP_LOG_EARLY_ENABLED(configs)) { esp_rom_printf(_ESP_LOG_DRAM_LOG_FORMAT(log_tag_letter, format), tag __VA_OPT__(,) __VA_ARGS__); } \
    } else { \
        if (ESP_LOG_ENABLED(configs)) { esp_log(ESP_LOG_CONFIG_INIT((configs) | ESP_LOG_CONFIGS_DEFAULT | ESP_LOG_CONFIG_CONSTRAINED_ENV | ESP_LOG_CONFIG_DIS_COLOR | ESP_LOG_CONFIG_DIS_TIMESTAMP), tag, ESP_LOG_ATTR_DRAM_STR(format) ESP_LOG_ARGS(__VA_ARGS__)); } \
    } } while(0)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_DRAM_LOG_IMPL(tag, format, configs, log_tag_letter, ...)  do { \
    if (ESP_LOG_VERSION == 1) { \
        if (_ESP_LOG_EARLY_ENABLED(configs)) { esp_rom_printf(_ESP_LOG_DRAM_LOG_FORMAT(log_tag_letter, format), tag, ##__VA_ARGS__); } \
    } else { \
        if (ESP_LOG_ENABLED(configs)) { esp_log(ESP_LOG_CONFIG_INIT((configs) | ESP_LOG_CONFIGS_DEFAULT | ESP_LOG_CONFIG_CONSTRAINED_ENV | ESP_LOG_CONFIG_DIS_COLOR | ESP_LOG_CONFIG_DIS_TIMESTAMP), tag, ESP_LOG_ATTR_DRAM_STR(format) ESP_LOG_ARGS(__VA_ARGS__)); } \
    } } while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
/** @endcond */

#ifdef __cplusplus
}
#endif
