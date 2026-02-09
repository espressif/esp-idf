/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <string.h>
#include <stdlib.h>
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_hmac_transparent.h"
#include "psa_crypto_driver_esp_sha.h"
#include "psa_crypto_driver_esp_md5.h"
#include "sdkconfig.h"

psa_status_t esp_hmac_abort_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

#if CONFIG_MBEDTLS_ROM_MD5
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(esp_hmac_ctx->alg);
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_abort(&esp_hmac_ctx->md5_ctx);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_abort(&esp_hmac_ctx->esp_sha_ctx);
    }

    if (status != PSA_SUCCESS) {
        return status;
    }

    // Free dynamically allocated opad buffer
    if (esp_hmac_ctx->opad != NULL) {
        mbedtls_platform_zeroize(esp_hmac_ctx->opad, PSA_HMAC_MAX_HASH_BLOCK_SIZE);
        free(esp_hmac_ctx->opad);
        esp_hmac_ctx->opad = NULL;
    }

    mbedtls_platform_zeroize(esp_hmac_ctx, sizeof(esp_hmac_transparent_operation_t));
    return status;
}

psa_status_t esp_hmac_setup_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx,
                            const psa_key_attributes_t *attributes,
                            const uint8_t *key_buffer,
                            size_t key_buffer_size,
                            psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(alg);
    uint8_t ipad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    size_t i;
    size_t hash_size = PSA_HASH_LENGTH(hash_alg);
    size_t block_size = PSA_HASH_BLOCK_LENGTH(hash_alg);

    if (esp_hmac_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memset(esp_hmac_ctx, 0, sizeof(esp_hmac_transparent_operation_t));

    // Allocate opad buffer dynamically
    esp_hmac_ctx->opad = (uint8_t *)malloc(PSA_HMAC_MAX_HASH_BLOCK_SIZE);
    if (esp_hmac_ctx->opad == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    memset(esp_hmac_ctx->opad, 0, PSA_HMAC_MAX_HASH_BLOCK_SIZE);

    if (
#if CONFIG_MBEDTLS_ROM_MD5
        hash_alg != PSA_ALG_MD5 &&
#endif // CONFIG_MBEDTLS_ROM_MD5
        (hash_alg < PSA_ALG_SHA_1
#if SOC_SHA_SUPPORT_SHA512
        || hash_alg > PSA_ALG_SHA_512
#else
        || hash_alg > PSA_ALG_SHA_256
#endif // SOC_SHA_SUPPORT_SHA512
        )) {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto error;
    }

    esp_hmac_ctx->alg = alg;

    /* Sanity checks on block_size, to guarantee that there won't be a buffer
    * overflow below. This should never trigger if the hash algorithm
    * is implemented correctly. */
    /* The size check against the ipad buffer also covers opad since both
    * have the same size (PSA_HMAC_MAX_HASH_BLOCK_SIZE). */
    if ((block_size > sizeof(ipad)) || (block_size < hash_size)) {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto error;
    }

    if (key_buffer_size > block_size) {
#if CONFIG_MBEDTLS_ROM_MD5
        if (hash_alg == PSA_ALG_MD5) {
            status = esp_md5_hash_compute(hash_alg, key_buffer, key_buffer_size,
                                    ipad, sizeof(ipad), &key_buffer_size);
        } else
#endif // CONFIG_MBEDTLS_ROM_MD5
        {
            status = esp_sha_hash_compute(hash_alg, key_buffer, key_buffer_size,
                                    ipad, sizeof(ipad), &key_buffer_size);
        }
        if (status != PSA_SUCCESS) {
            goto error;
        }
        /* After hashing, key_buffer_size is set to the hash size, which
        * should be <= block_size. Verify this for static analysis. */
        if (key_buffer_size > block_size) {
            status = PSA_ERROR_CORRUPTION_DETECTED;
            goto error;
        }
    }
    /* A 0-length key is not commonly used in HMAC when used as a MAC,
    * but it is permitted. It is common when HMAC is used in HKDF, for
    * example. Don't call `memcpy` in the 0-length because `key` could be
    * an invalid pointer which would make the behavior undefined. */
    else if (key_buffer_size != 0) {
        /* Additional safety check: ensure key fits in ipad buffer */
        if (key_buffer_size > sizeof(ipad)) {
            status = PSA_ERROR_INVALID_ARGUMENT;
            goto error;
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
        esp_hmac_ctx->opad[i] = ipad[i] ^ 0x36 ^ 0x5C;
    }
    /* Only fill remaining bytes if key_buffer_size < block_size.
    * When key_buffer_size == block_size, the entire buffer is already
    * processed, so no padding is needed. This check also prevents
    * out-of-bounds pointer arithmetic (esp_hmac_ctx->opad + key_buffer_size
    * would be out of bounds when key_buffer_size == block_size == sizeof(esp_hmac_ctx->opad)). */
    if (key_buffer_size < block_size) {
        /* At this point: key_buffer_size < block_size <= sizeof(esp_hmac_ctx->opad),
        * so esp_hmac_ctx->opad + key_buffer_size is guaranteed to be within bounds. */
        size_t fill_size = block_size - key_buffer_size;
        memset(esp_hmac_ctx->opad + key_buffer_size, 0x5C, fill_size);
    }

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_setup(&esp_hmac_ctx->md5_ctx, hash_alg);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_setup(&esp_hmac_ctx->esp_sha_ctx, hash_alg);
    }
    if (status != PSA_SUCCESS) {
        goto error;
    }

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_update(&esp_hmac_ctx->md5_ctx, ipad, block_size);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_update(&esp_hmac_ctx->esp_sha_ctx, ipad, block_size);
    }
    if (status != PSA_SUCCESS) {
        goto error;
    }

    return status;

error:
    esp_hmac_abort_transparent(esp_hmac_ctx);
    return status;
}

psa_status_t esp_hmac_update_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx, const uint8_t *data, size_t data_length)
{

    if (esp_hmac_ctx == NULL || data == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

#if CONFIG_MBEDTLS_ROM_MD5
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(esp_hmac_ctx->alg);
    if (hash_alg == PSA_ALG_MD5) {
        return esp_md5_hash_update(&esp_hmac_ctx->md5_ctx, data, data_length);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        return esp_sha_hash_update(&esp_hmac_ctx->esp_sha_ctx, data, data_length);
    }
}

psa_status_t esp_hmac_finish_transparent(
    esp_hmac_transparent_operation_t *esp_hmac_ctx,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (esp_hmac_ctx == NULL || mac == NULL || mac_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(esp_hmac_ctx->alg);

    uint8_t tmp[PSA_HASH_MAX_SIZE];
    size_t hash_size = 0;
    size_t block_size = PSA_HASH_BLOCK_LENGTH(hash_alg);

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_finish(&esp_hmac_ctx->md5_ctx, tmp, sizeof(tmp), &hash_size);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_finish(&esp_hmac_ctx->esp_sha_ctx, tmp, sizeof(tmp), &hash_size);
    }
    if (status != PSA_SUCCESS) {
        return status;
    }
    /* From here on, tmp needs to be wiped. */

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_setup(&esp_hmac_ctx->md5_ctx, hash_alg);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_setup(&esp_hmac_ctx->esp_sha_ctx, hash_alg);
    }
    if (status != PSA_SUCCESS) {
        goto exit;
    }

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_update(&esp_hmac_ctx->md5_ctx, esp_hmac_ctx->opad, block_size);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_update(&esp_hmac_ctx->esp_sha_ctx, esp_hmac_ctx->opad, block_size);
    }
    if (status != PSA_SUCCESS) {
        goto exit;
    }

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_update(&esp_hmac_ctx->md5_ctx, tmp, hash_size);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_update(&esp_hmac_ctx->esp_sha_ctx, tmp, hash_size);
    }
    if (status != PSA_SUCCESS) {
        goto exit;
    }

#if CONFIG_MBEDTLS_ROM_MD5
    if (hash_alg == PSA_ALG_MD5) {
        status = esp_md5_hash_finish(&esp_hmac_ctx->md5_ctx, tmp, sizeof(tmp), &hash_size);
    } else
#endif // CONFIG_MBEDTLS_ROM_MD5
    {
        status = esp_sha_hash_finish(&esp_hmac_ctx->esp_sha_ctx, tmp, sizeof(tmp), &hash_size);
    }
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (mac_size < hash_size) {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }
    memcpy(mac, tmp, hash_size);
    *mac_length = hash_size;

exit:
    mbedtls_platform_zeroize(tmp, hash_size);
    return status;
}

psa_status_t esp_hmac_compute_transparent(
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
    esp_hmac_transparent_operation_t esp_hmac_ctx = {0};

    status = esp_hmac_setup_transparent(&esp_hmac_ctx,
                        attributes, key_buffer, key_buffer_size,
                        alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (input_length > 0) {
        status = esp_hmac_update_transparent(&esp_hmac_ctx, input, input_length);
        if (status != PSA_SUCCESS) {
            goto exit;
        }
    }

    size_t actual_mac_length = 0;
    status = esp_hmac_finish_transparent(&esp_hmac_ctx, mac, mac_size, &actual_mac_length);
    if (status == PSA_SUCCESS) {
        *mac_length = actual_mac_length;
    }

exit:
    esp_hmac_abort_transparent(&esp_hmac_ctx);

    return status;

}

psa_status_t esp_hmac_verify_finish_transparent(
    esp_hmac_transparent_operation_t *esp_hmac_ctx,
    const uint8_t *mac,
    size_t mac_length)
{
    uint8_t actual_mac[PSA_MAC_MAX_SIZE];
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (esp_hmac_ctx == NULL || mac == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (mac_length > sizeof(actual_mac)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t actual_mac_length = 0;

    status = esp_hmac_finish_transparent(esp_hmac_ctx, actual_mac, sizeof(actual_mac), &actual_mac_length);
    if (status == PSA_SUCCESS) {
        if (memcmp(actual_mac, mac, mac_length) != 0) {
            status = PSA_ERROR_INVALID_SIGNATURE;
        }
    }

    mbedtls_platform_zeroize(actual_mac, sizeof(actual_mac));
    return status;
}
