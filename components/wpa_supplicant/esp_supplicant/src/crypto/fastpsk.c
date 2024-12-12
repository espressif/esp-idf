/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Specialized and optimized PBKDF2-SHA1 implementation for Wi-Fi PSK
 *
 * Initially authored by Chien Wong(m@xv97.com).
 */

#include "fastpsk.h"

#include <string.h>

#include <sha/sha_dma.h>
#include <hal/sha_hal.h>

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

#define FAST_PSK_SHA1_BLOCKS           2
#define SHA1_BLOCK_SZ                  64
#define SHA1_BLOCK_SZ_WORDS            16
#define SHA1_OUTPUT_SZ                 20
#define SHA1_OUTPUT_SZ_WORDS           5
#define FAST_PSK_SHA1_BLOCKS_BUF_BYTES (FAST_PSK_SHA1_BLOCKS * SHA1_BLOCK_SZ)
#define FAST_PSK_SHA1_BLOCKS_BUF_WORDS (FAST_PSK_SHA1_BLOCKS * SHA1_BLOCK_SZ / 4)

union hmac_block {
    union {
        uint32_t words[SHA1_BLOCK_SZ / 4];
        uint8_t bytes[SHA1_BLOCK_SZ];
    } block[FAST_PSK_SHA1_BLOCKS];
    uint8_t whole_bytes[FAST_PSK_SHA1_BLOCKS_BUF_BYTES];
    uint32_t whole_words[FAST_PSK_SHA1_BLOCKS_BUF_WORDS];
};
_Static_assert(sizeof(union hmac_block) == 128, "Incorrect layout of hmac_block");

struct fast_psk_context {
    union hmac_block inner, outer;
    uint32_t sum[SHA1_OUTPUT_SZ_WORDS];
};

static inline void sha1_setup(void)
{
    esp_sha_acquire_hardware();
}

static inline void sha1_teardown(void)
{
    esp_sha_release_hardware();
}

static void pad_blocks(union hmac_block *ctx, size_t len)
{
    size_t bits = len << 3;
    uint8_t *bytes = ctx->whole_bytes;
    bytes[len] = 0x80;
    // Set all remaining bytes to 0
    memset(&bytes[len + 1], 0, FAST_PSK_SHA1_BLOCKS_BUF_BYTES - (len + 1));

    /*
     * Simplified PUT_UINT64_BE(bits, bytes, FAST_PSK_SHA1_BLOCKS_BUF_BYTES - 8).
     * Since len < 128 => bits < 1024, we only need to update the two least significant
     * bytes, actually.
     */
    PUT_UINT32_BE(bits, bytes, FAST_PSK_SHA1_BLOCKS_BUF_BYTES - 4);
}

void sha1_op(uint32_t blocks[FAST_PSK_SHA1_BLOCKS_BUF_WORDS], uint32_t output[SHA1_OUTPUT_SZ_WORDS])
{
    sha_hal_hash_block(SHA1, blocks, SHA1_BLOCK_SZ_WORDS, true);
    sha_hal_hash_block(SHA1, &blocks[SHA1_BLOCK_SZ_WORDS], SHA1_BLOCK_SZ_WORDS, false);
    sha_hal_read_digest(SHA1, output);
}

void fast_psk_f(const char *password, size_t password_len, const uint8_t *ssid, size_t ssid_len, uint32_t count, uint8_t digest[SHA1_OUTPUT_SZ])
{
    struct fast_psk_context ctx_, *ctx = &ctx_;
    size_t i;

    memset(ctx, 0, sizeof(*ctx));

    memset(ctx->outer.block[0].bytes, 0x5c, SHA1_BLOCK_SZ);
    memset(ctx->inner.block[0].bytes, 0x36, SHA1_BLOCK_SZ);

    for (i = 0; i < password_len; ++i) {
        ctx->outer.block[0].bytes[i] ^= password[i];
        ctx->inner.block[0].bytes[i] ^= password[i];
    }

    // U1 = PRF(P, S || i)
    memcpy(ctx->inner.block[1].bytes, ssid, ssid_len);
    PUT_UINT32_BE(count, ctx->inner.block[1].bytes, ssid_len);
    pad_blocks(&ctx->inner, SHA1_BLOCK_SZ + ssid_len + 4);

    sha1_setup();

    uint32_t *pi, *po;
    pi = ctx->inner.whole_words;
    po = ctx->outer.whole_words;
    // T1 = SHA1(K ^ ipad, S || i)
    sha1_op(pi, ctx->outer.block[1].words);

    // U1 = SHA1(K ^ opad, T1)
    pad_blocks(&ctx->outer, SHA1_BLOCK_SZ + SHA1_OUTPUT_SZ);
    uint32_t *inner_blk1 = ctx->inner.block[1].words;
    uint32_t *outer_blk1 = ctx->outer.block[1].words;
    uint32_t *sum = ctx->sum;

    sha1_op(po, inner_blk1);
    memcpy(sum, inner_blk1, SHA1_OUTPUT_SZ);
    pad_blocks(&ctx->inner, SHA1_BLOCK_SZ + SHA1_OUTPUT_SZ);

    for (i = 1; i < 4096; ++i) {
        // Tn = SHA1(K ^ ipad, Un-1)
        sha1_op(pi, outer_blk1);

        // Un = SHA1(K ^ opad, Tn)
        sha1_op(po, inner_blk1);

        // F = U1 ^ U2 ^ ... Un
        for (size_t j = 0; j < SHA1_OUTPUT_SZ_WORDS; ++j) {
            sum[j] ^= inner_blk1[j];
        }
    }

    sha1_teardown();

    memcpy(digest, sum, SHA1_OUTPUT_SZ);

    memset(ctx, 0, sizeof(*ctx));
}

int esp_fast_psk(const char *password, size_t password_len, const uint8_t *ssid, size_t ssid_len, size_t iterations, uint8_t *output, size_t output_len)
{
    if (!(ssid_len <= 32 && password_len <= 63 && iterations == 4096 && output_len == 32)) {
        return -1;
    }
    fast_psk_f(password, password_len, ssid, ssid_len, 2, output);
    memcpy(output + SHA1_OUTPUT_SZ, output, 32 - SHA1_OUTPUT_SZ);
    fast_psk_f(password, password_len, ssid, ssid_len, 1, output);

    return 0;
}
