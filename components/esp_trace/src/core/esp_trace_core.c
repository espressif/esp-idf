/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ESP Trace Core - Central trace session management
 *
 * This module manages trace sessions, coordinating encoders (trace libraries like SystemView)
 * and transports (communication channels like JTAG, UART).
 *
 */

#include <stdlib.h>
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_trace_registry.h"
#include "esp_trace.h"
#include "esp_trace_port_transport.h"
#include "esp_private/startup_internal.h"

static const char *TAG = "esp_trace_core";

struct esp_trace_handle {
    esp_trace_encoder_t encoder;
    esp_trace_transport_t transport;
};

/**
 * Singleton session management
 * Only one trace session can be active at a time.
 */
static esp_trace_handle_t s_active_handle = NULL;
static portMUX_TYPE s_init_lock = portMUX_INITIALIZER_UNLOCKED;
static volatile int s_trace_handle_created = 0;

static esp_err_t esp_trace_create(const esp_trace_open_params_t *params)
{
    if (!params || !params->encoder_name || !params->transport_name) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_active_handle) {
        ESP_EARLY_LOGE(TAG, "Trace handle already created");
        return ESP_ERR_INVALID_STATE;
    }

    const esp_trace_encoder_vtable_t *enc_vt = esp_trace_find_encoder(params->encoder_name);
    const esp_trace_transport_vtable_t *tp_vt = esp_trace_find_transport(params->transport_name);

    if (!enc_vt || !tp_vt) {
        ESP_EARLY_LOGE(TAG, "Encoder '%s' or transport '%s' not found", params->encoder_name, params->transport_name);
        return ESP_ERR_NOT_FOUND;
    }

    esp_trace_handle_t h = heap_caps_calloc(1, sizeof(*h), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!h) {
        ESP_EARLY_LOGE(TAG, "Failed to allocate trace handle");
        return ESP_ERR_NO_MEM;
    }

    h->encoder.vt = enc_vt;
    h->transport.vt = tp_vt;
    h->encoder.tp = &h->transport;

    s_active_handle = h;

    return ESP_OK;
}

/* This function is called during early system initialization (before app_main) on all cores */
static esp_err_t esp_trace_init(const esp_trace_open_params_t *params)
{
    esp_err_t err = ESP_OK;
    esp_trace_handle_t h = NULL;
    int core_id = esp_cpu_get_core_id();

    /* Core 0 creates the handle if it doesn't exist yet */
    if (core_id == 0) {
        if (!s_trace_handle_created) {
            err = esp_trace_create(params);
            if (err != ESP_OK) {
                return err;
            }
            s_trace_handle_created = 1;
        }
    } else {
        // There is NO guarantee that system init functions will execute on core 0 first
        // So we need to wait for core 0 to create the handle
        while (!s_trace_handle_created) {
            esp_rom_delay_us(10);
        }
    }

    /* Handle is guaranteed to be created by now */
    h = s_active_handle;

    portENTER_CRITICAL(&s_init_lock);

    /* Setup transport first (encoder depends on it) */
    if (h->transport.vt->init) {
        err = h->transport.vt->init(&h->transport, params->transport_cfg);
        if (err != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Transport open failed: %d", err);
            goto cleanup;
        }
    }

    /* Setup encoder */
    if (h->encoder.vt->init) {
        err = h->encoder.vt->init(&h->encoder, params->encoder_cfg);
        if (err != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Encoder init failed: %d", err);
            goto cleanup;
        }
    }

    ESP_EARLY_LOGI(TAG, "Trace initialized on core %d: encoder='%s', transport='%s'",
                   core_id, params->encoder_name, params->transport_name);

    portEXIT_CRITICAL(&s_init_lock);
    return ESP_OK;

cleanup:
    if (s_active_handle) {
        /* Will not create a heap trace event. Returning error will cause an abort. */
        heap_caps_free(s_active_handle);
        s_active_handle = NULL;
        s_trace_handle_created = 0;
    }
    portEXIT_CRITICAL(&s_init_lock);
    return err;
}

esp_err_t esp_trace_write(esp_trace_handle_t h, const void *data, size_t size, unsigned long tmo)
{
    if (!h || !data || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!h->encoder.vt->write) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return h->encoder.vt->write(&h->encoder, data, size, tmo);
}

bool esp_trace_is_host_connected(esp_trace_handle_t h)
{
    if (!h || !h->transport.vt->is_host_connected) {
        return false;
    }

    return h->transport.vt->is_host_connected(&h->transport);
}

esp_trace_link_types_t esp_trace_get_link_type(esp_trace_handle_t h)
{
    if (!h || !h->transport.vt->get_link_type) {
        return ESP_TRACE_LINK_UNKNOWN;
    }

    return h->transport.vt->get_link_type(&h->transport);
}

esp_trace_handle_t esp_trace_get_active_handle(void)
{
    return s_active_handle;
}

void esp_trace_panic_handler(const void *info)
{
    esp_trace_handle_t h = s_active_handle;
    if (!h) {
        return;
    }
    if (h->encoder.vt->panic_handler) {
        h->encoder.vt->panic_handler(&h->encoder, info);
    }

    if (h->transport.vt->panic_handler) {
        h->transport.vt->panic_handler(&h->transport, info);
    }
}

esp_trace_open_params_t __attribute__((weak)) esp_trace_get_user_params(void)
{
    esp_trace_open_params_t trace_params = {
        .core_cfg = NULL,
        .encoder_name = CONFIG_ESP_TRACE_LIB_NAME,
        .encoder_cfg = NULL, /* Not used for now */
        .transport_name = CONFIG_ESP_TRACE_TRANSPORT_NAME,
        .transport_cfg = NULL, /* Null means configure with the default parameters */
    };
    return trace_params;
}

ESP_SYSTEM_INIT_FN(esp_trace_early_init, SECONDARY, ESP_SYSTEM_INIT_ALL_CORES, 120)
{
    esp_trace_open_params_t trace_params = esp_trace_get_user_params();
    return esp_trace_init(&trace_params);
}
