/*
 * SHA-256 implementation with hardware ESP support added.
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
#include "soc/soc_caps.h"

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
do {                                                    \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
} while( 0 )
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
do {                                                    \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
} while( 0 )
#endif

psa_status_t esp_sha256_driver_clone(const esp_sha256_context *source_ctx, esp_sha256_context *target_ctx)
{
    if (source_ctx == NULL || target_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(target_ctx, source_ctx, sizeof(esp_sha256_context));
    // If the source context is in hardware mode, we need to read the digest state
    // from the hardware engine to ensure the target context has the correct state
    if (source_ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_read_digest_state(SHA2_256, target_ctx->state);
        target_ctx->operation_mode = ESP_SHA_MODE_SOFTWARE; // Cloned context operates in software mode
    }
    return PSA_SUCCESS;
}

psa_status_t esp_sha256_starts(esp_sha256_context *ctx, int mode)
{
    memset(ctx, 0, sizeof(esp_sha256_context));
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if ( mode == SHA2_256 ) {
        /* SHA-256 */
        ctx->state[0] = 0x6A09E667;
        ctx->state[1] = 0xBB67AE85;
        ctx->state[2] = 0x3C6EF372;
        ctx->state[3] = 0xA54FF53A;
        ctx->state[4] = 0x510E527F;
        ctx->state[5] = 0x9B05688C;
        ctx->state[6] = 0x1F83D9AB;
        ctx->state[7] = 0x5BE0CD19;
    } else {
        /* SHA-224 */
        ctx->state[0] = 0xC1059ED8;
        ctx->state[1] = 0x367CD507;
        ctx->state[2] = 0x3070DD17;
        ctx->state[3] = 0xF70E5939;
        ctx->state[4] = 0xFFC00B31;
        ctx->state[5] = 0x68581511;
        ctx->state[6] = 0x64F98FA7;
        ctx->state[7] = 0xBEFA4FA4;
    }

    ctx->mode = mode;
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_unlock_engine(SHA2_256);
    }
    ctx->operation_mode = ESP_SHA_MODE_UNUSED;
    return PSA_SUCCESS;
}

static const uint32_t K[] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
(                                               \
    W[t] = S1(W[t -  2]) + W[t -  7] +          \
           S0(W[t - 15]) + W[t - 16]            \
)

#define P(a,b,c,d,e,f,g,h,x,K)                  \
{                                               \
    temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
    temp2 = S2(a) + F0(a,b,c);                  \
    d += temp1; h = temp1 + temp2;              \
}

static void esp_sha256_software_process(esp_sha256_context *ctx, const unsigned char data[64])
{
    uint32_t temp1, temp2, W[64] = {0};
    uint32_t A[8] = {0};
    unsigned int i = 0;

    for ( i = 0; i < 8; i++ ) {
        A[i] = ctx->state[i];
    }

#if defined(MBEDTLS_SHA256_SMALLER)
    for ( i = 0; i < 64; i++ ) {
        if ( i < 16 ) {
            GET_UINT32_BE( W[i], data, 4 * i );
        } else {
            R( i );
        }

        P( A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], W[i], K[i] );

        temp1 = A[7]; A[7] = A[6]; A[6] = A[5]; A[5] = A[4]; A[4] = A[3];
        A[3] = A[2]; A[2] = A[1]; A[1] = A[0]; A[0] = temp1;
    }
#else /* MBEDTLS_SHA256_SMALLER */
    for ( i = 0; i < 16; i++ ) {
        GET_UINT32_BE( W[i], data, 4 * i );
    }

    for ( i = 0; i < 16; i += 8 ) {
        P( A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], W[i + 0], K[i + 0] );
        P( A[7], A[0], A[1], A[2], A[3], A[4], A[5], A[6], W[i + 1], K[i + 1] );
        P( A[6], A[7], A[0], A[1], A[2], A[3], A[4], A[5], W[i + 2], K[i + 2] );
        P( A[5], A[6], A[7], A[0], A[1], A[2], A[3], A[4], W[i + 3], K[i + 3] );
        P( A[4], A[5], A[6], A[7], A[0], A[1], A[2], A[3], W[i + 4], K[i + 4] );
        P( A[3], A[4], A[5], A[6], A[7], A[0], A[1], A[2], W[i + 5], K[i + 5] );
        P( A[2], A[3], A[4], A[5], A[6], A[7], A[0], A[1], W[i + 6], K[i + 6] );
        P( A[1], A[2], A[3], A[4], A[5], A[6], A[7], A[0], W[i + 7], K[i + 7] );
    }

    for ( i = 16; i < 64; i += 8 ) {
        P( A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], R(i + 0), K[i + 0] );
        P( A[7], A[0], A[1], A[2], A[3], A[4], A[5], A[6], R(i + 1), K[i + 1] );
        P( A[6], A[7], A[0], A[1], A[2], A[3], A[4], A[5], R(i + 2), K[i + 2] );
        P( A[5], A[6], A[7], A[0], A[1], A[2], A[3], A[4], R(i + 3), K[i + 3] );
        P( A[4], A[5], A[6], A[7], A[0], A[1], A[2], A[3], R(i + 4), K[i + 4] );
        P( A[3], A[4], A[5], A[6], A[7], A[0], A[1], A[2], R(i + 5), K[i + 5] );
        P( A[2], A[3], A[4], A[5], A[6], A[7], A[0], A[1], R(i + 6), K[i + 6] );
        P( A[1], A[2], A[3], A[4], A[5], A[6], A[7], A[0], R(i + 7), K[i + 7] );
    }
#endif /* MBEDTLS_SHA256_SMALLER */

    for ( i = 0; i < 8; i++ ) {
        ctx->state[i] += A[i];
    }
}
static int esp_internal_sha256_parallel_engine_process(esp_sha256_context *ctx, const unsigned char data[64], bool read_digest)
{
    bool first_block = false;

    if (ctx->operation_mode == ESP_SHA_MODE_UNUSED) {
        /* try to use hardware for this digest */
        if (esp_sha_try_lock_engine(SHA2_256)
#if SOC_SHA_SUPPORT_SHA224
        && (ctx->mode != SHA2_224)
#endif
    ) {
            ctx->operation_mode = ESP_SHA_MODE_HARDWARE;
            first_block = true;
        } else {
            ctx->operation_mode = ESP_SHA_MODE_SOFTWARE;
        }
    }

    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_block(SHA2_256, data, first_block);
        if (read_digest) {
            esp_sha_read_digest_state(SHA2_256, ctx->state);
        }
    } else {
        esp_sha256_software_process(ctx, data);
    }

    return 0;
}

int esp_internal_sha256_process( esp_sha256_context *ctx, const unsigned char data[64] )
{
    return esp_internal_sha256_parallel_engine_process(ctx, data, true);
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

    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_read_digest_state(SHA2_256, ctx->state);
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen );
    }

    return 0;
}

static const unsigned char sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

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

    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_read_digest_state(SHA2_256, ctx->state);
    }

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );
    PUT_UINT32_BE( ctx->state[5], output, 20 );
    PUT_UINT32_BE( ctx->state[6], output, 24 );
    PUT_UINT32_BE( ctx->state[7], output, 28 );


out:
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_unlock_engine(SHA2_256);
        ctx->operation_mode = ESP_SHA_MODE_SOFTWARE;
    }
    return ret;
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

psa_status_t esp_sha256_driver_abort(esp_sha256_context *ctx)
{
    if (!ctx) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    // Also unlock the hardware engine if it was in use
    if (ctx->operation_mode == ESP_SHA_MODE_HARDWARE) {
        esp_sha_unlock_engine(SHA2_256);
        ctx->operation_mode = ESP_SHA_MODE_SOFTWARE;
    }
    memset(ctx, 0, sizeof(esp_sha256_context));
    return PSA_SUCCESS;
}
