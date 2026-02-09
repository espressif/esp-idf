/*
 * SHA-512 implementation with hardware ESP support added.
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
#include "../include/psa_crypto_driver_esp_sha512.h"
#include "esp_sha_internal.h"
#include "sha/sha_core.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_SOC_SHA_SUPPORT_SHA512

#ifndef PUT_UINT64_BE
#define PUT_UINT64_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ((n) >> 56);       \
    (b)[(i) + 1] = (unsigned char) ((n) >> 48);       \
    (b)[(i) + 2] = (unsigned char) ((n) >> 40);       \
    (b)[(i) + 3] = (unsigned char) ((n) >> 32);       \
    (b)[(i) + 4] = (unsigned char) ((n) >> 24);       \
    (b)[(i) + 5] = (unsigned char) ((n) >> 16);       \
    (b)[(i) + 6] = (unsigned char) ((n) >>  8);       \
    (b)[(i) + 7] = (unsigned char) ((n)      );       \
}
#endif /* PUT_UINT64_BE */

static const unsigned char sha512_padding[128] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

psa_status_t esp_sha512_starts(esp_sha512_context *ctx, int mode) {
    memset(ctx, 0, sizeof(esp_sha512_context));
    ctx->mode = mode;
    return PSA_SUCCESS;
}

static int esp_internal_sha_update_state(esp_sha512_context *ctx)
{
    if (ctx->sha_state == ESP_SHA512_STATE_INIT) {
        if (ctx->mode == SHA2_512T) {
            int ret = -1;
            if ((ret = esp_sha_512_t_init_hash(ctx->t_val)) != 0) {
                return ret;
            }
            ctx->first_block = false;
        } else {
            ctx->first_block = true;
        }
        ctx->sha_state = ESP_SHA512_STATE_IN_PROCESS;

    } else if (ctx->sha_state == ESP_SHA512_STATE_IN_PROCESS) {
        ctx->first_block = false;
        esp_sha_write_digest_state(ctx->mode, ctx->state);
    }
    return 0;
}

static void esp_internal_sha512_block_process(esp_sha512_context *ctx, const uint8_t *data)
{
    esp_sha_block(ctx->mode, data, ctx->first_block);

    if (ctx->first_block) {
        ctx->first_block = false;
    }
}

static int esp_sha512_update(esp_sha512_context *ctx, const unsigned char *input,
                               size_t ilen)
{
    size_t fill, left, len;
    uint32_t local_len = 0;

    if (ilen == 0) {
        return 0;
    }

    left = (size_t) (ctx->total[0] & 0x7F);
    fill = 128 - left;

    ctx->total[0] += (uint64_t) ilen;
    if (ctx->total[0] < (uint64_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), input, fill);

        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 128;
    }

    len = SHA_ALIGN_DOWN(ilen , 128);

    if (len || local_len) {

        esp_sha_acquire_hardware();

        esp_sha_set_mode(ctx->mode);

        int ret = esp_internal_sha_update_state(ctx);

        if (ret != 0) {
            esp_sha_release_hardware();
            return ret;
        }

#if SOC_SHA_SUPPORT_DMA
        if (sha_operation_mode(len) == SHA_DMA_MODE) {
            ret = esp_sha_dma(ctx->mode, input, len, ctx->buffer, local_len, ctx->first_block);
            if (ret != 0) {
                esp_sha_release_hardware();
                return ret;
            }
        } else
#endif /* SOC_SHA_SUPPORT_DMA */
        {
            /* First process buffered block, if any */
            if (local_len) {
                esp_internal_sha512_block_process(ctx, ctx->buffer);
            }

            uint32_t length_processed = 0;
            while (len - length_processed != 0) {
                esp_internal_sha512_block_process(ctx, input + length_processed);
                length_processed += 128;
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

static int esp_sha512_finish(esp_sha512_context *ctx, unsigned char *output)
{
    int ret = -1;
    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = (ctx->total[0] >> 61)
           | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    PUT_UINT64_BE(high, msglen, 0);
    PUT_UINT64_BE(low,  msglen, 8);

    last = (size_t)(ctx->total[0] & 0x7F);
    padn = (last < 112) ? (112 - last) : (240 - last);

    if ((ret = esp_sha512_update(ctx, sha512_padding, padn)) != 0) {
        return ret;
    }

    if ((ret = esp_sha512_update(ctx, msglen, 16)) != 0) {
        return ret;
    }

#if SOC_SHA_SUPPORT_SHA384
    if (ctx->mode == SHA2_384) {
        memcpy(output, ctx->state, 48);
    } else
#endif // SOC_SHA_SUPPORT_SHA384
    {
        memcpy(output, ctx->state, 64);
    }

    return ret;
}

psa_status_t esp_sha512_driver_compute(
    esp_sha512_context *ctx,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (!hash || !hash_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (alg != PSA_ALG_SHA_512 && alg != PSA_ALG_SHA_384) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hash_size < PSA_HASH_LENGTH(alg)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    int mode = SHA2_512;
#if SOC_SHA_SUPPORT_SHA384
    mode = (alg == PSA_ALG_SHA_384) ? SHA2_384 : SHA2_512;
#endif // SOC_SHA_SUPPORT_SHA384
    int ret = esp_sha512_starts(ctx, mode);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    ret = esp_sha512_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    ret = esp_sha512_finish(ctx, hash);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    *hash_length = PSA_HASH_LENGTH(alg);
    return PSA_SUCCESS;
}

psa_status_t esp_sha512_driver_update(
    esp_sha512_context *ctx,
    const uint8_t *input,
    size_t input_length)
{
    if (ctx == NULL || input == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha512_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_sha512_driver_finish(
    esp_sha512_context *ctx,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length,
    esp_sha_operation_type_t sha_type)
{
    if (ctx == NULL || hash == NULL || hash_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha512_finish(ctx, hash);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    if (sha_type == ESP_SHA_OPERATION_TYPE_SHA384) {
        *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_384);
    } else if (sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_512);
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hash_size < *hash_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_sha512_driver_abort(esp_sha512_context *ctx)
{
    if (!ctx) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memset(ctx, 0, sizeof(esp_sha512_context));
    return PSA_SUCCESS;
}

psa_status_t esp_sha512_driver_clone(const esp_sha512_context *source_ctx, esp_sha512_context *target_ctx)
{
    if (source_ctx == NULL || target_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(target_ctx, source_ctx, sizeof(esp_sha512_context));
    return PSA_SUCCESS;
}

#endif // SOC_SHA_SUPPORT_SHA512
