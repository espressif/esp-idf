/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_log.h"

#include "mbedtls/gcm.h"

#include "esp_tee.h"
#include "secure_service_num.h"
#include "example_service.h"

#define AES256_KEY_LEN          32
#define AES256_KEY_BITS         (AES256_KEY_LEN * 8)
#define AES256_NONCE_LEN        12

static const char *TAG = "tee_secure_service";

/* Fixed key (only for example purposes) */
static const uint8_t key[AES256_KEY_LEN] =  {[0 ... AES256_KEY_LEN - 1] = 0xA5};

/* Fixed nonce (only for example purposes) */
static const uint8_t nonce[AES256_NONCE_LEN] = {[0 ... AES256_NONCE_LEN - 1] = 0x5A};

static esp_err_t aes_gcm_crypt_common(example_aes_gcm_ctx_t *ctx, uint8_t *tag, size_t tag_len,
                                      uint8_t *output, bool is_encrypt)
{
    if (esp_cpu_get_curr_privilege_level() != ESP_CPU_S_MODE) {
        ESP_LOGE(TAG, "Operation executing from illegal privilege level!");
        return ESP_ERR_INVALID_STATE;
    }

    if (ctx == NULL || ctx->input == NULL || output == NULL || tag == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (ctx->input_len == 0 || tag_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Secure service call: PROTECTED M-mode");
    ESP_LOGI(TAG, "AES-256-GCM %s", is_encrypt ? "encryption" : "decryption");

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    esp_err_t err = ESP_FAIL;

    int ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, AES256_KEY_BITS);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in setting key: %d", ret);
        goto cleanup;
    }

    if (is_encrypt) {
        ret = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, ctx->input_len,
                                        nonce, AES256_NONCE_LEN,
                                        ctx->aad, ctx->aad_len,
                                        ctx->input, output,
                                        tag_len, tag);
        if (ret != 0) {
            ESP_LOGE(TAG, "Error in encrypting data: %d", ret);
            goto cleanup;
        }
    } else {
        ret = mbedtls_gcm_auth_decrypt(&gcm, ctx->input_len,
                                    nonce, AES256_NONCE_LEN,
                                    ctx->aad, ctx->aad_len,
                                    tag, tag_len,
                                    ctx->input, output);
        if (ret != 0) {
            ESP_LOGE(TAG, "Error in decrypting data: %d", ret);
            goto cleanup;
        }
    }
    err = ESP_OK;

cleanup:
    mbedtls_gcm_free(&gcm);
    return err;
}

esp_err_t _ss_example_sec_serv_aes_gcm_encrypt(example_aes_gcm_ctx_t *ctx, uint8_t *tag, size_t tag_len, uint8_t *output)
{
    return aes_gcm_crypt_common(ctx, tag, tag_len, output, true);
}

esp_err_t _ss_example_sec_serv_aes_gcm_decrypt(example_aes_gcm_ctx_t *ctx, const uint8_t *tag, size_t tag_len, uint8_t *output)
{
    return aes_gcm_crypt_common(ctx, (uint8_t *)tag, tag_len, output, false);
}
