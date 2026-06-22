/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/**
 * @brief Start recording compiler-instrumented function events.
 *
 * Resolves the active encoder's function-trace capability once and enables the
 * hooks. Requires an active trace session.
 *
 * @return ESP_OK on success,
 *         ESP_ERR_INVALID_STATE if no trace session is active,
 *         ESP_ERR_NOT_SUPPORTED if the encoder lacks function-trace callbacks.
 */
esp_err_t esp_trace_function_trace_start(void);

/**
 * @brief Stop recording function events.
 *
 * @return ESP_OK on success.
 */
esp_err_t esp_trace_function_trace_stop(void);

#ifdef __cplusplus
}
#endif
