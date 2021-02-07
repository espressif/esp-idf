// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef _HTTP_UTILS_H_
#define _HTTP_UTILS_H_
#include <sys/time.h>

/**
 * @brief      Assign new_str to *str pointer, and realloc *str if it not NULL
 *
 * @param      str      pointer to string pointer
 * @param      new_str  assign this tring to str
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
 * @brief      Gets the string between 2 string.
 *             It will allocate a new memory space for this string, so you need to free it when no longer use
 *
 * @param[in]  str    The source string
 * @param[in]  begin  The begin string
 * @param[in]  end    The end string
 *
 * @return     The string between begin and end
 */
char *http_utils_get_string_between(const char *str, const char *begin, const char *end);

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
char *http_utils_join_string(const char *first_str, int len_first, const char *second_str, int len_second);

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


#define HTTP_MEM_CHECK(TAG, a, action) if (!(a)) {                                                  \
        ESP_LOGE(TAG,"%s:%d (%s): %s", __FILE__, __LINE__, __FUNCTION__, "Memory exhausted");       \
        action;                                                                                     \
        }

#endif
