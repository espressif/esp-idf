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
#include "../include/psa_crypto_driver_esp_sha512.h"
#include "sha/sha_parallel_engine.h"
#include "esp_err.h"

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define UL64(x) x##ui64
#else
#define UL64(x) x##ULL
#endif

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

#ifndef GET_UINT64_BE
#define GET_UINT64_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint64_t) (b)[(i)    ] << 56 )       \
        | ( (uint64_t) (b)[(i) + 1] << 48 )       \
        | ( (uint64_t) (b)[(i) + 2] << 40 )       \
        | ( (uint64_t) (b)[(i) + 3] << 32 )       \
        | ( (uint64_t) (b)[(i) + 4] << 24 )       \
        | ( (uint64_t) (b)[(i) + 5] << 16 )       \
        | ( (uint64_t) (b)[(i) + 6] <<  8 )       \
        | ( (uint64_t) (b)[(i) + 7]       );      \
}
#endif /* GET_UINT64_BE */

#ifndef PUT_UINT64_BE
#define PUT_UINT64_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 56 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 48 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 40 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 32 );       \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 6] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 7] = (unsigned char) ( (n)       );       \
}
#endif /* PUT_UINT64_BE */

inline static esp_sha_type sha_type(const esp_sha512_context *ctx)
{
    return ctx->mode;
}

static int esp_internal_sha512_parallel_engine_process( esp_sha512_context *ctx, const unsigned char data[128], bool read_digest )
{
    if (ctx->sha_state == ESP_SHA512_STATE_INIT) {
        if (esp_sha_try_lock_engine(SHA2_512)) {
            printf("Got the SHA512 engine lock\n");
            ctx->first_block = true;
            ctx->sha_state = ESP_SHA512_STATE_IN_PROCESS;
        } else {
            printf("Failed to lock SHA512 engine\n");
            return -1;
        }
    } else if (ctx->sha_state == ESP_SHA512_STATE_IN_PROCESS) {
        // printf("SHA512 in process\n");
        ctx->first_block = false;
    }
    esp_sha_block(sha_type(ctx), data, ctx->first_block);
    if (read_digest) {
        esp_sha_read_digest_state(sha_type(ctx), ctx->state);
    }

    return 0;
}

static int esp_sha512_update(esp_sha512_context *ctx, const unsigned char *input,
                               size_t ilen)
{
    int ret = -1;
    size_t fill;
    unsigned int left;

    if ( ilen == 0 ) {
        return 0;
    }

    left = (unsigned int) (ctx->total[0] & 0x7F);
    fill = 128 - left;

    ctx->total[0] += (uint64_t) ilen;

    if ( ctx->total[0] < (uint64_t) ilen ) {
        ctx->total[1]++;
    }

    if ( left && ilen >= fill ) {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        if ( ( ret = esp_internal_sha512_parallel_engine_process( ctx, ctx->buffer, false ) ) != 0 ) {
            return ret;
        }

        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while ( ilen >= 128 ) {
        if ( ( ret = esp_internal_sha512_parallel_engine_process( ctx, input, false ) ) != 0 ) {
            return ret;
        }

        input += 128;
        ilen  -= 128;
    }

    // esp_sha_read_digest_state(sha_type(ctx), ctx->state);

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen );
    }

    return 0;
}

static int esp_sha512_starts( esp_sha512_context *ctx, int mode )
{
    memset( ctx, 0, sizeof( esp_sha512_context ) );
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if ( mode == SHA2_512 ) {
        /* SHA-512 */
        ctx->state[0] = UL64(0x6A09E667F3BCC908);
        ctx->state[1] = UL64(0xBB67AE8584CAA73B);
        ctx->state[2] = UL64(0x3C6EF372FE94F82B);
        ctx->state[3] = UL64(0xA54FF53A5F1D36F1);
        ctx->state[4] = UL64(0x510E527FADE682D1);
        ctx->state[5] = UL64(0x9B05688C2B3E6C1F);
        ctx->state[6] = UL64(0x1F83D9ABFB41BD6B);
        ctx->state[7] = UL64(0x5BE0CD19137E2179);
    } else {
        /* SHA-384 */
        printf("SHA-384 Init\n");
        ctx->state[0] = UL64(0xCBBB9D5DC1059ED8);
        ctx->state[1] = UL64(0x629A292A367CD507);
        ctx->state[2] = UL64(0x9159015A3070DD17);
        ctx->state[3] = UL64(0x152FECD8F70E5939);
        ctx->state[4] = UL64(0x67332667FFC00B31);
        ctx->state[5] = UL64(0x8EB44A8768581511);
        ctx->state[6] = UL64(0xDB0C2E0D64F98FA7);
        ctx->state[7] = UL64(0x47B5481DBEFA4FA4);
    }

    ctx->mode = mode;
    // esp_sha_unlock_engine(sha_type(ctx));
    ctx->sha_state = ESP_SHA512_STATE_INIT;

    return 0;
}

static int esp_sha512_finish(esp_sha512_context *ctx, unsigned char *output)
{
    int ret = -1;
    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = ( ctx->total[0] >> 61 )
           | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,  msglen, 8 );

    last = (size_t)( ctx->total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    if ( ( ret = esp_sha512_update( ctx, sha512_padding, padn ) ) != 0 ) {
        goto out;
    }

    if ( ( ret = esp_sha512_update( ctx, msglen, 16 ) ) != 0 ) {
        goto out;
    }

    if (ctx->sha_state == ESP_SHA512_STATE_IN_PROCESS) {
        // If there is no more input data, and state is in hardware, read it out to ctx->state
        // This ensures that ctx->state always has the latest digest state
        esp_sha_read_digest_state(SHA2_512, ctx->state);
        ctx->sha_state = ESP_SHA512_STATE_INIT;
    }

    PUT_UINT64_BE( ctx->state[0], output,  0 );
    PUT_UINT64_BE( ctx->state[1], output,  8 );
    PUT_UINT64_BE( ctx->state[2], output, 16 );
    PUT_UINT64_BE( ctx->state[3], output, 24 );
    PUT_UINT64_BE( ctx->state[4], output, 32 );
    PUT_UINT64_BE( ctx->state[5], output, 40 );

    if ( ctx->mode == SHA2_512 ) {
        PUT_UINT64_BE( ctx->state[6], output, 48 );
        PUT_UINT64_BE( ctx->state[7], output, 56 );
    }

out:
    printf("Releasing SHA512 engine lock\n");
    esp_sha_unlock_engine(sha_type(ctx));

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
    printf("SHA512 Driver Compute\n");
    if (!hash || !hash_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (alg != PSA_ALG_SHA_512 && alg != PSA_ALG_SHA_384) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hash_size < PSA_HASH_LENGTH(alg)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    int mode = (alg == PSA_ALG_SHA_384) ? SHA2_384 : SHA2_512;
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
