/*
 * SHA-256 implementation with hardware ESP support added.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */

#include <stdio.h>
#include <string.h>
#include "mbedtls/esp_config.h"
#include "psa_crypto_driver_esp_sha.h"
#include "../include/psa_crypto_driver_esp_sha256.h"
#include "esp_sha_internal.h"
#include "sha/sha_core.h"
#include "esp_err.h"
#include "soc/soc_caps.h"

static const unsigned char sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

psa_status_t esp_sha256_starts(esp_sha256_context *ctx, int mode) {
    memset(ctx, 0, sizeof(esp_sha256_context));
    ctx->mode = mode; /* SHA2_224 or SHA2_256 */
    return PSA_SUCCESS;
}

static void esp_internal_sha256_block_process(esp_sha256_context *ctx, const uint8_t *data)
{
    esp_sha_block(ctx->mode, data, ctx->first_block);

    if (ctx->first_block) {
        ctx->first_block = false;
    }
}

static void esp_internal_sha_update_state(esp_sha256_context *ctx)
{
    if (ctx->sha_state == ESP_SHA256_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA256_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA256_STATE_IN_PROCESS) {
        ctx->first_block = false;
        esp_sha_write_digest_state(ctx->mode, ctx->state);
    }
}

static int esp_sha256_update(esp_sha256_context *ctx, const unsigned char *input,
                               size_t ilen)
{
    size_t fill, left, len;
    uint32_t local_len = 0;

    if (ilen == 0 || input == NULL) {
        return 0;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    /* Check if any data pending from previous call to this API */
    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), input, fill);

        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = SHA_ALIGN_DOWN(ilen , 64);

    if (len || local_len) {

        esp_sha_acquire_hardware();

        esp_sha_set_mode(ctx->mode);

        esp_internal_sha_update_state(ctx);

#if SOC_SHA_SUPPORT_DMA
        if (sha_operation_mode(len) == SHA_DMA_MODE) {
            int ret = esp_sha_dma(ctx->mode, input, len, ctx->buffer, local_len, ctx->first_block);
            if (ret != 0) {
                esp_sha_release_hardware();
                return ret;
            }
        } else
#endif /* SOC_SHA_SUPPORT_DMA */
        {
            /* First process buffered block, if any */
            if (local_len) {
                esp_internal_sha256_block_process(ctx, ctx->buffer);
            }

            uint32_t length_processed = 0;
            while (len - length_processed != 0) {
                esp_internal_sha256_block_process(ctx, input + length_processed);
                length_processed += 64;
            }
        }

        esp_sha_read_digest_state(ctx->mode, ctx->state);

        esp_sha_release_hardware();
    }

    if (ilen > 0) {
        memcpy((void *) (ctx->buffer + left), input + len, ilen - len);
    }

    return 0;
}

static int esp_sha256_finish(esp_sha256_context *ctx, unsigned char *output)
{
    int ret = -1;
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = (ctx->total[0] >> 29)
           | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    PUT_UINT32_BE(high, msglen, 0);
    PUT_UINT32_BE(low,  msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    if ((ret = esp_sha256_update(ctx, sha256_padding, padn)) != 0) {
        return ret;
    }

    if ((ret = esp_sha256_update(ctx, msglen, 8)) != 0) {
        return ret;
    }

    if (ctx->mode == SHA2_224) {
        memcpy(output, ctx->state, 28);
    } else {
        memcpy(output, ctx->state, 32);
    }

    return 0;
}


psa_status_t esp_sha256_driver_compute(
    esp_sha256_context *ctx,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    // printf("SHA256 Driver Compute\n");
    if (!hash || !hash_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (alg != PSA_ALG_SHA_256
#if SOC_SHA_SUPPORT_SHA224
        && alg != PSA_ALG_SHA_224
#endif // SOC_SHA_SUPPORT_SHA224
    ) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hash_size < PSA_HASH_LENGTH(alg)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
#if SOC_SHA_SUPPORT_SHA224
    int mode = (alg == PSA_ALG_SHA_224) ? SHA2_224 : SHA2_256;
#else
    int mode = SHA2_256;
#endif // SOC_SHA_SUPPORT_SHA224
    int ret = esp_sha256_starts(ctx, mode);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    ret = esp_sha256_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    ret = esp_sha256_finish(ctx, hash);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    *hash_length = PSA_HASH_LENGTH(alg);
    return PSA_SUCCESS;
}

psa_status_t esp_sha256_driver_update(
    esp_sha256_context *ctx,
    const uint8_t *input,
    size_t input_length)
{
    if (ctx == NULL || input == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha256_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_sha256_driver_finish(
    esp_sha256_context *ctx,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length,
    esp_sha_operation_type_t sha_type)
{
    if (ctx == NULL || hash == NULL || hash_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha256_finish(ctx, hash);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
#if SOC_SHA_SUPPORT_SHA224
    if (sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_224);
    } else
#endif // SOC_SHA_SUPPORT_SHA224
    if (sha_type == ESP_SHA_OPERATION_TYPE_SHA256) {
        *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_256);
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hash_size < *hash_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_sha256_driver_abort(esp_sha256_context *ctx)
{
    if (!ctx) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memset(ctx, 0, sizeof(esp_sha256_context));
    return PSA_SUCCESS;
}

psa_status_t esp_sha256_driver_clone(const esp_sha256_context *source_ctx, esp_sha256_context *target_ctx)
{
    if (source_ctx == NULL || target_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(target_ctx, source_ctx, sizeof(esp_sha256_context));
    return PSA_SUCCESS;
}
