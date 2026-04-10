/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_trace_types.h"

/**
 * @brief Trace session handle
 *
 * Opaque handle representing an active trace session.
 */
typedef struct esp_trace_handle *esp_trace_handle_t;

/**
 * @brief Parameters for opening a trace session
 *
 * This structure configures all aspects of a trace session, including the core behavior,
 * encoder (trace library), and transport (communication channel).
 */
typedef struct {
    /**
     * @brief Core trace system configuration (optional)
     *
     * Currently reserved for future use.
     */
    const esp_trace_config_t *core_cfg;

    /**
     * @brief Encoder name (required)
     *
     * Must match a registered encoder name. Built-in encoders:
     * - "sysview" - SEGGER SystemView protocol for FreeRTOS tracing
     * - "raw" - Pass-through for raw binary data
     */
    const char *encoder_name;

    /**
     * @brief Encoder-specific configuration (optional)
     *
     * If NULL, encoder uses its default configuration. The structure type
     * depends on the encoder being used. See encoder-specific documentation.
     */
    const void *encoder_cfg;

    /**
     * @brief Transport name (required)
     *
     * Must match a registered transport name. Built-in transports:
     * - "apptrace" - Uses app_trace for JTAG or UART communication
     *
     */
    const char *transport_name;

    /**
     * @brief Transport-specific configuration (optional)
     *
     * If NULL, transport uses its default configuration. The structure type
     * depends on the transport being used:
     * - For "apptrace": `esp_apptrace_config_t*`
     *
     * See transport-specific documentation for details.
     */
    const void *transport_cfg;
} esp_trace_open_params_t;

/**
 * @brief Get trace initialization parameters for early system initialization
 *
 * User applications need to implement this function to provide a custom trace configuration.
 * This function is called during system initialization before any other trace functions are called.
 *
 * @return Configuration
 */
esp_trace_open_params_t esp_trace_get_user_params(void);

/**
 * @brief Get the active trace handle
 *
 * @return The active trace handle
 */
esp_trace_handle_t esp_trace_get_active_handle(void);

/**
 * @brief Write data to the trace handle
 *
 * @param handle The trace handle
 * @param data The data to write
 * @param size The size of the data
 * @param tmo The timeout in us
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_trace_write(esp_trace_handle_t handle, const void *data, size_t size, unsigned long tmo);

/**
 * @brief Check if the host is connected
 *
 * @param handle The trace handle
 *
 * @return true if the host is connected, otherwise false
 */
bool esp_trace_is_host_connected(esp_trace_handle_t handle);

/**
 * @brief Get the link type
 *
 * @param handle The trace handle
 *
 * @return The link type @see esp_trace_link_types_t
 */
esp_trace_link_types_t esp_trace_get_link_type(esp_trace_handle_t handle);

/**
 * @brief Panic flush the trace handle. This function is called from panic handler.
 *
 * @param handle The trace handle
 */
void esp_trace_panic_handler(const void *info);

#ifdef __cplusplus
}
#endif
