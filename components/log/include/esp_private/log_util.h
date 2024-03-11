/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert an unsigned integer value to a string representation in the specified radix.
 *
 * This function converts the given unsigned integer value to a string representation in the specified radix.
 * The resulting string is stored in the provided character buffer `buf`.
 *
 * @param val   The unsigned integer value to be converted.
 * @param buf   Pointer to the character buffer where the resulting string will be stored.
 *              The buffer must have enough space to accommodate the entire converted string,
 *              including the null-terminator.
 * @param radix The base of the numeral system to be used for the conversion.
 *              It determines the number of unique digits in the numeral system
 *              (e.g., 2 for binary, 10 for decimal, 16 for hexadecimal).
 * @param digits Pointer to a character array representing the digits of the numeral system.
 *              The array must contain characters in the order of increasing values,
 *              corresponding to the digits of the radix. For example, "0123456789ABCDEF" for hexadecimal.
 *
 * @return The length of the resulting string (excluding the null-terminator).
 *
 * @note The buffer `buf` must have sufficient space to hold the entire converted string, including the null-terminator.
 *       The caller is responsible for ensuring the buffer's size is large enough to prevent buffer overflow.
 * @note The provided `digits` array must have enough elements to cover the entire radix used for conversion. Otherwise, undefined behavior may occur.
 */

int esp_log_util_cvt(unsigned long long val, char *buf, long radix, const char *digits);

/**
 * @brief Convert an unsigned integer to a hexadecimal string with optional padding.
 *
 * This function converts an unsigned integer value to a hexadecimal string representation.
 * This function calls esp_log_util_cvt(val, buf, 16, pad, "0123456789abcdef") inside.
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
 * This function calls esp_log_util_cvt(val, buf, 10, pad, "0123456789") inside.
 *
 * @param val The unsigned integer value to be converted.
 * @param pad The optional padding width for the resulting string.
 * @param buf The buffer to store the decimal string.
 * @return The length of the converted string.
 */
int esp_log_util_cvt_dec(unsigned long long val, int pad, char *buf);

#ifdef __cplusplus
}
#endif
