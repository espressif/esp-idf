/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_cmac.h"
#include "psa_crypto_driver_esp_cmac_contexts.h"
#include "psa_crypto_driver_esp_aes.h"
#include "mbedtls/constant_time.h"

#define MBEDTLS_CMAC_MAX_BLOCK_SIZE 16

#if (__BYTE_ORDER__) == (__ORDER_BIG_ENDIAN__)
#define MBEDTLS_IS_BIG_ENDIAN 1
#else
#define MBEDTLS_IS_BIG_ENDIAN 0
#endif

#define MBEDTLS_BSWAP32 __builtin_bswap32

static inline uint32_t mbedtls_get_unaligned_uint32(const void *p)
{
    uint32_t r;
#if defined(UINT_UNALIGNED)
    mbedtls_uint32_unaligned_t *p32 = (mbedtls_uint32_unaligned_t *) p;
    r = *p32;
#elif defined(UINT_UNALIGNED_STRUCT)
    mbedtls_uint32_unaligned_t *p32 = (mbedtls_uint32_unaligned_t *) p;
    r = p32->x;
#else
    memcpy(&r, p, sizeof(r));
#endif
    return r;
}

static inline void mbedtls_put_unaligned_uint32(void *p, uint32_t x)
{
#if defined(UINT_UNALIGNED)
    mbedtls_uint32_unaligned_t *p32 = (mbedtls_uint32_unaligned_t *) p;
    *p32 = x;
#elif defined(UINT_UNALIGNED_STRUCT)
    mbedtls_uint32_unaligned_t *p32 = (mbedtls_uint32_unaligned_t *) p;
    p32->x = x;
#else
    memcpy(p, &x, sizeof(x));
#endif
}

#define MBEDTLS_GET_UINT32_BE(data, offset)                                \
    ((MBEDTLS_IS_BIG_ENDIAN)                                               \
        ? mbedtls_get_unaligned_uint32((data) + (offset))                  \
        : MBEDTLS_BSWAP32(mbedtls_get_unaligned_uint32((data) + (offset))) \
    )

#define MBEDTLS_PUT_UINT32_BE(n, data, offset)                                   \
    do {                                                                         \
        mbedtls_put_unaligned_uint32((data) + (offset),                          \
            (MBEDTLS_IS_BIG_ENDIAN)                                              \
                ? (uint32_t) (n)                                                 \
                : MBEDTLS_BSWAP32((uint32_t) (n)));                              \
    } while (0)

psa_status_t esp_cmac_abort(esp_cmac_operation_t *esp_cmac_ctx)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    status = psa_destroy_key(esp_cmac_ctx->key_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = esp_aes_cipher_abort(&esp_cmac_ctx->esp_aes_ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    mbedtls_platform_zeroize(esp_cmac_ctx, sizeof(esp_cmac_operation_t));
    return status;
}

psa_status_t esp_cmac_setup(esp_cmac_operation_t *esp_cmac_ctx,
                            const psa_key_attributes_t *attributes,
                            const uint8_t *key_buffer,
                            size_t key_buffer_size,
                            psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    memset(esp_cmac_ctx, 0, sizeof(*esp_cmac_ctx));

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t key_type = psa_get_key_type(attributes);
    size_t key_bits = psa_get_key_bits(attributes);
    psa_algorithm_t cmac_aes_key_alg = PSA_ALG_ECB_NO_PADDING;
    /* Set up key attributes for PSA import */
    psa_set_key_type(&key_attributes, key_type);
    psa_set_key_bits(&key_attributes, key_bits);
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&key_attributes, cmac_aes_key_alg);

    /* Import key for cipher operations */
    status = psa_import_key(&key_attributes, key_buffer, key_buffer_size, &esp_cmac_ctx->key_id);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = esp_aes_cipher_encrypt_setup(&esp_cmac_ctx->esp_aes_ctx, &key_attributes, key_buffer, key_buffer_size, cmac_aes_key_alg);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(esp_cmac_ctx->key_id);
        goto exit;
    }

    psa_reset_key_attributes(&key_attributes);
    esp_cmac_ctx->alg = alg;
    esp_cmac_ctx->unprocessed_len = 0;
    esp_cmac_ctx->cipher_block_length = PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type);
    mbedtls_platform_zeroize(esp_cmac_ctx->state, sizeof(esp_cmac_ctx->state));
    mbedtls_platform_zeroize(esp_cmac_ctx->unprocessed_block, sizeof(esp_cmac_ctx->unprocessed_block));

exit:
    psa_reset_key_attributes(&key_attributes);
    return status;
}

static void xor_no_simd(unsigned char *output, const unsigned char *input1, const unsigned char *input2, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        output[i] = input1[i] ^ input2[i];
    }
}

psa_status_t esp_cmac_update(esp_cmac_operation_t *esp_cmac_ctx, const uint8_t *data, size_t data_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    size_t n, j, olen, block_size;
    unsigned char *state;

    if (esp_cmac_ctx == NULL || data == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    state = esp_cmac_ctx->state;
    block_size = esp_cmac_ctx->cipher_block_length;

    /* Is there data still to process from the last call, that's greater in
    * size than a block? */
    if (esp_cmac_ctx->unprocessed_len > 0 && data_length > block_size - esp_cmac_ctx->unprocessed_len) {
        memcpy(&esp_cmac_ctx->unprocessed_block[esp_cmac_ctx->unprocessed_len], data, block_size - esp_cmac_ctx->unprocessed_len);
        xor_no_simd(state, esp_cmac_ctx->unprocessed_block, state, block_size);

        if ((status = esp_aes_cipher_update(&esp_cmac_ctx->esp_aes_ctx, state, block_size, state, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != PSA_SUCCESS) {
            goto exit;
        }

        data += block_size - esp_cmac_ctx->unprocessed_len;
        data_length -= block_size - esp_cmac_ctx->unprocessed_len;
        esp_cmac_ctx->unprocessed_len = 0;
    }

    /* n is the number of blocks including any final partial block */
    n = (data_length + block_size - 1) / block_size;
    /* Iterate across the input data in block sized chunks, excluding any
     * final partial or complete block */
    for (j = 1; j < n; j++) {
        xor_no_simd(state, data, state, block_size);
        if ((status = esp_aes_cipher_update(&esp_cmac_ctx->esp_aes_ctx, state, block_size, state, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != PSA_SUCCESS) {
            goto exit;
        }

        data_length -= block_size;
        data += block_size;
    }

    /* If there is data left over that wasn't aligned to a block */
    if (data_length > 0) {
        memcpy(&esp_cmac_ctx->unprocessed_block[esp_cmac_ctx->unprocessed_len], data, data_length);
        esp_cmac_ctx->unprocessed_len += data_length;
    }

    status = PSA_SUCCESS;
exit:
    return status;
}

static psa_status_t cmac_multiply_by_u(unsigned char *output,
                              const unsigned char *input,
                              size_t blocksize)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    const unsigned char R_128 = 0x87;
    unsigned char R_n;
    uint32_t overflow = 0x00;
    int i;

    if (blocksize == PSA_AES_BLOCK_SIZE) {
        R_n = R_128;
    }
#if defined(PSA_WANT_KEY_TYPE_DES)
    else if (blocksize == PSA_DES_BLOCK_SIZE) {
        const unsigned char R_64 = 0x1B;
        R_n = R_64;
    }
#endif
    else {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    for (i = (int) blocksize - 4; i >= 0; i -= 4) {
        uint32_t i32 = MBEDTLS_GET_UINT32_BE(&input[i], 0);
        uint32_t new_overflow = i32 >> 31;
        i32 = (i32 << 1) | overflow;
        MBEDTLS_PUT_UINT32_BE(i32, &output[i], 0);
        overflow = new_overflow;
    }

    unsigned char msb = (input[0] >> 7) & 1;
    output[blocksize - 1] ^= (unsigned char)(msb * R_n);

    status = PSA_SUCCESS;
exit:
    return status;
}

static psa_status_t cmac_generate_subkeys(esp_aes_operation_t *esp_aes_driver_ctx, size_t block_size,
                                            uint8_t *K1, uint8_t *K2)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    uint8_t L[PSA_CMAC_MAX_BLOCK_SIZE];
    size_t olen;

    mbedtls_platform_zeroize(L, sizeof(L));

    /* Calculate Ek(0) */
    if ((status = esp_aes_cipher_update(esp_aes_driver_ctx, L, block_size, L, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != PSA_SUCCESS) {
        goto exit;
    }

    /*
     * Generate K1 and K2
     */
    if ((status = cmac_multiply_by_u(K1, L, block_size)) != PSA_SUCCESS) {
        goto exit;
    }

    if ((status = cmac_multiply_by_u(K2, K1, block_size)) != PSA_SUCCESS) {
        goto exit;
    }

exit:
    mbedtls_platform_zeroize(L, sizeof(L));

    return status;
}

static void cmac_pad(unsigned char padded_block[MBEDTLS_CMAC_MAX_BLOCK_SIZE],
                     size_t padded_block_len,
                     const unsigned char *last_block,
                     size_t last_block_len)
{
    size_t j;

    for (j = 0; j < padded_block_len; j++) {
        if (j < last_block_len) {
            padded_block[j] = last_block[j];
        } else if (j == last_block_len) {
            padded_block[j] = 0x80;
        } else {
            padded_block[j] = 0x00;
        }
    }
}

psa_status_t esp_cmac_finish(
    esp_cmac_operation_t *esp_cmac_ctx,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t olen, block_size;

    unsigned char *state, *last_block;
    unsigned char K1[PSA_CMAC_MAX_BLOCK_SIZE];
    unsigned char K2[PSA_CMAC_MAX_BLOCK_SIZE];
    unsigned char M_last[PSA_CMAC_MAX_BLOCK_SIZE];

    if (esp_cmac_ctx == NULL || mac == NULL || mac_size == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    state = esp_cmac_ctx->state;
    block_size = esp_cmac_ctx->cipher_block_length;

    mbedtls_platform_zeroize(K1, sizeof(K1));
    mbedtls_platform_zeroize(K2, sizeof(K2));
    cmac_generate_subkeys(&esp_cmac_ctx->esp_aes_ctx, block_size, K1, K2);

    last_block = esp_cmac_ctx->unprocessed_block;

    /* Calculate last block */
    if (esp_cmac_ctx->unprocessed_len < block_size) {
        cmac_pad(M_last, block_size, last_block, esp_cmac_ctx->unprocessed_len);
        xor_no_simd(M_last, M_last, K2, block_size);
    } else {
    /* Last block is complete block */
        xor_no_simd(M_last, last_block, K1, block_size);
    }

    xor_no_simd(state, M_last, state, block_size);
    if ((status = esp_aes_cipher_update(&esp_cmac_ctx->esp_aes_ctx, state, block_size, state, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != PSA_SUCCESS) {
        goto exit;
    }

    /* CMAC output is always the cipher block size, regardless of requested mac_size */
    size_t output_length = (mac_size < block_size) ? mac_size : block_size;
    memcpy(mac, state, output_length);
    *mac_length = output_length;
exit:
    mbedtls_platform_zeroize(K1, sizeof(K1));
    mbedtls_platform_zeroize(K2, sizeof(K2));

    esp_cmac_ctx->unprocessed_len = 0;
    mbedtls_platform_zeroize(esp_cmac_ctx->unprocessed_block, sizeof(esp_cmac_ctx->unprocessed_block));
    mbedtls_platform_zeroize(state, PSA_CMAC_MAX_BLOCK_SIZE);

    return status;
}

psa_status_t esp_cmac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    esp_cmac_operation_t esp_cmac_ctx = {0};

    status = esp_cmac_setup(&esp_cmac_ctx,
                           attributes, key_buffer, key_buffer_size,
                           alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (input_length > 0) {
        status = esp_cmac_update(&esp_cmac_ctx, input, input_length);
        if (status != PSA_SUCCESS) {
            goto exit;
        }
    }
    size_t actual_mac_length = 0;
    status = esp_cmac_finish(&esp_cmac_ctx, mac, mac_size, &actual_mac_length);
    if (status == PSA_SUCCESS) {
        *mac_length = actual_mac_length;
    }

exit:
    esp_cmac_abort(&esp_cmac_ctx);

    return status;

}

psa_status_t esp_cmac_verify_finish(
    esp_cmac_operation_t *esp_cmac_ctx,
    const uint8_t *mac,
    size_t mac_length)
{
    uint8_t actual_mac[PSA_MAC_MAX_SIZE];
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (esp_cmac_ctx == NULL || mac == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (mac_length > sizeof(actual_mac)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t actual_mac_length = 0;

    status = esp_cmac_finish(esp_cmac_ctx, actual_mac, sizeof(actual_mac), &actual_mac_length);
    if (status == PSA_SUCCESS) {
        if (memcmp(actual_mac, mac, mac_length) == 0) {
            return PSA_SUCCESS;
        } else {
            return PSA_ERROR_INVALID_SIGNATURE;
        }
    }

    return status;
}
