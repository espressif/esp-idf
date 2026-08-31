/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_trace_types.h"
#include "esp_trace_registry.h"
#include "esp_trace_port_encoder.h"
#include "esp_trace_port_transport.h"
#include "esp_trace_util.h"
#include "trace_FreeRTOS.h"

typedef struct {
    esp_trace_lock_t lock;
} ext_trace_lib_ctx_t;
/**
 * @brief Initializes ext_trace_lib encoder.
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
    (void)enc_cfg;

    // Ensure the encoder is initialized only once unless something todo for both cores
    static bool initialized = false;

    if (!enc) {
        return ESP_ERR_INVALID_ARG;
    }

    if (initialized) {
        return ESP_OK;
    }

    ext_trace_lib_ctx_t *ctx = heap_caps_calloc(1, sizeof(*ctx),
                                                MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }
    esp_trace_lock_init(&ctx->lock);
    enc->ctx = ctx;

    init_trace_lib(enc);

    initialized = true;

    return ESP_OK;
}

static esp_err_t write(esp_trace_encoder_t *enc, const void *data, size_t size, uint32_t tmo)
{
    if (!enc || !data || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!enc->tp || !enc->tp->vt->write) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return enc->tp->vt->write(enc->tp, data, size, tmo);
}

static esp_err_t start(esp_trace_encoder_t *enc)
{
    (void)enc;
    trace_lib_start();
    return ESP_OK;
}

static esp_err_t stop(esp_trace_encoder_t *enc)
{
    (void)enc;
    trace_lib_stop();
    return ESP_OK;
}

static esp_err_t flush(esp_trace_encoder_t *enc)
{
    if (!enc || !enc->tp || !enc->tp->vt->flush_nolock) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return enc->tp->vt->flush_nolock(enc->tp);
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
    (void)info;
    flush(enc);
}

static unsigned int take_lock(esp_trace_encoder_t *enc, uint32_t tmo_us)
{
    if (!enc || !enc->ctx) {
        return 0;
    }
    ext_trace_lib_ctx_t *ctx = enc->ctx;
    esp_trace_lock_take(&ctx->lock, tmo_us);
    return ctx->lock.int_state;
}

static void give_lock(esp_trace_encoder_t *enc, unsigned int int_state)
{
    if (!enc || !enc->ctx) {
        return;
    }
    ext_trace_lib_ctx_t *ctx = enc->ctx;
    ctx->lock.int_state = int_state;
    esp_trace_lock_give(&ctx->lock);
}

static const esp_trace_encoder_vtable_t s_ext_trace_lib_vt = {
    .init                  = init,
    .write                 = write,
    .panic_handler         = panic_handler,
    .start                 = start,
    .stop                  = stop,
    .flush                 = flush,
    .take_lock             = take_lock,
    .give_lock             = give_lock,
};

ESP_TRACE_REGISTER_ENCODER("ext_trace_lib", &s_ext_trace_lib_vt);
