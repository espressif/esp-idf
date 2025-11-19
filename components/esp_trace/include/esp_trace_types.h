/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_trace_config {
    // Reserved for future use
    int reserved;
} esp_trace_config_t;

typedef enum {
    ESP_TRACE_LINK_UNKNOWN = 0,
    ESP_TRACE_LINK_DEBUG_PROBE,
    ESP_TRACE_LINK_UART,
} esp_trace_link_types_t;

/* Timeout constants for trace operations */
#define ESP_TRACE_TMO_INFINITE          (UINT32_MAX)

#ifdef __cplusplus
}
#endif
