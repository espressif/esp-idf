/*
 *  SHA-512 implementation with hardware ESP support added.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016-2020, Espressif Systems (Shanghai) PTE LTD
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

/*
 *  The SHA-512 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_SHA512_ALT)

#include "mbedtls/sha512.h"

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define UL64(x) x##ui64
#else
#define UL64(x) x##ULL
#endif

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
    volatile unsigned char *p = v;
    while ( n-- ) {
        *p++ = 0;
    }
}

/*
 * 64-bit integer manipulation macros (big endian)
 */
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

void esp_sha512_set_mode(mbedtls_sha512_context *ctx, esp_sha_type type)
{
    switch (type) {
    case SHA2_384:
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
        ctx->mode = type;
        break;
    default:
        ctx->mode = SHA2_512;
        break;
    }
}


/* For SHA512/t mode the intial hash value will depend on t */
void esp_sha512_set_t( mbedtls_sha512_context *ctx, uint16_t t_val)
{
    ctx->t_val = t_val;
}

void mbedtls_sha512_init( mbedtls_sha512_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_free( mbedtls_sha512_context *ctx )
{
    if ( ctx == NULL ) {
        return;
    }

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_clone( mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src )
{
    memcpy(dst, src, sizeof(mbedtls_sha512_context));
}

/*
 * SHA-512 context setup
 */
int mbedtls_sha512_starts_ret( mbedtls_sha512_context *ctx, int is384 )
{
    mbedtls_zeroize( ctx, sizeof( mbedtls_sha512_context ) );

    if ( is384 ) {
        ctx->mode = SHA2_384;
    } else {
        ctx->mode = SHA2_512;
    }

    return 0;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_starts( mbedtls_sha512_context *ctx,
                            int is384 )
{
    mbedtls_sha512_starts_ret( ctx, is384 );
}
#endif

static int esp_internal_sha512_dma_process(mbedtls_sha512_context *ctx,
        const uint8_t *data, size_t len,
        uint8_t *buf, size_t buf_len)
{


    return esp_sha_dma(ctx->mode, data, len, buf, buf_len, ctx->first_block);


}

int mbedtls_internal_sha512_process( mbedtls_sha512_context *ctx, const unsigned char data[128] )
{
    int ret;
    esp_sha_acquire_hardware();
    ret = esp_internal_sha512_dma_process(ctx, data, 128, 0, 0);
    esp_sha_release_hardware();

    return ret;

}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_process( mbedtls_sha512_context *ctx,
                             const unsigned char data[128] )
{
    mbedtls_internal_sha512_process( ctx, data );
}
#endif

/*
 * SHA-512 process buffer
 */
int mbedtls_sha512_update_ret( mbedtls_sha512_context *ctx, const unsigned char *input,
                               size_t ilen )
{
    int ret;
    size_t fill;
    unsigned int left, len, local_len = 0;

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

        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 128;
    }

    len = (ilen / 128) * 128;

    if ( len || local_len) {

        esp_sha_acquire_hardware();

        if (ctx->sha_state == ESP_SHA512_STATE_INIT) {

            if (ctx->mode == SHA2_512T) {
                esp_sha_512_t_init_hash(ctx->t_val);
                ctx->first_block = false;
            } else {
                ctx->first_block = true;
            }
            ctx->sha_state = ESP_SHA512_STATE_IN_PROCESS;

        } else if (ctx->sha_state == ESP_SHA512_STATE_IN_PROCESS) {
            ctx->first_block = false;
            esp_sha_write_digest_state(ctx->mode, ctx->state);
        }

        ret = esp_internal_sha512_dma_process(ctx, input, len, ctx->buffer, local_len);

        esp_sha_read_digest_state(ctx->mode, ctx->state);

        esp_sha_release_hardware();

        if (ret != 0) {
            return ret;
        }

    }


    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input + len, ilen - len );
    }

    return 0;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_update( mbedtls_sha512_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    mbedtls_sha512_update_ret( ctx, input, ilen );
}
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

/*
 * SHA-512 final digest
 */
int mbedtls_sha512_finish_ret( mbedtls_sha512_context *ctx, unsigned char output[64] )
{
    int ret;
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

    if ( ( ret = mbedtls_sha512_update_ret( ctx, sha512_padding, padn ) ) != 0 ) {
        return ret;
    }

    if ( ( ret = mbedtls_sha512_update_ret( ctx, msglen, 16 ) ) != 0 ) {
        return ret;
    }

    if (ctx->mode == SHA2_384) {
        memcpy(output, ctx->state, 48);
    } else {
        memcpy(output, ctx->state, 64);
    }

    return ret;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_finish( mbedtls_sha512_context *ctx,
                            unsigned char output[64] )
{
    mbedtls_sha512_finish_ret( ctx, output );
}
#endif

#endif /* MBEDTLS_SHA512_C && MBEDTLS_SHA512_ALT */
