/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_transport.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mock transport operation modes for testing error conditions
 */
typedef enum {
    MOCK_TRANSPORT_MODE_NORMAL,              /*!< Normal operation - successful read/write */
    MOCK_TRANSPORT_MODE_CONNECT_FAIL,        /*!< Connection fails */
    MOCK_TRANSPORT_MODE_READ_TIMEOUT,        /*!< Read operation times out */
    MOCK_TRANSPORT_MODE_WRITE_FAIL,          /*!< Write operation fails */
    MOCK_TRANSPORT_MODE_INCOMPLETE_READ,     /*!< Connection closes mid-read (incomplete data) */
    MOCK_TRANSPORT_MODE_WRITE_PARTIAL,       /*!< Write only partial data (simulates buffer full) */
} mock_transport_mode_t;

/**
 * @brief Configuration for mock HTTP transport
 */
typedef struct {
    mock_transport_mode_t mode;              /*!< Operation mode for error injection */
    const char *response_data;               /*!< Canned HTTP response data to return on read */
    size_t response_len;                     /*!< Length of response data (0 = use strlen) */
    int bytes_before_error;                  /*!< Number of bytes to process before injecting error (-1 = no limit) */
    int connect_delay_ms;                    /*!< Delay before connect succeeds (0 = immediate) */
    bool track_calls;                        /*!< Enable call tracking for verification */
    int async_connect_polls;                 /*!< connect_async returns "in progress" this many times, then succeeds */
    int would_block_reads;                   /*!< First N reads return -1 with errno = EAGAIN */
    int would_block_writes;                  /*!< First N writes return -1 with errno = EAGAIN */
    int read_bytes_before_error;             /*!< READ-side-only error budget for MOCK_TRANSPORT_MODE_INCOMPLETE_READ:
                                                   counts only bytes actually delivered via mock_read(), independent of
                                                   mock_write()'s byte count, so truncation can be pinned to an offset
                                                   inside the response body regardless of request size. -1 (default)
                                                   disables this and falls back to the shared bytes_before_error/
                                                   bytes_processed counter used by the original tests. */
    int max_write_chunk;                     /*!< If > 0, caps every mock_write() call's accepted byte count to at
                                                   most this many bytes, independent of `mode`. Simulates a transport
                                                   whose per-call send buffer is smaller than the caller's write,
                                                   forcing the client's short-write retry loop (esp_http_client_write()
                                                   / esp_http_client_request_send()) to run across multiple calls.
                                                   Added because MOCK_TRANSPORT_MODE_WRITE_PARTIAL's existing
                                                   bytes_before_error budget only splits a write when the budget lands
                                                   inside that specific call, which is brittle (coupled to exact
                                                   serialized header length); this field reliably forces a split on
                                                   every call regardless of request size. 0 (default) = no cap, and
                                                   existing tests are unaffected. */
} mock_http_transport_config_t;

/**
 * @brief Default configuration initializer
 */
#define MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG() { \
    .mode = MOCK_TRANSPORT_MODE_NORMAL, \
    .response_data = NULL, \
    .response_len = 0, \
    .bytes_before_error = -1, \
    .connect_delay_ms = 0, \
    .track_calls = true, \
    .async_connect_polls = 0, \
    .would_block_reads = 0, \
    .would_block_writes = 0, \
    .read_bytes_before_error = -1, \
    .max_write_chunk = 0, \
}

/**
 * @brief Statistics tracked by mock transport
 */
typedef struct {
    int connect_calls;                       /*!< Number of connect() calls */
    int read_calls;                          /*!< Number of read() calls */
    int write_calls;                         /*!< Number of write() calls */
    int close_calls;                         /*!< Number of close() calls */
    int poll_read_calls;                     /*!< Number of poll_read() calls */
    int poll_write_calls;                    /*!< Number of poll_write() calls */
    size_t total_bytes_written;              /*!< Total bytes written */
    size_t total_bytes_read;                 /*!< Total bytes read */
} mock_http_transport_stats_t;

/**
 * @brief Create a mock HTTP transport handle
 *
 * Creates a transport handle that can be used with esp_http_client for testing.
 * The transport simulates network behavior according to the configuration.
 *
 * @param[in] config Configuration for mock behavior (can be NULL for defaults)
 *
 * @return
 *     - Mock transport handle on success
 *     - NULL on error (memory allocation failure)
 *
 * @note The returned handle must be destroyed with mock_http_transport_destroy()
 */
esp_transport_handle_t mock_http_transport_create(const mock_http_transport_config_t *config);

/**
 * @brief Destroy mock HTTP transport
 *
 * Frees all resources associated with the mock transport.
 *
 * @param[in] transport Mock transport handle
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if transport is NULL
 */
esp_err_t mock_http_transport_destroy(esp_transport_handle_t transport);

/**
 * @brief Update mock transport configuration at runtime
 *
 * Allows changing the mock behavior between requests without recreating the transport.
 * This is useful for testing client reuse after errors.
 *
 * @param[in] transport Mock transport handle
 * @param[in] config New configuration
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if transport is NULL or not a mock transport
 */
esp_err_t mock_http_transport_set_config(esp_transport_handle_t transport,
                                          const mock_http_transport_config_t *config);

/**
 * @brief Get statistics from mock transport
 *
 * Retrieves call counts and byte counters for verification in tests.
 *
 * @param[in] transport Mock transport handle
 * @param[out] stats Statistics structure to fill
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if transport or stats is NULL
 */
esp_err_t mock_http_transport_get_stats(esp_transport_handle_t transport,
                                         mock_http_transport_stats_t *stats);

/**
 * @brief Reset statistics counters
 *
 * Clears all call counters and byte counters. Useful between test iterations.
 *
 * @param[in] transport Mock transport handle
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if transport is NULL
 */
esp_err_t mock_http_transport_reset_stats(esp_transport_handle_t transport);

/**
 * @brief Set canned response data
 *
 * Convenience function to update just the response data without changing other config.
 *
 * @param[in] transport Mock transport handle
 * @param[in] response_data HTTP response to return on read (can be NULL to clear)
 * @param[in] response_len Length of response (0 = use strlen)
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if transport is NULL
 */
esp_err_t mock_http_transport_set_response(esp_transport_handle_t transport,
                                            const char *response_data,
                                            size_t response_len);

/**
 * @brief Queue a response to be served after the next request boundary
 *
 * Appends to a FIFO of up to 8 responses. When mock_write() detects a request
 * boundary (the previously active response has been fully read and a new
 * write arrives), the next queued response is popped into the active buffer
 * via mock_http_transport_set_response(). Useful for testing keep-alive
 * request/response sequences on a single connection.
 *
 * @param[in] t Mock transport handle
 * @param[in] data Response data to queue (caller-owned, must outlive its use - e.g. a string literal)
 * @param[in] len Length of data (0 = use strlen)
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if t is NULL, data is NULL, or the queue is full (8 entries)
 */
esp_err_t mock_http_transport_queue_response(esp_transport_handle_t t,
                                              const char *data, size_t len);

/**
 * @brief Retrieve the bytes captured from the most recent request
 *
 * mock_write() appends every written byte (capped at 2048 bytes) into an
 * internal capture buffer. The buffer is reset only on a queue-advance: a
 * write arriving after the current response has been fully read AND a
 * queued response is still pending (see mock_http_transport_queue_response()).
 * With the queue empty or exhausted, no reset happens and the capture
 * concatenates bytes across requests. This lets tests assert on the
 * serialized request content (headers, body).
 *
 * @param[in] t Mock transport handle
 * @param[out] buf Buffer to receive the captured request bytes, NUL-terminated
 * @param[in] buf_len Size of buf (including space for the NUL terminator)
 * @param[out] out_len Optional, number of bytes copied (excluding NUL terminator)
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if t is NULL, buf is NULL, or buf_len is 0
 */
esp_err_t mock_http_transport_get_last_request(esp_transport_handle_t t,
                                                char *buf, size_t buf_len, size_t *out_len);

#ifdef __cplusplus
}
#endif
