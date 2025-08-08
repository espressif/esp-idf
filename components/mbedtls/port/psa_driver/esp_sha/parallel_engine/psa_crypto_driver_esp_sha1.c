/*
 * SHA-1 implementation with hardware ESP support added.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */

#include <string.h>
#include "psa_crypto_driver_esp_sha.h"
#include "../include/psa_crypto_driver_esp_sha1.h"
#include "sha/sha_parallel_engine.h"
#include "esp_err.h"

static const unsigned char sha1_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int esp_sha1_starts(esp_sha1_context *ctx) {
    memset(ctx, 0, sizeof(esp_sha1_context));
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    // esp_sha_unlock_engine(SHA1);
    ctx->sha_state = ESP_SHA1_STATE_INIT;
    return ESP_OK;
}

// static void esp_internal_sha_update_state(esp_sha1_context *ctx)
// {
//     if (ctx->sha_state == ESP_SHA1_STATE_INIT) {
//         ctx->first_block = true;
//         ctx->sha_state = ESP_SHA1_STATE_IN_PROCESS;
//     } else if (ctx->sha_state == ESP_SHA1_STATE_IN_PROCESS) {
//         ctx->first_block = false;
//         // esp_sha_write_digest_state(SHA1, ctx->state);
//     }
// }

static int esp_internal_sha1_parallel_engine_process( esp_sha1_context *ctx, const unsigned char data[64], bool read_digest )
{
    if (ctx->sha_state == ESP_SHA1_STATE_INIT) {
        if (esp_sha_try_lock_engine(SHA1)) {
            printf("Got the SHA1 engine lock\n");
            ctx->first_block = true;
            ctx->sha_state = ESP_SHA1_STATE_IN_PROCESS;
        } else {
            printf("Failed to lock SHA1 engine\n");
            return -1;
        }
    } else if (ctx->sha_state == ESP_SHA1_STATE_IN_PROCESS) {
        // printf("SHA1 in process\n");
        ctx->first_block = false;
    }
    esp_sha_block(SHA1, data, ctx->first_block);
    if (read_digest) {
        esp_sha_read_digest_state(SHA1, ctx->state);
    }

    return 0;
}

static int esp_sha1_update(esp_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    int ret = -1;
    size_t fill;
    uint32_t left;

    if ( ilen == 0 ) {
        return 0;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if ( ctx->total[0] < (uint32_t) ilen ) {
        ctx->total[1]++;
    }

    if ( left && ilen >= fill ) {
        memcpy( (void *) (ctx->buffer + left), input, fill );

        if ( ( ret = esp_internal_sha1_parallel_engine_process( ctx, ctx->buffer, false ) ) != 0 ) {
            return ret;
        }

        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while ( ilen >= 64 ) {
        if ( ( ret = esp_internal_sha1_parallel_engine_process( ctx, input, false ) ) != 0 ) {
            return ret;
        }

        input += 64;
        ilen  -= 64;
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen );
    }

    return 0;
}

psa_status_t esp_sha1_driver_update(
    esp_sha1_context *ctx,
    const uint8_t *input,
    size_t input_length)
{
    if (ctx == NULL || input == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha1_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return PSA_SUCCESS;
}

static int esp_sha1_finish(esp_sha1_context *ctx, uint8_t *output)
{
    int ret = -1;
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
           | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    if ((ret = esp_sha1_update(ctx, sha1_padding, padn)) != 0) {
        goto out;
    }
    if ((ret = esp_sha1_update(ctx, msglen, 8)) != 0) {
        goto out;
    }

    if (ctx->sha_state == ESP_SHA1_STATE_IN_PROCESS) {
        // If there is no more input data, and state is in hardware, read it out to ctx->state
        // This ensures that ctx->state always has the latest digest state
        esp_sha_read_digest_state(SHA1, ctx->state);
    }

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );

out:
    esp_sha_unlock_engine(SHA1);

    return ret;
}

psa_status_t esp_sha1_driver_finish(
    esp_sha1_context *ctx,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (ctx == NULL || hash == NULL || hash_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha1_finish(ctx, hash);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_1);
    return PSA_SUCCESS;
}

psa_status_t esp_sha1_driver_compute(
    esp_sha1_context *ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    int ret = esp_sha1_starts(ctx);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    ret = esp_sha1_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    ret = esp_sha1_finish(ctx, hash);
    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_1);
    return PSA_SUCCESS;
}
