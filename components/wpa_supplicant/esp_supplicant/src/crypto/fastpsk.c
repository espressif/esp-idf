/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Specialized and optimized PBKDF2-SHA1 implementation for Wi-Fi PSK
 *
 * Initially authored by Chien Wong(m@xv97.com).
 */

/*
 * This implementation derives a Pre-Shared Key (PSK) for WPA/WPA2 using a custom PBKDF2-like approach.
 *
 * PBKDF2 (Password-Based Key Derivation Function 2) is a standard algorithm used to derive cryptographic keys
 * from a password and salt. It relies on iteratively applying a pseudorandom function, such as HMAC, to the input.
 * The derived key is designed to be computationally expensive to generate, making brute-force attacks more difficult.
 *
 * In standard PBKDF2, the process is as follows:
 * 1. Combine the password and salt (SSID in WPA).
 * 2. Compute HMAC for this combination, iteratively applying the previous HMAC output as input for the next iteration.
 * 3. XOR all intermediate results to produce the final derived key.
 *
 * This implementation adapts PBKDF2 for WPA/WPA2 by leveraging the SHA1 hashing algorithm and fixed parameters:
 * - The password is up to 63 characters long.
 * - The SSID (salt) is up to 32 bytes.
 * - The iteration count is fixed at 4096, as required by WPA.
 * - The output key length is 32 bytes, suitable for WPA.
 *
 * Key Differences from Standard PBKDF2:
 * - Instead of a general-purpose pseudorandom function, this implementation uses a fixed combination of SHA1 blocks.
 * - The logic for handling HMAC is explicitly implemented to optimize for this specific use case.
 * - Padding and block alignment are carefully managed to fit within hardware constraints (e.g., the ESP32 SHA1 hardware).
 *
 * How This Implementation Works:
 * 1. The `fast_psk_f` function computes one segment of the derived key. It takes as input:
 *    - The password.
 *    - The SSID.
 *    - A counter value (`count`) that varies for each segment.
 * 2. HMAC-SHA1 is implemented explicitly using:
 *    - An inner padding block (`ipad`) initialized with 0x36 XORed with the password.
 *    - An outer padding block (`opad`) initialized with 0x5C XORed with the password.
 * 3. Intermediate hashes (`U1`, `U2`, ..., `Un`) are computed iteratively. Each `U` value depends on the previous one.
 *    - `U1` is derived from the password, SSID, and counter.
 *    - Subsequent `U` values are derived using SHA1 on the previous `U` value.
 * 4. All intermediate values are XORed together to produce the final segment of the key.
 * 5. The `esp_fast_psk` function combines two invocations of `fast_psk_f` to produce the complete 32-byte key.
 *    - The first invocation computes the first 20 bytes (SHA1_OUTPUT_SZ).
 *    - The second invocation computes the remaining 12 bytes.
 *
 * The ipad/opad SHA states are
 * computed once and restored via sha_hal_write_digest() each iteration,
 * halving the SHA block operations per iteration from 4 to 2.
 *
 * - The code uses the ESP SHA1 hardware accelerator for faster computation.
 */

#include "fastpsk.h"

#include <string.h>
#include "sha/sha_dma.h"
#include "hal/sha_hal.h"

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n, b, i)                          \
{                                                       \
    (b)[(i)] = (unsigned char)((n) >> 24);              \
    (b)[(i) + 1] = (unsigned char)((n) >> 16);          \
    (b)[(i) + 2] = (unsigned char)((n) >> 8);           \
    (b)[(i) + 3] = (unsigned char)((n));                \
}
#endif

#define FAST_PSK_SHA1_BLOCKS           2
#define SHA1_BLOCK_SZ                  64
#define SHA1_BLOCK_SZ_WORDS            16
#define SHA1_OUTPUT_SZ                 20
#define SHA1_OUTPUT_SZ_WORDS           5
#define FAST_PSK_SHA1_BLOCKS_BUF_BYTES (FAST_PSK_SHA1_BLOCKS * SHA1_BLOCK_SZ)
#define FAST_PSK_SHA1_BLOCKS_BUF_WORDS (FAST_PSK_SHA1_BLOCKS * SHA1_BLOCK_SZ / 4)

/* Union to represent SHA1 HMAC blocks */
union hmac_block {
    union {
        uint32_t words[SHA1_BLOCK_SZ / 4]; /* SHA1 block in words */
        uint8_t bytes[SHA1_BLOCK_SZ];      /* SHA1 block in bytes */
    } block[FAST_PSK_SHA1_BLOCKS];
    uint8_t whole_bytes[FAST_PSK_SHA1_BLOCKS_BUF_BYTES]; /* Complete block as bytes */
    uint32_t whole_words[FAST_PSK_SHA1_BLOCKS_BUF_WORDS]; /* Complete block as words */
};
_Static_assert(sizeof(union hmac_block) == 128, "Incorrect layout of hmac_block");

/* Structure to hold HMAC context */
struct fast_psk_context {
    union hmac_block inner, outer; /* Inner and outer padding */
    uint32_t sum[SHA1_OUTPUT_SZ_WORDS]; /* Intermediate hash result */
};

/* Acquire SHA1 hardware for exclusive use */
static inline void sha1_setup(void)
{
    esp_sha_acquire_hardware();
}

/* Release SHA1 hardware */
static inline void sha1_teardown(void)
{
    esp_sha_release_hardware();
}

/*
 * Pads the given HMAC block context with the appropriate SHA1 padding.
 * Length is the number of bytes of actual data in the block.
 */
static void pad_blocks(union hmac_block *ctx, size_t len)
{
    size_t bits = len << 3; /* Convert length to bits */
    uint8_t *bytes = ctx->whole_bytes;
    bytes[len] = 0x80; /* Append 0x80 as per SHA1 padding rules */

    // Set all remaining bytes to 0
    memset(&bytes[len + 1], 0, FAST_PSK_SHA1_BLOCKS_BUF_BYTES - (len + 1));

    /*
     * Simplified PUT_UINT64_BE(bits, bytes, FAST_PSK_SHA1_BLOCKS_BUF_BYTES - 8).
     * Since len < 128 => bits < 1024, we only need to update the two least significant
     * bytes, actually.
     */
    PUT_UINT32_BE(bits, bytes, FAST_PSK_SHA1_BLOCKS_BUF_BYTES - 4);
}

static void fast_psk_f(const char *password, size_t password_len, const uint8_t *ssid, size_t ssid_len, uint32_t count, uint8_t digest[SHA1_OUTPUT_SZ])
{
    struct fast_psk_context ctx_, *ctx = &ctx_;
    size_t i;
    uint32_t ipad_state[SHA1_OUTPUT_SZ_WORDS];
    uint32_t opad_state[SHA1_OUTPUT_SZ_WORDS];

    /* Clear the context */
    memset(ctx, 0, sizeof(*ctx));

    /* Initialize inner and outer padding */
    memset(ctx->outer.block[0].bytes, 0x5c, SHA1_BLOCK_SZ);
    memset(ctx->inner.block[0].bytes, 0x36, SHA1_BLOCK_SZ);

    /* XOR the password into the padding */
    for (i = 0; i < password_len; ++i) {
        ctx->outer.block[0].bytes[i] ^= password[i];
        ctx->inner.block[0].bytes[i] ^= password[i];
    }

    /* Prepare the first input block for HMAC (S || i) */
    /* Copy SSID */
    memcpy(ctx->inner.block[1].bytes, ssid, ssid_len);
    /* Append the counter */
    PUT_UINT32_BE(count, ctx->inner.block[1].bytes, ssid_len);
    /* Pad the block */
    pad_blocks(&ctx->inner, SHA1_BLOCK_SZ + ssid_len + 4);

    sha1_setup();

    uint32_t *inner_blk1 = ctx->inner.block[1].words;
    uint32_t *outer_blk1 = ctx->outer.block[1].words;
    uint32_t *sum = ctx->sum;

    /*
     * Save ipad/opad SHA states once, restore each iteration.
     * Each iteration processes only 1 data block per HMAC = 2 block ops total.
     */
    /* Process ipad/opad blocks once, save intermediate SHA1 states */
    sha_hal_hash_block(SHA1, ctx->inner.block[0].words, SHA1_BLOCK_SZ_WORDS, true);
    sha_hal_read_digest(SHA1, ipad_state);
    sha_hal_hash_block(SHA1, ctx->outer.block[0].words, SHA1_BLOCK_SZ_WORDS, true);
    sha_hal_read_digest(SHA1, opad_state);

    // T1 = SHA1(ipad_state, S || i)
    sha_hal_write_digest(SHA1, ipad_state);
    sha_hal_hash_block(SHA1, inner_blk1, SHA1_BLOCK_SZ_WORDS, false);
    sha_hal_read_digest(SHA1, outer_blk1);

    // U1 = SHA1(opad_state, T1)
    pad_blocks(&ctx->outer, SHA1_BLOCK_SZ + SHA1_OUTPUT_SZ);
    sha_hal_write_digest(SHA1, opad_state);
    sha_hal_hash_block(SHA1, outer_blk1, SHA1_BLOCK_SZ_WORDS, false);
    sha_hal_read_digest(SHA1, inner_blk1);
    memcpy(sum, inner_blk1, SHA1_OUTPUT_SZ);
    pad_blocks(&ctx->inner, SHA1_BLOCK_SZ + SHA1_OUTPUT_SZ);

    /*
     * Iterations 2..4096.
     * sha_hal_read_digest() writes exactly 20 bytes (5 words) for SHA1,
     * so the padding at bytes [20..63] in each block stays intact.
     */
    for (i = 1; i < 4096; ++i) {
        // Tn = SHA1(ipad_state, Un-1)
        sha_hal_write_digest(SHA1, ipad_state);
        sha_hal_hash_block(SHA1, inner_blk1, SHA1_BLOCK_SZ_WORDS, false);
        sha_hal_read_digest(SHA1, outer_blk1);

        // Un = SHA1(opad_state, Tn)
        sha_hal_write_digest(SHA1, opad_state);
        sha_hal_hash_block(SHA1, outer_blk1, SHA1_BLOCK_SZ_WORDS, false);
        sha_hal_read_digest(SHA1, inner_blk1);

        for (size_t j = 0; j < SHA1_OUTPUT_SZ_WORDS; ++j) {
            sum[j] ^= inner_blk1[j];
        }
    }

    sha1_teardown();

    /* Copy the final result to the output digest */
    memcpy(digest, sum, SHA1_OUTPUT_SZ);

    /* Clear context */
    memset(ctx, 0, sizeof(*ctx));
}

int esp_fast_psk(const char *password, size_t password_len, const uint8_t *ssid, size_t ssid_len, size_t iterations, uint8_t *output, size_t output_len)
{
    if (!(ssid_len <= 32 && password_len <= 63 && iterations == 4096 && output_len == 32)) {
        return -1; /* Invalid input parameters */
    }

    /*
     * PBKDF2 with dkLen=32 needs two 20-byte F() blocks:
     *   output[0..19]  = F(password, ssid, 4096, 1)
     *   output[20..31] = F(password, ssid, 4096, 2)[0..11]
     *
     * Compute block 2 first so its first 12 bytes can be placed at
     * output[20..31], then block 1 overwrites output[0..19].
     */
    fast_psk_f(password, password_len, ssid, ssid_len, 2, output);
    memcpy(output + SHA1_OUTPUT_SZ, output, 32 - SHA1_OUTPUT_SZ);
    fast_psk_f(password, password_len, ssid, ssid_len, 1, output);

    return 0; /* Success */
}
