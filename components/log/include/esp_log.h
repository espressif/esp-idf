/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "esp_log_level.h"
#include "esp_log_color.h"
#include "esp_log_buffer.h"
#include "esp_log_timestamp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*vprintf_like_t)(const char *, va_list);

/**
 * @brief Set function used to output log entries
 *
 * By default, log output goes to UART0. This function can be used to redirect log
 * output to some other destination, such as file or network. Returns the original
 * log handler, which may be necessary to return output to the previous destination.
 *
 * @note Please note that function callback here must be re-entrant as it can be
 * invoked in parallel from multiple thread context.
 *
 * @param func new Function used for output. Must have same signature as vprintf.
 *
 * @return func old Function used for output.
 */
vprintf_like_t esp_log_set_vprintf(vprintf_like_t func);

/**
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * ESP_LOGE, ESP_LOGW, ESP_LOGI, ESP_LOGD, ESP_LOGV macros.
 *
 * This function or these macros should not be used from an interrupt.
 */
void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...) __attribute__((format(printf, 3, 4)));

/**
 * @brief Write message into the log, va_list variant
 * @see esp_log_write()
 *
 * This function is provided to ease integration toward other logging framework,
 * so that esp_log can be used as a log sink.
 */
void esp_log_writev(esp_log_level_t level, const char* tag, const char* format, va_list args);

/** @cond */

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%" PRIu32 ") %s: " format LOG_RESET_COLOR "\n"
#define LOG_SYSTEM_TIME_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%s) %s: " format LOG_RESET_COLOR "\n"

/** @endcond */

/// macro to output logs in startup code, before heap allocator and syscalls have been initialized.
/// Log at ``ESP_LOG_ERROR`` level. @see ``printf``,``ESP_LOGE``,``ESP_DRAM_LOGE``

/**
 * In the future, we want to become compatible with clang.
 * Hence, we provide two versions of the following macros which are using variadic arguments.
 * The first one is using the GNU extension \#\#__VA_ARGS__. The second one is using the C++20 feature __VA_OPT__(,).
 * This allows users to compile their code with standard C++20 enabled instead of the GNU extension.
 * Below C++20, we haven't found any good alternative to using \#\#__VA_ARGS__.
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_EARLY_LOGE( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_ERROR,   E __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_WARN`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGW( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_WARN,    W __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_INFO`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGI( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_INFO,    I __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_DEBUG`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGD( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_DEBUG,   D __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_VERBOSE`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGV( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_VERBOSE, V __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_EARLY_LOGE( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_ERROR,   E, ##__VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_WARN`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGW( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_WARN,    W, ##__VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_INFO`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGI( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_INFO,    I, ##__VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_DEBUG`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGD( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_DEBUG,   D, ##__VA_ARGS__)
/// macro to output logs in startup code at ``ESP_LOG_VERBOSE`` level.  @see ``ESP_EARLY_LOGE``,``ESP_LOGE``, ``printf``
#define ESP_EARLY_LOGV( tag, format, ... ) ESP_LOG_EARLY_IMPL(tag, format, ESP_LOG_VERBOSE, V, ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

#define ESP_LOG_EARLY_IMPL(tag, format, log_level, log_tag_letter, ...) do {                             \
        if (_ESP_LOG_EARLY_ENABLED(log_level)) {                                                         \
            esp_rom_printf(LOG_FORMAT(log_tag_letter, format), esp_log_timestamp(), tag, ##__VA_ARGS__); \
        }} while(0)

#ifndef NON_OS_BUILD
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_LOGE( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   tag, format __VA_OPT__(,) __VA_ARGS__)
#define ESP_LOGW( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    tag, format __VA_OPT__(,) __VA_ARGS__)
#define ESP_LOGI( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    tag, format __VA_OPT__(,) __VA_ARGS__)
#define ESP_LOGD( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   tag, format __VA_OPT__(,) __VA_ARGS__)
#define ESP_LOGV( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_LOGE( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   tag, format, ##__VA_ARGS__)
#define ESP_LOGW( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    tag, format, ##__VA_ARGS__)
#define ESP_LOGI( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    tag, format, ##__VA_ARGS__)
#define ESP_LOGD( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#define ESP_LOGV( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
#else

/**
 * Macro to output logs at ESP_LOG_ERROR level.
 *
 * @note This macro cannot be used when interrupts are disabled or inside an ISR. @see ``ESP_DRAM_LOGE``.
 *
 * @param tag tag of the log, which can be used to change the log level by ``esp_log_level_set`` at runtime.
 *
 * @see ``printf``
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_LOGE( tag, format, ... )  ESP_EARLY_LOGE(tag, format __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs at ``ESP_LOG_WARN`` level.  @see ``ESP_LOGE``
#define ESP_LOGW( tag, format, ... )  ESP_EARLY_LOGW(tag, format __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs at ``ESP_LOG_INFO`` level.  @see ``ESP_LOGE``
#define ESP_LOGI( tag, format, ... )  ESP_EARLY_LOGI(tag, format __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs at ``ESP_LOG_DEBUG`` level.  @see ``ESP_LOGE``
#define ESP_LOGD( tag, format, ... )  ESP_EARLY_LOGD(tag, format __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs at ``ESP_LOG_VERBOSE`` level.  @see ``ESP_LOGE``
#define ESP_LOGV( tag, format, ... )  ESP_EARLY_LOGV(tag, format __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_LOGE( tag, format, ... )  ESP_EARLY_LOGE(tag, format, ##__VA_ARGS__)
/// macro to output logs at ``ESP_LOG_WARN`` level.  @see ``ESP_LOGE``
#define ESP_LOGW( tag, format, ... )  ESP_EARLY_LOGW(tag, format, ##__VA_ARGS__)
/// macro to output logs at ``ESP_LOG_INFO`` level.  @see ``ESP_LOGE``
#define ESP_LOGI( tag, format, ... )  ESP_EARLY_LOGI(tag, format, ##__VA_ARGS__)
/// macro to output logs at ``ESP_LOG_DEBUG`` level.  @see ``ESP_LOGE``
#define ESP_LOGD( tag, format, ... )  ESP_EARLY_LOGD(tag, format, ##__VA_ARGS__)
/// macro to output logs at ``ESP_LOG_VERBOSE`` level.  @see ``ESP_LOGE``
#define ESP_LOGV( tag, format, ... )  ESP_EARLY_LOGV(tag, format, ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
#endif  // !NON_OS_BUILD

/** runtime macro to output logs at a specified level.
 *
 * @param tag tag of the log, which can be used to change the log level by ``esp_log_level_set`` at runtime.
 * @param level level of the output log.
 * @param format format of the output log. See ``printf``
 * @param ... variables to be replaced into the log. See ``printf``
 *
 * @see ``printf``
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#if CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
#define ESP_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==ESP_LOG_ERROR )          { esp_log_write(ESP_LOG_ERROR,      tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (level==ESP_LOG_WARN )      { esp_log_write(ESP_LOG_WARN,       tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (level==ESP_LOG_DEBUG )     { esp_log_write(ESP_LOG_DEBUG,      tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (level==ESP_LOG_VERBOSE )   { esp_log_write(ESP_LOG_VERBOSE,    tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else                                { esp_log_write(ESP_LOG_INFO,       tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
    } while(0)
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM
#define ESP_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==ESP_LOG_ERROR )          { esp_log_write(ESP_LOG_ERROR,      tag, LOG_SYSTEM_TIME_FORMAT(E, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (level==ESP_LOG_WARN )      { esp_log_write(ESP_LOG_WARN,       tag, LOG_SYSTEM_TIME_FORMAT(W, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (level==ESP_LOG_DEBUG )     { esp_log_write(ESP_LOG_DEBUG,      tag, LOG_SYSTEM_TIME_FORMAT(D, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else if (level==ESP_LOG_VERBOSE )   { esp_log_write(ESP_LOG_VERBOSE,    tag, LOG_SYSTEM_TIME_FORMAT(V, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
        else                                { esp_log_write(ESP_LOG_INFO,       tag, LOG_SYSTEM_TIME_FORMAT(I, format), esp_log_system_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \
    } while(0)
#endif //CONFIG_LOG_TIMESTAMP_SOURCE_xxx
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#if CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
#define ESP_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==ESP_LOG_ERROR )          { esp_log_write(ESP_LOG_ERROR,      tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==ESP_LOG_WARN )      { esp_log_write(ESP_LOG_WARN,       tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==ESP_LOG_DEBUG )     { esp_log_write(ESP_LOG_DEBUG,      tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==ESP_LOG_VERBOSE )   { esp_log_write(ESP_LOG_VERBOSE,    tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else                                { esp_log_write(ESP_LOG_INFO,       tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
    } while(0)
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM
#define ESP_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==ESP_LOG_ERROR )          { esp_log_write(ESP_LOG_ERROR,      tag, LOG_SYSTEM_TIME_FORMAT(E, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==ESP_LOG_WARN )      { esp_log_write(ESP_LOG_WARN,       tag, LOG_SYSTEM_TIME_FORMAT(W, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==ESP_LOG_DEBUG )     { esp_log_write(ESP_LOG_DEBUG,      tag, LOG_SYSTEM_TIME_FORMAT(D, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==ESP_LOG_VERBOSE )   { esp_log_write(ESP_LOG_VERBOSE,    tag, LOG_SYSTEM_TIME_FORMAT(V, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
        else                                { esp_log_write(ESP_LOG_INFO,       tag, LOG_SYSTEM_TIME_FORMAT(I, format), esp_log_system_timestamp(), tag, ##__VA_ARGS__); } \
    } while(0)
#endif //CONFIG_LOG_TIMESTAMP_SOURCE_xxx
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

/** runtime macro to output logs at a specified level. Also check the level with ``LOG_LOCAL_LEVEL``.
 * If ``CONFIG_LOG_MASTER_LEVEL`` set, also check first against ``esp_log_get_level_master()``.
 *
 * @see ``printf``, ``ESP_LOG_LEVEL``
 */
#define ESP_LOG_LEVEL_LOCAL(level, tag, format, ...) do {               \
        if (_ESP_LOG_ENABLED(level)) ESP_LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
    } while(0)

/**
 * @brief Macro to output logs when the cache is disabled. Log at ``ESP_LOG_ERROR`` level.
 *
 * @note Unlike normal logging macros, it's possible to use this macro when interrupts are
 * disabled or inside an ISR.
 *
 * Similar to @see ``ESP_EARLY_LOGE``, the log level cannot be changed per-tag, however
 * esp_log_level_set("*", level) will set the default level which controls these log lines also.
 *
 * Usage: `ESP_DRAM_LOGE(DRAM_STR("my_tag"), "format", or `ESP_DRAM_LOGE(TAG, "format", ...)`,
 * where TAG is a char* that points to a str in the DRAM.
 *
 * @note Placing log strings in DRAM reduces available DRAM, so only use when absolutely essential.
 *
 * @see ``esp_rom_printf``,``ESP_LOGE``
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_DRAM_LOGE( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_ERROR,   E __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_WARN`` level.  @see ``ESP_DRAM_LOGW``,``ESP_LOGW``, ``esp_rom_printf``
#define ESP_DRAM_LOGW( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_WARN,    W __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_INFO`` level.  @see ``ESP_DRAM_LOGI``,``ESP_LOGI``, ``esp_rom_printf``
#define ESP_DRAM_LOGI( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_INFO,    I __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_DEBUG`` level.  @see ``ESP_DRAM_LOGD``,``ESP_LOGD``, ``esp_rom_printf``
#define ESP_DRAM_LOGD( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_DEBUG,   D __VA_OPT__(,) __VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_VERBOSE`` level.  @see ``ESP_DRAM_LOGV``,``ESP_LOGV``, ``esp_rom_printf``
#define ESP_DRAM_LOGV( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_VERBOSE, V __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_DRAM_LOGE( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_ERROR,   E, ##__VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_WARN`` level.  @see ``ESP_DRAM_LOGW``,``ESP_LOGW``, ``esp_rom_printf``
#define ESP_DRAM_LOGW( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_WARN,    W, ##__VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_INFO`` level.  @see ``ESP_DRAM_LOGI``,``ESP_LOGI``, ``esp_rom_printf``
#define ESP_DRAM_LOGI( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_INFO,    I, ##__VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_DEBUG`` level.  @see ``ESP_DRAM_LOGD``,``ESP_LOGD``, ``esp_rom_printf``
#define ESP_DRAM_LOGD( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_DEBUG,   D, ##__VA_ARGS__)
/// macro to output logs when the cache is disabled at ``ESP_LOG_VERBOSE`` level.  @see ``ESP_DRAM_LOGV``,``ESP_LOGV``, ``esp_rom_printf``
#define ESP_DRAM_LOGV( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_VERBOSE, V, ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

/** @cond */
#define _ESP_LOG_DRAM_LOG_FORMAT(letter, format)  DRAM_STR(#letter " %s: " format "\n")

#if defined(__cplusplus) && (__cplusplus >  201703L)
#define ESP_DRAM_LOG_IMPL(tag, format, log_level, log_tag_letter, ...) do {                                  \
        if (_ESP_LOG_EARLY_ENABLED(log_level)) {                                                             \
            esp_rom_printf(_ESP_LOG_DRAM_LOG_FORMAT(log_tag_letter, format), tag __VA_OPT__(,) __VA_ARGS__); \
        }} while(0)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define ESP_DRAM_LOG_IMPL(tag, format, log_level, log_tag_letter, ...) do {                       \
        if (_ESP_LOG_EARLY_ENABLED(log_level)) {                                                  \
            esp_rom_printf(_ESP_LOG_DRAM_LOG_FORMAT(log_tag_letter, format), tag, ##__VA_ARGS__); \
        }} while(0)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
/** @endcond */

#ifdef __cplusplus
}
#endif
