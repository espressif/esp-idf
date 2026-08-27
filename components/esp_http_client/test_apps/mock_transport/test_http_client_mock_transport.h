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

#ifdef __cplusplus
}
#endif
