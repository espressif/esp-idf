/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once


#include "esp_types.h"
#include "psa/crypto_driver_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_AES_DRIVER_ENABLED)
#define ESP_MBEDTLS_AES_MAX_BLOCK_LENGTH 16
#define ESP_MBEDTLS_AES_MAX_IV_LENGTH 16

typedef struct {
    void *esp_aes_ctx;
    uint8_t iv[ESP_MBEDTLS_AES_MAX_IV_LENGTH];
    uint8_t unprocessed_data[ESP_MBEDTLS_AES_MAX_BLOCK_LENGTH];
    size_t unprocessed_len;
    psa_algorithm_t aes_alg;
    psa_encrypt_or_decrypt_t mode;
    uint8_t block_length;
} esp_aes_operation_t;

typedef struct {
    void *esp_aes_gcm_ctx;
    psa_encrypt_or_decrypt_t mode;
    size_t tag_length;  /* GCM authentication tag length (4, 8, 12-16 bytes) */
} esp_aes_gcm_operation_t;

#endif /* ESP_AES_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
