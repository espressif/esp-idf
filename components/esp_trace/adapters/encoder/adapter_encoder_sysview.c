/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_trace_types.h"
#include "esp_trace_registry.h"
#include "esp_trace_port_encoder.h"
#include "esp_trace_port_transport.h"
#include "adapters/adapter_encoder_sysview.h"
#include "esp_trace_util.h"
#include "esp_sysview_trace.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"

/*
 * This adapter is used to create a public system-wide APIs for SystemView.
 * All encoding and transport operations are done by SystemView component. (SEGGER_RTT_esp.c)
 */

#define SYSVIEW_FLUSH_TMO_US (1000 * 1000)  /* 1second */
#define SYSVIEW_FLUSH_THRESH 0

/**
 * @brief Initializes sysview encoder.
 *        This function is called for each core.
 *        Adapter implementations do NOT need their own multi-core protection. Core does it for them.
 *
 * @param enc Pointer to the encoder structure. Must not be NULL.
 * @param enc_cfg Pointer to the encoder configuration. Can be NULL for defaults.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t init(esp_trace_encoder_t *enc, const void *enc_cfg)
{
    static bool initialized = false;

    if (!enc) {
        return ESP_ERR_INVALID_ARG;
    }

    if (initialized) {
        return ESP_OK;
    }

    const esp_trace_sysview_config_t *cfg = enc_cfg;
    int dest_cpu = 0;  // Default to CPU0

    if (cfg) {
        dest_cpu = cfg->dest_cpu;
    }

    // Allocate and initialize encoder context
    sysview_encoder_ctx_t *ctx = heap_caps_calloc(1, sizeof(*ctx), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }

    /* Segger Sysview needs locking mechanism. */
    esp_trace_lock_init(&ctx->lock);
    ctx->dest_cpu = dest_cpu;
    enc->ctx = ctx;

    if (SEGGER_RTT_ESP_SetEncoder(enc) != 0) {
        heap_caps_free(ctx);
        enc->ctx = NULL;
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Configure transport for SystemView requirements */
    if (enc->tp->vt->set_config) {
        uint32_t flush_tmo = SYSVIEW_FLUSH_TMO_US;
        uint32_t flush_thresh = SYSVIEW_FLUSH_THRESH;
        uint32_t header_size = 2; /* SystemView uses 2-byte (16-bit) trace headers */
        enc->tp->vt->set_config(enc->tp, ESP_TRACE_TRANSPORT_CFG_FLUSH_TMO, &flush_tmo);
        enc->tp->vt->set_config(enc->tp, ESP_TRACE_TRANSPORT_CFG_FLUSH_THRESH, &flush_thresh);
        enc->tp->vt->set_config(enc->tp, ESP_TRACE_TRANSPORT_CFG_HEADER_SIZE, &header_size);
    }

    SEGGER_SYSVIEW_Conf();

    initialized = true;

    return ESP_OK;
}

/**
 * @brief Panic handler
 *
 * Called during system panic to finalize encoder state.
 *
 * @param enc Pointer to the encoder structure. Must not be NULL.
 * @param info Panic information
 */
static void panic_handler(esp_trace_encoder_t *enc, const void *info)
{
    (void)enc;
    (void)info;

    SEGGER_RTT_ESP_Flush();
}

/**
 * @brief Takes the lock of sysview encoder.
 *
 * @param enc Pointer to the encoder structure. Must not be NULL.
 * @param tmo Timeout for the operation (in us).
 */
static unsigned int take_lock(esp_trace_encoder_t *enc, uint32_t tmo_us)
{
    sysview_encoder_ctx_t *ctx = enc->ctx;
    esp_trace_lock_take(&ctx->lock, tmo_us);

    return ctx->lock.int_state;
}

/**
 * @brief Gives the lock of sysview encoder.
 *
 * @param enc Pointer to the encoder structure. Must not be NULL.
 * @param int_state The interrupt state.
 */
static void give_lock(esp_trace_encoder_t *enc, unsigned int_state)
{
    sysview_encoder_ctx_t *ctx = enc->ctx;

    // Restore interrupt state before releasing lock
    ctx->lock.int_state = int_state;
    esp_trace_lock_give(&ctx->lock);
}

/**
 * @brief Sysview encoder vtable.
 */
static const esp_trace_encoder_vtable_t s_sysview_vt = {
    .init                  = init,
    .panic_handler         = panic_handler,
    .take_lock             = take_lock,
    .give_lock             = give_lock,
};

ESP_TRACE_REGISTER_ENCODER("sysview", &s_sysview_vt);
