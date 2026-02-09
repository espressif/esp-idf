/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_trace_types.h"    // esp_trace_link_types_t

/* forward declarations */
typedef struct esp_trace_transport esp_trace_transport_t;

/**
 * @brief Transport configuration keys
 *
 * Standard configuration keys that encoders can use to configure transport behavior.
 */
typedef enum {
    ESP_TRACE_TRANSPORT_CFG_HEADER_SIZE,    ///< (uint32_t*) Trace header size in bytes
    ESP_TRACE_TRANSPORT_CFG_FLUSH_TMO,      ///< (uint32_t*) Flush timeout in microseconds
    ESP_TRACE_TRANSPORT_CFG_FLUSH_THRESH,   ///< (uint32_t*) Flush threshold in bytes
} esp_trace_transport_cfg_key_t;

/**
 * @brief Transport Virtual Table
 *
 * Defines the interface for trace transports.
 *
 */
typedef struct {
    /**
     * @brief Initialize transport
     * @param tp Transport instance
     * @param tp_cfg Transport-specific configuration
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*init)(esp_trace_transport_t *tp, const void *tp_cfg);

    /**
     * @brief Set transport configuration
     * @param tp Transport instance
     * @param key Configuration key
     * @param value Configuration value
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*set_config)(esp_trace_transport_t *tp, esp_trace_transport_cfg_key_t key, const void *value);

    /**
     * @brief Get transport configuration
     * @param tp Transport instance
     * @param key Configuration key
     * @param value Pointer to store configuration value
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*get_config)(esp_trace_transport_t *tp, esp_trace_transport_cfg_key_t key, void *value);

    /**
     * @brief Read trace data
     * @param tp Transport instance
     * @param data Buffer to read data into
     * @param size Pointer to size (in: buffer size, out: bytes read)
     * @param tmo Timeout in microseconds
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*read)(esp_trace_transport_t *tp, void *data, size_t *size, uint32_t tmo);

    /**
     * @brief Write trace data
     * @param tp Transport instance
     * @param data Data buffer to write
     * @param size Size of data in bytes
     * @param tmo Timeout in microseconds
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*write)(esp_trace_transport_t *tp, const void *data, size_t size, uint32_t tmo);

    /**
     * @brief Flush without lock
     *
     * The transport should use its internally stored flush configuration (timeout, threshold) that was
     * set during initialization.
     *
     * @param tp Transport instance
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*flush_nolock)(esp_trace_transport_t *tp);

    /**
     * @brief Configure down buffer
     * @param tp Transport instance
     * @param buf Buffer pointer
     * @param size Buffer size in bytes
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*down_buffer_config)(esp_trace_transport_t *tp, uint8_t *buf, uint32_t size);

    /**
     * @brief Check if host is connected
     * @param tp Transport instance
     * @return true if host is connected, false otherwise
     */
    bool (*is_host_connected)(esp_trace_transport_t *tp);

    /**
     * @brief Get link type @see esp_trace_link_types_t
     * @param tp Transport instance
     * @return Link type
     */
    esp_trace_link_types_t (*get_link_type)(esp_trace_transport_t *tp);

    /**
     * @brief Panic handler
     * @param tp Transport instance
     * @param info Panic information
     */
    void (*panic_handler)(esp_trace_transport_t *tp, const void *info);
} esp_trace_transport_vtable_t;

/**
 * @brief Transport structure representing an active transport instance.
 */
struct esp_trace_transport {
    const esp_trace_transport_vtable_t *vt;  ///< Pointer to transport vtable
    void *ctx;                               ///< Transport-specific context
};

#ifdef __cplusplus
}
#endif
