/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_aes_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_AES_DRIVER_ENABLED)

psa_status_t esp_crypto_aes_gcm_encrypt_setup(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t esp_crypto_aes_gcm_decrypt_setup(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t esp_crypto_aes_gcm_set_nonce(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const uint8_t *nonce,
    size_t nonce_length);

psa_status_t esp_crypto_aes_gcm_update_ad(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const uint8_t *aad,
    size_t aad_length);

psa_status_t esp_crypto_aes_gcm_update(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t esp_crypto_aes_gcm_finish(
    esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx,
    uint8_t *output,
    size_t output_size,
    size_t *output_length,
    uint8_t *tag,
    size_t tag_size,
    size_t *tag_length);

psa_status_t esp_crypto_aes_gcm_abort(esp_aes_gcm_operation_t *esp_aes_gcm_driver_ctx);

psa_status_t esp_crypto_aes_gcm_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *plaintext, size_t plaintext_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

psa_status_t esp_crypto_aes_gcm_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *ciphertext, size_t ciphertext_length,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length);

#endif /* ESP_AES_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
