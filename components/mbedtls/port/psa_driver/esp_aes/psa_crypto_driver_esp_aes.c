/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_aes.h"
#include "../include/psa_crypto_driver_esp_aes_contexts.h"
#include "esp_aes.h"
#include "psa_crypto_core.h"
#include "constant_time_internal.h"
#include "esp_log.h"

static const char *TAG = "psa_crypto_driver_esp_aes";

static psa_status_t esp_aes_cipher_setup(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, psa_encrypt_or_decrypt_t mode)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    if (!PSA_ALG_IS_CIPHER(alg)) {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    switch (alg) {
        case PSA_ALG_ECB_NO_PADDING:
        case PSA_ALG_CBC_NO_PADDING:
        case PSA_ALG_CBC_PKCS7:
        case PSA_ALG_CTR:
        case PSA_ALG_XTS:
        case PSA_ALG_CFB:
        case PSA_ALG_OFB:
            break;
        default:
            status = PSA_ERROR_NOT_SUPPORTED;
            goto exit;
    }

    esp_aes_context *ctx = (esp_aes_context *) malloc(sizeof(esp_aes_context));
    if (ctx == NULL) {
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
        goto exit;
    }

    esp_aes_init(ctx);

    status = mbedtls_to_psa_error(esp_aes_setkey(ctx, key_buffer, key_buffer_size * 8));

    if (status != PSA_SUCCESS) {
        free(ctx);
        goto exit;
    }

    esp_aes_driver_ctx->aes_alg = alg;
    esp_aes_driver_ctx->mode = mode;
    esp_aes_driver_ctx->esp_aes_ctx = (void *) ctx;
    esp_aes_driver_ctx->block_length = (PSA_ALG_IS_STREAM_CIPHER(alg) ? 1 : PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES));
exit:
    return status;
}

psa_status_t esp_aes_cipher_encrypt_setup(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg)
{
    return esp_aes_cipher_setup(esp_aes_driver_ctx, attributes,
                                key_buffer, key_buffer_size,
                                alg, PSA_CRYPTO_DRIVER_ENCRYPT);
}

psa_status_t esp_aes_cipher_decrypt_setup(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg)
{
    return esp_aes_cipher_setup(esp_aes_driver_ctx, attributes,
                                key_buffer, key_buffer_size,
                                alg, PSA_CRYPTO_DRIVER_DECRYPT);
}

psa_status_t esp_aes_cipher_set_iv(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const uint8_t *iv,
    size_t iv_length)
{
    if (iv_length != PSA_CIPHER_IV_LENGTH(PSA_KEY_TYPE_AES, esp_aes_driver_ctx->aes_alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(esp_aes_driver_ctx->iv, iv, iv_length);
    return PSA_SUCCESS;
}

static psa_status_t esp_aes_ecb_update(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t *output_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    *output_length = 0;

    esp_aes_context *ctx = (esp_aes_context *) esp_aes_driver_ctx->esp_aes_ctx;

    if (esp_aes_driver_ctx->unprocessed_len > 0) {
        /* Fill up to block size, and run the block if there's a full one. */
        size_t bytes_to_copy = esp_aes_driver_ctx->block_length - esp_aes_driver_ctx->unprocessed_len;

        if (input_length < bytes_to_copy) {
            bytes_to_copy = input_length;
        }

        memcpy(&(esp_aes_driver_ctx->unprocessed_data[esp_aes_driver_ctx->unprocessed_len]), input, bytes_to_copy);
        input_length -= bytes_to_copy;
        input += bytes_to_copy;
        esp_aes_driver_ctx->unprocessed_len += bytes_to_copy;

        if (esp_aes_driver_ctx->unprocessed_len == esp_aes_driver_ctx->block_length) {
            status = mbedtls_to_psa_error(esp_aes_crypt_ecb(ctx, esp_aes_driver_ctx->mode, esp_aes_driver_ctx->unprocessed_data, output));
            if (status != PSA_SUCCESS) {
                goto exit;
            }

            output += esp_aes_driver_ctx->block_length;
            *output_length += esp_aes_driver_ctx->block_length;
            esp_aes_driver_ctx->unprocessed_len = 0;
        }
    }

    while (input_length >= esp_aes_driver_ctx->block_length) {
        /* Run all full blocks we have, one by one */
        status = mbedtls_to_psa_error(esp_aes_crypt_ecb(ctx, esp_aes_driver_ctx->mode, input, output));
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        input_length -= esp_aes_driver_ctx->block_length;
        input += esp_aes_driver_ctx->block_length;

        output += esp_aes_driver_ctx->block_length;
        *output_length += esp_aes_driver_ctx->block_length;
    }

    if (input_length > 0) {
        /* Save unprocessed bytes for later processing */
        memcpy(&(esp_aes_driver_ctx->unprocessed_data[esp_aes_driver_ctx->unprocessed_len]), input, input_length);
        esp_aes_driver_ctx->unprocessed_len += input_length;
    }

    status = PSA_SUCCESS;
exit:
    return status;
}

static psa_status_t esp_aes_cbc_update(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size,
    size_t *output_length)
{
    int ret = -1;
    esp_aes_context *ctx = (esp_aes_context *) esp_aes_driver_ctx->esp_aes_ctx;
    size_t copy_len = 0;
    *output_length = 0;

    /*
    * If there is not enough data for a full block, cache it.
    */
    if ((esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_DECRYPT &&
        esp_aes_driver_ctx->aes_alg != PSA_ALG_CBC_NO_PADDING &&
        input_length <= esp_aes_driver_ctx->block_length - esp_aes_driver_ctx->unprocessed_len) ||
        (esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_DECRYPT &&
        esp_aes_driver_ctx->aes_alg == PSA_ALG_CBC_NO_PADDING &&
        input_length < esp_aes_driver_ctx->block_length - esp_aes_driver_ctx->unprocessed_len) ||
        (esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_ENCRYPT &&
        input_length < esp_aes_driver_ctx->block_length - esp_aes_driver_ctx->unprocessed_len)) {

        memcpy(&(esp_aes_driver_ctx->unprocessed_data[esp_aes_driver_ctx->unprocessed_len]), input, input_length);
        esp_aes_driver_ctx->unprocessed_len += input_length;
        return PSA_SUCCESS;
    }

    /*
    * Process cached data first
    */
    if (esp_aes_driver_ctx->unprocessed_len != 0) {
        copy_len = esp_aes_driver_ctx->block_length - esp_aes_driver_ctx->unprocessed_len;

        memcpy(&(esp_aes_driver_ctx->unprocessed_data[esp_aes_driver_ctx->unprocessed_len]), input, copy_len);

        ret = esp_aes_crypt_cbc(ctx,
                               esp_aes_driver_ctx->mode,
                               esp_aes_driver_ctx->block_length,
                               esp_aes_driver_ctx->iv,
                               esp_aes_driver_ctx->unprocessed_data,
                               output);
        if (ret != 0) {
            goto exit;
        }

        *output_length += esp_aes_driver_ctx->block_length;
        output += esp_aes_driver_ctx->block_length;
        esp_aes_driver_ctx->unprocessed_len = 0;

        input += copy_len;
        input_length -= copy_len;
    }
    /*
     * Cache final, incomplete block
     */
    if (input_length != 0) {
        /* Encryption: only cache partial blocks
         * Decryption w/ padding: always keep at least one whole block
         * Decryption w/o padding: only cache partial blocks
         */
        copy_len = input_length % esp_aes_driver_ctx->block_length;
        if (copy_len == 0 &&
            esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_DECRYPT &&
            esp_aes_driver_ctx->aes_alg != PSA_ALG_CBC_NO_PADDING) {
            copy_len = esp_aes_driver_ctx->block_length;
        }

        memcpy(esp_aes_driver_ctx->unprocessed_data, &(input[input_length - copy_len]), copy_len);

        esp_aes_driver_ctx->unprocessed_len += copy_len;
        input_length -= copy_len;
    }
    /*
     * Process remaining full blocks
     */
    if (input_length) {
        ret = esp_aes_crypt_cbc(ctx, esp_aes_driver_ctx->mode,
                                input_length, esp_aes_driver_ctx->iv,
                                input,
                                output);
        if (ret != 0) {
            goto exit;
        }

        *output_length += input_length;
    }

exit:
    return mbedtls_to_psa_error(ret);
}

psa_status_t esp_aes_cipher_update(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    int ret = -1;
    size_t expected_output_size;
    *output_length = 0;

    if (!PSA_ALG_IS_STREAM_CIPHER(esp_aes_driver_ctx->aes_alg)) {
        /* Take the unprocessed partial block left over from previous
         * update calls, if any, plus the input to this call. Remove
         * the last partial block, if any. You get the data that will be
         * output in this call. */
        expected_output_size = (esp_aes_driver_ctx->unprocessed_len + input_length) / esp_aes_driver_ctx->block_length * esp_aes_driver_ctx->block_length;
    } else {
        expected_output_size = input_length;
    }

    if (output_size < expected_output_size) {
        ESP_LOGE(TAG, "Output buffer too small: have %zu, need %zu", output_size, expected_output_size);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (input_length == 0) {
        /* There is no input, nothing to be done */
        *output_length = 0;
        return PSA_SUCCESS;
    }
    else if (esp_aes_driver_ctx->aes_alg == PSA_ALG_ECB_NO_PADDING) {
        /* esp_aes_crypt_ecb  will only process a single block at a time in
         * ECB mode. Abstract this away to match the PSA API behaviour. */
        ret = esp_aes_ecb_update(esp_aes_driver_ctx,
                                        input,
                                        input_length,
                                        output,
                                        output_length);
    } else {
        if (input == output &&
            (esp_aes_driver_ctx->unprocessed_len != 0 || input_length % esp_aes_driver_ctx->block_length)) {
            ret = MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
            goto exit;
        }

        switch (esp_aes_driver_ctx->aes_alg) {
#if CONFIG_MBEDTLS_CIPHER_MODE_CTR
            case PSA_ALG_CTR:
                ret = esp_aes_crypt_ctr(esp_aes_driver_ctx->esp_aes_ctx,
                                        input_length,
                                        &esp_aes_driver_ctx->unprocessed_len,
                                        esp_aes_driver_ctx->iv,
                                        esp_aes_driver_ctx->unprocessed_data,
                                        input,
                                        output);
                *output_length = input_length;
                break;
#endif /* CONFIG_MBEDTLS_CIPHER_MODE_CTR */
#if CONFIG_MBEDTLS_CIPHER_MODE_CFB
            case PSA_ALG_CFB:
                ret = esp_aes_crypt_cfb128(esp_aes_driver_ctx->esp_aes_ctx,
                                        esp_aes_driver_ctx->mode,
                                        input_length,
                                        &esp_aes_driver_ctx->unprocessed_len,
                                        esp_aes_driver_ctx->iv,
                                        input,
                                        output);
                *output_length = input_length;
                break;
#endif /* CONFIG_MBEDTLS_CIPHER_MODE_CFB */
#if CONFIG_MBEDTLS_CIPHER_MODE_OFB
            case PSA_ALG_OFB:
                ret = esp_aes_crypt_ofb(esp_aes_driver_ctx->esp_aes_ctx,
                                        input_length,
                                        &esp_aes_driver_ctx->unprocessed_len,
                                        esp_aes_driver_ctx->iv,
                                        input,
                                        output);
                *output_length = input_length;
                break;
#endif /* CONFIG_MBEDTLS_CIPHER_MODE_OFB */
            case PSA_ALG_CBC_NO_PADDING:
            case PSA_ALG_CBC_PKCS7:
                ret = esp_aes_cbc_update(esp_aes_driver_ctx,
                                                input,
                                                input_length,
                                                output,
                                                output_size,
                                                output_length);
                break;
            default:
                ret = MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
                goto exit;
        }

        if (*output_length > output_size) {
            return PSA_ERROR_CORRUPTION_DETECTED;
        }
    }

exit:
    return mbedtls_to_psa_error(ret);
}

/*
 * PKCS7 (and PKCS5) padding: fill with ll bytes, with ll = padding_len
 */
static void add_pkcs_padding(unsigned char *output, size_t output_len,
                             size_t data_len)
{
    size_t padding_len = output_len - data_len;
    unsigned char i;

    for (i = 0; i < padding_len; i++) {
        output[data_len + i] = (unsigned char) padding_len;
    }
}

static int get_pkcs_padding(unsigned char *input, size_t input_len, size_t *data_len)
{
    size_t i, pad_idx;
    unsigned char padding_len;

    if (NULL == input || NULL == data_len) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    padding_len = input[input_len - 1];
    if (padding_len == 0 || padding_len > input_len) {
        return MBEDTLS_ERR_CIPHER_INVALID_PADDING;
    }
    *data_len = input_len - padding_len;

    mbedtls_ct_condition_t bad = mbedtls_ct_uint_gt(padding_len, input_len);
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_eq(padding_len, 0));

    /* The number of bytes checked must be independent of padding_len,
     * so pick input_len, which is usually 8 or 16 (one block) */
    pad_idx = input_len - padding_len;
    for (i = 0; i < input_len; i++) {
        mbedtls_ct_condition_t in_padding = mbedtls_ct_uint_ge(i, pad_idx);
        mbedtls_ct_condition_t different  = mbedtls_ct_uint_ne(input[i], padding_len);
        bad = mbedtls_ct_bool_or(bad, mbedtls_ct_bool_and(in_padding, different));
    }

    return mbedtls_ct_error_if_else_0(bad, MBEDTLS_ERR_CIPHER_INVALID_PADDING);
}

psa_status_t esp_aes_cipher_finish(
    esp_aes_operation_t *esp_aes_driver_ctx,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    int ret = -1;
    esp_aes_context *ctx = (esp_aes_context *) esp_aes_driver_ctx->esp_aes_ctx;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    uint8_t temp_output_buffer[ESP_MBEDTLS_AES_MAX_BLOCK_LENGTH];

    if (esp_aes_driver_ctx->unprocessed_len != 0) {
        if (esp_aes_driver_ctx->aes_alg == PSA_ALG_ECB_NO_PADDING ||
            esp_aes_driver_ctx->aes_alg == PSA_ALG_CBC_NO_PADDING) {
            status = PSA_ERROR_INVALID_ARGUMENT;
            goto exit;
        }
    }

    *output_length = 0;
    switch (esp_aes_driver_ctx->aes_alg) {
        case PSA_ALG_ECB_NO_PADDING:
        case PSA_ALG_CTR:
        case PSA_ALG_XTS:
        case PSA_ALG_CFB:
        case PSA_ALG_OFB:
            status = PSA_SUCCESS;
            break;
        case PSA_ALG_CBC_PKCS7:
            if (esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
                /* PKCS7 padding: always add padding, even if data is block-aligned.
                 * If unprocessed_len == 0, we add a full padding block.
                 * Otherwise, we pad the partial block. */
                add_pkcs_padding(esp_aes_driver_ctx->unprocessed_data, esp_aes_driver_ctx->block_length, esp_aes_driver_ctx->unprocessed_len);
            } else if (esp_aes_driver_ctx->unprocessed_len != esp_aes_driver_ctx->block_length) {
                /*
                 * For decrypt operations, expect a full block,
                 * or an empty block if no padding
                 */
                if (esp_aes_driver_ctx->unprocessed_len == 0) {
                    status = PSA_SUCCESS;
                    break;
                }
                return mbedtls_to_psa_error(MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);
            }
            ret = esp_aes_crypt_cbc(ctx, esp_aes_driver_ctx->mode,
                                    esp_aes_driver_ctx->block_length,
                                    esp_aes_driver_ctx->iv,
                                    esp_aes_driver_ctx->unprocessed_data,
                                    temp_output_buffer);
            if (ret != 0) {
                return mbedtls_to_psa_error(ret);
            }

            if (esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_DECRYPT) {
                ret = get_pkcs_padding(temp_output_buffer, esp_aes_driver_ctx->block_length, output_length);
                if (ret != 0) {
                    return mbedtls_to_psa_error(ret);
                }
            } else {
                *output_length = esp_aes_driver_ctx->block_length;
            }
            status = PSA_SUCCESS;

            break;
        case PSA_ALG_CBC_NO_PADDING:
            if (esp_aes_driver_ctx->mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
                if (esp_aes_driver_ctx->unprocessed_len == 0) {
                    status = PSA_SUCCESS;
                    break;
                }
            } else if (esp_aes_driver_ctx->unprocessed_len != esp_aes_driver_ctx->block_length) {
                if (esp_aes_driver_ctx->unprocessed_len == 0) {
                    return PSA_SUCCESS;
                }
                return mbedtls_to_psa_error(MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);
            }

            ret = esp_aes_crypt_cbc(ctx, esp_aes_driver_ctx->mode,
                                    esp_aes_driver_ctx->block_length,
                                    esp_aes_driver_ctx->iv,
                                    esp_aes_driver_ctx->unprocessed_data,
                                    temp_output_buffer);
            if (ret != 0) {
                return mbedtls_to_psa_error(ret);
            }

            *output_length = esp_aes_driver_ctx->block_length;
            status = PSA_SUCCESS;
            break;
        default:
            status = mbedtls_to_psa_error(MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);
            goto exit;
    }

    if (*output_length == 0) {
        ; /* Nothing to copy. Note that output may be NULL in this case. */
    } else if (output_size >= *output_length) {
        memcpy(output, temp_output_buffer, *output_length);
    } else {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
    }

exit:
    mbedtls_platform_zeroize(temp_output_buffer, sizeof(temp_output_buffer));
    return status;
}

psa_status_t esp_aes_cipher_abort(
    esp_aes_operation_t *esp_aes_driver_ctx)
{
    esp_aes_context *ctx = (esp_aes_context *) esp_aes_driver_ctx->esp_aes_ctx;
    if (ctx == NULL) {
        return PSA_SUCCESS;
    }
    esp_aes_free(ctx);
    free(ctx);
    return PSA_SUCCESS;
}

psa_status_t esp_aes_cipher_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    esp_aes_operation_t esp_aes_driver_ctx;
    memset(&esp_aes_driver_ctx, 0, sizeof(esp_aes_operation_t));
    size_t update_output_length, finish_output_length;

    status = esp_aes_cipher_encrypt_setup(&esp_aes_driver_ctx, attributes,
                                        key_buffer, key_buffer_size,
                                        alg);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to setup encryption: %ld", status);
        goto exit;
    }

    if (iv_length > 0) {
        status = esp_aes_cipher_set_iv(&esp_aes_driver_ctx, iv, iv_length);
        if (status != PSA_SUCCESS) {
            ESP_LOGE(TAG, "Failed to set IV: %ld", status);
            goto exit;
        }
    }

    status = esp_aes_cipher_update(&esp_aes_driver_ctx, input, input_length,
                                output, output_size,
                                &update_output_length);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to update: %ld", status);
        goto exit;
    }

    status = esp_aes_cipher_finish(&esp_aes_driver_ctx,
        mbedtls_buffer_offset(output, update_output_length),
        output_size - update_output_length, &finish_output_length);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to finish: %ld", status);
        goto exit;
    }

    *output_length = update_output_length + finish_output_length;

exit:
    esp_aes_cipher_abort(&esp_aes_driver_ctx);

    return status;
}

psa_status_t esp_aes_cipher_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    esp_aes_operation_t esp_aes_driver_ctx;
    memset(&esp_aes_driver_ctx, 0, sizeof(esp_aes_operation_t));
    size_t olength, accumulated_length;

    status = esp_aes_cipher_decrypt_setup(&esp_aes_driver_ctx, attributes,
                                              key, key_length,
                                              alg);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to setup decryption: %ld", status);
        goto exit;
    }

    uint8_t iv_length = PSA_CIPHER_IV_LENGTH(psa_get_key_type(attributes), alg);

    if (iv_length > 0) {
        status = esp_aes_cipher_set_iv(&esp_aes_driver_ctx,
                                           input, iv_length);
        if (status != PSA_SUCCESS) {
            ESP_LOGE(TAG, "Failed to set IV: %ld", status);
            goto exit;
        }
    }

    status = esp_aes_cipher_update(&esp_aes_driver_ctx,
        mbedtls_buffer_offset_const(input, iv_length),
        input_length - iv_length,
        output, output_size, &olength);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to update: %ld", status);
        goto exit;
    }

    accumulated_length = olength;

    status = esp_aes_cipher_finish(&esp_aes_driver_ctx,
        mbedtls_buffer_offset(output, accumulated_length),
        output_size - accumulated_length, &olength);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to finish: %ld", status);
        goto exit;
    }

    *output_length = accumulated_length + olength;

exit:
    esp_aes_cipher_abort(&esp_aes_driver_ctx);

    return status;
}
