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
#include "esp_trace_port_encoder.h"
#include "esp_trace_port_transport.h"

typedef struct {
    const char *name;                        ///< Encoder name for lookup (e.g., "sysview")
    const esp_trace_encoder_vtable_t *vtable; ///< Pointer to function table
} esp_trace_encoder_desc_t;

typedef struct {
    const char *name;                           ///< Transport name for lookup (e.g., "apptrace")
    const esp_trace_transport_vtable_t *vtable; ///< Pointer to function table
} esp_trace_transport_desc_t;

#define ESP_TRACE_SECTION(name) __attribute__((used, section(name)))

/**
 * @brief Register an encoder at compile time
 *
 * Place this macro in your encoder implementation file after defining the vtable.
 * The encoder will be automatically discovered by the trace system at runtime.
 *
 * @note This creates a static descriptor in the .esp_trace_encoder_desc section
 * @note The linker collects all descriptors and creates esp_trace_encoder_array
 * @note Only use once per file (one encoder per implementation file)
 *
 * @param name_str  Encoder name string (e.g., "sysview")
 * @param vt_ptr    Pointer to the encoder vtable structure
 *
 */
#define ESP_TRACE_REGISTER_ENCODER(name_str, vt_ptr)                          \
    static const esp_trace_encoder_desc_t _esp_trace_encoder_desc            \
    ESP_TRACE_SECTION(".esp_trace_encoder_desc") = { (name_str), (vt_ptr) }

/**
 * @brief Register a transport at compile time
 *
 * Place this macro in your transport implementation file after defining the vtable.
 * The transport will be automatically discovered by the trace system at runtime.
 *
 * @note This creates a static descriptor in the .esp_trace_transport_desc section
 * @note The linker collects all descriptors and creates esp_trace_transport_array
 * @note Only use once per file (one transport per implementation file)
 *
 * @param name_str  Transport name string (e.g., "apptrace")
 * @param vt_ptr    Pointer to the transport vtable structure
 *
 */
#define ESP_TRACE_REGISTER_TRANSPORT(name_str, vt_ptr)                        \
    static const esp_trace_transport_desc_t _esp_trace_transport_desc        \
    ESP_TRACE_SECTION(".esp_trace_transport_desc") = { (name_str), (vt_ptr) }

/* Lookup functions */
const esp_trace_encoder_vtable_t *esp_trace_find_encoder(const char *name);
const esp_trace_transport_vtable_t *esp_trace_find_transport(const char *name);

#ifdef __cplusplus
}
#endif
