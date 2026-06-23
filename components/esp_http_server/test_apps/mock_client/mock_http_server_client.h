/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file mock_http_server_client.h
 * @brief Scripted TCP client for black-box testing of esp_http_server.
 *
 * Allows tests to send arbitrary (including malformed or fragmented) byte
 * sequences to a running httpd instance over loopback and capture the raw
 * response. The server under test is started/stopped by the test itself using
 * the normal httpd_start() / httpd_stop() APIs; this module only provides the
 * "client" side.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum number of response bytes the mock client will buffer. */
#define MOCK_SERVER_CLIENT_RESP_BUF_SIZE  4096

/**
 * @brief Describes a single scripted request the mock client sends.
 *
 * Only @p data is required. All other fields have safe defaults when zero.
 */
typedef struct {
    const char *data;             /**< Raw bytes to send (must not be NULL) */
    size_t      len;              /**< Length of @p data; 0 → strlen(data) */

    /** Maximum bytes per write() call. -1 (or 0) = send all at once.
     *  Set to a small value to simulate a slow/trickled sender. */
    int         max_bytes_per_write;

    /** Delay in milliseconds between fragmented writes. 0 = no delay. */
    int         write_delay_ms;

    /** Close the TCP write-side after sending this many bytes.
     *  0 (default) or negative = send everything, keep write-side open.
     *  Positive N = close after N bytes (useful for partial-request / abrupt-close tests). */
    int         close_after_bytes;

    /** How long (ms) to wait for a response before giving up. 0 → 2000 ms. */
    int         recv_timeout_ms;
} mock_server_request_t;

/**
 * @brief Result captured from the server after sending a scripted request.
 */
typedef struct {
    char   data[MOCK_SERVER_CLIENT_RESP_BUF_SIZE]; /**< Raw response bytes */
    size_t len;                                     /**< Bytes actually received */
    int    status_code;    /**< HTTP status code parsed from the status line,
                            *   or -1 if the response is not a valid HTTP response. */
    bool   server_closed;  /**< True if the server closed the connection (recv returned 0). */
} mock_server_response_t;

/**
 * @brief Connect to a running httpd on 127.0.0.1:@p port, send the scripted
 *        request and capture the response.
 *
 * The response struct is heap-allocated; the caller must free it with
 * mock_server_response_free() when done.  The TCP connection is always
 * closed before returning.
 *
 * @param[in] port  TCP port the httpd server is listening on.
 * @param[in] req   Scripted request to send.
 *
 * @return  Heap-allocated response, or NULL if a TCP-level error occurred.
 */
mock_server_response_t *mock_server_send_request(uint16_t port,
                                                  const mock_server_request_t *req);

/**
 * @brief Free a response returned by mock_server_send_request().
 */
void mock_server_response_free(mock_server_response_t *resp);

/**
 * @brief Assert that @p resp contains a valid HTTP response with the expected
 *        status code.  Calls TEST_FAIL() (Unity) if the assertion does not hold.
 */
void mock_server_assert_status(const mock_server_response_t *resp, int expected_status);

#ifdef __cplusplus
}
#endif
