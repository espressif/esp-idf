/*
 * TEE Secure Storage example
 *
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

#include "mbedtls/ecp.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"

#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"

#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP256R1_KEY_LEN  (32)
#define AES256_GCM_TAG_LEN       (16)
#define AES256_GCM_AAD_LEN       (16)

#define KEY_SLOT_ID              (CONFIG_EXAMPLE_TEE_SEC_STG_SLOT_ID)
#define MAX_AES_PLAINTEXT_LEN    (128)

static const char *message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";

static const char *TAG = "example_tee_sec_stg";

static esp_err_t verify_ecdsa_secp256r1_sign(const uint8_t *digest, size_t len, const esp_tee_sec_storage_pubkey_t *pubkey, const esp_tee_sec_storage_sign_t *sign)
{
    if (pubkey == NULL || digest == NULL || sign == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (len == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ESP_FAIL;

    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    int ret = mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        goto exit;
    }

    size_t plen = mbedtls_mpi_size(&ecdsa_context.MBEDTLS_PRIVATE(grp).P);

    ret = mbedtls_mpi_read_binary(&r, sign->sign_r, plen);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_read_binary(&s, sign->sign_s, plen);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), pubkey->pub_x, plen);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), pubkey->pub_y, plen);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_lset(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Z), 1);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_ecdsa_verify(&ecdsa_context.MBEDTLS_PRIVATE(grp), digest, len, &ecdsa_context.MBEDTLS_PRIVATE(Q), &r, &s);
    if (ret != 0) {
        goto exit;
    }

    err = ESP_OK;

exit:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecdsa_free(&ecdsa_context);

    return err;
}

static void example_tee_sec_stg_sign_verify(void *pvParameter)
{
    char *msg = (char *)pvParameter;
    ESP_LOGI(TAG, "Message-to-be-signed: %s", msg);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    int ret = mbedtls_sha256((const unsigned char *)msg, strlen(msg), msg_digest, false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to calculate message hash!");
        goto exit;
    }

    esp_err_t err = esp_tee_sec_storage_clear_slot(KEY_SLOT_ID);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear slot %d!", KEY_SLOT_ID);
        goto exit;
    }

    err = esp_tee_sec_storage_gen_key(KEY_SLOT_ID, ESP_SEC_STG_KEY_ECDSA_SECP256R1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate keypair!");
        goto exit;
    }

    esp_tee_sec_storage_sign_t sign = {};
    err = esp_tee_sec_storage_get_signature(KEY_SLOT_ID, msg_digest, sizeof(msg_digest), &sign);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate signature!");
        goto exit;
    }

    ESP_LOG_BUFFER_HEX("Signature", &sign, sizeof(sign));

    esp_tee_sec_storage_pubkey_t pubkey = {};
    err = esp_tee_sec_storage_get_pubkey(KEY_SLOT_ID, &pubkey);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch public-key!");
        goto exit;
    }

    err = verify_ecdsa_secp256r1_sign(msg_digest, sizeof(msg_digest), &pubkey, &sign);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify signature!");
        goto exit;
    }

    ESP_LOGI(TAG, "Signature verified successfully!");

exit:
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

    err = esp_tee_sec_storage_clear_slot(KEY_SLOT_ID);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear slot %d!", KEY_SLOT_ID);
        goto exit;
    }

    err = esp_tee_sec_storage_gen_key(KEY_SLOT_ID, ESP_SEC_STG_KEY_AES256);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate key!");
        goto exit;
    }

    err = esp_tee_sec_storage_encrypt(KEY_SLOT_ID, (uint8_t *)plaintext, plaintext_len, aad_buf, sizeof(aad_buf), tag, sizeof(tag), ciphertext);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to encrypt data!");
        goto exit;
    }

    ESP_LOG_BUFFER_HEX("Encrypted data", ciphertext, plaintext_len);

    err = esp_tee_sec_storage_decrypt(KEY_SLOT_ID, (uint8_t *)ciphertext, plaintext_len, aad_buf, sizeof(aad_buf), tag, sizeof(tag), ciphertext);
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

    ESP_ERROR_CHECK(esp_tee_sec_storage_init());

    xTaskCreate(example_tee_sec_stg_sign_verify, "tee_sec_stg_sign_verify", 4096, (void *)message, 5, NULL);
    xTaskCreate(example_tee_sec_stg_encrypt_decrypt, "tee_sec_stg_encrypt_decrypt", 4096, (void *)message, 5, NULL);
}
