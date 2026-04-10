/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_log.h"
#include "psa_crypto_core.h"
#include "aes/esp_aes_gcm.h"
#include "psa_crypto_driver_esp_aes_gcm.h"
#include "../include/psa_crypto_driver_esp_aes_contexts.h"

#if defined(ESP_AES_DRIVER_ENABLED)

#define ESP_AES_GCM_TAG_LENGTH_DEFAULT 16

static psa_status_t esp_crypto_aes_gcm_setup(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, psa_encrypt_or_decrypt_t mode)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    esp_gcm_context *ctx = NULL;
    size_t tag_length;

    /* Extract the base algorithm and tag length */
    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0);
    if (base_alg != PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)) {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    /* Get the tag length from the algorithm */
    tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);

    if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    ctx = (esp_gcm_context *) malloc(sizeof(esp_gcm_context));
    if (ctx == NULL) {
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
        goto exit;
    }

    esp_aes_gcm_init(ctx);

    status = mbedtls_to_psa_error(esp_aes_gcm_setkey(ctx, 2, key_buffer, key_buffer_size * 8));

    if (status != PSA_SUCCESS) {
        free(ctx);
        goto exit;
    }

    esp_aes_gcm_driver_ctx->esp_aes_gcm_ctx = (void *) ctx;
    esp_aes_gcm_driver_ctx->mode = mode;
    esp_aes_gcm_driver_ctx->tag_length = tag_length;

exit:
    return status;
}

psa_status_t esp_crypto_aes_gcm_encrypt_setup(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg)
{
    return esp_crypto_aes_gcm_setup(esp_aes_gcm_driver_ctx, attributes,
                                key_buffer, key_buffer_size,
                                alg, PSA_CRYPTO_DRIVER_ENCRYPT);
}

psa_status_t esp_crypto_aes_gcm_decrypt_setup(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg)
{
    return esp_crypto_aes_gcm_setup(esp_aes_gcm_driver_ctx, attributes,
                                key_buffer, key_buffer_size,
                                alg, PSA_CRYPTO_DRIVER_DECRYPT);
}

psa_status_t esp_crypto_aes_gcm_set_nonce(
   esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
   const uint8_t *nonce,
   size_t nonce_length)
{
    esp_gcm_context *ctx = (esp_gcm_context *) esp_aes_gcm_driver_ctx->esp_aes_gcm_ctx;
    return mbedtls_to_psa_error(esp_aes_gcm_starts(ctx, esp_aes_gcm_driver_ctx->mode, nonce, nonce_length));
}

psa_status_t esp_crypto_aes_gcm_update_ad(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const uint8_t *aad,
    size_t aad_length)
{
    esp_gcm_context *ctx = (esp_gcm_context *) esp_aes_gcm_driver_ctx->esp_aes_gcm_ctx;
    return mbedtls_to_psa_error(esp_aes_gcm_update_ad(ctx, aad, aad_length));
}

psa_status_t esp_crypto_aes_gcm_update(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    size_t update_output_length = input_length;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    esp_gcm_context *ctx = (esp_gcm_context *) esp_aes_gcm_driver_ctx->esp_aes_gcm_ctx;
    status = mbedtls_to_psa_error(esp_aes_gcm_update(ctx, input, input_length, output, output_size, &update_output_length));
    if (status == PSA_SUCCESS) {
        *output_length = update_output_length;
    }
    return status;
}

psa_status_t esp_crypto_aes_gcm_finish(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    uint8_t *output,
    size_t output_size,
    size_t *output_length,
    uint8_t *tag,
    size_t tag_size,
    size_t *tag_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t finish_output_size = 0;
    size_t requested_tag_length = esp_aes_gcm_driver_ctx->tag_length;

    if (tag_size < requested_tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (requested_tag_length > ESP_AES_GCM_TAG_LENGTH_DEFAULT) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    esp_gcm_context *ctx = (esp_gcm_context *) esp_aes_gcm_driver_ctx->esp_aes_gcm_ctx;

    /* esp_aes_gcm_finish always generates a 16-byte tag, but we'll only use
     * the first requested_tag_length bytes */
    uint8_t full_tag[ESP_AES_GCM_TAG_LENGTH_DEFAULT];
    status = mbedtls_to_psa_error(esp_aes_gcm_finish(ctx, output, output_size, output_length, full_tag, sizeof(full_tag)));
    if (status == PSA_SUCCESS) {
        /* This will be zero for all supported algorithms currently, but left
         * here for future support. */
        *output_length = finish_output_size;
        *tag_length = requested_tag_length;
        /* Copy only the requested tag length */
        memcpy(tag, full_tag, requested_tag_length);
    }
    return status;
}

psa_status_t esp_crypto_aes_gcm_abort(esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx)
{
    esp_gcm_context *ctx = (esp_gcm_context *) esp_aes_gcm_driver_ctx->esp_aes_gcm_ctx;
    if (ctx == NULL) {
        return PSA_SUCCESS;
    }
    esp_aes_gcm_free(ctx);
    free(ctx);
    return PSA_SUCCESS;
}

psa_status_t esp_crypto_aes_gcm_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *plaintext, size_t plaintext_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length)
{
    uint8_t *tag = NULL;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    esp_aes_gcm_operation_t esp_aes_gcm_driver_ctx;
    memset(&esp_aes_gcm_driver_ctx, 0, sizeof(esp_aes_gcm_operation_t));

    status = esp_crypto_aes_gcm_encrypt_setup(&esp_aes_gcm_driver_ctx, attributes,
                                            key_buffer, key_buffer_size,
                                            alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    size_t tag_length = esp_aes_gcm_driver_ctx.tag_length;

    /* For all currently supported modes, the tag is at the end of the
     * ciphertext. */
    if (ciphertext_size < (plaintext_length + tag_length)) {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }
    tag = ciphertext + plaintext_length;

    /* esp_aes_gcm_crypt_and_tag always generates a 16-byte tag */
    uint8_t full_tag[ESP_AES_GCM_TAG_LENGTH_DEFAULT];
    status = mbedtls_to_psa_error(
                esp_aes_gcm_crypt_and_tag((esp_gcm_context *) esp_aes_gcm_driver_ctx.esp_aes_gcm_ctx,
                                      PSA_CRYPTO_DRIVER_ENCRYPT,
                                      plaintext_length,
                                      nonce, nonce_length,
                                      additional_data, additional_data_length,
                                      plaintext, ciphertext,
                                      ESP_AES_GCM_TAG_LENGTH_DEFAULT, full_tag));

    if (status == PSA_SUCCESS) {
        /* Copy only the requested tag length */
        memcpy(tag, full_tag, tag_length);
        *ciphertext_length = plaintext_length + tag_length;
    }

exit:
    esp_crypto_aes_gcm_abort(&esp_aes_gcm_driver_ctx);

    return status;
}

/* Locate the tag in a ciphertext buffer containing the encrypted data
 * followed by the tag. Return the length of the part preceding the tag in
 * *plaintext_length. This is the size of the plaintext in modes where
 * the encrypted data has the same size as the plaintext, such as
 * CCM and GCM. */
static psa_status_t psa_aead_unpadded_locate_tag(size_t tag_length,
                                                 const uint8_t *ciphertext,
                                                 size_t ciphertext_length,
                                                 size_t plaintext_size,
                                                 const uint8_t **p_tag)
{
    size_t payload_length;
    if (tag_length > ciphertext_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    payload_length = ciphertext_length - tag_length;
    if (payload_length > plaintext_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    *p_tag = ciphertext + payload_length;
    return PSA_SUCCESS;
}

psa_status_t esp_crypto_aes_gcm_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *ciphertext, size_t ciphertext_length,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    esp_aes_gcm_operation_t esp_aes_gcm_driver_ctx;
    memset(&esp_aes_gcm_driver_ctx, 0, sizeof(esp_aes_gcm_operation_t));
    const uint8_t *tag = NULL;

    status = esp_crypto_aes_gcm_decrypt_setup(&esp_aes_gcm_driver_ctx, attributes,
                                            key_buffer, key_buffer_size,
                                            alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    size_t tag_length = esp_aes_gcm_driver_ctx.tag_length;

    status = psa_aead_unpadded_locate_tag(tag_length, ciphertext, ciphertext_length, plaintext_size, &tag);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    /* esp_aes_gcm_auth_decrypt supports variable tag lengths (4-16 bytes)
     * and will compare only the first tag_length bytes */
    status = mbedtls_to_psa_error(
        esp_aes_gcm_auth_decrypt((esp_gcm_context *) esp_aes_gcm_driver_ctx.esp_aes_gcm_ctx,
                                ciphertext_length - tag_length,
                                nonce, nonce_length,
                                additional_data,
                                additional_data_length,
                                tag, tag_length,
                                ciphertext, plaintext));

    if (status == PSA_SUCCESS) {
        *plaintext_length = ciphertext_length - tag_length;
    }

exit:
    esp_crypto_aes_gcm_abort(&esp_aes_gcm_driver_ctx);
    return status;
}
// #endif /* ESP_MBEDTLS_AES_ACCEL */
#endif /* ESP_AES_DRIVER_ENABLED */
