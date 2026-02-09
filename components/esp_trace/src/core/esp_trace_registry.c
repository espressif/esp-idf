/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_trace_registry.h"

/**
 * @file esp_trace_registry.c
 * @brief Linker-time registration implementation
 *
 * The linker collects all ESP_TRACE_REGISTER_ENCODER() and ESP_TRACE_REGISTER_TRANSPORT()
 * registrations into arrays with SURROUND symbols that mark the start and end:
 *
 * - _esp_trace_encoder_array_start ... _esp_trace_encoder_array_end
 * - _esp_trace_transport_array_start ... _esp_trace_transport_array_end
 *
 * These are populated automatically by the linker script (linker.lf).
 */

extern esp_trace_encoder_desc_t _esp_trace_encoder_array_start;
extern esp_trace_encoder_desc_t _esp_trace_encoder_array_end;
extern esp_trace_transport_desc_t _esp_trace_transport_array_start;
extern esp_trace_transport_desc_t _esp_trace_transport_array_end;

const esp_trace_encoder_vtable_t *esp_trace_find_encoder(const char *name)
{
    if (!name) {
        return NULL;
    }

    for (const esp_trace_encoder_desc_t *it = &_esp_trace_encoder_array_start;
            it < &_esp_trace_encoder_array_end; ++it) {
        if (it->name && strcmp(it->name, name) == 0) {
            return it->vtable;
        }
    }

    return NULL;
}

const esp_trace_transport_vtable_t *esp_trace_find_transport(const char *name)
{
    if (!name) {
        return NULL;
    }

    for (const esp_trace_transport_desc_t *it = &_esp_trace_transport_array_start;
            it < &_esp_trace_transport_array_end; ++it) {
        if (it->name && strcmp(it->name, name) == 0) {
            return it->vtable;
        }
    }

    return NULL;
}
