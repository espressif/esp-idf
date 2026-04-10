/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"

#include "esp_log.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "sys/param.h"
#include "soc/lldesc.h"

#if SOC_SHA_SUPPORTED
#if SOC_SHA_SUPPORT_DMA

#include "sha/sha_core.h"
#include "sha_dma.h"

#if defined(SOC_SHA_SUPPORT_SHA1)

static void esp_internal_sha1_update_state(sha1_ctx *ctx, esp_sha_type sha_type)
{
    if (ctx->sha_state == ESP_SHA_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA_STATE_IN_PROCESS) {
        ctx->first_block = false;
        sha_hal_write_digest(sha_type, ctx->state);
    }
}

static void sha1_update_dma(sha1_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left, len, local_len = 0;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), input, fill);
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = (ilen / 64) * 64;
    if (len || local_len) {
        /* Enable peripheral module */
        esp_sha_acquire_hardware();

        esp_sha_set_mode(sha_type);

        esp_internal_sha1_update_state(ctx, sha_type);

        int ret = esp_sha_dma(sha_type, input, len, ctx->buffer, local_len, ctx->first_block);
        if (ret != 0) {
            esp_sha_release_hardware();
            return ;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        esp_sha_release_hardware();
    }

    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), input + len, ilen - len);
    }
}

void sha1_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha1_ctx ctx;
    ctx.total[0] = 0;
    ctx.total[1] = 0;

    memset(&ctx, 0, sizeof(sha1_ctx));
    ctx.mode = SHA1;

    sha1_update_dma(&ctx, sha_type, input, ilen);

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = (ctx.total[0] >> 29)
           | (ctx.total[1] <<  3);
    low  = (ctx.total[0] <<  3);

    PUT_UINT32_BE(high, msglen, 0);
    PUT_UINT32_BE(low,  msglen, 4);

    last = ctx.total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    sha1_update_dma(&ctx, sha_type, sha1_padding, padn);

    sha1_update_dma(&ctx, sha_type, msglen, 8);

    memcpy(output, ctx.state, 20);
}

#endif /* defined(SOC_SHA_SUPPORT_SHA1) */

#if defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256)

static void esp_internal_sha256_update_state(sha256_ctx *ctx)
{
    if (ctx->sha_state == ESP_SHA_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA_STATE_IN_PROCESS) {
        ctx->first_block = false;
        sha_hal_write_digest(ctx->mode, ctx->state);
    }
}

static void sha256_update_dma(sha256_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left, len, local_len = 0;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), input, fill);
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = (ilen / 64) * 64;
    if (len || local_len) {
        /* Enable peripheral module */
        esp_sha_acquire_hardware();

        esp_sha_set_mode(sha_type);

        esp_internal_sha256_update_state(ctx);

        int ret = esp_sha_dma(ctx->mode, input, len, ctx->buffer, local_len, ctx->first_block);

        if (ret != 0) {
            /* Disable peripheral module */
            esp_sha_release_hardware();
            return;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        esp_sha_release_hardware();
    }

    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), input + len, ilen - len);
    }
}

void sha256_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha256_ctx ctx;
    memset(&ctx, 0, sizeof(sha256_ctx));
    ctx.mode = sha_type;

    sha256_update_dma(&ctx, sha_type, input, ilen);

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = (ctx.total[0] >> 29)
           | (ctx.total[1] <<  3);
    low  = (ctx.total[0] <<  3);

    PUT_UINT32_BE(high, msglen, 0);
    PUT_UINT32_BE(low,  msglen, 4);

    last = ctx.total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    sha256_update_dma(&ctx, sha_type, sha256_padding, padn);

    sha256_update_dma(&ctx, sha_type, msglen, 8);

    if (sha_type == SHA2_256) {
        memcpy(output, ctx.state, 32);
    } else if (sha_type == SHA2_224) {
        memcpy(output, ctx.state, 28);
    }
}

#endif /* defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256) */

#if defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512)

#if SOC_SHA_SUPPORT_SHA512_T

int sha_512_t_init_hash_dma(uint16_t t)
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

static void esp_internal_sha512_update_state(sha512_ctx *ctx)
{
    if (ctx->sha_state == ESP_SHA_STATE_INIT) {
        if (ctx->mode == SHA2_512T) {
            int ret = -1;
            if ((ret = sha_512_t_init_hash_dma(ctx->t_val)) != 0) {
                esp_sha_release_hardware();
                return;
            }
            ctx->first_block = false;
        } else {
            ctx->first_block = true;
        }
        ctx->sha_state = ESP_SHA_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA_STATE_IN_PROCESS) {
        ctx->first_block = false;
        sha_hal_write_digest(ctx->mode, ctx->state);
    }
}

static void sha512_update_dma(sha512_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{

    size_t fill;
    unsigned int left, len, local_len = 0;

    left = (unsigned int)(ctx->total[0] & 0x7F);
    fill = 128 - left;

    ctx->total[0] += (uint64_t) ilen;

    if (ctx->total[0] < (uint64_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), input, fill);
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 128;
    }

    len = (ilen / 128) * 128;

    if (len || local_len) {

        /* Enable peripheral module */
        esp_sha_acquire_hardware();

        esp_sha_set_mode(sha_type);

        esp_internal_sha512_update_state(ctx);

        int ret = esp_sha_dma(ctx->mode, input, len, ctx->buffer, local_len, ctx->first_block);

        if (ret != 0) {
            esp_sha_release_hardware();
            return;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        esp_sha_release_hardware();
    }

    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), input + len, ilen - len);
    }
}

void sha512_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha512_ctx ctx;
    memset(&ctx, 0, sizeof(sha512_ctx));
    ctx.mode = sha_type;

    sha512_update_dma(&ctx, sha_type, input, ilen);

    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = (ctx.total[0] >> 61)
           | (ctx.total[1] <<  3);
    low  = (ctx.total[0] <<  3);

    PUT_UINT64_BE(high, msglen, 0);
    PUT_UINT64_BE(low,  msglen, 8);

    last = (size_t)(ctx.total[0] & 0x7F);
    padn = (last < 112) ? (112 - last) : (240 - last);

    sha512_update_dma(&ctx, sha_type, sha512_padding, padn);

    sha512_update_dma(&ctx, sha_type, msglen, 16);

    if (sha_type == SHA2_384) {
        memcpy(output, ctx.state, 48);
    } else {
        memcpy(output, ctx.state, 64);
    }

}

#endif /* defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512) */

#if SOC_SHA_SUPPORT_SHA512_T

void sha512t_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output, uint32_t t_val)
{
    sha512_ctx ctx;
    memset(&ctx, 0, sizeof(sha512_ctx));
    ctx.t_val = t_val;
    ctx.mode = sha_type;

    sha512_update_dma(&ctx, sha_type, input, ilen);

    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = (ctx.total[0] >> 61)
           | (ctx.total[1] <<  3);
    low  = (ctx.total[0] <<  3);

    PUT_UINT64_BE(high, msglen, 0);
    PUT_UINT64_BE(low,  msglen, 8);

    last = (size_t)(ctx.total[0] & 0x7F);
    padn = (last < 112) ? (112 - last) : (240 - last);

    sha512_update_dma(&ctx, sha_type, sha512_padding, padn);

    sha512_update_dma(&ctx, sha_type, msglen, 16);

    if (sha_type == SHA2_384) {
        memcpy(output, ctx.state, 48);
    } else {
        memcpy(output, ctx.state, 64);
    }
}

#endif /*SOC_SHA_SUPPORT_SHA512_T*/

#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /*SOC_SHA_SUPPORTED*/
