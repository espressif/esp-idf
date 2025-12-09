/* ESP-TEE (Trusted Execution Environment) Example
 *
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_random.h"

#include "esp_tee.h"
#include "secure_service_num.h"
#include "example_service.h"

#define EXAMPLE_BUF_SZ        (32)
#define AES256_GCM_TAG_LEN    (12)
#define AES256_GCM_AAD_LEN    (16)

static const char *TAG = "example_tee_basic";

/*
 * Example workflow:
 * 1. The REE generates random plaintext and AAD (Additional Authenticated Data)
 * 2. The REE initiates an AES-256-GCM encryption request and provides output buffers
 *    for ciphertext and authentication tag
 * 3. The TEE receives the request and performs encryption using a protected key and
 *    nonce that are only accessible within the secure environment
 * 4. The encrypted ciphertext and authentication tag are returned to the REE
 * 5. The REE initiates a decryption request with the ciphertext and authentication tag
 * 6. The TEE performs authenticated decryption, verifying the tag and returning the plaintext
 * 7. The REE verifies that the decrypted data matches the original plaintext
 */
void app_main(void)
{
    ESP_LOGI(TAG, "AES-256-GCM operations in TEE");

    uint8_t plain_text[EXAMPLE_BUF_SZ];
    uint8_t cipher_text[EXAMPLE_BUF_SZ] = {0};
    uint8_t decrypted_text[EXAMPLE_BUF_SZ] = {0};
    uint8_t tag[AES256_GCM_TAG_LEN] = {0};
    uint8_t aad_buf[AES256_GCM_AAD_LEN];

    /* Generate random plaintext and AAD */
    esp_fill_random(plain_text, sizeof(EXAMPLE_BUF_SZ));
    esp_fill_random(aad_buf, AES256_GCM_AAD_LEN);

    /* Encryption operation */
    example_aes_gcm_ctx_t enc_ctx = {
        .aad = aad_buf,
        .aad_len = sizeof(aad_buf),
        .input = plain_text,
        .input_len = sizeof(plain_text),
    };

    uint32_t ret = esp_tee_service_call(5, SS_EXAMPLE_SEC_SERV_AES_GCM_ENCRYPT, &enc_ctx, tag, AES256_GCM_TAG_LEN, cipher_text);
    ESP_ERROR_CHECK((esp_err_t)ret);

    ESP_LOG_BUFFER_HEX_LEVEL("ciphertext", cipher_text, sizeof(cipher_text), ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEX_LEVEL("tag", tag, AES256_GCM_TAG_LEN, ESP_LOG_INFO);

    /* Decryption operation */
    example_aes_gcm_ctx_t dec_ctx = {
        .aad = aad_buf,
        .aad_len = sizeof(aad_buf),
        .input = cipher_text,
        .input_len = sizeof(cipher_text),
    };

    ret = esp_tee_service_call(5, SS_EXAMPLE_SEC_SERV_AES_GCM_DECRYPT, &dec_ctx, tag, AES256_GCM_TAG_LEN, decrypted_text);
    ESP_ERROR_CHECK((esp_err_t)ret);

    if (memcmp(decrypted_text, plain_text, sizeof(plain_text)) != 0) {
        ESP_LOGE(TAG, "Decrypted data mismatch!");
    } else {
        ESP_LOGI(TAG, "AES-GCM decryption successful!");
    }
}
