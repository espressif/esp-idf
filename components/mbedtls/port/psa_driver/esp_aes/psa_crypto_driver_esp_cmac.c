/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_cmac.h"
#include "psa_crypto_driver_esp_cmac_contexts.h"
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

psa_status_t esp_cmac_mac_abort(esp_cmac_operation_t *operation)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    status = psa_destroy_key(operation->key_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_cipher_abort(&operation->cipher_ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    mbedtls_platform_zeroize(&operation->cipher_ctx, sizeof(psa_cipher_operation_t));
    return status;
}

static psa_status_t mac_init(
    esp_cmac_operation_t *operation,
    psa_algorithm_t alg)
{
    memset(operation, 0, sizeof(*operation));

    return PSA_SUCCESS;
}

static psa_status_t esp_cmac_setup_internal(esp_cmac_operation_t *cmac,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size)
{
    int status = PSA_ERROR_CORRUPTION_DETECTED;

#if defined(PSA_WANT_KEY_TYPE_DES)
    /* Mbed TLS CMAC does not accept 3DES with only two keys, nor does it accept
     * to do CMAC with pure DES, so return NOT_SUPPORTED here. */
    if (psa_get_key_type(attributes) == PSA_KEY_TYPE_DES &&
        (psa_get_key_bits(attributes) == 64 ||
         psa_get_key_bits(attributes) == 128)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
#endif

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t key_type = psa_get_key_type(attributes);
    size_t key_bits = psa_get_key_bits(attributes);
    psa_algorithm_t alg = PSA_ALG_ECB_NO_PADDING;

    /* Set up key attributes for PSA import */
    psa_set_key_type(&key_attributes, key_type);
    psa_set_key_bits(&key_attributes, key_bits);
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&key_attributes, alg);

    /* Import key for cipher operations */
    status = psa_import_key(&key_attributes, key_buffer, key_buffer_size, &cmac->key_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_cipher_encrypt_setup(&cmac->cipher_ctx, cmac->key_id, alg);
    if (status != 0) {
        return status;
    }

    cmac->unprocessed_len = 0;
    cmac->cipher_block_length = PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type);

    mbedtls_platform_zeroize(cmac->state, sizeof(cmac->state));
    mbedtls_platform_zeroize(cmac->unprocessed_block, sizeof(cmac->unprocessed_block));

    return PSA_SUCCESS;
}

static psa_status_t esp_cmac_mac_setup_cmac(esp_cmac_operation_t *operation,
                                  const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer,
                                  size_t key_buffer_size,
                                  psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    status = mac_init(operation, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = esp_cmac_setup_internal(operation, attributes, key_buffer, key_buffer_size);

    if (status != PSA_SUCCESS) {
        esp_cmac_mac_abort(operation);
    }

    return status;
}

psa_status_t esp_cmac_mac_setup(esp_cmac_operation_t *operation,
                            const psa_key_attributes_t *attributes,
                            const uint8_t *key_buffer,
                            size_t key_buffer_size,
                            psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (PSA_ALG_FULL_LENGTH_MAC(alg) == PSA_ALG_CMAC) {
        status = esp_cmac_mac_setup_cmac(operation, attributes, key_buffer, key_buffer_size, alg);
        operation->alg = alg;
    } else if (PSA_ALG_IS_HMAC(alg)) {
        psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(alg);
        uint8_t ipad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
        size_t i;
        size_t hash_size = PSA_HASH_LENGTH(hash_alg);
        size_t block_size = PSA_HASH_BLOCK_LENGTH(hash_alg);

        /* Sanity checks on block_size, to guarantee that there won't be a buffer
        * overflow below. This should never trigger if the hash algorithm
        * is implemented correctly. */
        /* The size checks against the ipad and opad buffers cannot be written
        * `block_size > sizeof( ipad ) || block_size > sizeof( hmac->opad )`
        * because that triggers -Wlogical-op on GCC 7.3. */
        if (block_size > sizeof(ipad)) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        if (block_size > sizeof(operation->opad)) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        if (block_size < hash_size) {
            return PSA_ERROR_NOT_SUPPORTED;
        }

        if (key_buffer_size > block_size) {
            status = esp_sha_hash_compute(hash_alg, key_buffer, key_buffer_size,
                                    ipad, sizeof(ipad), &key_buffer_size);
            if (status != PSA_SUCCESS) {
                return status;
            }
            /* After hashing, key_buffer_size is set to the hash size, which
             * should be <= block_size. Verify this for static analysis. */
            if (key_buffer_size > block_size) {
                return PSA_ERROR_CORRUPTION_DETECTED;
            }
        }
        /* A 0-length key is not commonly used in HMAC when used as a MAC,
        * but it is permitted. It is common when HMAC is used in HKDF, for
        * example. Don't call `memcpy` in the 0-length because `key` could be
        * an invalid pointer which would make the behavior undefined. */
        else if (key_buffer_size != 0) {
            /* Additional safety check: ensure key fits in ipad buffer */
            if (key_buffer_size > sizeof(ipad)) {
                return PSA_ERROR_INVALID_ARGUMENT;
            }
            memcpy(ipad, key_buffer, key_buffer_size);
        }

        /* ipad contains the key followed by garbage. Xor and fill with 0x36
        * to create the ipad value. */
        for (i = 0; i < key_buffer_size; i++) {
            ipad[i] ^= 0x36;
        }
        /* Only fill remaining bytes if key_buffer_size < block_size.
         * When key_buffer_size == block_size, the entire buffer is already
         * processed, so no padding is needed. This check also prevents
         * out-of-bounds pointer arithmetic (ipad + key_buffer_size would be
         * out of bounds when key_buffer_size == block_size == sizeof(ipad)). */
        if (key_buffer_size < block_size) {
            /* At this point: key_buffer_size < block_size <= sizeof(ipad),
             * so ipad + key_buffer_size is guaranteed to be within bounds. */
            size_t fill_size = block_size - key_buffer_size;
            memset(ipad + key_buffer_size, 0x36, fill_size);
        }

        /* Copy the key material from ipad to opad, flipping the requisite bits,
        * and filling the rest of opad with the requisite constant. */
        for (i = 0; i < key_buffer_size; i++) {
            operation->opad[i] = ipad[i] ^ 0x36 ^ 0x5C;
        }
        /* Only fill remaining bytes if key_buffer_size < block_size.
         * When key_buffer_size == block_size, the entire buffer is already
         * processed, so no padding is needed. This check also prevents
         * out-of-bounds pointer arithmetic (operation->opad + key_buffer_size
         * would be out of bounds when key_buffer_size == block_size == sizeof(operation->opad)). */
        if (key_buffer_size < block_size) {
            /* At this point: key_buffer_size < block_size <= sizeof(operation->opad),
             * so operation->opad + key_buffer_size is guaranteed to be within bounds. */
            size_t fill_size = block_size - key_buffer_size;
            memset(operation->opad + key_buffer_size, 0x5C, fill_size);
        }
        status = esp_sha_hash_setup(&operation->hmac_operation, hash_alg);
        if (status != PSA_SUCCESS) {
            return status;
        }

        status = esp_sha_hash_update(&operation->hmac_operation, ipad, block_size);
        if (status != PSA_SUCCESS) {
            return status;
        }
        operation->alg = alg;
    } else {
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        status = PSA_ERROR_NOT_SUPPORTED;
    }
    return status;
}

static void xor_no_simd(unsigned char *output, const unsigned char *input1, const unsigned char *input2, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        output[i] = input1[i] ^ input2[i];
    }
}

static psa_status_t esp_cmac_mac_update_internal(esp_cmac_operation_t *cmac, const uint8_t *data, size_t data_length)
{
    unsigned char *state = cmac->state;
    int ret = 0;
    size_t n, j, olen, block_size;

    if (cmac == NULL || data == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    block_size = cmac->cipher_block_length;

    /* Is there data still to process from the last call, that's greater in
    * size than a block? */
    if (cmac->unprocessed_len > 0 && data_length > block_size - cmac->unprocessed_len) {
        memcpy(&cmac->unprocessed_block[cmac->unprocessed_len], data, block_size - cmac->unprocessed_len);
        xor_no_simd(state, cmac->unprocessed_block, state, block_size);

        if ((ret = psa_cipher_update(&cmac->cipher_ctx, state, block_size, state, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != 0) {
            goto exit;
        }

        data += block_size - cmac->unprocessed_len;
        data_length -= block_size - cmac->unprocessed_len;
        cmac->unprocessed_len = 0;
    }

    /* n is the number of blocks including any final partial block */
    n = (data_length + block_size - 1) / block_size;
    /* Iterate across the input data in block sized chunks, excluding any
     * final partial or complete block */
    for (j = 1; j < n; j++) {
        xor_no_simd(state, data, state, block_size);
        if ((ret = psa_cipher_update(&cmac->cipher_ctx, state, block_size, state, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != 0) {
            goto exit;
        }

        data_length -= block_size;
        data += block_size;
    }

    /* If there is data left over that wasn't aligned to a block */
    if (data_length > 0) {
        memcpy(&cmac->unprocessed_block[cmac->unprocessed_len], data, data_length);
        cmac->unprocessed_len += data_length;
    }

exit:
    return ret;

}

psa_status_t esp_cmac_mac_update(esp_cmac_operation_t *cmac, const uint8_t *data, size_t data_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (PSA_ALG_FULL_LENGTH_MAC(cmac->alg) == PSA_ALG_CMAC) {
        status = esp_cmac_mac_update_internal(cmac, data, data_length);
    } else if (PSA_ALG_IS_HMAC(cmac->alg)) {
        status = esp_sha_hash_update(&cmac->hmac_operation, data, data_length);
    } else {
        (void) cmac;
        (void) data;
        (void) data_length;
        status = PSA_ERROR_NOT_SUPPORTED;
    }

    return status;
}

static inline unsigned mbedtls_ct_uint_if_else_0(uint32_t condition, unsigned if1)
{
    return (unsigned) (condition & if1);
}

static int cmac_multiply_by_u(unsigned char *output,
                              const unsigned char *input,
                              size_t blocksize)
{
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
        return PSA_ERROR_INVALID_ARGUMENT;
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


    return 0;
}

static int cmac_generate_subkeys(psa_cipher_operation_t *ctx, size_t block_size,
                                unsigned char *K1, unsigned char *K2)
{
    int ret = PSA_ERROR_CORRUPTION_DETECTED;
    unsigned char L[PSA_CMAC_MAX_BLOCK_SIZE];
    size_t olen;

    mbedtls_platform_zeroize(L, sizeof(L));

    /* Calculate Ek(0) */
    if ((ret = psa_cipher_update(ctx, L, block_size, L, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != 0) {
        goto exit;
    }

    /*
     * Generate K1 and K2
     */
    if ((ret = cmac_multiply_by_u(K1, L, block_size)) != 0) {
        goto exit;
    }

    if ((ret = cmac_multiply_by_u(K2, K1, block_size)) != 0) {
        goto exit;
    }

exit:
    mbedtls_platform_zeroize(L, sizeof(L));

    return ret;
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

static psa_status_t esp_cmac_mac_finish_internal(
    esp_cmac_operation_t *cmac,
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

    if (cmac == NULL || mac == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    state = cmac->state;
    block_size = cmac->cipher_block_length;

    mbedtls_platform_zeroize(K1, sizeof(K1));
    mbedtls_platform_zeroize(K2, sizeof(K2));
    cmac_generate_subkeys(&cmac->cipher_ctx, block_size, K1, K2);

    last_block = cmac->unprocessed_block;

    /* Calculate last block */
    if (cmac->unprocessed_len < block_size) {
        cmac_pad(M_last, block_size, last_block, cmac->unprocessed_len);
        xor_no_simd(M_last, M_last, K2, block_size);
    } else {
    /* Last block is complete block */
        xor_no_simd(M_last, last_block, K1, block_size);
    }

    xor_no_simd(state, M_last, state, block_size);
    if ((status = psa_cipher_update(&cmac->cipher_ctx, state, block_size, state, PSA_CMAC_MAX_BLOCK_SIZE, &olen)) != 0) {
        goto exit;
    }

    /* CMAC output is always the cipher block size, regardless of requested mac_size */
    size_t output_length = (mac_size < block_size) ? mac_size : block_size;
    memcpy(mac, state, output_length);
    *mac_length = output_length;
exit:
    mbedtls_platform_zeroize(K1, sizeof(K1));
    mbedtls_platform_zeroize(K2, sizeof(K2));

    cmac->unprocessed_len = 0;
    mbedtls_platform_zeroize(cmac->unprocessed_block, sizeof(cmac->unprocessed_block));
    mbedtls_platform_zeroize(state, PSA_CMAC_MAX_BLOCK_SIZE);

    return status;
}

psa_status_t esp_cmac_mac_finish(
    esp_cmac_operation_t *cmac,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (PSA_ALG_FULL_LENGTH_MAC(cmac->alg) == PSA_ALG_CMAC) {
        status = esp_cmac_mac_finish_internal(cmac, mac, mac_size, mac_length);
    } else if (PSA_ALG_IS_HMAC(cmac->alg)) {
        psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(cmac->alg);

        uint8_t tmp[PSA_HASH_MAX_SIZE];
        size_t hash_size = 0;
        size_t block_size = PSA_HASH_BLOCK_LENGTH(hash_alg);

        status = esp_sha_hash_finish(&cmac->hmac_operation, tmp, sizeof(tmp), &hash_size);
        if (status != PSA_SUCCESS) {
            return status;
        }
        /* From here on, tmp needs to be wiped. */

        status = esp_sha_hash_setup(&cmac->hmac_operation, hash_alg);
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        status = esp_sha_hash_update(&cmac->hmac_operation, cmac->opad, block_size);
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        status = esp_sha_hash_update(&cmac->hmac_operation, tmp, hash_size);
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        status = esp_sha_hash_finish(&cmac->hmac_operation, tmp, sizeof(tmp), &hash_size);
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        memcpy(mac, tmp, mac_size);

        *mac_length = mac_size;
exit:
    mbedtls_platform_zeroize(tmp, hash_size);
    } else {
        (void) cmac;
        (void) mac;
        (void) mac_length;
        (void) mac_size;
        status = PSA_ERROR_NOT_SUPPORTED;
    }
    return status;
}

psa_status_t esp_cmac_mac_compute(
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
    esp_cmac_operation_t operation = {0};

    memset(&operation, 0, sizeof(operation));

    status = esp_cmac_mac_setup(&operation,
                           attributes, key_buffer, key_buffer_size,
                           alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (input_length > 0) {
        status = esp_cmac_mac_update(&operation, input, input_length);
        if (status != PSA_SUCCESS) {
            goto exit;
        }
    }
    size_t actual_mac_length = 0;
    status = esp_cmac_mac_finish(&operation, mac, mac_size, &actual_mac_length);
    if (status == PSA_SUCCESS) {
        *mac_length = actual_mac_length;
    }

exit:
    esp_cmac_mac_abort(&operation);

    return status;

}

psa_status_t esp_cmac_mac_verify_finish(
    esp_cmac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length)
{
    uint8_t actual_mac[PSA_MAC_MAX_SIZE];
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (operation == NULL || mac == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (mac_length > sizeof(actual_mac)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t actual_mac_length = 0;

    status = esp_cmac_mac_finish(operation, actual_mac, sizeof(actual_mac), &actual_mac_length);
    if (status == PSA_SUCCESS) {
        if (memcmp(actual_mac, mac, mac_length) == 0) {
            return PSA_SUCCESS;
        } else {
            return PSA_ERROR_INVALID_SIGNATURE;
        }
    }

    return status;
}
