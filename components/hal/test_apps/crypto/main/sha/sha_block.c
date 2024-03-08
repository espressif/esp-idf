/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"

#if SOC_SHA_SUPPORTED

#include "soc/periph_defs.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "hal/sha_hal.h"
#include "hal/sha_ll.h"
#include "sha_block.h"

static inline size_t block_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
    case SHA2_224:
    case SHA2_256:
        return 64;
#if SOC_SHA_SUPPORT_SHA384
    case SHA2_384:
#endif
#if SOC_SHA_SUPPORT_SHA512
    case SHA2_512:
#endif
#if SOC_SHA_SUPPORT_SHA512_T
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
#endif
        return 128;
    default:
        return 0;
    }
}

#if defined(SOC_SHA_SUPPORT_SHA1)

static void sha1_update_block(sha1_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left, local_len = 0;

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

    if ( (ilen >= 64) || local_len) {
        /* Enable peripheral module */
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(true);
            sha_ll_reset_register();
        }

        if (ctx->first_block == 0) {
            /* Writes the message digest to the SHA engine */
            sha_hal_write_digest(sha_type, ctx->state);
        }

        /* First process buffered block, if any */
        if ( local_len ) {
            /* Hash a single block */
            sha_hal_hash_block(sha_type, ctx->buffer, block_length(sha_type)/4, ctx->first_block);
            if (ctx->first_block == 1) {
                ctx->first_block = 0;
            }
        }

        while ( ilen >= 64 ) {
            sha_hal_hash_block(sha_type, input, block_length(sha_type)/4, ctx->first_block);
            if (ctx->first_block == 1) {
                ctx->first_block = 0;
            }
            input += 64;
            ilen  -= 64;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(false);
        }
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen);
    }
}

void sha1_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha1_ctx ctx;
    memset(&ctx, 0, sizeof(sha1_ctx));
    ctx.first_block = 1;

    sha1_update_block(&ctx, sha_type, input, ilen);

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx.total[0] >> 29 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx.total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha1_update_block(&ctx, sha_type, sha1_padding, padn);

    sha1_update_block(&ctx, sha_type, msglen, 8);

    memcpy(output, ctx.state, 20);
}

#endif /* defined(SOC_SHA_SUPPORT_SHA1) */

#if defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256)

static void sha256_update_block(sha256_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left, local_len = 0;

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

    if ( (ilen >= 64) || local_len) {
        /* Enable peripheral module */
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(true);
            sha_ll_reset_register();
        }

        if (ctx->first_block == 0) {
            /* Writes the message digest to the SHA engine */
            sha_hal_write_digest(sha_type, ctx->state);
        }

        /* First process buffered block, if any */
        if ( local_len ) {
            /* Hash a single block */
            sha_hal_hash_block(sha_type, ctx->buffer, block_length(sha_type)/4, ctx->first_block);
            if (ctx->first_block == 1) {
                ctx->first_block = 0;
            }
        }

        while ( ilen >= 64 ) {
            sha_hal_hash_block(sha_type, input, block_length(sha_type)/4, ctx->first_block);
            if (ctx->first_block == 1) {
                ctx->first_block = 0;
            }
            input += 64;
            ilen  -= 64;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(false);
        }
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen);
    }
}

void sha256_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha256_ctx ctx;
    memset(&ctx, 0, sizeof(sha256_ctx));
    ctx.first_block = 1;

    sha256_update_block(&ctx, sha_type, input, ilen);

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx.total[0] >> 29 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx.total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha256_update_block(&ctx, sha_type, sha256_padding, padn);

    sha256_update_block(&ctx, sha_type, msglen, 8);

    if (sha_type == SHA2_256) {
        memcpy(output, ctx.state, 32);
    } else if (sha_type == SHA2_224) {
        memcpy(output, ctx.state, 28);
    }
}

#endif /* defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256) */

#if defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512)

#if SOC_SHA_SUPPORT_SHA512_T

int sha_512_t_init_hash_block(uint16_t t)
{
    uint32_t t_string = 0;
    uint8_t t0, t1, t2, t_len;

    if (t == 384) {
        return -1;
    }

    if (t <= 9) {
        t_string = (uint32_t)((1 << 23) | ((0x30 + t) << 24));
        t_len = 0x48;
    } else if (t <= 99) {
        t0 = t % 10;
        t1 = (t / 10) % 10;
        t_string = (uint32_t)((1 << 15) | ((0x30 + t0) << 16) |
                              (((0x30 + t1) << 24)));
        t_len = 0x50;
    } else if (t <= 512) {
        t0 = t % 10;
        t1 = (t / 10) % 10;
        t2 = t / 100;
        t_string = (uint32_t)((1 << 7) | ((0x30 + t0) << 8) |
                              (((0x30 + t1) << 16) + ((0x30 + t2) << 24)));
        t_len = 0x58;
    } else {
        return -1;
    }

    /* Calculates and sets the initial digiest for SHA512_t */
    sha_hal_sha512_init_hash(t_string, t_len);

    return 0;
}

#endif //SOC_SHA_SUPPORT_SHA512_T

static void sha512_update_block(sha512_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{

    size_t fill;
    unsigned int left, local_len = 0;

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


    if ( (ilen >= 128) || local_len) {

        /* Enable peripheral module */
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(true);
            sha_ll_reset_register();
        }

        if (ctx->first_block && sha_type == SHA2_512T){
            sha_512_t_init_hash_block(ctx->t_val);
            ctx->first_block = 0;
        }
        else if (ctx->first_block == 0) {
            /* Writes the message digest to the SHA engine */
            sha_hal_write_digest(sha_type, ctx->state);
        }

        /* First process buffered block, if any */
        if ( local_len ) {
            /* Hash a single block */
            sha_hal_hash_block(sha_type, ctx->buffer, block_length(sha_type)/4, ctx->first_block);
            if (ctx->first_block == 1) {
                ctx->first_block = 0;
            }
        }

        while ( ilen >= 128 ) {
            sha_hal_hash_block(sha_type, input, block_length(sha_type)/4, ctx->first_block);
            if (ctx->first_block == 1) {
                ctx->first_block = 0;
            }
            input += 128;
            ilen  -= 128;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(false);
        }
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input, ilen);
    }
}

void sha512_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha512_ctx ctx;
    memset(&ctx, 0, sizeof(sha512_ctx));
    ctx.first_block = 1;

    sha512_update_block(&ctx, sha_type, input, ilen);

    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = ( ctx.total[0] >> 61 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,  msglen, 8 );

    last = (size_t)( ctx.total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    sha512_update_block( &ctx, sha_type, sha512_padding, padn );

    sha512_update_block( &ctx, sha_type, msglen, 16 );

    if (sha_type == SHA2_384) {
        memcpy(output, ctx.state, 48);
    } else {
        memcpy(output, ctx.state, 64);
    }

}

#endif /* defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512) */

#if SOC_SHA_SUPPORT_SHA512_T

void sha512t_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output, uint32_t t_val)
{
    sha512_ctx ctx;
    memset(&ctx, 0, sizeof(sha512_ctx));
    ctx.first_block = 1;
    ctx.t_val = t_val;

    sha512_update_block(&ctx, sha_type, input, ilen);

    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = ( ctx.total[0] >> 61 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,  msglen, 8 );

    last = (size_t)( ctx.total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    sha512_update_block( &ctx, sha_type, sha512_padding, padn );

    sha512_update_block( &ctx, sha_type, msglen, 16 );

    if (sha_type == SHA2_384) {
        memcpy(output, ctx.state, 48);
    } else {
        memcpy(output, ctx.state, 64);
    }
}

#endif /*SOC_SHA_SUPPORT_SHA512_T*/

#endif /*SOC_SHA_SUPPORTED*/
