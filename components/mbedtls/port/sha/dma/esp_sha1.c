/*
 * SHA-1 implementation with hardware ESP support added.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2023 Espressif Systems (Shanghai) CO LTD
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#include <mbedtls/build_info.h>

#if defined(MBEDTLS_SHA1_C) && defined(MBEDTLS_SHA1_ALT)

#include "mbedtls/sha1.h"

#include <string.h>

#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST */

#include "sha/sha_dma.h"

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n )
{
    volatile unsigned char *p = (unsigned char *)v;
    while ( n-- ) {
        *p++ = 0;
    }
}

/*
 * 32-bit integer manipulation macros (big endian)
 */

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if ( ctx == NULL ) {
        return;
    }
    mbedtls_zeroize( ctx, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    memcpy(dst, src, sizeof(mbedtls_sha1_context));
}

/*
 * SHA-1 context setup
 */
int mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    memset( ctx, 0, sizeof( mbedtls_sha1_context ) );
    ctx->mode = SHA1;

    return 0;
}

static void esp_internal_sha_update_state(mbedtls_sha1_context *ctx)
{
    if (ctx->sha_state == ESP_SHA1_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA1_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA1_STATE_IN_PROCESS) {
        ctx->first_block = false;
        esp_sha_write_digest_state(ctx->mode, ctx->state);
    }
}

static int esp_internal_sha1_dma_process(mbedtls_sha1_context *ctx,
        const uint8_t *data, size_t len,
        uint8_t *buf, size_t buf_len)
{
    return esp_sha_dma(SHA1, data, len, buf, buf_len, ctx->first_block);
}

int mbedtls_internal_sha1_process( mbedtls_sha1_context *ctx, const unsigned char data[64] )
{
    int ret = -1;
    esp_sha_acquire_hardware();
    esp_internal_sha_update_state(ctx);

    ret = esp_sha_dma(ctx->mode, data, 64, 0, 0, ctx->first_block);
    if (ret != 0) {
        esp_sha_release_hardware();
        return ret;
    }

    esp_sha_read_digest_state(ctx->mode, ctx->state);
    esp_sha_release_hardware();
    return ret;
}

int mbedtls_sha1_update( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    size_t fill;
    uint32_t left, len, local_len = 0;

    if ( !ilen || (input == NULL)) {
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

        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = (ilen / 64) * 64;
    if ( len || local_len) {

        esp_sha_acquire_hardware();

        esp_internal_sha_update_state(ctx);

        int ret = esp_internal_sha1_dma_process(ctx, input, len, ctx->buffer, local_len);
        if (ret != 0) {
            esp_sha_release_hardware();
            return ret;
        }

        esp_sha_read_digest_state(SHA1, ctx->state);

        esp_sha_release_hardware();

    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input + len, ilen - len );
    }

    return 0;
}

static const unsigned char sha1_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
* SHA-1 final digest
 */
int mbedtls_sha1_finish( mbedtls_sha1_context *ctx, unsigned char output[20] )
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


    if ( ( ret = mbedtls_sha1_update( ctx, sha1_padding, padn ) ) != 0 ) {
        return ret;
    }
    if ( ( ret = mbedtls_sha1_update( ctx, msglen, 8 ) ) != 0 ) {
        return ret;
    }

    memcpy(output, ctx->state, 20);

    return ret;
}

#endif /* MBEDTLS_SHA1_C && MBEDTLS_SHA1_ALT */
