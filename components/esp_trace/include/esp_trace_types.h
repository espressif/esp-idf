/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Core trace system configuration.
 *
 * Reserved for future core-level trace configuration.
 */
typedef struct esp_trace_config {
    int reserved;  ///< Reserved for future use.
} esp_trace_config_t;

/**
 * @brief Trace transport link type.
 */
typedef enum {
    ESP_TRACE_LINK_UNKNOWN = 0,       ///< Unknown or unavailable link type.
    ESP_TRACE_LINK_DEBUG_PROBE,       ///< Debug probe link, for example JTAG through OpenOCD.
    ESP_TRACE_LINK_UART,              ///< UART link.
    ESP_TRACE_LINK_USB_SERIAL_JTAG,   ///< USB Serial JTAG link.
} esp_trace_link_types_t;

/**
 * @brief Infinite timeout for trace operations.
 */
#define ESP_TRACE_TMO_INFINITE          (UINT32_MAX)

#ifdef __cplusplus
}
#endif
