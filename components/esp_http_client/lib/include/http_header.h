/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HTTP_HEADER_H_
#define _HTTP_HEADER_H_

#include "sys/queue.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct http_header *http_header_handle_t;
typedef struct http_header_item *http_header_item_handle_t;

/**
 * @brief      initialize and allocate the memory for the header object
 *
 * @return
 *     - http_header_handle_t
 *     - NULL if any errors
 */
http_header_handle_t http_header_init(void);

/**
 * @brief      Cleanup and free all http header pairs
 *
 * @param[in]  header  The header
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t http_header_clean(http_header_handle_t header);

/**
 * @brief      Cleanup with http_header_clean and destroy http header handle object
 *
 * @param[in]  header  The header
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t http_header_destroy(http_header_handle_t header);

/**
 * @brief      Add a key-value pair of http header to the list,
 *             note that with value = NULL, this function will remove the header with `key` already exists in the list.
 *
 * @param[in]  header  The header
 * @param[in]  key     The key
 * @param[in]  value   The value
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t http_header_set(http_header_handle_t header, const char *key, const char *value);

/**
 * @brief      Sample as `http_header_set` but the value can be formated
 *
 * @param[in]  header     The header
 * @param[in]  key        The key
 * @param[in]  format     The format
 * @param[in]  ...        format parameters
 *
 * @return     Total length of value
 */
int http_header_set_format(http_header_handle_t header, const char *key, const char *format, ...);

/**
 * @brief      Get a value of header in header list
 *             The address of the value will be assign set to `value` parameter or NULL if no header with the key exists in the list
 *
 * @param[in]  header  The header
 * @param[in]  key     The key
 * @param[out] value   The value
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t http_header_get(http_header_handle_t header, const char *key, char **value);

/**
 * @brief      Create HTTP header string from the header with index, output string to buffer with buffer_len
 *             Also return the last index of header was generated
 *
 * @param[in]  header      The header
 * @param[in]  index       The index
 * @param      buffer      The buffer
 * @param      buffer_len  The buffer length
 *
 * @return     The last index of header was generated
 */
int http_header_generate_string(http_header_handle_t header, int index, char *buffer, int *buffer_len);

/**
 * @brief      Remove the header with key from the headers list
 *
 * @param[in]  header  The header
 * @param[in]  key     The key
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t http_header_delete(http_header_handle_t header, const char *key);

#ifdef __cplusplus
}
#endif

#endif
