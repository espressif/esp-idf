/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !NON_OS_BUILD || __DOXYGEN__

/**
 * @brief Logs a buffer of hexadecimal bytes at the specified log level.
 *
 * This function logs a buffer of hexadecimal bytes with 16 bytes per line. The
 * log level determines the severity of the log message.
 *
 * @note This function does not check the log level against the ESP_LOCAL_LEVEL.
 * The log level comparison should be done in esp_log.h.
 *
 * @param tag       Description tag to identify the log.
 * @param buffer    Pointer to the buffer array containing the data to be logged.
 * @param buff_len  Length of the buffer in bytes.
 * @param level     Log level indicating the severity of the log message.
*/
void esp_log_buffer_hex_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t level);

/**
 * @brief This function logs a buffer of characters with 16 characters per line.
 * The buffer should contain only printable characters. The log level determines
 * the severity of the log message.
 *
 * @note This function does not check the log level against the ESP_LOCAL_LEVEL.
 * The log level comparison should be done in esp_log.h.
 *
 * @param tag       Description tag to identify the log.
 * @param buffer    Pointer to the buffer array containing the data to be logged.
 * @param buff_len  Length of the buffer in bytes.
 * @param level     Log level indicating the severity of the log message.
*/
void esp_log_buffer_char_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t level);

/**
 * @brief This function dumps a buffer to the log in a formatted hex dump style,
 * displaying both the memory address and the corresponding hex and ASCII values
 * of the bytes. The log level determines the severity of the log message.
 *
 * @note This function does not check the log level against the ESP_LOCAL_LEVEL.
 * The log level comparison should be done in esp_log.h.
 * @note It is recommended to use terminals with a width of at least 102
 * characters to display the log dump properly.
 *
 * @param tag       Description tag to identify the log.
 * @param buffer    Pointer to the buffer array containing the data to be logged.
 * @param buff_len  Length of the buffer in bytes.
 * @param log_level Log level indicating the severity of the log message.
*/
void esp_log_buffer_hexdump_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level);

/**
 * @brief Log a buffer of hex bytes at specified level, separated into 16 bytes each line.
 *
 * The hex log shows just like the one below:
 *
 *      I (954) log_example: 54 68 65 20 77 61 79 20 74 6f 20 67 65 74 20 73
 *      I (962) log_example: 74 61 72 74 65 64 20 69 73 20 74 6f 20 71 75 69
 *      I (969) log_example: 74 20 74 61 6c 6b 69 6e 67 20 61 6e 64 20 62 65
 *      I (977) log_example: 67 69 6e 20 64 6f 69 6e 67 2e 20 2d 20 57 61 6c
 *      I (984) log_example: 74 20 44 69 73 6e 65 79 00
 *
 * @param  tag      Description tag to identify the log.
 * @param  buffer   Pointer to the buffer array containing the data to be logged.
 * @param  buff_len Length of the buffer in bytes.
 * @param  level    Log level
 */
#define ESP_LOG_BUFFER_HEX_LEVEL(tag, buffer, buff_len, level) \
    do { if (LOG_LOCAL_LEVEL >= (level)) {esp_log_buffer_hex_internal(tag, buffer, buff_len, level);} } while(0)

/**
 * @brief Log a buffer of characters at specified level, separated into 16 bytes each line. Buffer should contain only printable characters.
 *
 * The char log shows just like the one below:
 *
 *      I (980) log_example: The way to get s
 *      I (985) log_example: tarted is to qui
 *      I (989) log_example: t talking and be
 *      I (994) log_example: gin doing. - Wal
 *      I (999) log_example: t Disney
 *
 * @param  tag      Description tag to identify the log.
 * @param  buffer   Pointer to the buffer array containing the data to be logged.
 * @param  buff_len Length of the buffer in bytes.
 * @param  level    Log level.
 *
 */
#define ESP_LOG_BUFFER_CHAR_LEVEL(tag, buffer, buff_len, level) \
    do { if (LOG_LOCAL_LEVEL >= (level)) {esp_log_buffer_char_internal(tag, buffer, buff_len, level);} } while(0)

/**
 * @brief Dump a buffer to the log at specified level.
 *
 * The dump log shows just like the one below:
 *
 *      I (1013) log_example: 0x3ffb5bc0   54 68 65 20 77 61 79 20  74 6f 20 67 65 74 20 73  |The way to get s|
 *      I (1024) log_example: 0x3ffb5bd0   74 61 72 74 65 64 20 69  73 20 74 6f 20 71 75 69  |tarted is to qui|
 *      I (1034) log_example: 0x3ffb5be0   74 20 74 61 6c 6b 69 6e  67 20 61 6e 64 20 62 65  |t talking and be|
 *      I (1044) log_example: 0x3ffb5bf0   67 69 6e 20 64 6f 69 6e  67 2e 20 2d 20 57 61 6c  |gin doing. - Wal|
 *      I (1054) log_example: 0x3ffb5c00   74 20 44 69 73 6e 65 79  00                       |t Disney.|
 *
 * @note It is highly recommended to use terminals with over 102 text width.
 *
 * @param tag       Description tag to identify the log.
 * @param buffer    Pointer to the buffer array containing the data to be logged.
 * @param buff_len  Length of the buffer in bytes.
 * @param level     Log level.
 */
#define ESP_LOG_BUFFER_HEXDUMP(tag, buffer, buff_len, level) \
    do { if (LOG_LOCAL_LEVEL >= (level)) {esp_log_buffer_hexdump_internal(tag, buffer, buff_len, level);} } while(0)

/**
 * @brief Log a buffer of hex bytes at Info level
 *
 * @param  tag      Description tag to identify the log.
 * @param  buffer   Pointer to the buffer array containing the data to be logged.
 * @param  buff_len Length of the buffer in bytes.
 *
 * @see ``ESP_LOG_BUFFER_HEX_LEVEL``
 *
 */
#define ESP_LOG_BUFFER_HEX(tag, buffer, buff_len) \
    do { if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {ESP_LOG_BUFFER_HEX_LEVEL(tag, buffer, buff_len, ESP_LOG_INFO);} } while(0)

/**
 * @brief Log a buffer of characters at Info level. Buffer should contain only printable characters.
 *
 * @param  tag      Description tag to identify the log.
 * @param  buffer   Pointer to the buffer array containing the data to be logged.
 * @param  buff_len Length of the buffer in bytes.
 *
 * @see ``ESP_LOG_BUFFER_CHAR_LEVEL``
 *
 */
#define ESP_LOG_BUFFER_CHAR(tag, buffer, buff_len) \
    do { if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {ESP_LOG_BUFFER_CHAR_LEVEL(tag, buffer, buff_len, ESP_LOG_INFO);} } while(0)

/** @cond */
/**
 * @note For back compatible
 * @deprecated This function is deprecated and will be removed in the future.
 *             Please use ESP_LOG_BUFFER_HEX
 */
__attribute__((deprecated("Use 'ESP_LOG_BUFFER_HEX' instead")))
static inline void esp_log_buffer_hex(const char *tag, const void *buffer, uint16_t buff_len)
{
    ESP_LOG_BUFFER_HEX(tag, buffer, buff_len);
}

/**
 * @note For back compatible
 * @deprecated This function is deprecated and will be removed in the future.
 *             Please use ESP_LOG_BUFFER_CHAR
 */
__attribute__((deprecated("Use 'ESP_LOG_BUFFER_CHAR' instead")))
static inline void esp_log_buffer_char(const char *tag, const void *buffer, uint16_t buff_len)
{
    ESP_LOG_BUFFER_CHAR(tag, buffer, buff_len);
}
/** @endcond */

#endif // !NON_OS_BUILD || __DOXYGEN__

#ifdef __cplusplus
}
#endif
