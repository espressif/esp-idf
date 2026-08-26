/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "esp_err.h"
#include "esp_trace_port_encoder.h"

/**
 * @brief Returns the active encoder instance, or NULL if no session exists.
 *
 * @return The active encoder instance, or NULL if no session exists.
 */
esp_trace_encoder_t *esp_trace_get_active_encoder(void);

/**
 * @brief Notify the function-trace runtime of the encoder's recording state.
 *
 * @param active True if the encoder is recording, false otherwise.
 */
void esp_trace_function_trace_notify_recording(bool active);
