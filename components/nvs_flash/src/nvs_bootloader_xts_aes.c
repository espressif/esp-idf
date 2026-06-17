/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_log.h"

#include "nvs_bootloader_aes.h"
#include "nvs_bootloader_xts_aes.h"

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_AES_SUPPORTED
/*
 * NOTE: The implementation of the below APIs have been copied
 * from the mbedtls (v3.6.2) implementation of the XTS-AES APIs.
 */
void nvs_bootloader_xts_aes_init(nvs_bootloader_xts_aes_context *ctx)
{
    bzero(&ctx->crypt_key, sizeof(ctx->crypt_key));
    bzero(&ctx->tweak_key, sizeof(ctx->tweak_key));
}

void nvs_bootloader_xts_aes_free(nvs_bootloader_xts_aes_context *ctx)
{
    if (ctx) {
        bzero(&ctx->crypt_key, sizeof(ctx->crypt_key));
        bzero(&ctx->tweak_key, sizeof(ctx->tweak_key));
    }
}

int nvs_bootloader_xts_aes_setkey(nvs_bootloader_xts_aes_context *ctx,
                                const unsigned char *key,
                                unsigned int key_bytes)
{
    size_t xts_key_bytes = key_bytes / 2;

    memcpy(&ctx->crypt_key, key, xts_key_bytes);
    memcpy(&ctx->tweak_key, &key[xts_key_bytes], xts_key_bytes);
    return 0;
}

/* Endianness with 64 bits values */
#ifndef GET_UINT64_LE
#define GET_UINT64_LE(n,b,i)                          \
{                                                     \
    (n) = ((uint64_t) (b)[(i) + 7] << 56)             \
        | ((uint64_t) (b)[(i) + 6] << 48)             \
        | ((uint64_t) (b)[(i) + 5] << 40)             \
        | ((uint64_t) (b)[(i) + 4] << 32)             \
        | ((uint64_t) (b)[(i) + 3] << 24)             \
        | ((uint64_t) (b)[(i) + 2] << 16)             \
        | ((uint64_t) (b)[(i) + 1] <<  8)             \
        | ((uint64_t) (b)[(i)    ]      );            \
}
#endif

#ifndef PUT_UINT64_LE
#define PUT_UINT64_LE(n,b,i)                          \
{                                                     \
    (b)[(i) + 7] = (unsigned char) ((n) >> 56);       \
    (b)[(i) + 6] = (unsigned char) ((n) >> 48);       \
    (b)[(i) + 5] = (unsigned char) ((n) >> 40);       \
    (b)[(i) + 4] = (unsigned char) ((n) >> 32);       \
    (b)[(i) + 3] = (unsigned char) ((n) >> 24);       \
    (b)[(i) + 2] = (unsigned char) ((n) >> 16);       \
    (b)[(i) + 1] = (unsigned char) ((n) >>  8);       \
    (b)[(i)    ] = (unsigned char) ((n)      );       \
}
#endif

/*
 * GF(2^128) multiplication function
 *
 * This function multiplies a field element by x in the polynomial field
 * representation. It uses 64-bit word operations to gain speed but compensates
 * for machine endianness and hence works correctly on both big and little
 * endian machines.
 */
static void bootloader_gf128mul_x_ble(unsigned char r[16],
                                    const unsigned char x[16])
{
    uint64_t a, b, ra, rb;

    GET_UINT64_LE(a, x, 0);
    GET_UINT64_LE(b, x, 8);

    ra = (a << 1)  ^ 0x0087 >> (8 - ((b >> 63) << 3));
    rb = (a >> 63) | (b << 1);

    PUT_UINT64_LE(ra, r, 0);
    PUT_UINT64_LE(rb, r, 8);
}

/*
 * XTS-AES buffer encryption/decryption
 */
int nvs_bootloader_aes_crypt_xts(nvs_bootloader_xts_aes_context *ctx,
                                enum AES_TYPE mode,
                                size_t length,
                                const unsigned char data_unit[16],
                                const unsigned char *input,
                                unsigned char *output)
{
    int ret;
    size_t blocks = length / 16;
    size_t leftover = length % 16;
    unsigned char tweak[16] = {};
    unsigned char prev_tweak[16] = {};
    unsigned char tmp[16] = {};

    /* Sectors must be at least 16 bytes. */
    if (length < 16) {
        return NVS_BOOTLOADER_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* NIST SP 80-38E disallows data units larger than 2**20 blocks. */
    if (length > ( 1 << 20 ) * 16) {
        return NVS_BOOTLOADER_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Compute the tweak. */
    ret = nvs_bootloader_aes_crypt_ecb(AES_ENC, (const unsigned char *) &ctx->tweak_key,
                                    AES256, data_unit, tweak);
    if (ret != 0) {
        return (ret);
    }

    while (blocks--) {
        size_t i;

        if (leftover && (mode == AES_DEC) && blocks == 0) {
            /* We are on the last block in a decrypt operation that has
             * leftover bytes, so we need to use the next tweak for this block,
             * and this tweak for the lefover bytes. Save the current tweak for
             * the leftovers and then update the current tweak for use on this,
             * the last full block. */
            memcpy(prev_tweak, tweak, sizeof(tweak));
            bootloader_gf128mul_x_ble(tweak, tweak);
        }

        for (i = 0; i < 16; i++) {
            tmp[i] = input[i] ^ tweak[i];
        }

        ret = nvs_bootloader_aes_crypt_ecb(mode, (const unsigned char *) &ctx->crypt_key, AES256, tmp, tmp);
        if (ret != 0) {
            return (ret);
        }

        for (i = 0; i < 16; i++) {
            output[i] = tmp[i] ^ tweak[i];
        }

        /* Update the tweak for the next block. */
        bootloader_gf128mul_x_ble(tweak, tweak);

        output += 16;
        input += 16;
    }

    if (leftover) {
        /* If we are on the leftover bytes in a decrypt operation, we need to
         * use the previous tweak for these bytes (as saved in prev_tweak). */
        unsigned char *t = mode == AES_DEC ? prev_tweak : tweak;

        /* We are now on the final part of the data unit, which doesn't divide
         * evenly by 16. It's time for ciphertext stealing. */
        size_t i;
        unsigned char *prev_output = output - 16;

        /* Copy ciphertext bytes from the previous block to our output for each
         * byte of ciphertext we won't steal. At the same time, copy the
         * remainder of the input for this final round (since the loop bounds
         * are the same). */
        for (i = 0; i < leftover; i++) {
            output[i] = prev_output[i];
            tmp[i] = input[i] ^ t[i];
        }

        /* Copy ciphertext bytes from the previous block for input in this
         * round. */
        for (; i < 16; i++) {
            tmp[i] = prev_output[i] ^ t[i];
        }

        ret = nvs_bootloader_aes_crypt_ecb(mode, (const unsigned char *) &ctx->crypt_key, AES256, tmp, tmp);
        if (ret != 0) {
            return ret;
        }

        /* Write the result back to the previous block, overriding the previous
         * output we copied. */
        for (i = 0; i < 16; i++) {
            prev_output[i] = tmp[i] ^ t[i];
        }
    }

    return 0;
}
#else /* SOC_AES_SUPPORTED */
#if BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER
#if CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB
#error "Enable `CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER` for non SOC_AES_SUPPORTED targets for supporting NVS encryption in bootloader build"
#else /* !CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB */
// TODO: IDF-11673
// Due to unavailability of an software AES layer for bootloader build,
// we cannot support the below NVS bootloader's AES operations
// Thus we are adding stub APIs to indicate that the following operation fail.

static const char *TAG = "nvs_bootloader_xts_aes";
static const char *op_unsupported_error = "XTS-AES operation in bootloader unsupported for this target";

void nvs_bootloader_xts_aes_init(nvs_bootloader_xts_aes_context *ctx)
{
    (void) ctx;
    ESP_EARLY_LOGE(TAG, "%s", op_unsupported_error);
    abort();
}

void nvs_bootloader_xts_aes_free(nvs_bootloader_xts_aes_context *ctx)
{
    (void) ctx;
    ESP_EARLY_LOGE(TAG, "%s", op_unsupported_error);
    abort();
}

int nvs_bootloader_xts_aes_setkey(nvs_bootloader_xts_aes_context *ctx,
                                const unsigned char *key,
                                unsigned int key_bytes)
{
    (void) ctx;
    ESP_EARLY_LOGE(TAG, "%s", op_unsupported_error);
    abort();
    return -1;
}
/*
 * XTS-AES buffer encryption/decryption
 */
int nvs_bootloader_aes_crypt_xts(nvs_bootloader_xts_aes_context *ctx,
                                enum AES_TYPE mode,
                                size_t length,
                                const unsigned char data_unit[16],
                                const unsigned char *input,
                                unsigned char *output)
{
    (void) ctx;
    ESP_EARLY_LOGE(TAG, "XTS-AES operation in bootloader unsupported");
    abort();
    return -1;
}

#endif /* CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB */
#else /* BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER */
#include "nvs_xts_aes.h"

static XTS_CONTEXT ctx_xts;

void nvs_bootloader_xts_aes_init(nvs_bootloader_xts_aes_context *ctx)
{
    (void) ctx;
    XTS_FUNC(xts_init)(&ctx_xts);
}

void nvs_bootloader_xts_aes_free(nvs_bootloader_xts_aes_context *ctx)
{
    (void) ctx;
    XTS_FUNC(xts_free)(&ctx_xts);
}

int nvs_bootloader_xts_aes_setkey(nvs_bootloader_xts_aes_context *ctx,
                                const unsigned char *key,
                                unsigned int key_bytes)
{
    (void) ctx;
    return XTS_FUNC(xts_setkey_dec)(&ctx_xts, key, key_bytes * 8);
}
/*
 * XTS-AES buffer encryption/decryption
 */
int nvs_bootloader_aes_crypt_xts(nvs_bootloader_xts_aes_context *ctx,
                                enum AES_TYPE mode,
                                size_t length,
                                const unsigned char data_unit[16],
                                const unsigned char *input,
                                unsigned char *output)
{
    (void) ctx;
    int xts_mode = mode == AES_ENC ? XTS_MODE(ENCRYPT) : XTS_MODE(DECRYPT);
    return XTS_FUNC(crypt_xts)(&ctx_xts, xts_mode, length, data_unit, input, output);
}
#endif /* !(BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER) */
#endif /* !SOC_AES_SUPPORTED */
