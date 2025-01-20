/* ESP-TEE (Trusted Execution Environment) Example
 *
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_err.h"
#include "aes/esp_aes.h"

#include "esp_tee.h"
#include "secure_service_num.h"

#define BUF_SZ  (16)

static const char *TAG = "example_tee_basic";

static const uint8_t expected_cipher[] = {
    0xee, 0x04, 0x9b, 0xee, 0x95, 0x6f, 0x25, 0x04,
    0x1e, 0x8c, 0xe4, 0x4e, 0x8e, 0x4e, 0x7a, 0xd3
};

static const uint8_t nonce[IV_BYTES] =  {[0 ... IV_BYTES - 1] = 0xFF};

/*
 * Example workflow:
 * 1. The REE initiates an AES operation request via the secure service call interface
 * 2. The TEE receives the request and performs encryption/decryption using AES-256-CBC mode
 * 3. The TEE uses a protected key that is only accessible within the secure environment
 * 4. The encrypted/decrypted result is returned to the non-secure world through an output buffer
 *    provided in the secure service call
 */
void app_main(void)
{
    ESP_LOGI(TAG, "AES-256-CBC operations in TEE");

    uint8_t plain_text[BUF_SZ] =  {[0 ... BUF_SZ - 1] = 0x3A};
    uint8_t cipher_text[BUF_SZ] = {0};
    uint8_t decrypted_text[BUF_SZ] = {0};
    uint8_t iv[IV_BYTES] = {0};

    memcpy(iv, nonce, sizeof(iv));
    uint32_t ret = esp_tee_service_call(6, SS_EXAMPLE_SEC_SERV_AES_OP, ESP_AES_ENCRYPT, sizeof(plain_text), iv, plain_text, cipher_text);
    if (ret != ESP_OK || memcmp(cipher_text, expected_cipher, sizeof(expected_cipher))) {
        ESP_LOGE(TAG, "Failed to encrypt data!");
    } else {
        ESP_LOGI(TAG, "AES encryption successful!");
    }

    ESP_LOGI(TAG, "Cipher text -");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, cipher_text, sizeof(cipher_text), ESP_LOG_INFO);

    memcpy(iv, nonce, sizeof(iv));
    ret = esp_tee_service_call(6, SS_EXAMPLE_SEC_SERV_AES_OP, ESP_AES_DECRYPT, sizeof(cipher_text), iv, cipher_text, decrypted_text);
    if (ret != ESP_OK || memcmp(decrypted_text, plain_text, sizeof(plain_text))) {
        ESP_LOGE(TAG, "Failed to decrypt data!");
    } else {
        ESP_LOGI(TAG, "AES decryption successful!");
    }
}
