/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SM3 (GM/T 0004-2012) implementation on the ESP SHA peripheral.
 *
 * SM3 uses the same message schedule as the SHA-256 family: a 64-byte block,
 * a 256-bit state, and the same padding. Only the compression function
 * differs, and the SHA peripheral provides it as mode SM3.
 */

#include <string.h>

#include "esp_err.h"
#include "esp_macros.h"
#include "hal/sha_types.h"
#include "esp_sm3.h"
#include "soc/soc_caps.h"
#include "hal/sha_hal.h"
#include "esp_crypto_periph_clk.h"
#include "esp_crypto_lock.h"
#include "esp_heap_caps.h"

#if SOC_SHA_SUPPORT_SM3

/**
 * @brief SM3 operation context.
 *
 * esp_sm3_create() allocates the context for the streaming API, and
 * esp_sm3_delete() releases it. esp_sm3() keeps one on its own stack. The caller
 * never sees the members.
 */
struct esp_sm3_ctx_s {
    unsigned char buffer[ESP_SM3_BLOCK_LEN];        /*!< Message bytes that do not fill a block yet */
    uint64_t total;                                 /*!< Number of message bytes processed */
    uint32_t state[ESP_SM3_DIGEST_LEN / 4];         /*!< Intermediate digest state */
    bool state_valid;                               /*!< True when state holds an intermediate digest */
};

static const unsigned char sm3_padding[ESP_SM3_BLOCK_LEN] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void esp_sm3_zeroize(void *buf, size_t len)
{
    volatile uint8_t *p = (volatile uint8_t *)buf;
    for (size_t i = 0; i < len; i++) {
        *p++ = 0;
    }
}

static inline void sm3_put_be32(uint32_t value, unsigned char *out)
{
    out[0] = (unsigned char)(value >> 24);
    out[1] = (unsigned char)(value >> 16);
    out[2] = (unsigned char)(value >> 8);
    out[3] = (unsigned char)(value);
}

/* Hash the pending block, then every whole block of input. The caller must hold
   the SHA peripheral and must have selected mode SM3. */
static void sm3_hash_blocks(esp_sm3_ctx_handle_t ctx, const unsigned char *input,
                            size_t len, bool has_pending_block, bool first_block)
{
    if (has_pending_block) {
        sha_hal_hash_block(SM3, ctx->buffer, ESP_SM3_BLOCK_LEN / 4, first_block);
        first_block = false;
    }

    for (size_t done = 0; done < len; done += ESP_SM3_BLOCK_LEN) {
        sha_hal_hash_block(SM3, input + done, ESP_SM3_BLOCK_LEN / 4, first_block);
        first_block = false;
    }
}

/* Run len bytes of input, plus any pending block, through the peripheral. */
static esp_err_t sm3_process(esp_sm3_ctx_handle_t ctx, const unsigned char *input,
                             size_t len, bool has_pending_block)
{
    /* Repeat the check here. An application can burn DIS_SM_CRYPT after
       esp_sm3_create(). From that point the peripheral must stay unreachable. */
    if (!sha_hal_is_sm3_supported()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* The hardware keeps no state between calls. Give it back the digest that
       the previous call read out, or start the mode on an empty context. */
    bool first_block = !ctx->state_valid;

    esp_crypto_sha_aes_lock_acquire();
    esp_crypto_sha_enable_periph_clk(true);
    sha_hal_wait_idle();
    sha_hal_set_mode(SM3);
    if (!first_block) {
        sha_hal_write_digest(SM3, ctx->state);
    }

    sm3_hash_blocks(ctx, input, len, has_pending_block, first_block);

    sha_hal_read_digest(SM3, ctx->state);
    ctx->state_valid = true;

    esp_crypto_sha_enable_periph_clk(false);
    esp_crypto_sha_aes_lock_release();

    return ESP_OK;
}

esp_err_t esp_sm3_create(esp_sm3_ctx_handle_t *ctx)
{
    if (ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *ctx = NULL;

    /* An eFuse can disable SM2 and SM3 permanently. On such a part the SHA
       peripheral gives no valid digest in mode SM3. */
    if (!sha_hal_is_sm3_supported()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    struct esp_sm3_ctx_s *sm3_ctx = heap_caps_calloc(1, sizeof(*sm3_ctx), MALLOC_CAP_INTERNAL);
    if (sm3_ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }
    *ctx = sm3_ctx;

    return ESP_OK;
}

esp_err_t esp_sm3_update(esp_sm3_ctx_handle_t ctx, const void *input, size_t ilen)
{
    if (ctx == NULL || (input == NULL && ilen != 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (ilen == 0) {
        return ESP_OK;
    }

    const unsigned char *in = (const unsigned char *)input;
    size_t left = (size_t)(ctx->total & (ESP_SM3_BLOCK_LEN - 1));
    size_t fill = ESP_SM3_BLOCK_LEN - left;
    bool has_pending_block = false;

    ctx->total += ilen;

    /* Complete the block that the previous call left pending. */
    if (left && ilen >= fill) {
        memcpy(ctx->buffer + left, in, fill);
        in += fill;
        ilen -= fill;
        left = 0;
        has_pending_block = true;
    }

    size_t len = ESP_ALIGN_DOWN(ilen, ESP_SM3_BLOCK_LEN);

    if (len || has_pending_block) {
        esp_err_t ret = sm3_process(ctx, in, len, has_pending_block);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    if (ilen > len) {
        memcpy(ctx->buffer + left, in + len, ilen - len);
    }

    return ESP_OK;
}

esp_err_t esp_sm3_finish(esp_sm3_ctx_handle_t ctx, uint8_t *output, size_t olen)
{
    if (ctx == NULL || output == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Check the buffer before any byte reaches it. */
    if (olen < ESP_SM3_DIGEST_LEN) {
        return ESP_ERR_INVALID_SIZE;
    }

    /* Read the counter before the update advances it. */
    uint64_t bits = ctx->total << 3;
    size_t last = (size_t)(ctx->total & (ESP_SM3_BLOCK_LEN - 1));
    size_t padn = (last < 56) ? (56 - last) : (120 - last);

    /* Stage the padding and the length field together. One update then closes
       the message with a single hold of the peripheral. */
    unsigned char tail[ESP_SM3_BLOCK_LEN + 8];

    memcpy(tail, sm3_padding, padn);
    sm3_put_be32((uint32_t)(bits >> 32), tail + padn);
    sm3_put_be32((uint32_t)bits, tail + padn + 4);

    esp_err_t ret = esp_sm3_update(ctx, tail, padn + 8);
    if (ret != ESP_OK) {
        return ret;
    }

    memcpy(output, ctx->state, ESP_SM3_DIGEST_LEN);

    return ESP_OK;
}

void esp_sm3_delete(esp_sm3_ctx_handle_t ctx)
{
    if (ctx != NULL) {
        esp_sm3_zeroize(ctx, sizeof(*ctx));
    }
    free(ctx);
}

esp_err_t esp_sm3(const void *input, size_t ilen, uint8_t *output, size_t olen)
{
    if (output == NULL || (input == NULL && ilen != 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (olen < ESP_SM3_DIGEST_LEN) {
        return ESP_ERR_INVALID_SIZE;
    }

    /* An eFuse can disable SM2 and SM3 permanently. On such a part the SHA
       peripheral gives no valid digest in mode SM3. */
    if (!sha_hal_is_sm3_supported()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* The context is private to this call, so keep it on the stack. The
       one-shot path then needs no allocation. */
    struct esp_sm3_ctx_s ctx;

    memset(&ctx, 0, sizeof(ctx));

    esp_err_t ret = esp_sm3_update(&ctx, input, ilen);
    if (ret == ESP_OK) {
        ret = esp_sm3_finish(&ctx, output, olen);
    }

    esp_sm3_zeroize(&ctx, sizeof(ctx));

    return ret;
}

#endif /* SOC_SHA_SUPPORT_SM3 */
