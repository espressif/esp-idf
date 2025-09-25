/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/**
 * @brief FreeRTOS trace macros dispatcher
 *
 * This header provides a unified interface for FreeRTOS tracing.
 * It dispatches to the appropriate implementation based on configuration:
 *
 * - CONFIG_ESP_TRACE_LIB_SEGGER_SYSVIEW: Uses SEGGER SystemView
 * - CONFIG_ESP_TRACE_LIB_EXTERNAL: Uses external component's implementation
 *   (external component must provide esp_trace_freertos_impl.h see components/esp_trace/README.md for more details)
 */

#if CONFIG_ESP_TRACE_LIB_SEGGER_SYSVIEW
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#undef INLINE /* to avoid redefinition */
#elif CONFIG_ESP_TRACE_LIB_EXTERNAL
#include "esp_trace_freertos_impl.h"
#endif
