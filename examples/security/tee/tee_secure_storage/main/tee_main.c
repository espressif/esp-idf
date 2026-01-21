/*
 * TEE Secure Storage example
 *
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_ecdsa.h"

#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"
#include "sdkconfig.h"

#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP256R1_KEY_LEN  (32)
#define AES256_GCM_TAG_LEN       (16)
#define AES256_GCM_AAD_LEN       (16)

#define SIGN_KEY_STR_ID          (CONFIG_EXAMPLE_TEE_SEC_STG_SIGN_KEY_STR_ID)
#define ENC_KEY_STR_ID           (CONFIG_EXAMPLE_TEE_SEC_STG_ENC_KEY_STR_ID)
#define MAX_AES_PLAINTEXT_LEN    (128)

static const char *message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";

static const char *TAG = "example_tee_sec_stg";

static esp_err_t verify_ecdsa_secp256r1_sign(const uint8_t *digest, size_t len, const uint8_t *pub_key, size_t pub_key_len, const uint8_t *signature, size_t signature_len)
{
    if (pub_key == NULL || pub_key_len == 0 || digest == NULL || len == 0 || signature == NULL || signature_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_FAIL;

    psa_key_id_t pub_key_id = 0;
    psa_key_attributes_t pub_key_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&pub_key_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&pub_key_attr, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&pub_key_attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    psa_status_t status = psa_import_key(&pub_key_attr, pub_key, pub_key_len, &pub_key_id);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = psa_verify_hash(pub_key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), digest, len, signature, signature_len);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    psa_destroy_key(pub_key_id);
    psa_reset_key_attributes(&pub_key_attr);

    err = ESP_OK;

exit:

    return err;
}

static void example_tee_sec_stg_sign_verify(void *pvParameter)
{
    psa_key_id_t priv_key_id = 0;
    psa_key_attributes_t priv_key_attr = PSA_KEY_ATTRIBUTES_INIT;

    char *msg = (char *)pvParameter;
    ESP_LOGI(TAG, "Message-to-be-signed: %s", msg);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    size_t msg_digest_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, (const uint8_t *)msg, strlen(msg), msg_digest, sizeof(msg_digest), &msg_digest_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to calculate message hash!");
        goto exit;
    }

    esp_tee_sec_storage_key_cfg_t cfg = {
        .id = (const char *)(SIGN_KEY_STR_ID),
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1
    };

    esp_err_t err = esp_tee_sec_storage_clear_key(cfg.id);
    if (err != ESP_OK && err != ESP_ERR_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to clear key %d!", cfg.id);
        goto exit;
    }

    err = esp_tee_sec_storage_gen_key(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate keypair!");
        goto exit;
    }

    esp_ecdsa_opaque_key_t opaque_key = {
        .curve = ESP_ECDSA_CURVE_SECP256R1,
        .tee_key_id = cfg.id,
    };

    psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    psa_set_key_type(&priv_key_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&priv_key_attr, ECDSA_SECP256R1_KEY_LEN * 8);
    psa_set_key_usage_flags(&priv_key_attr, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&priv_key_attr, alg);
    psa_set_key_lifetime(&priv_key_attr, PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE);

    status = psa_import_key(&priv_key_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import private key!");
        goto exit;
    }

    uint8_t signature[2 * ECDSA_SECP256R1_KEY_LEN];
    size_t signature_len = 0;
    status = psa_sign_hash(priv_key_id, alg, msg_digest, msg_digest_len, signature, sizeof(signature), &signature_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to generate signature!");
        psa_destroy_key(priv_key_id);
        goto exit;
    }

    ESP_LOG_BUFFER_HEX("Signature", signature, signature_len);

    uint8_t pub_key[2 * ECDSA_SECP256R1_KEY_LEN + 1];
    size_t pub_key_len = 0;
    status = psa_export_public_key(priv_key_id, pub_key, sizeof(pub_key), &pub_key_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to fetch public-key!");
        psa_destroy_key(priv_key_id);
        goto exit;
    }

    err = verify_ecdsa_secp256r1_sign(msg_digest, msg_digest_len, pub_key, pub_key_len, signature, signature_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify signature!");
        psa_destroy_key(priv_key_id);
        goto exit;
    }

    ESP_LOGI(TAG, "Signature verified successfully!");

exit:
    psa_reset_key_attributes(&priv_key_attr);
    vTaskDelete(NULL);
}

static void example_tee_sec_stg_encrypt_decrypt(void *pvParameter)
{
    char *plaintext = (char *)pvParameter;
    const size_t plaintext_len = strnlen(plaintext, MAX_AES_PLAINTEXT_LEN);
    ESP_LOG_BUFFER_HEX("Plaintext", plaintext, plaintext_len);

    if (plaintext_len == MAX_AES_PLAINTEXT_LEN && plaintext[MAX_AES_PLAINTEXT_LEN] != '\0') {
        ESP_LOGW(TAG, "Plaintext exceeds the max length limit - only the first %d bytes will be encrypted", MAX_AES_PLAINTEXT_LEN);
    }

    uint8_t tag[AES256_GCM_TAG_LEN];
    uint8_t aad_buf[AES256_GCM_AAD_LEN];
    memset(aad_buf, 0xA5, sizeof(aad_buf));

    esp_err_t err = ESP_FAIL;

    uint8_t *ciphertext = calloc(plaintext_len + 1, sizeof(uint8_t));
    if (ciphertext == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    esp_tee_sec_storage_key_cfg_t cfg = {
        .id = (const char *)(ENC_KEY_STR_ID),
        .type = ESP_SEC_STG_KEY_AES256
    };

    err = esp_tee_sec_storage_clear_key(cfg.id);
    if (err != ESP_OK && err != ESP_ERR_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to clear key %d!", cfg.id);
        goto exit;
    }

    err = esp_tee_sec_storage_gen_key(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate key!");
        goto exit;
    }

    esp_tee_sec_storage_aead_ctx_t ctx = {
        .key_id = cfg.id,
        .aad = aad_buf,
        .aad_len = sizeof(aad_buf),
    };

    ctx.input = (const uint8_t *)plaintext;
    ctx.input_len = plaintext_len;
    err = esp_tee_sec_storage_aead_encrypt(&ctx, tag, sizeof(tag), ciphertext);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to encrypt data!");
        goto exit;
    }

    ESP_LOG_BUFFER_HEX("Encrypted data", ciphertext, plaintext_len);

    ctx.input = (const uint8_t *)ciphertext;
    ctx.input_len = plaintext_len;
    err = esp_tee_sec_storage_aead_decrypt(&ctx, tag, sizeof(tag), ciphertext);
    if (err != ESP_OK || memcmp(ciphertext, plaintext, plaintext_len) != 0) {
        ESP_LOGE(TAG, "Encryption verification failed!");
        err = ESP_FAIL;
        goto exit;
    }

    ESP_LOG_BUFFER_HEX("Tag", tag, sizeof(tag));
    ESP_LOGI(TAG, "Done with encryption/decryption!");

exit:
    if (ciphertext != NULL) {
        free(ciphertext);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "TEE Secure Storage");

    xTaskCreate(example_tee_sec_stg_sign_verify, "tee_sec_stg_sign_verify", 4096, (void *)message, 5, NULL);
    xTaskCreate(example_tee_sec_stg_encrypt_decrypt, "tee_sec_stg_encrypt_decrypt", 4096, (void *)message, 5, NULL);
}
