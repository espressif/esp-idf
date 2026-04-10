/*
 * SHA-1 implementation with hardware ESP support added.
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
#include "../include/psa_crypto_driver_esp_sha1.h"
#include "esp_sha_internal.h"
#include "sha/sha_core.h"
#include "esp_err.h"
#include "soc/soc_caps.h"

#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

static const unsigned char sha1_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int esp_sha1_starts(esp_sha1_context *ctx) {
    memset(ctx, 0, sizeof(esp_sha1_context));
    return ESP_OK;
}

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
static void esp_internal_sha1_block_process(esp_sha1_context *ctx, const uint8_t *data)
{
    esp_sha_block(SHA1, data, ctx->first_block);

    if (ctx->first_block) {
        ctx->first_block = false;
    }
}

static void esp_internal_sha_update_state(esp_sha1_context *ctx)
{
    if (ctx->sha_state == ESP_SHA1_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA1_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA1_STATE_IN_PROCESS) {
        ctx->first_block = false;
        esp_sha_write_digest_state(SHA1, ctx->state);
    }
}

#else

static void esp_sha1_software_process( esp_sha1_context *ctx, const unsigned char data[64] )
{
    uint32_t temp, W[16], A, B, C, D, E;

    GET_UINT32_BE( W[ 0], data,  0 );
    GET_UINT32_BE( W[ 1], data,  4 );
    GET_UINT32_BE( W[ 2], data,  8 );
    GET_UINT32_BE( W[ 3], data, 12 );
    GET_UINT32_BE( W[ 4], data, 16 );
    GET_UINT32_BE( W[ 5], data, 20 );
    GET_UINT32_BE( W[ 6], data, 24 );
    GET_UINT32_BE( W[ 7], data, 28 );
    GET_UINT32_BE( W[ 8], data, 32 );
    GET_UINT32_BE( W[ 9], data, 36 );
    GET_UINT32_BE( W[10], data, 40 );
    GET_UINT32_BE( W[11], data, 44 );
    GET_UINT32_BE( W[12], data, 48 );
    GET_UINT32_BE( W[13], data, 52 );
    GET_UINT32_BE( W[14], data, 56 );
    GET_UINT32_BE( W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                            \
(                                                       \
    temp = W[( t -  3 ) & 0x0F] ^ W[( t - 8 ) & 0x0F] ^ \
           W[( t - 14 ) & 0x0F] ^ W[  t       & 0x0F],  \
    ( W[t & 0x0F] = S(temp,1) )                         \
)

#define P(a,b,c,d,e,x)                                  \
{                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

    P( A, B, C, D, E, W[0]  );
    P( E, A, B, C, D, W[1]  );
    P( D, E, A, B, C, W[2]  );
    P( C, D, E, A, B, W[3]  );
    P( B, C, D, E, A, W[4]  );
    P( A, B, C, D, E, W[5]  );
    P( E, A, B, C, D, W[6]  );
    P( D, E, A, B, C, W[7]  );
    P( C, D, E, A, B, W[8]  );
    P( B, C, D, E, A, W[9]  );
    P( A, B, C, D, E, W[10] );
    P( E, A, B, C, D, W[11] );
    P( D, E, A, B, C, W[12] );
    P( C, D, E, A, B, W[13] );
    P( B, C, D, E, A, W[14] );
    P( A, B, C, D, E, W[15] );
    P( E, A, B, C, D, R(16) );
    P( D, E, A, B, C, R(17) );
    P( C, D, E, A, B, R(18) );
    P( B, C, D, E, A, R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    P( A, B, C, D, E, R(20) );
    P( E, A, B, C, D, R(21) );
    P( D, E, A, B, C, R(22) );
    P( C, D, E, A, B, R(23) );
    P( B, C, D, E, A, R(24) );
    P( A, B, C, D, E, R(25) );
    P( E, A, B, C, D, R(26) );
    P( D, E, A, B, C, R(27) );
    P( C, D, E, A, B, R(28) );
    P( B, C, D, E, A, R(29) );
    P( A, B, C, D, E, R(30) );
    P( E, A, B, C, D, R(31) );
    P( D, E, A, B, C, R(32) );
    P( C, D, E, A, B, R(33) );
    P( B, C, D, E, A, R(34) );
    P( A, B, C, D, E, R(35) );
    P( E, A, B, C, D, R(36) );
    P( D, E, A, B, C, R(37) );
    P( C, D, E, A, B, R(38) );
    P( B, C, D, E, A, R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    P( A, B, C, D, E, R(40) );
    P( E, A, B, C, D, R(41) );
    P( D, E, A, B, C, R(42) );
    P( C, D, E, A, B, R(43) );
    P( B, C, D, E, A, R(44) );
    P( A, B, C, D, E, R(45) );
    P( E, A, B, C, D, R(46) );
    P( D, E, A, B, C, R(47) );
    P( C, D, E, A, B, R(48) );
    P( B, C, D, E, A, R(49) );
    P( A, B, C, D, E, R(50) );
    P( E, A, B, C, D, R(51) );
    P( D, E, A, B, C, R(52) );
    P( C, D, E, A, B, R(53) );
    P( B, C, D, E, A, R(54) );
    P( A, B, C, D, E, R(55) );
    P( E, A, B, C, D, R(56) );
    P( D, E, A, B, C, R(57) );
    P( C, D, E, A, B, R(58) );
    P( B, C, D, E, A, R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    P( A, B, C, D, E, R(60) );
    P( E, A, B, C, D, R(61) );
    P( D, E, A, B, C, R(62) );
    P( C, D, E, A, B, R(63) );
    P( B, C, D, E, A, R(64) );
    P( A, B, C, D, E, R(65) );
    P( E, A, B, C, D, R(66) );
    P( D, E, A, B, C, R(67) );
    P( C, D, E, A, B, R(68) );
    P( B, C, D, E, A, R(69) );
    P( A, B, C, D, E, R(70) );
    P( E, A, B, C, D, R(71) );
    P( D, E, A, B, C, R(72) );
    P( C, D, E, A, B, R(73) );
    P( B, C, D, E, A, R(74) );
    P( A, B, C, D, E, R(75) );
    P( E, A, B, C, D, R(76) );
    P( D, E, A, B, C, R(77) );
    P( C, D, E, A, B, R(78) );
    P( B, C, D, E, A, R(79) );

#undef K
#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
}
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1

int esp_internal_sha1_process( esp_sha1_context *ctx, const unsigned char data[64] )
{
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    esp_sha_acquire_hardware();

    esp_sha_set_mode(SHA1);

    esp_internal_sha_update_state(ctx);

#if SOC_SHA_SUPPORT_DMA
    // Unlikely to use DMA because data size is 64 bytes which is smaller than the DMA threshold
    if (unlikely(sha_operation_mode(64) == SHA_DMA_MODE)) {
        int ret = esp_sha_dma(SHA1, data, 64, NULL, 0, ctx->first_block);
        if (ret != 0) {
            esp_sha_release_hardware();
            return ret;
        }
    } else
#endif /* SOC_SHA_SUPPORT_DMA */
    {
        esp_sha_block(SHA1, data, ctx->first_block);
    }

    esp_sha_read_digest_state(SHA1, ctx->state);
    esp_sha_release_hardware();
#else
    esp_sha1_software_process(ctx, data);
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
    return 0;

}

int esp_sha1_update(esp_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    size_t fill, left, len;
    uint32_t local_len = 0;

    if (!ilen || (input == NULL)) {
        return 0;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), input, fill);
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = SHA_ALIGN_DOWN(ilen , 64);

    if (len || local_len) {
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
        esp_sha_acquire_hardware();

        esp_sha_set_mode(SHA1);

        esp_internal_sha_update_state(ctx);

#if SOC_SHA_SUPPORT_DMA
        if (sha_operation_mode(len) == SHA_DMA_MODE) {
            int ret = esp_sha_dma(SHA1, input, len, ctx->buffer, local_len, ctx->first_block);
            if (ret != 0) {
                esp_sha_release_hardware();
                return ret;
            }
        } else
#endif /* SOC_SHA_SUPPORT_DMA */
        {
            /* First process buffered block, if any */
            if (local_len) {
                esp_internal_sha1_block_process(ctx, ctx->buffer);
            }

            uint32_t length_processed = 0;
            while (len - length_processed != 0) {
                esp_internal_sha1_block_process(ctx, input + length_processed);
                length_processed += 64;
            }
        }

        esp_sha_read_digest_state(SHA1, ctx->state);

        esp_sha_release_hardware();
#else
        esp_sha1_software_process(ctx, input);
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
    }

    if (ilen > 0) {
        memcpy((void *) (ctx->buffer + left), input + len, ilen - len);
    }
    return 0;
}

int esp_sha1_finish(esp_sha1_context *ctx, uint8_t *hash)
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

    if ((ret = esp_sha1_update(ctx, sha1_padding, padn)) != 0) {
        return ret;
    }
    if ((ret = esp_sha1_update(ctx, msglen, 8)) != 0) {
        return ret;
    }

    memcpy(hash, ctx->state, 20);

    return ret;
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

psa_status_t esp_sha1_driver_abort(esp_sha1_context *ctx)
{
    if (!ctx) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memset(ctx, 0, sizeof(esp_sha1_context));
    return PSA_SUCCESS;
}

psa_status_t esp_sha1_driver_clone(const esp_sha1_context *source_ctx, esp_sha1_context *target_ctx)
{
    if (source_ctx == NULL || target_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(target_ctx, source_ctx, sizeof(esp_sha1_context));
    return PSA_SUCCESS;
}
