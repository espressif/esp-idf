/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert an unsigned integer value to a string representation in the specified radix.
 *
 * This function converts the given unsigned integer value to a string representation in the specified radix.
 * The resulting string is stored in the provided character buffer `buf`.
 *
 * @param[in] val    The unsigned integer value to be converted.
 * @param[in] radix  The base of the numeral system to be used for the conversion.
 *                   It determines the number of unique digits in the numeral system
 *                   (e.g., 2 for binary, 10 for decimal, 16 for hexadecimal).
 * @param[in] pad    The optional padding width (0 - unused) for the resulting string. It adds zero-padding.
 *                   (val=123, pad=6 -> result=000123).
 * @param[in] digits Pointer to a character array representing the digits of the
 *                   numeral system. The array must contain characters in the order of increasing
 *                   values, corresponding to the digits of the radix. For example, "0123456789ABCDEF"
 *                   or hexadecimal.
 * @param[out] buf   Pointer to the character buffer where the resulting string will
 *                   be stored. The buffer must have enough space to accommodate the entire converted
 *                   string, including the null-terminator.
 *
 * @return The length of the resulting string (excluding the null-terminator).
 *
 * @note The buffer `buf` must have sufficient space to hold the entire converted string, including the null-terminator.
 *       The caller is responsible for ensuring the buffer's size is large enough to prevent buffer overflow.
 * @note The provided `digits` array must have enough elements to cover the entire radix used for conversion. Otherwise, undefined behavior may occur.
 */
int esp_log_util_cvt(unsigned long long val, long radix, int pad, const char *digits, char *buf);

/**
 * @brief Convert an unsigned integer to a hexadecimal string with optional padding.
 *
 * This function converts an unsigned integer value to a hexadecimal string representation.
 * This function calls esp_log_util_cvt(val, 16, pad, "0123456789abcdef", buf) inside.
 *
 * @param val The unsigned integer value to be converted.
 * @param pad The optional padding width for the resulting string.
 * @param buf The buffer to store the hexadecimal string.
 * @return The length of the converted string.
 */
int esp_log_util_cvt_hex(unsigned long long val, int pad, char *buf);

/**
 * @brief Convert an unsigned integer to a decimal string with optional padding.
 *
 * This function converts an unsigned integer value to a decimal string representation.
 * This function calls esp_log_util_cvt(val, 10, pad, "0123456789", buf) inside.
 *
 * @param val The unsigned integer value to be converted.
 * @param pad The optional padding width for the resulting string.
 * @param buf The buffer to store the decimal string.
 * @return The length of the converted string.
 */
int esp_log_util_cvt_dec(unsigned long long val, int pad, char *buf);

/**
 * @typedef esp_log_cache_enabled_t
 * @brief Callback function type for checking the state of the SPI flash cache.
 *
 * This function pointer is used to determine whether the SPI flash cache is enabled
 * during logging operations.
 *
 * @return
 *      - true if the SPI flash cache is enabled.
 *      - false if the SPI flash cache is disabled.
 */
typedef bool (*esp_log_cache_enabled_t)(void);

/**
 * @brief Sets the callback function to check the SPI flash cache state.
 *
 * This function allows setting a custom callback to check whether the SPI flash
 * cache is enabled. If a callback is provided, it will be used during logging
 * operations to ensure that logging does not interfere with cache-disabled scenarios.
 *
 * @note This function must be called during system startup to initialize it.
 *
 * @param[in] func Pointer to the callback function of type `esp_log_cache_enabled_t`.
 *                 Pass `NULL` to disable the cache check.
 */
void esp_log_util_set_cache_enabled_cb(esp_log_cache_enabled_t func);

/**
 * @brief Check if the current context is constrained.
 *
 * This function checks if logging in the current context is doing from:
 * - ISR context.
 * - Disabled SPI flash cache.
 * - Task scheduler not running.
 *
 * @return true if the context is constrained, false otherwise.
 */
bool esp_log_util_is_constrained(void);

#ifdef __cplusplus
}
#endif
