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
    char *response_buffer;                   /*!< Internal copy of response data */
} mock_http_transport_ctx_t;

// Forward declarations of transport function implementations
static int mock_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms);
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

    ESP_LOGI(TAG, "Mock connect succeeded");
    return 0;
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

    // Handle incomplete read mode (close connection mid-stream)
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_INCOMPLETE_READ) {
        if (should_inject_error(ctx, to_read)) {
            // Read partial data then close connection
            size_t partial = ctx->config.bytes_before_error - ctx->bytes_processed;
            if (partial > 0 && partial < to_read) {
                memcpy(buffer, ctx->response_buffer + ctx->read_offset, partial);
                ctx->read_offset += partial;
                ctx->bytes_processed += partial;
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

    if (ctx->config.track_calls) {
        ctx->stats.total_bytes_read += to_read;
    }

    ESP_LOGI(TAG, "Mock read: returned %zu bytes (offset now=%zu, remaining=%zu)",
             to_read, ctx->read_offset, ctx->config.response_len - ctx->read_offset);
    return to_read;
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

    // Handle write failure mode
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_WRITE_FAIL) {
        if (should_inject_error(ctx, len)) {
            ESP_LOGI(TAG, "Mock write: FAILED (simulated) - connection broken");
            // Write failure (EPIPE) indicates broken connection
            // This simulates real-world behavior where write errors break the connection
            ctx->is_connected = false;
            errno = EPIPE;
            return -1;
        }
    }

    // Handle partial write mode
    if (ctx->config.mode == MOCK_TRANSPORT_MODE_WRITE_PARTIAL) {
        if (should_inject_error(ctx, len)) {
            // Write only partial data
            int partial = ctx->config.bytes_before_error - ctx->bytes_processed;
            if (partial > 0 && partial < len) {
                ctx->bytes_processed += partial;
                ctx->stats.total_bytes_written += partial;
                ESP_LOGI(TAG, "Mock write: partial write %d bytes (out of %d)", partial, len);
                return partial;
            }
        }
    }

    // Normal write (just track it, don't actually store)
    ctx->bytes_processed += len;

    if (ctx->config.track_calls) {
        ctx->stats.total_bytes_written += len;
    }

    ESP_LOGI(TAG, "Mock write: completed %d bytes (total_processed=%zu)",
             len, ctx->bytes_processed);
    return len;
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

    ESP_LOGD(TAG, "Mock transport response updated (%zu bytes)", ctx->config.response_len);
    return ESP_OK;
}
