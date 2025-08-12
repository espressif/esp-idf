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
#include "../include/psa_crypto_driver_esp_sha256.h"
#include "sha/sha_parallel_engine.h"
#include "esp_err.h"

static const unsigned char sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int esp_internal_sha256_parallel_engine_process( esp_sha256_context *ctx, const unsigned char data[64], bool read_digest )
{
    if (ctx->sha_state == ESP_SHA256_STATE_INIT) {
        if (esp_sha_try_lock_engine(SHA2_256)) {
            printf("Got the SHA2_256 engine lock\n");
            ctx->first_block = true;
            ctx->sha_state = ESP_SHA256_STATE_IN_PROCESS;
        } else {
            printf("Failed to lock SHA2_256 engine\n");
            return -1;
        }
    } else if (ctx->sha_state == ESP_SHA256_STATE_IN_PROCESS) {
        // printf("SHA1 in process\n");
        ctx->first_block = false;
    }
    esp_sha_block(SHA2_256, data, ctx->first_block);
    if (read_digest) {
        esp_sha_read_digest_state(SHA2_256, ctx->state);
    }

    return 0;
}

static int esp_sha256_update(esp_sha256_context *ctx, const unsigned char *input,
                               size_t ilen)
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

        if ( ( ret = esp_internal_sha256_parallel_engine_process( ctx, ctx->buffer, false ) ) != 0 ) {
            return ret;
        }

        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while ( ilen >= 64 ) {
        if ( ( ret = esp_internal_sha256_parallel_engine_process( ctx, input, false ) ) != 0 ) {
            return ret;
        }

        input += 64;
        ilen  -= 64;
    }

    // esp_sha_read_digest_state(SHA2_256, ctx->state);


    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen );
    }

    return 0;
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

static int esp_sha256_starts( esp_sha256_context *ctx, int is224 )
{
    memset( ctx, 0, sizeof( esp_sha256_context ) );
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x6A09E667;
    ctx->state[1] = 0xBB67AE85;
    ctx->state[2] = 0x3C6EF372;
    ctx->state[3] = 0xA54FF53A;
    ctx->state[4] = 0x510E527F;
    ctx->state[5] = 0x9B05688C;
    ctx->state[6] = 0x1F83D9AB;
    ctx->state[7] = 0x5BE0CD19;

    // esp_sha_unlock_engine(SHA2_256);
    ctx->sha_state = ESP_SHA256_STATE_INIT;
    return 0;
}

static int esp_sha256_finish(esp_sha256_context *ctx, unsigned char *output)
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

    if ( ( ret = esp_sha256_update( ctx, sha256_padding, padn ) ) != 0 ) {
        goto out;
    }

    if ( ( ret = esp_sha256_update( ctx, msglen, 8 ) ) != 0 ) {
        goto out;
    }

    esp_sha_read_digest_state(SHA2_256, ctx->state);

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );
    PUT_UINT32_BE( ctx->state[5], output, 20 );
    PUT_UINT32_BE( ctx->state[6], output, 24 );

    PUT_UINT32_BE( ctx->state[7], output, 28 );


out:
    esp_sha_unlock_engine(SHA2_256);
    return ret;
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
    if (sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_224);
    } else if (sha_type == ESP_SHA_OPERATION_TYPE_SHA256) {
        *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_256);
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hash_size < *hash_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    return PSA_SUCCESS;
}
