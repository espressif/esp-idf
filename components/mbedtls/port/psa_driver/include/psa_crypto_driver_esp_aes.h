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
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

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
    size_t *output_length);

psa_status_t esp_aes_cipher_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t esp_aes_cipher_decrypt_setup(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t esp_aes_cipher_encrypt_setup(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t esp_aes_cipher_set_iv(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const uint8_t *iv,
    size_t iv_length);

psa_status_t esp_aes_cipher_update(
    esp_aes_operation_t *esp_aes_driver_ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t esp_aes_cipher_finish(
    esp_aes_operation_t *esp_aes_driver_ctx,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t esp_aes_cipher_abort(
    esp_aes_operation_t *esp_aes_driver_ctx);
#endif /* ESP_AES_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
