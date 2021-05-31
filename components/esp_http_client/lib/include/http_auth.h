/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _HTTP_BASIC_AUTH_H_
#define _HTTP_BASIC_AUTH_H_

/**
 * HTTP Digest authentication data
 */
typedef struct {
    char *method;       /*!< Request method, example: GET */
    char *algorithm;    /*!< Authentication algorithm */
    char *uri;          /*!< URI of request example: /path/to/file.html */
    char *realm;        /*!< Authentication realm */
    char *nonce;        /*!< Authentication nonce */
    char *qop;          /*!< Authentication qop */
    char *opaque;       /*!< Authentication opaque */
    uint64_t cnonce;    /*!< Authentication cnonce */
    int nc;             /*!< Authentication nc */
} esp_http_auth_data_t;

/**
 * @brief      This use for Http digest authentication method, create http header for digest authentication.
 *             The returned string need to free after use
 *
 * @param[in]  username   The username
 * @param[in]  password   The password
 * @param      auth_data  The auth data
 *
 * @return
 *     - HTTP Header value of Authorization, valid for digest authentication, must be freed after usage
 *     - NULL
 */
char *http_auth_digest(const char *username, const char *password, esp_http_auth_data_t *auth_data);

/**
 * @brief      This use for Http basic authentication method, create header value for basic http authentication
 *            The returned string need to free after use
 *
 * @param[in]  username  The username
 * @param[in]  password  The password
 *
 * @return
 *     - HTTP Header value of Authorization, valid for basic authentication, must be free after use
 *     - NULL
 */
char *http_auth_basic(const char *username, const char *password);
#endif
