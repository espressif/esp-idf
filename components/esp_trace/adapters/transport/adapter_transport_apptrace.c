/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_trace_registry.h"
#include "esp_trace_port_transport.h"
#include "esp_trace_types.h"
#include "esp_app_trace.h"

static const char *TAG = "adapter_transport_apptrace";

typedef struct {
    uint32_t flush_tmo;
    uint32_t flush_thresh;
} apptrace_ctx_t;

/**
 * @brief Initializes apptrace transport.
 *        This function is called for each core at system init time.
 *        Allocated context will not be freed and kept alive until the end of the program.
 *        Adapter implementations do NOT need their own multi-core protection. Core does it for them.
 *
 * @param tp Pointer to the transport structure. Must not be NULL.
 * @param tp_cfg Pointer to the transport configuration. NULL means use default configuration.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t apptrace_init(esp_trace_transport_t *tp, const void *tp_cfg)
{
    if (!tp) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Create context if not already done */
    if (!tp->ctx) {
        apptrace_ctx_t *ctx = heap_caps_calloc(1, sizeof(*ctx), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (!ctx) {
            return ESP_ERR_NO_MEM;
        }
        tp->ctx = ctx;
    }

    const esp_apptrace_config_t *cfg = (esp_apptrace_config_t *)tp_cfg;
    esp_apptrace_config_t default_cfg = APPTRACE_CONFIG_DEFAULT();

    if (!cfg) {
        cfg = &default_cfg;
    }

    /* Initialize context */
    apptrace_ctx_t *ctx = (apptrace_ctx_t *)tp->ctx;
    ctx->flush_tmo = cfg->flush_tmo;
    ctx->flush_thresh = cfg->flush_thresh;

    // Initialize underlying apptrace
    esp_err_t err = esp_apptrace_init(cfg);
    if (err != ESP_OK) {
        heap_caps_free(ctx);
        tp->ctx = NULL;
        return err;
    }

    return ESP_OK;
}

/**
 * @brief Reads data from apptrace transport.
 *
 * @param tp Pointer to the transport structure. Ignored.
 * @param data Pointer to the data buffer.
 * @param size Pointer to the size of the data buffer.
 * @param tmo Timeout for the operation (in us).
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t apptrace_read(esp_trace_transport_t *tp, void *data, size_t *size, uint32_t tmo)
{
    (void)tp;
    return esp_apptrace_read(data, (uint32_t *)size, tmo);
}

/**
 * @brief Writes data to apptrace transport.
 *
 * @param tp Pointer to the transport structure. Ignored.
 * @param data Pointer to the data buffer.
 * @param size Size of the data buffer.
 * @param tmo Timeout for the operation (in us).
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t apptrace_write(esp_trace_transport_t *tp, const void *data, size_t size, uint32_t tmo)
{
    (void)tp;
    return esp_apptrace_write(data, (uint32_t)size, tmo);
}

/**
 * @brief Configures down buffer for apptrace transport.
 *
 * @param tp Pointer to the transport structure. Ignored.
 * @param buf Pointer to the data buffer.
 * @param size Size of the data buffer.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t apptrace_down_buffer_config(esp_trace_transport_t *tp, uint8_t *buf, uint32_t size)
{
    (void)tp;
    return esp_apptrace_down_buffer_config(buf, size);
}

/**
 * @brief Flushes data from apptrace transport without locking.
 *
 * Uses the panic configuration stored during initialization.
 *
 * @param tp Pointer to the transport structure.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t apptrace_flush_nolock(esp_trace_transport_t *tp)
{
    apptrace_ctx_t *ctx = (apptrace_ctx_t *)tp->ctx;
    if (!ctx) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_apptrace_flush_nolock(ctx->flush_thresh, ctx->flush_tmo);
}

/**
 * @brief Checks if the host is connected to apptrace transport.
 *
 * @param tp Pointer to the transport structure. Ignored.
 *
 * @return True if the host is connected, otherwise false.
 */
static bool apptrace_is_host_connected(esp_trace_transport_t *tp)
{
    (void)tp;
    return esp_apptrace_host_is_connected();
}

/**
 * @brief Gets the link type of apptrace transport.
 *
 * @param tp Pointer to the transport structure. Ignored.
 *
 * @return The link type of apptrace transport.
 */
static esp_trace_link_types_t apptrace_get_link_type(esp_trace_transport_t *tp)
{
    (void)tp;

    switch (esp_apptrace_get_destination()) {
    case ESP_APPTRACE_DEST_JTAG:
        return ESP_TRACE_LINK_DEBUG_PROBE;
    case ESP_APPTRACE_DEST_UART:
        return ESP_TRACE_LINK_UART;
    default:
        return ESP_TRACE_LINK_UNKNOWN;
    }
}

/**
 * @brief Sets configuration for apptrace transport.
 *
 * @param tp Pointer to the transport structure. Must not be NULL.
 * @param key The key of the configuration.
 * @param value The value of the configuration.
 */
static esp_err_t apptrace_set_config(esp_trace_transport_t *tp, esp_trace_transport_cfg_key_t key, const void *value)
{
    if (!value) {
        return ESP_ERR_INVALID_ARG;
    }

    apptrace_ctx_t *ctx = (apptrace_ctx_t *)tp->ctx;
    if (!ctx) {
        return ESP_ERR_INVALID_STATE;
    }

    switch (key) {
    case ESP_TRACE_TRANSPORT_CFG_HEADER_SIZE: {
        esp_apptrace_header_size_t header_size = *(const esp_apptrace_header_size_t *)value;
        return esp_apptrace_set_header_size(header_size);
    }
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_TMO:
        ctx->flush_tmo = *(const uint32_t *)value;
        return ESP_OK;
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_THRESH:
        ctx->flush_thresh = *(const uint32_t *)value;
        return ESP_OK;
    default:
        ESP_EARLY_LOGE(TAG, "Key %d is not supported", key);
        return ESP_ERR_NOT_SUPPORTED;
    }
}

/**
 * @brief Gets configuration from apptrace transport.
 *
 * @param tp Pointer to the transport structure. Must not be NULL.
 * @param key Configuration key.
 * @param value Pointer to store the configuration value.
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if parameters are invalid,
 *         ESP_ERR_NOT_SUPPORTED if key is not supported.
 */
static esp_err_t apptrace_get_config(esp_trace_transport_t *tp, esp_trace_transport_cfg_key_t key, void *value)
{
    apptrace_ctx_t *ctx = (apptrace_ctx_t *)tp->ctx;

    if (!value) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (key) {
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_TMO:
        *(uint32_t *)value = ctx->flush_tmo;
        return ESP_OK;
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_THRESH:
        *(uint32_t *)value = ctx->flush_thresh;
        return ESP_OK;
    default:
        ESP_EARLY_LOGE(TAG, "Key %d is not supported", key);
        return ESP_ERR_NOT_SUPPORTED;
    }
}

void apptrace_panic_handler(esp_trace_transport_t *tp, const void *info)
{
    (void)info;

    apptrace_ctx_t *ctx = (apptrace_ctx_t *)tp->ctx;
    if (!ctx) {
        return;
    }

    esp_apptrace_flush_nolock(ctx->flush_thresh, ctx->flush_tmo);
}

/**
 * @brief Apptrace transport vtable.
 */
static const esp_trace_transport_vtable_t s_apptrace_vt = {
    .init                  = apptrace_init,
    .set_config            = apptrace_set_config,
    .get_config            = apptrace_get_config,
    .read                  = apptrace_read,
    .write                 = apptrace_write,
    .flush_nolock          = apptrace_flush_nolock,
    .down_buffer_config    = apptrace_down_buffer_config,
    .is_host_connected     = apptrace_is_host_connected,
    .get_link_type         = apptrace_get_link_type,
    .panic_handler         = apptrace_panic_handler,
};

ESP_TRACE_REGISTER_TRANSPORT("apptrace", &s_apptrace_vt);
