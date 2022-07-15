/*
 * SHA-256 implementation with hardware ESP support added.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include <mbedtls/build_info.h>

#if defined(MBEDTLS_SHA256_C) && defined(MBEDTLS_SHA256_ALT)

#include "mbedtls/sha256.h"

#include <string.h>
#include <assert.h>

#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST */

#include "sha/sha_block.h"

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n )
{
    volatile unsigned char *p = v;
    while ( n-- ) {
        *p++ = 0;
    }
}

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

void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    assert(ctx != NULL);

    memset( ctx, 0, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if ( ctx == NULL ) {
        return;
    }

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 )
{
    memset( ctx, 0, sizeof( mbedtls_sha256_context ) );

    if ( is224 ) {
        ctx->mode = SHA2_224;
    } else {
        ctx->mode = SHA2_256;
    }

    return 0;
}

static void esp_internal_sha256_block_process(mbedtls_sha256_context *ctx, const uint8_t *data)
{
    esp_sha_block(ctx->mode, data, ctx->first_block);

    if (ctx->first_block) {
        ctx->first_block = false;
    }
}

int mbedtls_internal_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    esp_sha_acquire_hardware();
    esp_sha_block(ctx->mode, data, ctx->first_block);
    esp_sha_release_hardware();
    return 0;
}

/*
 * SHA-256 process buffer
 */
int mbedtls_sha256_update( mbedtls_sha256_context *ctx, const unsigned char *input,
                               size_t ilen )
{
    size_t fill;
    uint32_t left, local_len = 0;

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

    /* Check if any data pending from previous call to this API */
    if ( left && ilen >= fill ) {
        memcpy( (void *) (ctx->buffer + left), input, fill );

        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    if ( (ilen >= 64) || local_len) {

        esp_sha_acquire_hardware();
        if (ctx->sha_state == ESP_SHA256_STATE_INIT) {
            ctx->first_block = true;

            ctx->sha_state = ESP_SHA256_STATE_IN_PROCESS;
        } else if (ctx->sha_state == ESP_SHA256_STATE_IN_PROCESS) {
            esp_sha_write_digest_state(ctx->mode, ctx->state);
        }

        /* First process buffered block, if any */
        if ( local_len ) {
            esp_internal_sha256_block_process(ctx, ctx->buffer);
        }

        while ( ilen >= 64 ) {
            esp_internal_sha256_block_process(ctx, input);

            input += 64;
            ilen  -= 64;
        }
        esp_sha_read_digest_state(ctx->mode, ctx->state);

        esp_sha_release_hardware();

    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen);
    }

    return 0;
}

static const unsigned char sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish( mbedtls_sha256_context *ctx, unsigned char *output )
{
    int ret;
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

    if ( ( ret = mbedtls_sha256_update( ctx, sha256_padding, padn ) ) != 0 ) {
        return ret;
    }

    if ( ( ret = mbedtls_sha256_update( ctx, msglen, 8 ) ) != 0 ) {
        return ret;
    }

    memcpy(output, ctx->state, 32);

    return ret;
}

#endif /* MBEDTLS_SHA256_C && MBEDTLS_SHA256_ALT */
