/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _HTTP_UTILS_H_
#define _HTTP_UTILS_H_
#include <sys/time.h>

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 * It logs the message in debug mode.
 */
#define HTTP_RET_ON_ERR_DBG(x, log_tag, format, ...) do {                                       \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_LOGD(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns with the supplied 'err_code'.
 * It logs the message in debug mode.
 */
#define HTTP_RET_ON_FALSE_DBG(a, err_code, log_tag, format, ...) do {                           \
        if (unlikely(!(a))) {                                                                   \
            ESP_LOGD(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message,
 * sets the local variable 'ret' to the supplied 'err_code', and then exits by jumping to 'goto_tag'.
 * It logs the message in debug mode.
 */
#define HTTP_GOTO_ON_FALSE_DBG(a, err_code, goto_tag, log_tag, format, ...) do {                \
        if (unlikely(!(a))) {                                                                   \
            ESP_LOGD(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

/**
 * @brief      Assign new_str to *str pointer, and realloc *str if it not NULL
 *
 * @param      str      pointer to string pointer
 * @param      new_str  assign this string to str
 * @param      len      length of string, less than 0 if new_str is zero terminated
 *
 * @return
 *  - new_str pointer
 *  - NULL
 */
char *http_utils_assign_string(char **str, const char *new_str, int len);

/**
 * @brief      Realloc *str and append new_str to it if new_str is not NULL; return *str pointer if new_str is NULL
 *
 * @param      str      pointer to string pointer
 * @param      new_str  append this string to str
 * @param      len      length of string, less than 0 if new_str is zero terminated
 *
 * @return
 *  - *str pointer
 */
char *http_utils_append_string(char **str, const char *new_str, int len);

/**
 * @brief      Remove white space at begin and end of string
 *
 * @param[in]  str   The string
 *
 * @return     New strings have been trimmed
 */
void http_utils_trim_whitespace(char **str);

/**
 * @brief      Extracts the substring between two specified delimiters.
 *             Allocates memory for the extracted substring and stores it in `out`.
 *             The caller must free the allocated memory when no longer needed.
 *
 * @param[in]  str    The source string to search.
 * @param[in]  begin  The starting delimiter string.
 * @param[in]  end    The ending delimiter string.
 * @param[out] out    Pointer to store the allocated substring. Set to NULL if the substring is not found.
 *
 * @return
 * - ESP_OK: Operation succeeded (even if no substring is found).
 * - ESP_ERR_NO_MEM: Memory allocation failed.
 */
esp_err_t http_utils_get_string_between(const char *str, const char *begin, const char *end, char **out);

/**
 * @brief      Extracts the substring starting after a specified delimiter until the end of the source string.
 *             Allocates memory for the extracted substring and stores it in `out`.
 *             The caller must free the allocated memory when no longer needed.
 *
 * @param[in]  str    The source string to search.
 * @param[in]  begin  The delimiter string to search for.
 * @param[out] out    Pointer to store the allocated substring. Set to NULL if the substring is not found.
 *
 * @return
 * - ESP_OK: Operation succeeded (even if no substring is found).
 * - ESP_ERR_NO_MEM: Memory allocation failed.
 */
esp_err_t http_utils_get_string_after(const char *str, const char *begin, char **out);

/**
 * @brief      Join 2 strings to one
 *             It will allocate a new memory space for this string, so you need to free it when no longer use
 *
 * @param[in]  first_str   The first string
 * @param[in]  len_first   The length first
 * @param[in]  second_str  The second string
 * @param[in]  len_second  The length second
 *
 * @return
 * - New string pointer
 * - NULL: Invalid input
 */
char *http_utils_join_string(const char *first_str, size_t len_first, const char *second_str, size_t len_second);

/**
 * @brief      Check if ``str`` is start with ``start``
 *
 * @param[in]  str    The string
 * @param[in]  start  The start
 *
 * @return
 *     - (-1) if length of ``start`` larger than length of ``str``
 *     - (1) if ``start`` NOT starts with ``start``
 *     - (0) if ``str`` starts with ``start``
 */
int http_utils_str_starts_with(const char *str, const char *start);

#endif
