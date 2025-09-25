/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_trace_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SystemView encoder configuration
 */
typedef struct {
    /**
     * @brief CPU to trace (0 or 1)
     *
     * Determines which CPU's events are captured by SystemView.
     * Only relevant when using UART apptrace transport.
     *
     * - 0: Capture events from CPU0 (Pro CPU)
     * - 1: Capture events from CPU1 (App CPU)
     *
     * @note This parameter is ignored for single-core systems
     * @note This parameter is ignored when using JTAG transport
     */
    int dest_cpu;
} esp_trace_sysview_config_t;

/**
 * @brief SystemView encoder context structure
 *
 * This structure is shared between the sysview adapter and the SEGGER RTT layer
 * to allow proper access to encoder-specific configuration.
 */
typedef struct {
    int dest_cpu;                        ///< CPU to trace (0 or 1)
    esp_trace_lock_t lock;               ///< Encoder lock
} sysview_encoder_ctx_t;

#ifdef __cplusplus
}
#endif
