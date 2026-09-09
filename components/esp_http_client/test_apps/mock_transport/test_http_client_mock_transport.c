/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_http_client_mock_transport.h"
#include "esp_log.h"
#include <string.h>
#include <errno.h>
#include <sys/time.h>

static const char *TAG = "mock_transport";

/**
 * @brief Internal context for mock transport
 */
typedef struct {
    mock_http_transport_config_t config;     /*!< Current configuration */
    mock_http_transport_stats_t stats;       /*!< Call statistics */
    bool is_connected;                       /*!< Connection state */
    size_t read_offset;                      /*!< Current position in response data */
    size_t bytes_processed;                  /*!< Bytes processed (for error injection) */
    size_t read_only_bytes;                  /*!< Bytes delivered via mock_read() alone (for
                                                   read_bytes_before_error injection, independent
                                                   of bytes written via mock_write()) */
    char *response_buffer;                   /*!< Internal copy of response data */
    int async_polls_left;                    /*!< Remaining "in progress" returns from mock_connect_async() */
    int wb_reads_left;                       /*!< Remaining EAGAIN injections for mock_read() */
    int wb_writes_left;                      /*!< Remaining EAGAIN injections for mock_write() */
    const char *resp_queue[8];               /*!< FIFO of queued response pointers (caller-owned) */
    size_t resp_queue_len[8];                /*!< Lengths matching resp_queue entries */
    int resp_queue_count;                    /*!< Number of entries queued */
    int resp_queue_next;                     /*!< Index of the next entry to pop */
    char req_capture[2048];                  /*!< Bytes written since the last request boundary */
    size_t req_capture_len;                  /*!< Number of valid bytes in req_capture */
} mock_http_transport_ctx_t;

// Forward declarations of transport function implementations
static int mock_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms);
static int mock_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms);
static int mock_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms);
static int mock_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms);
static int mock_close(esp_transport_handle_t t);
static int mock_poll_read(esp_transport_handle_t t, int timeout_ms);
static int mock_poll_write(esp_transport_handle_t t, int timeout_ms);
static int mock_destroy(esp_transport_handle_t t);

/**
 * @brief Simulate delay (for timeouts and connection delays)
 */
static void simulate_delay_ms(int delay_ms)
{
    if (delay_ms <= 0) {
        return;
    }

    struct timeval tv;
    tv.tv_sec = delay_ms / 1000;
    tv.tv_usec = (delay_ms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
}

/**
 * @brief Check if error should be injected based on bytes processed
 */
static bool should_inject_error(mock_http_transport_ctx_t *ctx, size_t bytes_about_to_process)
{
    if (ctx->config.bytes_before_error < 0) {
        return false;  // No error injection configured
    }

    return (ctx->bytes_processed + bytes_about_to_process) > (size_t)ctx->config.bytes_before_error;
}

/**
 * @brief Check if a READ-side-only error should be injected, based on bytes
 *        delivered via mock_read() alone (ignores mock_write() entirely)
 */
static bool should_inject_read_error(mock_http_transport_ctx_t *ctx, size_t bytes_about_to_process)
{
    if (ctx->config.read_bytes_before_error < 0) {
        return false;  // Read-side budget not configured
    }

    return (ctx->read_only_bytes + bytes_about_to_process) > (size_t)ctx->config.read_bytes_before_error;
}

/**
 * @brief Mock connect implementation
 */
static int mock_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx) {
        ESP_LOGE(TAG, "Invalid transport context");
        errno = EINVAL;
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.connect_calls++;
    }

    ESP_LOGD(TAG, "Mock connect to %s:%d (mode=%d)", host, port, ctx->config.mode);

    // Simulate connection delay
    if (ctx->config.connect_delay_ms > 0) {
        simulate_delay_ms(ctx->config.connect_delay_ms);
    }

    // Handle connect failure mode
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_CONNECT_FAIL) {
        ESP_LOGD(TAG, "Mock connect failed (simulated)");
        errno = ECONNREFUSED;
        return -1;
    }

    // Success
    ctx->is_connected = true;
    ctx->read_offset = 0;
    ctx->bytes_processed = 0;
    ctx->read_only_bytes = 0;

    ESP_LOGI(TAG, "Mock connect succeeded");
    return 0;
}

/**
 * @brief Mock async connect implementation
 *
 * Simulates a non-blocking connect(): returns "in progress" for
 * `async_connect_polls` calls, then reports success. Honors
 * MOCK_TRANSPORT_MODE_CONNECT_FAIL to simulate an async connect failure.
 */
static int mock_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx) {
        ESP_LOGE(TAG, "Invalid transport context");
        errno = EINVAL;
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.connect_calls++;
    }

    if (ctx->config.mode == MOCK_TRANSPORT_MODE_CONNECT_FAIL) {
        ESP_LOGD(TAG, "Mock connect_async failed (simulated)");
        return -1; /* ASYNC_TRANS_CONNECT_FAIL */
    }

    if (ctx->async_polls_left > 0) {
        ctx->async_polls_left--;
        ESP_LOGD(TAG, "Mock connect_async: still connecting (%d polls left)", ctx->async_polls_left);
        return 0;  /* ASYNC_TRANS_CONNECTING */
    }

    ctx->is_connected = true;
    ctx->read_offset = 0;
    ctx->bytes_processed = 0;
    ctx->read_only_bytes = 0;

    ESP_LOGI(TAG, "Mock connect_async succeeded");
    return 1;      /* ASYNC_TRANS_CONNECT_PASS */
}

/**
 * @brief Mock read implementation
 */
static int mock_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx || !buffer || len <= 0) {
        errno = EINVAL;
        return -1;
    }

    // Would-block injection: simulate a non-blocking socket returning EAGAIN
    if (ctx->wb_reads_left > 0) {
        ctx->wb_reads_left--;
        ESP_LOGD(TAG, "Mock read: EAGAIN (simulated would-block, %d left)", ctx->wb_reads_left);
        errno = EAGAIN;
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.read_calls++;
    }

    if (!ctx->is_connected) {
        ESP_LOGD(TAG, "Mock read: not connected");
        errno = ENOTCONN;
        return -1;
    }

    ESP_LOGD(TAG, "Mock read: requested %d bytes (mode=%d)", len, ctx->config.mode);

    // Handle read timeout mode
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_READ_TIMEOUT) {
        ESP_LOGD(TAG, "Mock read: timeout (simulated)");
        errno = ETIMEDOUT;
        return -1;
    }

    // If no response data configured, return 0 (connection closed)
    if (!ctx->response_buffer || ctx->config.response_len == 0) {
        ESP_LOGD(TAG, "Mock read: no data available (EOF)");
        return 0;
    }

    // Calculate available data
    size_t remaining = ctx->config.response_len - ctx->read_offset;
    if (remaining == 0) {
        ESP_LOGD(TAG, "Mock read: all data consumed (EOF)");
        return 0;  // All data consumed
    }

    // Determine how much to read
    size_t to_read = (len < remaining) ? len : remaining;

    // Handle incomplete read mode (close connection mid-stream).
    // read_bytes_before_error, when set (>= 0), is a READ-side-only budget:
    // it is checked against bytes delivered via mock_read() alone, so
    // truncation lands at a byte offset inside the response body
    // regardless of how many bytes the request write consumed. When unset
    // (-1, the default), falls back to the original shared bytes_processed
    // counter (also incremented by mock_write()) used by earlier tests.
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_INCOMPLETE_READ) {
        bool use_read_budget = (ctx->config.read_bytes_before_error >= 0);
        bool inject = use_read_budget ? should_inject_read_error(ctx, to_read)
                                       : should_inject_error(ctx, to_read);
        if (inject) {
            size_t budget = use_read_budget ? (size_t)ctx->config.read_bytes_before_error
                                             : (size_t)ctx->config.bytes_before_error;
            size_t processed = use_read_budget ? ctx->read_only_bytes : ctx->bytes_processed;
            // Read partial data then close connection
            size_t partial = budget - processed;
            if (partial > 0 && partial < to_read) {
                memcpy(buffer, ctx->response_buffer + ctx->read_offset, partial);
                ctx->read_offset += partial;
                ctx->bytes_processed += partial;
                ctx->read_only_bytes += partial;
                ctx->stats.total_bytes_read += partial;
                ESP_LOGD(TAG, "Mock read: incomplete data %zu bytes, then EOF", partial);
                return partial;
            }
            // Connection closed
            ctx->is_connected = false;
            ESP_LOGD(TAG, "Mock read: connection closed (incomplete data)");
            return 0;
        }
    }

    // Normal read
    memcpy(buffer, ctx->response_buffer + ctx->read_offset, to_read);
    ctx->read_offset += to_read;
    ctx->bytes_processed += to_read;
    ctx->read_only_bytes += to_read;

    if (ctx->config.track_calls) {
        ctx->stats.total_bytes_read += to_read;
    }

    ESP_LOGI(TAG, "Mock read: returned %zu bytes (offset now=%zu, remaining=%zu)",
             to_read, ctx->read_offset, ctx->config.response_len - ctx->read_offset);
    return to_read;
}

/**
 * @brief Append accepted write bytes into the request-capture buffer
 *
 * Shared by every mock_write() path that accepts bytes (full accept,
 * max_write_chunk-capped accept, and the WRITE_PARTIAL short-accept branch)
 * so req_capture always reflects exactly what the mock reported as written
 * to the caller - never more, and never skipped for a partial accept.
 */
static void mock_capture_bytes(mock_http_transport_ctx_t *ctx, const char *buffer, size_t n)
{
    size_t space = sizeof(ctx->req_capture) - ctx->req_capture_len;
    size_t copy = n < space ? n : space;
    memcpy(ctx->req_capture + ctx->req_capture_len, buffer, copy);
    ctx->req_capture_len += copy;
}

/**
 * @brief Check MOCK_TRANSPORT_MODE_WRITE_FAIL and break the connection if triggered
 *
 * @return true if the caller should report the write as failed (EPIPE)
 */
static bool mock_write_check_fail(mock_http_transport_ctx_t *ctx, int accept_len)
{
    if (ctx->config.mode != MOCK_TRANSPORT_MODE_WRITE_FAIL || !should_inject_error(ctx, accept_len)) {
        return false;
    }
    ESP_LOGI(TAG, "Mock write: FAILED (simulated) - connection broken");
    // Write failure (EPIPE) indicates broken connection - this simulates
    // real-world behavior where write errors break the connection
    ctx->is_connected = false;
    errno = EPIPE;
    return true;
}

/**
 * @brief Check MOCK_TRANSPORT_MODE_WRITE_PARTIAL and, if triggered, capture
 *        and account for the short accept
 *
 * @return Number of bytes short-accepted (> 0) if a partial write was
 *         performed and the caller should return that count directly, or 0
 *         if WRITE_PARTIAL did not trigger and the caller should proceed
 *         with its normal (possibly max_write_chunk-capped) full accept.
 */
static int mock_write_try_partial(mock_http_transport_ctx_t *ctx, const char *buffer, int accept_len)
{
    if (ctx->config.mode != MOCK_TRANSPORT_MODE_WRITE_PARTIAL || !should_inject_error(ctx, accept_len)) {
        return 0;
    }
    int partial = ctx->config.bytes_before_error - ctx->bytes_processed;
    if (partial <= 0 || partial >= accept_len) {
        return 0;
    }
    // Capture exactly the bytes reported as written - this path used to
    // return before the capture code below ever ran, so a partially-accepted
    // write silently vanished from req_capture.
    mock_capture_bytes(ctx, buffer, (size_t)partial);
    ctx->bytes_processed += partial;
    ctx->stats.total_bytes_written += partial;
    ESP_LOGI(TAG, "Mock write: partial write %d bytes (out of %d)", partial, accept_len);
    return partial;
}

/**
 * @brief Mock write implementation
 */
static int mock_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx || !buffer || len <= 0) {
        errno = EINVAL;
        return -1;
    }

    // Would-block injection: simulate a non-blocking socket returning EAGAIN
    if (ctx->wb_writes_left > 0) {
        ctx->wb_writes_left--;
        ESP_LOGD(TAG, "Mock write: EAGAIN (simulated would-block, %d left)", ctx->wb_writes_left);
        errno = EAGAIN;
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.write_calls++;
    }

    if (!ctx->is_connected) {
        ESP_LOGD(TAG, "Mock write: not connected");
        errno = ENOTCONN;
        return -1;
    }

    ESP_LOGI(TAG, "Mock write: %d bytes (mode=%d, bytes_processed=%zu)",
             len, ctx->config.mode, ctx->bytes_processed);

    // Optional per-call cap: simulate a transport whose send buffer only
    // accepts up to max_write_chunk bytes per call, independent of `mode`.
    // 0 (default) leaves accept_len == len, so every pre-existing test's
    // behavior (including the byte-budget math below) is unchanged.
    int accept_len = len;
    if (ctx->config.max_write_chunk > 0 && accept_len > ctx->config.max_write_chunk) {
        accept_len = ctx->config.max_write_chunk;
    }

    // Handle write failure mode
    if (mock_write_check_fail(ctx, accept_len)) {
        return -1;
    }

    // Handle partial write mode
    int partial = mock_write_try_partial(ctx, buffer, accept_len);
    if (partial > 0) {
        return partial;
    }

    // Request boundary: previous response fully consumed and a new request starts.
    // Pop the next queued response into the active buffer via set_response()
    // (which owns free/realloc + read_offset reset) and start a fresh capture.
    if (ctx->resp_queue_next < ctx->resp_queue_count &&
        ctx->response_buffer && ctx->read_offset >= ctx->config.response_len) {
        mock_http_transport_set_response(t, ctx->resp_queue[ctx->resp_queue_next],
                                         ctx->resp_queue_len[ctx->resp_queue_next]);
        ctx->resp_queue_next++;
        ctx->req_capture_len = 0;
    }

    // Capture written bytes (request content) for test assertions, capped to
    // avoid overflowing the fixed-size buffer.
    mock_capture_bytes(ctx, buffer, (size_t)accept_len);

    // Normal write (just track it, don't actually store)
    ctx->bytes_processed += accept_len;

    if (ctx->config.track_calls) {
        ctx->stats.total_bytes_written += accept_len;
    }

    ESP_LOGI(TAG, "Mock write: completed %d bytes (total_processed=%zu)",
             accept_len, ctx->bytes_processed);
    return accept_len;
}

/**
 * @brief Mock close implementation
 */
static int mock_close(esp_transport_handle_t t)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx) {
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.close_calls++;
    }

    ESP_LOGI(TAG, "Mock close (was_connected=%d)", ctx->is_connected);

    ctx->is_connected = false;
    ctx->read_offset = 0;
    ctx->bytes_processed = 0;
    ctx->read_only_bytes = 0;

    return 0;
}

/**
 * @brief Mock poll_read implementation
 */
static int mock_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.poll_read_calls++;
    }

    if (!ctx->is_connected) {
        errno = ENOTCONN;
        return -1;
    }

    // In normal mode, indicate data is available if we have response data
    if (ctx->response_buffer && ctx->read_offset < ctx->config.response_len) {
        return 1;  // Data available
    }

    // Handle timeout mode
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_READ_TIMEOUT) {
        simulate_delay_ms(timeout_ms);
        errno = ETIMEDOUT;
        return 0;  // Timeout
    }

    return 0;  // No data or timeout
}

/**
 * @brief Mock poll_write implementation
 */
static int mock_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->config.track_calls) {
        ctx->stats.poll_write_calls++;
    }

    if (!ctx->is_connected) {
        errno = ENOTCONN;
        return -1;
    }

    // Usually can write (unless in write failure mode)
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_WRITE_FAIL) {
        return 0;  // Can't write
    }

    return 1;  // Can write
}

/**
 * @brief Mock destroy implementation
 */
static int mock_destroy(esp_transport_handle_t t)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (ctx) {
        if (ctx->response_buffer) {
            free(ctx->response_buffer);
        }
        free(ctx);
    }
    return 0;
}

/* ============================================
 * Public API Implementation
 * ============================================ */

esp_transport_handle_t mock_http_transport_create(const mock_http_transport_config_t *config)
{
    // Create transport handle
    esp_transport_handle_t transport = esp_transport_init();
    if (!transport) {
        ESP_LOGE(TAG, "Failed to create transport handle");
        return NULL;
    }

    // Allocate context
    mock_http_transport_ctx_t *ctx = calloc(1, sizeof(mock_http_transport_ctx_t));
    if (!ctx) {
        ESP_LOGE(TAG, "Failed to allocate mock transport context");
        esp_transport_destroy(transport);
        return NULL;
    }

    // Initialize with config or defaults
    if (config) {
        memcpy(&ctx->config, config, sizeof(mock_http_transport_config_t));

        // Copy response data if provided
        if (config->response_data) {
            size_t len = config->response_len > 0 ? config->response_len : strlen(config->response_data);
            ctx->response_buffer = malloc(len);
            if (ctx->response_buffer) {
                memcpy(ctx->response_buffer, config->response_data, len);
                ctx->config.response_len = len;
            } else {
                ESP_LOGE(TAG, "Failed to allocate response buffer");
                free(ctx);
                esp_transport_destroy(transport);
                return NULL;
            }
        }
    } else {
        // Use defaults
        ctx->config.mode = MOCK_TRANSPORT_MODE_NORMAL;
        ctx->config.bytes_before_error = -1;
        ctx->config.track_calls = true;
    }

    // Initialize error-injection countdown counters from config
    ctx->async_polls_left = ctx->config.async_connect_polls;
    ctx->wb_reads_left = ctx->config.would_block_reads;
    ctx->wb_writes_left = ctx->config.would_block_writes;

    // Set context
    esp_transport_set_context_data(transport, ctx);

    // Set transport functions
    esp_transport_set_func(transport,
                          mock_connect,
                          mock_read,
                          mock_write,
                          mock_close,
                          mock_poll_read,
                          mock_poll_write,
                          mock_destroy);
    esp_transport_set_async_connect_func(transport, mock_connect_async);

    ESP_LOGI(TAG, "Mock HTTP transport created (mode=%d)", ctx->config.mode);
    return transport;
}

esp_err_t mock_http_transport_destroy(esp_transport_handle_t transport)
{
    if (!transport) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_transport_destroy(transport);
}

esp_err_t mock_http_transport_set_config(esp_transport_handle_t transport,
                                          const mock_http_transport_config_t *config)
{
    if (!transport || !config) {
        return ESP_ERR_INVALID_ARG;
    }

    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(transport);
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    // Update config
    memcpy(&ctx->config, config, sizeof(mock_http_transport_config_t));

    // Update response data if provided
    if (config->response_data) {
        if (ctx->response_buffer) {
            free(ctx->response_buffer);
            ctx->response_buffer = NULL;
        }

        size_t len = config->response_len > 0 ? config->response_len : strlen(config->response_data);
        ctx->response_buffer = malloc(len);
        if (ctx->response_buffer) {
            memcpy(ctx->response_buffer, config->response_data, len);
            ctx->config.response_len = len;
        } else {
            ESP_LOGE(TAG, "Failed to allocate response buffer");
            return ESP_ERR_NO_MEM;
        }
    }

    // Reset state for new config (but don't change connection state)
    // The connection state should be managed through connect/close calls
    ctx->read_offset = 0;
    ctx->bytes_processed = 0;
    ctx->read_only_bytes = 0;

    // Re-initialize error-injection countdown counters from the new config
    ctx->async_polls_left = ctx->config.async_connect_polls;
    ctx->wb_reads_left = ctx->config.would_block_reads;
    ctx->wb_writes_left = ctx->config.would_block_writes;

    ESP_LOGD(TAG, "Mock transport config updated (mode=%d)", ctx->config.mode);
    return ESP_OK;
}

esp_err_t mock_http_transport_get_stats(esp_transport_handle_t transport,
                                         mock_http_transport_stats_t *stats)
{
    if (!transport || !stats) {
        return ESP_ERR_INVALID_ARG;
    }

    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(transport);
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(stats, &ctx->stats, sizeof(mock_http_transport_stats_t));
    return ESP_OK;
}

esp_err_t mock_http_transport_reset_stats(esp_transport_handle_t transport)
{
    if (!transport) {
        return ESP_ERR_INVALID_ARG;
    }

    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(transport);
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(&ctx->stats, 0, sizeof(mock_http_transport_stats_t));
    ESP_LOGD(TAG, "Mock transport stats reset");
    return ESP_OK;
}

esp_err_t mock_http_transport_set_response(esp_transport_handle_t transport,
                                            const char *response_data,
                                            size_t response_len)
{
    if (!transport) {
        return ESP_ERR_INVALID_ARG;
    }

    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(transport);
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    // Free old buffer
    if (ctx->response_buffer) {
        free(ctx->response_buffer);
        ctx->response_buffer = NULL;
        ctx->config.response_len = 0;
    }

    // Set new response if provided
    if (response_data) {
        size_t len = response_len > 0 ? response_len : strlen(response_data);
        ctx->response_buffer = malloc(len);
        if (!ctx->response_buffer) {
            ESP_LOGE(TAG, "Failed to allocate response buffer");
            return ESP_ERR_NO_MEM;
        }
        memcpy(ctx->response_buffer, response_data, len);
        ctx->config.response_len = len;
    }

    // Reset read position
    ctx->read_offset = 0;
    ctx->bytes_processed = 0;
    ctx->read_only_bytes = 0;

    ESP_LOGD(TAG, "Mock transport response updated (%zu bytes)", ctx->config.response_len);
    return ESP_OK;
}

esp_err_t mock_http_transport_queue_response(esp_transport_handle_t t,
                                              const char *data, size_t len)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx || ctx->resp_queue_count >= 8 || !data) {
        return ESP_ERR_INVALID_ARG;
    }

    // Queue entries are caller-owned (e.g. string literals); store the pointer only.
    ctx->resp_queue[ctx->resp_queue_count] = data;
    ctx->resp_queue_len[ctx->resp_queue_count] = len > 0 ? len : strlen(data);
    ctx->resp_queue_count++;
    return ESP_OK;
}

esp_err_t mock_http_transport_get_last_request(esp_transport_handle_t t,
                                                char *buf, size_t buf_len, size_t *out_len)
{
    mock_http_transport_ctx_t *ctx = esp_transport_get_context_data(t);
    if (!ctx || !buf || buf_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t n = ctx->req_capture_len < buf_len - 1 ? ctx->req_capture_len : buf_len - 1;
    memcpy(buf, ctx->req_capture, n);
    buf[n] = '\0';
    if (out_len) {
        *out_len = n;
    }
    return ESP_OK;
}
