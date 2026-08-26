/*
 * SHA-1 implementation with hardware ESP support added.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2025-2026 Espressif Systems (Shanghai) CO LTD
 */

#include <string.h>
#include "psa_crypto_driver_esp_sha.h"
#include "../include/psa_crypto_driver_esp_sha1.h"
#include "sha/sha_parallel_engine.h"
#include "esp_err.h"
#include "mbedtls/platform_util.h"

#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

psa_status_t esp_sha1_driver_clone(const esp_sha1_context *source_ctx, esp_sha1_context *target_ctx)
{
    if (source_ctx == NULL || target_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(target_ctx, source_ctx, sizeof(esp_sha1_context));
    // If the source context is in hardware mode, we need to read the digest state
    // from the hardware engine to ensure the target context has the correct state
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (source_ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_read_digest_state(SHA1, target_ctx->state);
        target_ctx->operation_mode = ESP_SHA_MODE_SOFTWARE; // Cloned context operates in software mode
    }
#else
    target_ctx->operation_mode = ESP_SHA_MODE_SOFTWARE; // Cloned context operates in software mode
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
    return PSA_SUCCESS;
}

int esp_sha1_starts(esp_sha1_context *ctx)
{
    memset(ctx, 0, sizeof(esp_sha1_context));
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_unlock_engine(SHA1);
    }
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
    ctx->operation_mode = ESP_SHA_MODE_UNUSED;

    return ESP_OK;
}

static void esp_sha1_software_process( esp_sha1_context *ctx, const unsigned char data[64] )
{
    struct {
        uint32_t temp, W[16], A, B, C, D, E;
    } local;

    GET_UINT32_BE( local.W[ 0], data,  0 );
    GET_UINT32_BE( local.W[ 1], data,  4 );
    GET_UINT32_BE( local.W[ 2], data,  8 );
    GET_UINT32_BE( local.W[ 3], data, 12 );
    GET_UINT32_BE( local.W[ 4], data, 16 );
    GET_UINT32_BE( local.W[ 5], data, 20 );
    GET_UINT32_BE( local.W[ 6], data, 24 );
    GET_UINT32_BE( local.W[ 7], data, 28 );
    GET_UINT32_BE( local.W[ 8], data, 32 );
    GET_UINT32_BE( local.W[ 9], data, 36 );
    GET_UINT32_BE( local.W[10], data, 40 );
    GET_UINT32_BE( local.W[11], data, 44 );
    GET_UINT32_BE( local.W[12], data, 48 );
    GET_UINT32_BE( local.W[13], data, 52 );
    GET_UINT32_BE( local.W[14], data, 56 );
    GET_UINT32_BE( local.W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                            \
(                                                       \
    local.temp = local.W[( t -  3 ) & 0x0F] ^ local.W[( t - 8 ) & 0x0F] ^ \
           local.W[( t - 14 ) & 0x0F] ^ local.W[  t       & 0x0F],  \
    ( local.W[t & 0x0F] = S(local.temp,1) )                         \
)

#define P(a,b,c,d,e,x)                                  \
{                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
}

    local.A = ctx->state[0];
    local.B = ctx->state[1];
    local.C = ctx->state[2];
    local.D = ctx->state[3];
    local.E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

    P( local.A, local.B, local.C, local.D, local.E, local.W[0]  );
    P( local.E, local.A, local.B, local.C, local.D, local.W[1]  );
    P( local.D, local.E, local.A, local.B, local.C, local.W[2]  );
    P( local.C, local.D, local.E, local.A, local.B, local.W[3]  );
    P( local.B, local.C, local.D, local.E, local.A, local.W[4]  );
    P( local.A, local.B, local.C, local.D, local.E, local.W[5]  );
    P( local.E, local.A, local.B, local.C, local.D, local.W[6]  );
    P( local.D, local.E, local.A, local.B, local.C, local.W[7]  );
    P( local.C, local.D, local.E, local.A, local.B, local.W[8]  );
    P( local.B, local.C, local.D, local.E, local.A, local.W[9]  );
    P( local.A, local.B, local.C, local.D, local.E, local.W[10] );
    P( local.E, local.A, local.B, local.C, local.D, local.W[11] );
    P( local.D, local.E, local.A, local.B, local.C, local.W[12] );
    P( local.C, local.D, local.E, local.A, local.B, local.W[13] );
    P( local.B, local.C, local.D, local.E, local.A, local.W[14] );
    P( local.A, local.B, local.C, local.D, local.E, local.W[15] );
    P( local.E, local.A, local.B, local.C, local.D, R(16) );
    P( local.D, local.E, local.A, local.B, local.C, R(17) );
    P( local.C, local.D, local.E, local.A, local.B, R(18) );
    P( local.B, local.C, local.D, local.E, local.A, R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    P( local.A, local.B, local.C, local.D, local.E, R(20) );
    P( local.E, local.A, local.B, local.C, local.D, R(21) );
    P( local.D, local.E, local.A, local.B, local.C, R(22) );
    P( local.C, local.D, local.E, local.A, local.B, R(23) );
    P( local.B, local.C, local.D, local.E, local.A, R(24) );
    P( local.A, local.B, local.C, local.D, local.E, R(25) );
    P( local.E, local.A, local.B, local.C, local.D, R(26) );
    P( local.D, local.E, local.A, local.B, local.C, R(27) );
    P( local.C, local.D, local.E, local.A, local.B, R(28) );
    P( local.B, local.C, local.D, local.E, local.A, R(29) );
    P( local.A, local.B, local.C, local.D, local.E, R(30) );
    P( local.E, local.A, local.B, local.C, local.D, R(31) );
    P( local.D, local.E, local.A, local.B, local.C, R(32) );
    P( local.C, local.D, local.E, local.A, local.B, R(33) );
    P( local.B, local.C, local.D, local.E, local.A, R(34) );
    P( local.A, local.B, local.C, local.D, local.E, R(35) );
    P( local.E, local.A, local.B, local.C, local.D, R(36) );
    P( local.D, local.E, local.A, local.B, local.C, R(37) );
    P( local.C, local.D, local.E, local.A, local.B, R(38) );
    P( local.B, local.C, local.D, local.E, local.A, R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    P( local.A, local.B, local.C, local.D, local.E, R(40) );
    P( local.E, local.A, local.B, local.C, local.D, R(41) );
    P( local.D, local.E, local.A, local.B, local.C, R(42) );
    P( local.C, local.D, local.E, local.A, local.B, R(43) );
    P( local.B, local.C, local.D, local.E, local.A, R(44) );
    P( local.A, local.B, local.C, local.D, local.E, R(45) );
    P( local.E, local.A, local.B, local.C, local.D, R(46) );
    P( local.D, local.E, local.A, local.B, local.C, R(47) );
    P( local.C, local.D, local.E, local.A, local.B, R(48) );
    P( local.B, local.C, local.D, local.E, local.A, R(49) );
    P( local.A, local.B, local.C, local.D, local.E, R(50) );
    P( local.E, local.A, local.B, local.C, local.D, R(51) );
    P( local.D, local.E, local.A, local.B, local.C, R(52) );
    P( local.C, local.D, local.E, local.A, local.B, R(53) );
    P( local.B, local.C, local.D, local.E, local.A, R(54) );
    P( local.A, local.B, local.C, local.D, local.E, R(55) );
    P( local.E, local.A, local.B, local.C, local.D, R(56) );
    P( local.D, local.E, local.A, local.B, local.C, R(57) );
    P( local.C, local.D, local.E, local.A, local.B, R(58) );
    P( local.B, local.C, local.D, local.E, local.A, R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    P( local.A, local.B, local.C, local.D, local.E, R(60) );
    P( local.E, local.A, local.B, local.C, local.D, R(61) );
    P( local.D, local.E, local.A, local.B, local.C, R(62) );
    P( local.C, local.D, local.E, local.A, local.B, R(63) );
    P( local.B, local.C, local.D, local.E, local.A, R(64) );
    P( local.A, local.B, local.C, local.D, local.E, R(65) );
    P( local.E, local.A, local.B, local.C, local.D, R(66) );
    P( local.D, local.E, local.A, local.B, local.C, R(67) );
    P( local.C, local.D, local.E, local.A, local.B, R(68) );
    P( local.B, local.C, local.D, local.E, local.A, R(69) );
    P( local.A, local.B, local.C, local.D, local.E, R(70) );
    P( local.E, local.A, local.B, local.C, local.D, R(71) );
    P( local.D, local.E, local.A, local.B, local.C, R(72) );
    P( local.C, local.D, local.E, local.A, local.B, R(73) );
    P( local.B, local.C, local.D, local.E, local.A, R(74) );
    P( local.A, local.B, local.C, local.D, local.E, R(75) );
    P( local.E, local.A, local.B, local.C, local.D, R(76) );
    P( local.D, local.E, local.A, local.B, local.C, R(77) );
    P( local.C, local.D, local.E, local.A, local.B, R(78) );
    P( local.B, local.C, local.D, local.E, local.A, R(79) );

#undef K
#undef F

    ctx->state[0] += local.A;
    ctx->state[1] += local.B;
    ctx->state[2] += local.C;
    ctx->state[3] += local.D;
    ctx->state[4] += local.E;

    /* Zeroise buffers and variables to clear sensitive data from memory. */
    mbedtls_platform_zeroize(&local, sizeof(local));
}

static int esp_internal_sha1_parallel_engine_process( esp_sha1_context *ctx, const unsigned char data[64], bool read_digest )
{
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    bool first_block = false;

    if (ctx->operation_mode == ESP_SHA_MODE_UNUSED) {
        /* try to use hardware for this digest */
        if (esp_sha_try_lock_engine(SHA1)) {
            ctx->operation_mode = ESP_SHA_MODE_HARDWARE;
            first_block = true;
        } else {
            ctx->operation_mode = ESP_SHA_MODE_SOFTWARE;
        }
    }

    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_block(SHA1, data, first_block);
        if (read_digest) {
            esp_sha_read_digest_state(SHA1, ctx->state);
        }
    } else {
        esp_sha1_software_process(ctx, data);
    }
#else
    esp_sha1_software_process(ctx, data);
#endif
    return 0;
}

int esp_internal_sha1_process( esp_sha1_context *ctx, const unsigned char data[64] )
{
    return esp_internal_sha1_parallel_engine_process(ctx, data, true);
}


int esp_sha1_update(esp_sha1_context *ctx, const unsigned char *input, size_t ilen)
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

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_read_digest_state(SHA1, ctx->state);
    }
#endif // #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen );
    }

    return 0;
}

static const unsigned char sha1_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int esp_sha1_finish(esp_sha1_context *ctx, uint8_t *output)
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

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_read_digest_state(SHA1, ctx->state);
    }
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );

out:
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_unlock_engine(SHA1);
        ctx->operation_mode = ESP_SHA_MODE_SOFTWARE;
    }
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
    return ret;
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

psa_status_t esp_sha1_driver_abort(esp_sha1_context *ctx);

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
        goto hw_fail;
    }

    ret = esp_sha1_update(ctx, input, input_length);
    if (ret != ESP_OK) {
        goto hw_fail;
    }

    ret = esp_sha1_finish(ctx, hash);
    if (ret != ESP_OK) {
        goto hw_fail;
    }

    *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_1);
    return PSA_SUCCESS;

hw_fail:
    esp_sha1_driver_abort(ctx);
    return PSA_ERROR_HARDWARE_FAILURE;
}

psa_status_t esp_sha1_driver_abort(esp_sha1_context *ctx)
{
    if (!ctx) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    // Also unlock the hardware engine if it was in use
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_unlock_engine(SHA1);
        ctx->operation_mode = ESP_SHA_MODE_SOFTWARE;
    }
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
    mbedtls_platform_zeroize(ctx, sizeof(esp_sha1_context));
    return PSA_SUCCESS;
}
