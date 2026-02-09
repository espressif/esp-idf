/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_log.h"

#include "psa/crypto.h"

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

    esp_err_t err = ESP_FAIL;
    psa_aead_operation_t operation = PSA_AEAD_OPERATION_INIT;
    psa_status_t status;
    psa_key_id_t key_id;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_algorithm_t alg = PSA_ALG_GCM;
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&key_attributes, alg);
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&key_attributes, AES256_KEY_BITS);
    status = psa_import_key(&key_attributes, key, sizeof(key), &key_id);
    psa_reset_key_attributes(&key_attributes);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Error in importing key: %d", status);
        return ESP_ERR_INVALID_STATE;
    }
    if (is_encrypt) {
        status = psa_aead_encrypt_setup(&operation, key_id, alg);
    } else {
        status = psa_aead_decrypt_setup(&operation, key_id, alg);
    }
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Error in AEAD setup: %d", status);
        goto cleanup;
    }
    status = psa_aead_set_lengths(&operation, ctx->aad_len, ctx->input_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Error in setting lengths: %d", status);
        goto cleanup;
    }
    psa_aead_set_nonce(&operation, nonce, AES256_NONCE_LEN);
    if (ctx->aad_len > 0) {
        status = psa_aead_update_ad(&operation, ctx->aad, ctx->aad_len);
        if (status != PSA_SUCCESS) {
            ESP_LOGE(TAG, "Error in updating AAD: %d", status);
            goto cleanup;
        }
    }

    size_t output_len = 0;
    status = psa_aead_update(&operation, ctx->input, ctx->input_len, output, ctx->input_len, &output_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Error in updating aead: %d", status);
        goto cleanup;
    }

    if (is_encrypt) {
        size_t output_tag_len = 0;
        status = psa_aead_finish(&operation, output + output_len, ctx->input_len + tag_len - output_len, &output_len, tag, tag_len, &output_tag_len);
        if (status != PSA_SUCCESS) {
            ESP_LOGE(TAG, "Error in finishing encryption: %d", status);
            goto cleanup;
        }
    } else {
        size_t plaintext_len = 0;
        status = psa_aead_verify(&operation, output, ctx->input_len, &plaintext_len, tag, tag_len);
        if (status != PSA_SUCCESS) {
            ESP_LOGE(TAG, "Error in verifying decryption: %d", status);
            goto cleanup;
        }
    }
    err = ESP_OK;

cleanup:
    psa_aead_abort(&operation);
    psa_destroy_key(key_id);
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
